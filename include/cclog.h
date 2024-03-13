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

class Tag {
public:
	virtual ~Tag() = default;
	std::string get_key() const {
		return key_;
	}
	std::string get_text() const {
		return text_;
	}
protected:
	Tag(std::string const& key, std::string const& value) : key_(key), text_(key + "=\"" + value + "\"") {}
private:
	std::string key_;
	std::string const text_;
};

class LogLevel : public Tag {
public:
	LogLevel(std::string const& text) : Tag("log_level", text) {}
};


inline std::fstream log(std::string_view pre_message = "") {
	auto const now = std::chrono::system_clock::now();
	std::time_t const start_time = std::chrono::system_clock::to_time_t(now);
	auto const ms = duration_cast<std::chrono::milliseconds>(now.time_since_epoch()) % 1000;
	std::fstream log_file("cclog.log", std::ios::app);
	log_file << '\n'
			<< std::put_time(std::gmtime(&start_time), "%Y-%m-%dT%H:%M:%S.")
			<< std::setw(3) << std::setfill('0')
			<< std::to_string(ms.count()) << " "
			<< pre_message << " ";
	return log_file;
}

inline std::string to_string(Tag const& tag) {
	return tag.get_text();
}

inline std::fstream log(Tag const& tag) {
	return log(to_string(tag));
}


} // namespace cclog

inline cclog::LogLevel error("error");
inline cclog::LogLevel info("info");
inline cclog::LogLevel debug("debug");

#endif // CCLOG_H
