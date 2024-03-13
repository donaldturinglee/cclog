#ifndef CCLOG_H
#define CCLOG_H

#include <fstream>
#include <iostream>
#include <string_view>

namespace cclog {

inline void log(std::string_view message) {
	std::fstream log_file("cclog.log", std::ios::app);
	log_file << message << '\n';
}

} // namespace cclog

#endif // CCLOG_H
