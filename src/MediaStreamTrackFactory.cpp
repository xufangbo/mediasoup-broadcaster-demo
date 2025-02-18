#define MSC_CLASS "MediaStreamTrackFactory"

#include <iostream>

#include "CapturerTrackSource.hpp"
#include "MediaSoupClientErrors.hpp"
#include "MediaStreamTrackFactory.hpp"
#include "pc/test/fake_audio_capture_module.h"
#include "pc/test/fake_periodic_video_track_source.h"
#include "pc/test/frame_generator_capturer_video_track_source.h"
#include "system_wrappers/include/clock.h"
#include "api/audio_codecs/builtin_audio_decoder_factory.h"
#include "api/audio_codecs/builtin_audio_encoder_factory.h"
#include "api/create_peerconnection_factory.h"
#include "api/video_codecs/builtin_video_decoder_factory.h"
#include "api/video_codecs/builtin_video_encoder_factory.h"
#include <future>

#include "log.hpp"

using namespace mediasoupclient;

static rtc::scoped_refptr<webrtc::PeerConnectionFactoryInterface> factory;

/* MediaStreamTrack holds reference to the threads of the PeerConnectionFactory.
 * Use plain pointers in order to avoid threads being destructed before tracks.
 */
static rtc::Thread* networkThread;
static rtc::Thread* signalingThread;
static rtc::Thread* workerThread;

static void createFactory()
{
	// 这里是release，创建在mediasoupclient::PeerConnection()构造函数中
	networkThread   = rtc::Thread::Create().release();
	signalingThread = rtc::Thread::Create().release();
	workerThread    = rtc::Thread::Create().release();

	networkThread->SetName("network_thread", nullptr);
	signalingThread->SetName("signaling_thread", nullptr);
	workerThread->SetName("worker_thread", nullptr);

	if (!networkThread->Start() || !signalingThread->Start() || !workerThread->Start())
	{
		MSC_THROW_INVALID_STATE_ERROR("thread start errored");
	}

	webrtc::PeerConnectionInterface::RTCConfiguration config;

	auto fakeAudioCaptureModule = FakeAudioCaptureModule::Create();
	if (!fakeAudioCaptureModule)
	{
		MSC_THROW_INVALID_STATE_ERROR("audio capture module creation errored");
	}

	factory = webrtc::CreatePeerConnectionFactory(
	  networkThread,
	  workerThread,
	  signalingThread,
	  fakeAudioCaptureModule,
	  webrtc::CreateBuiltinAudioEncoderFactory(),
	  webrtc::CreateBuiltinAudioDecoderFactory(),
	  webrtc::CreateBuiltinVideoEncoderFactory(),
	  webrtc::CreateBuiltinVideoDecoderFactory(),
	  nullptr /*audio_mixer*/,
	  nullptr /*audio_processing*/);

	if (!factory)
	{
		MSC_THROW_ERROR("error ocurred creating peerconnection factory");
	}
}

// Audio track creation.
rtc::scoped_refptr<webrtc::AudioTrackInterface> createAudioTrack(const std::string& label)
{
	if (!factory)
		createFactory();

	cricket::AudioOptions options;
	options.highpass_filter = false;

	rtc::scoped_refptr<webrtc::AudioSourceInterface> source = factory->CreateAudioSource(options);

	return factory->CreateAudioTrack(label, source.get());
}

// Video track creation.
rtc::scoped_refptr<webrtc::VideoTrackInterface> createPeriodicVideoTrack(const std::string& label)
{
	if (!factory)
		createFactory();

	auto* videoTrackSource =
	  new rtc::RefCountedObject<webrtc::FakePeriodicVideoTrackSource>(false /* remote */);

	return factory->CreateVideoTrack(label, videoTrackSource);
	//  return nullptr;
}

rtc::scoped_refptr<webrtc::VideoTrackInterface> createSquaresVideoTrack(const std::string& label)
{
	if (!factory)
		createFactory();

	// std::promise<rtc::scoped_refptr<webrtc::VideoTrackInterface>> promise;
	// signalingThread->PostTask([signalingThread, label, &promise]() {
	// 	auto* videoTrackSource =
	// 	  new rtc::RefCountedObject<webrtc::FrameGeneratorCapturerVideoTrackSource>(
	// 	    webrtc::FrameGeneratorCapturerVideoTrackSource::Config(),
	// 	    webrtc::Clock::GetRealTimeClock(),
	// 	    false);
	// 	videoTrackSource->Start();

	// 	std::cout << "[INFO] creating video track" << std::endl;
	// 	auto videoTrack = factory->CreateVideoTrack(label, videoTrackSource);

	// 	promise.set_value(videoTrack);
	// });
	// auto future = promise.get_future();
	// return future.get();

	rtc::scoped_refptr<webrtc::VideoTrackInterface> videoTrack;
	signalingThread->BlockingCall([&label, &videoTrack] {
		auto* videoTrackSource =
		  new rtc::RefCountedObject<webrtc::FrameGeneratorCapturerVideoTrackSource>(
		    webrtc::FrameGeneratorCapturerVideoTrackSource::Config(),
		    webrtc::Clock::GetRealTimeClock(),
		    false);
		videoTrackSource->Start();

		std::cout << "[INFO] creating video track" << std::endl;
		videoTrack = factory->CreateVideoTrack(label, videoTrackSource);
	});
	return videoTrack;
}

rtc::scoped_refptr<webrtc::VideoTrackInterface> createLocalCameraTrack(const std::string& label)
{
	if (!factory)
		createFactory();

	rtc::scoped_refptr<CapturerTrackSource> video_device = CapturerTrackSource::Create();

	// 创建 VideoTrack
	rtc::scoped_refptr<webrtc::VideoTrackInterface> video_track =
	  factory->CreateVideoTrack(video_device, label);

	return video_track;
}
