#ifndef MSC_TEST_MEDIA_STREAM_TRACK_FACTORY_HPP
#define MSC_TEST_MEDIA_STREAM_TRACK_FACTORY_HPP

 #define RTC_DCHECK_IS_ON 0

#include "api/media_stream_interface.h"

rtc::scoped_refptr<webrtc::AudioTrackInterface> createAudioTrack(const std::string& label);

rtc::scoped_refptr<webrtc::VideoTrackInterface> createPeriodicVideoTrack(const std::string& label);

rtc::scoped_refptr<webrtc::VideoTrackInterface> createSquaresVideoTrack(const std::string& label);

rtc::scoped_refptr<webrtc::VideoTrackInterface> createLocalCameraTrack(const std::string& label);

#endif
