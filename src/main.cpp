﻿#include "Broadcaster.hpp"
#include "mediasoupclient.hpp"
#include <cpr/cpr.h>
#include <csignal> // sigsuspend()
#include <cstdlib>
#include <iostream>
#include <string>

#include "log.hpp"
#include "MyTest.hpp"

using json = nlohmann::json;

void signalHandler(int signum)
{
	std::cout << "[INFO] interrupt signal (" << signum << ") received" << std::endl;

	std::cout << "[INFO] leaving!" << std::endl;

	std::exit(signum);
}

int main(int /*argc*/, char* /*argv*/[])
{
	// MyTest().test();
	// return 0;

	// Register signal SIGINT and signal handler.
	signal(SIGINT, signalHandler);

	// ::setenv("MY_VAR", "my_value", 1);
	::putenv("SERVER_URL=http://101.201.247.187:4443");
	::putenv("ROOM_ID=abcdefgh");
	::putenv("VERIFY_SSL=false");
	::putenv("ENABLE_AUDIO=true");
	// log_init_default();
	log_options opts = { .is_color = true, .writeFile = true, .hideWorkingPath = true};
	opts.is_time = true;
	sprintf(opts.fileMode, "%s", "w");
	log_init(opts);

	// Retrieve configuration from environment variables.
	const char* envServerUrl    = std::getenv("SERVER_URL");
	const char* envRoomId       = std::getenv("ROOM_ID");
	const char* envEnableAudio  = std::getenv("ENABLE_AUDIO");
	const char* envUseSimulcast = std::getenv("USE_SIMULCAST");
	const char* envWebrtcDebug  = std::getenv("WEBRTC_DEBUG");
	const char* envVerifySsl    = std::getenv("VERIFY_SSL");

	if (envServerUrl == nullptr)
	{
		std::cerr << "[ERROR] missing 'SERVER_URL' environment variable" << std::endl;

		return 1;
	}

	if (envRoomId == nullptr)
	{
		std::cerr << "[ERROR] missing 'ROOM_ID' environment variable" << std::endl;

		return 1;
	}

	std::string baseUrl = envServerUrl;
	baseUrl.append("/rooms/").append(envRoomId);

	bool enableAudio = true;

	if (envEnableAudio && std::string(envEnableAudio) == "false")
		enableAudio = false;

	bool useSimulcast = true;

	if (envUseSimulcast && std::string(envUseSimulcast) == "false")
		useSimulcast = false;

	bool verifySsl = true;
	if (envVerifySsl && std::string(envVerifySsl) == "false")
		verifySsl = false;

	// Set RTC logging severity.
	if (envWebrtcDebug)
	{
		if (std::string(envWebrtcDebug) == "info")
			rtc::LogMessage::LogToDebug(rtc::LoggingSeverity::LS_INFO);
		else if (std::string(envWebrtcDebug) == "warn")
			rtc::LogMessage::LogToDebug(rtc::LoggingSeverity::LS_WARNING);
		else if (std::string(envWebrtcDebug) == "error")
			rtc::LogMessage::LogToDebug(rtc::LoggingSeverity::LS_ERROR);
	}

	auto logLevel = mediasoupclient::Logger::LogLevel::LOG_DEBUG;
	mediasoupclient::Logger::SetLogLevel(logLevel);
	mediasoupclient::Logger::SetDefaultHandler();

	// Initilize mediasoupclient.
	mediasoupclient::Initialize();

	std::cout << "[INFO] welcome to mediasoup broadcaster app!\n" << std::endl;

	std::cout << "[INFO] verifying that room '" << envRoomId << "' exists..." << std::endl;

	log_trace("%s", baseUrl.c_str());
	auto r = cpr::GetAsync(cpr::Url{ baseUrl }, cpr::VerifySsl{ verifySsl }).get();

	if (r.status_code != 200)
	{
		std::cerr << "[ERROR] unable to retrieve room info" << " [status code:" << r.status_code
		          << ", body:\"" << r.text << "\"]" << std::endl;

		return 1;
	}
	else
	{
		// log_debug("-----------");
		// std::cout << r.text << std::endl;
		// log_debug("-----------");
		std::cout << "[INFO] found room" << envRoomId << std::endl;
	}

	auto response = nlohmann::json::parse(r.text);

	Broadcaster broadcaster;

	broadcaster.Start(baseUrl, enableAudio, useSimulcast, response, verifySsl);

	std::cout << "[INFO] press Ctrl+C or Cmd+C to leave..." << std::endl;

	while (true)
	{
		std::cin.get();
	}

	return 0;
}
