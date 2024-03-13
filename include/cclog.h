#ifndef CCLOG_H
#define CCLOG_H

#include <algorithm>
#include <chrono>
#include <ctime>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <map>
#include <memory>
#include <string>
#include <string_view>
#include <vector>

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
	virtual std::unique_ptr<Tag> clone() const = 0;
protected:
	Tag(std::string const& key, std::string const& value) : key_(key), text_(key + "=\"" + value + "\"") {}
private:
	std::string key_;
	std::string const text_;
};

class LogLevel : public Tag {
public:
	LogLevel(std::string const& text) : Tag("log_level", text) {}
	std::unique_ptr<Tag> clone() const override {
		return std::unique_ptr<Tag>(new LogLevel(*this));
	}
};

class Color : public Tag {
public:
	Color(std::string const& text) : Tag("color", text) {}
	std::unique_ptr<Tag> clone() const override {
		return std::unique_ptr<Tag>(new Color(*this));
	}
};

class Size : public Tag {
public:
	Size(std::string const& text) : Tag("size", text) {}
	std::unique_ptr<Tag> clone() const override {
		return std::unique_ptr<Tag>(new Size(*this));
	}
};

inline std::string to_string(Tag const& tag) {
	return tag.get_text();
}

inline std::map<std::string, std::unique_ptr<Tag>>& get_default_tags() {
	static std::map<std::string, std::unique_ptr<Tag>> tags;
	return tags;
}

inline void add_default_tag(Tag const& tag) {
	auto& tags = get_default_tags();
	tags[tag.get_key()] = tag.clone();
}

inline std::fstream log(std::vector<Tag const*> tags = {}) {
	auto const now = std::chrono::system_clock::now();
	std::time_t const start_time = std::chrono::system_clock::to_time_t(now);
	auto const ms = duration_cast<std::chrono::milliseconds>(now.time_since_epoch()) % 1000;
	std::fstream log_file("cclog.log", std::ios::app);
	log_file << '\n'
			<< std::put_time(std::gmtime(&start_time), "%Y-%m-%dT%H:%M:%S.")
			<< std::setw(3) << std::setfill('0')
			<< std::to_string(ms.count());
	for(auto const& default_tag : get_default_tags()) {
		if(std::find_if(tags.begin(), tags.end(), [&default_tag](auto const& tag) {
			return default_tag.first == tag->get_key();	
		}) == tags.end()) {
			log_file << " " << default_tag.second->get_text();
		}
	}
	for(auto const& tag : tags) {
		log_file << " " << tag->get_text();
	}
	log_file << " ";

	return log_file;
}


inline auto log(Tag const& tag1) {
	return log({&tag1});
}

inline auto log(Tag const& tag1, Tag const& tag2) {
	return log({&tag1, &tag2});
}

inline auto log(Tag const& tag1, Tag const& tag2, Tag const& tag3) {
	return log({&tag1, &tag2, &tag3});
}

} // namespace cclog

inline cclog::LogLevel error("error");
inline cclog::LogLevel info("info");
inline cclog::LogLevel debug("debug");
inline cclog::Color red("red");
inline cclog::Color green("green");
inline cclog::Color blue("blue");
inline cclog::Size small("small");
inline cclog::Size medium("medium");
inline cclog::Size large("large");

#endif // CCLOG_H
