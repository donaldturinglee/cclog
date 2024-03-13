#ifndef CCLOG_H
#define CCLOG_H

#include <chrono>
#include <ctime>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <string>
#include <string_view>

namespace cclog {

inline std::fstream log() {
	auto const now = std::chrono::system_clock::now();
	std::time_t const start = std::chrono::system_clock::to_time_t(now);
	auto const ms = duration_cast<std::chrono::milliseconds>(now.time_since_epoch()) % 1000;
	std::fstream log_file("cclog.log", std::ios::app);
	log_file << '\n'
			<< std::put_time(std::gmtime(&start), "%Y-%m-%dT%H:%M:%S.")
			<< std::setw(3) << std::setfill('0')
			<< std::to_string(ms.count()) << " ";
	return log_file;
}

} // namespace cclog

#endif // CCLOG_H
