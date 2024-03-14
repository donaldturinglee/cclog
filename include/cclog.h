#ifndef CCLOG_H
#define CCLOG_H

#include <algorithm>
#include <chrono>
#include <ctime>
#include <fstream>
#include <iomanip>
#include <map>
#include <memory>
#include <ostream>
#include <sstream>
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
	virtual bool match(Tag const& source) const = 0;

protected:
	Tag (std::string const& key, std::string const& value) : key_(key), text_(key + "=\"" + value + "\"") {}
    Tag (std::string const& key, int value) : key_(key), text_(key + "=" + std::to_string(value)) {}
    Tag (std::string const& key, long long value) : key_(key), text_(key + "=" + std::to_string(value)) {}
    Tag (std::string const& key, double value) : key_(key), text_(key + "=" + std::to_string(value)) {}
    Tag (std::string const & key, bool value) : key_(key), text_(key + "=" + (value?"true":"false")){}

private:
	std::string key_;
	std::string const text_;
};

inline std::string to_string(Tag const& tag) {
	return tag.get_text();
}

inline std::ostream& operator<<(std::ostream&& os, Tag const& tag) {
	os << to_string(tag);
	return os;
}

enum class TagOperation {
	kNone,
	kEqual,
	kLessThan,
	kLessThanOrEqual,
	kGreaterThan,
	kGreaterThanOrEqual
};

template<typename T, typename ValueType>
class TagType : public Tag {
public:
	std::unique_ptr<Tag> clone() const override {
		return std::unique_ptr<Tag>(new T(*static_cast<T const*>(this)));
	}

	bool match(Tag const& source) const override {
		if(get_key() != source.get_key()) {
			return false;
		}
		TagType const& source_cast = static_cast<TagType const&>(source);
		if(operation_ == TagOperation::kNone) {
			switch(source_cast.operation_) {
				case TagOperation::kNone:
					return value_ == source_cast.value_;
				default:
					return compare_tag_types(value_, source_cast.operation_, source_cast.value_);
			}
		}
		switch(source_cast.operation_) {
			case TagOperation::kNone:
				return compare_tag_types(source_cast.value_, operation_, value_);
			default:
				return false;
		}
	}

	ValueType get_value() const {
		return value_;
	}

protected:
	TagType(ValueType const& value, TagOperation operation) : Tag(T::key, value), value_(value), operation_(operation) {}
	virtual bool compare_tag_types(ValueType const& value, TagOperation operation, ValueType const& criteria) const {
		return false;
	}
    ValueType value_;
    TagOperation operation_;
};

template<typename T>
class StringTagType : public TagType<T, std::string> {
protected:
	StringTagType(std::string const& value, TagOperation operation) : TagType<T, std::string>(value, operation) {}
	bool compare_tag_types(std::string const& value, TagOperation operation, std::string const& criteria) const override {
		int result = value.compare(criteria);
		switch(operation) {
			case TagOperation::kEqual:
				return result == 0;
			case TagOperation::kLessThan:
				return result == -1;
			case TagOperation::kLessThanOrEqual:
				return result == 0 || result == -1;
			case TagOperation::kGreaterThan:
				return result == 1;
			case TagOperation::kGreaterThanOrEqual:
				return result == 0 || result == 1;
			default:
				return false;
		}
	}
};

template<typename T>
class IntegerTagType : public TagType<T, int> {
protected:
	IntegerTagType(int const& value, TagOperation operation) : TagType<T, int>(value, operation) {}
	bool compare_tag_types(int const& value, TagOperation operation, int const& criteria) const override {
		switch(operation) {
			case TagOperation::kEqual:
				return value == criteria;
			case TagOperation::kLessThan:
				return value < criteria ;
			case TagOperation::kLessThanOrEqual:
				return value <= criteria;
			case TagOperation::kGreaterThan:
				return value > criteria;
			case TagOperation::kGreaterThanOrEqual:
				return value >= criteria;
			default:
				return false;
		}
	}
};

template<typename T>
class LongLongTagType : public TagType<T, long long> {
protected:
	LongLongTagType(long long const& value, TagOperation operation) : TagType<T, long long>(value, operation) {}
	bool compare_tag_types(long long const& value, TagOperation operation, long long const& criteria) const override {
		switch(operation) {
			case TagOperation::kEqual:
				return value == criteria;
			case TagOperation::kLessThan:
				return value < criteria ;
			case TagOperation::kLessThanOrEqual:
				return value <= criteria;
			case TagOperation::kGreaterThan:
				return value > criteria;
			case TagOperation::kGreaterThanOrEqual:
				return value >= criteria;
			default:
				return false;
		}
	}
};

template<typename T>
class DoubleTagType : public TagType<T, double> {
protected:
	DoubleTagType(double const& value, TagOperation operation) : TagType<T, double>(value, operation) {}
	bool compare_tag_types(double const& value, TagOperation operation, double const& criteria) const override {
		switch(operation) {
			case TagOperation::kEqual:
				return value == criteria;
			case TagOperation::kLessThan:
				return value < criteria ;
			case TagOperation::kLessThanOrEqual:
				return value <= criteria;
			case TagOperation::kGreaterThan:
				return value > criteria;
			case TagOperation::kGreaterThanOrEqual:
				return value >= criteria;
			default:
				return false;
		}
	}
};

template<typename T>
class BoolTagType : public TagType<T, bool> {
protected:
	BoolTagType(bool const& value, TagOperation operation) : TagType<T, bool>(value, operation) {}
	bool compare_tag_types(bool const& value, TagOperation operation, bool const& criteria) const override {
		switch(operation) {
			case TagOperation::kEqual:
				return value == criteria;
			default:
				return false;
		}
	}
};

