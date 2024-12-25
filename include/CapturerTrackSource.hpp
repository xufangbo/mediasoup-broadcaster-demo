#pragma once

#include "modules/video_capture/video_capture_factory.h"
#include "test/vcm_capturer.h"
#include "pc/test/fake_periodic_video_track_source.h"
#include "pc/test/frame_generator_capturer_video_track_source.h"

class CapturerTrackSource : public webrtc::VideoTrackSource
{
public:
	static rtc::scoped_refptr<CapturerTrackSource> Create();

protected:
	explicit CapturerTrackSource(std::unique_ptr<webrtc::test::VcmCapturer> capturer);

private:
	rtc::VideoSourceInterface<webrtc::VideoFrame>* source();
	std::unique_ptr<webrtc::test::VcmCapturer> capturer_;
};

// class CapturerTrackSource : public webrtc::VideoTrackSource {
//  public:
//   static rtc::scoped_refptr<CapturerTrackSource> Create() {
//     const size_t kWidth = 640;
//     const size_t kHeight = 480;
//     const size_t kFps = 30;
//     std::unique_ptr<webrtc::test::VcmCapturer> capturer;
//     std::unique_ptr<webrtc::VideoCaptureModule::DeviceInfo> info(
//     webrtc::VideoCaptureFactory::CreateDeviceInfo()); if (!info) {
//       return nullptr;
//     }
//     int num_devices = info->NumberOfDevices();
//     for (int i = 0; i < num_devices; ++i) {
//       capturer = absl::WrapUnique( webrtc::test::VcmCapturer::Create(kWidth, kHeight, kFps, i));
//       if (capturer) {
//         return rtc::make_ref_counted<CapturerTrackSource>(std::move(capturer));
//       }
//     }

//     return nullptr;
//   }

//  protected:
//   explicit CapturerTrackSource( std::unique_ptr<webrtc::test::VcmCapturer> capturer)
//       : VideoTrackSource(/*remote=*/false), capturer_(std::move(capturer)) {}

//  private:
//   rtc::VideoSourceInterface<webrtc::VideoFrame>* source() override {
//     return capturer_.get();
//   }
//   std::unique_ptr<webrtc::test::VcmCapturer> capturer_;
// };