class LogLevel : public StringTagType<LogLevel> {
public:
	static constexpr char key[] = "log_level";
	LogLevel(std::string const& value, TagOperation operation = TagOperation::kNone) : StringTagType(value, operation) {}
};

inline std::map<std::string, std::unique_ptr<Tag>>& get_default_tags() {
	static std::map<std::string, std::unique_ptr<Tag>> tags;
	return tags;
}

inline void add_default_tag(Tag const& tag) {
	auto& tags = get_default_tags();
	tags[tag.get_key()] = tag.clone();
}

class FilterClause {
public:
	std::vector<std::unique_ptr<Tag>> normal_literals;
	std::vector<std::unique_ptr<Tag>> inverted_literals;
};

inline std::map<int, FilterClause>& get_filter_clauses() {
	static std::map<int, FilterClause> clauses;
	return clauses;
}

inline int create_filter_clause() {
	static int current_id = 0;
	++current_id;
	auto& clauses = get_filter_clauses();
	clauses[current_id] = FilterClause();
	return current_id;
}

inline void add_filter_literal(int filter_id, Tag const& tag, bool normal = true) {
	auto& clauses = get_filter_clauses();
	if(clauses.contains(filter_id)) {
		if(normal) {
			clauses[filter_id].normal_literals.push_back(tag.clone());
		} else {
			clauses[filter_id].inverted_literals.push_back(tag.clone());
		}
	}
}

inline void clear_filter_clause(int filter_id) {
	auto& clauses = get_filter_clauses();
	clauses.erase(filter_id);
}

class LogStream : public std::stringstream {
public:
	LogStream(std::string const& filename, std::ios_base::openmode mode = ios_base::app) : proceed_(true), fs_(filename, mode) {}
	LogStream(LogStream const& source) = delete;
	LogStream(LogStream&& source) : std::stringstream(std::move(source)), proceed_(source.proceed_), fs_(std::move(source.fs_)) {}
	~LogStream() {
		if(!proceed_) {
			return;
		}
		fs_ << this->str();
		fs_ << '\n';
	}

	LogStream& operator=(LogStream const& rhs) = delete;
	LogStream& operator=(LogStream&& rhs) = delete;
	
	void ignore() {
		proceed_ = false;
	}
private:
	bool proceed_;
	std::fstream fs_;
};

inline LogStream log(std::vector<Tag const*> tags = {}) {
	auto const time = std::chrono::system_clock::now();
	std::time_t const start_time = std::chrono::system_clock::to_time_t(time);
	auto const ms = duration_cast<std::chrono::milliseconds>(time.time_since_epoch()) % 1000;

	LogStream ls("cclog.log");
	ls << std::put_time(std::gmtime(&start_time), "%Y-%m-%dT%H:%M:%S.")
		<< std::setw(3) << std::setfill('0') << std::to_string(ms.count());
	
	std::map<std::string, Tag const*> active_tags;
    for (auto const& default_tag: get_default_tags())
    {
        active_tags[default_tag.first] = default_tag.second.get();
    }
    for (auto const& tag: tags)
    {
        active_tags[tag->get_key()] = tag;
    }
    for (auto const& active_entry: active_tags)
    {
        ls << " " << active_entry.second->get_text();
    }
    ls << " ";

	bool proceed = true;
	for (auto const & clause: get_filter_clauses()) {
		proceed = false;
		bool all_literals_match = true;
		for(auto const& normal : clause.second.normal_literals) {
			if(!active_tags.contains(normal->get_key())) {
				all_literals_match = false;
				break;
			}
			if(!active_tags[normal->get_key()]->match(*normal)) {
				all_literals_match = false;
				break;
			}
		}
		if(!all_literals_match) {
			continue;
		}
		for(auto const& inverted : clause.second.inverted_literals) {
			if(active_tags.contains(inverted->get_key())) {
				if(active_tags[inverted->get_key()]->match(*inverted)) {
					all_literals_match = false;
				}
				break;
			}
		}
		if(all_literals_match) {
			proceed = true;
			break;
		}
	}
	if(!proceed) {
		ls.ignore();
	}
	return ls;
}

inline auto log(Tag const& tag1) {
    return log({&tag1});
}

inline auto log(Tag const& tag1, Tag const& tag2) {
    return log({&tag1, &tag2});
}

inline auto log(Tag const& tag1, Tag const& tag2, Tag const& tag3){
	return log({&tag1, &tag2, &tag3});
}

class Color : public StringTagType<Color> {
public:
	static constexpr char key[] = "color";
	Color(std::string const& value, TagOperation operation = TagOperation::kNone) : StringTagType(value, operation) {}
};

class Size : public StringTagType<Size> {
public:
	static constexpr char key[] = "size";
	Size(std::string const& value, TagOperation operation = TagOperation::kNone) : StringTagType(value, operation) {}
};

class Count : public IntegerTagType<Count> {
public:
	static constexpr char key[] = "count";
	Count(int value, TagOperation operation = TagOperation::kNone) : IntegerTagType(value, operation) {}
};

class Identity : public LongLongTagType<Identity> {
public:
	static constexpr char key[] = "id";
	Identity(long long value, TagOperation operation = TagOperation::kNone) : LongLongTagType(value, operation) {}
};

class Scale : public DoubleTagType<Scale> {
public:
	static constexpr char key[] = "scale";
	Scale(double value, TagOperation operation = TagOperation::kNone) : DoubleTagType(value, operation) {}
};

class CacheHit : public BoolTagType<CacheHit> {
public:
	static constexpr char key[] = "cache_hit";
	CacheHit(bool value, TagOperation operation = TagOperation::kNone) : BoolTagType(value, operation) {}
};

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
inline cclog::CacheHit cache_hit(true);
inline cclog::CacheHit cache_miss(false);

#endif // CCLOG_H
