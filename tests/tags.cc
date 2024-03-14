#include "cclog.h"
#include "utility.h"
#include "ccunit.h"

class TempFilterClause {
public:
	void setup() {
		id_ = cclog::create_filter_clause();
	}
	void teardown() {
		cclog::clear_filter_clause(id_);
	}
	int get_id() const {
		return id_;
	}
private:
	int id_;
};

TEST("Message can be tagged in log") {
	std::string message = "simple tag ";
	message += Utility::random_string();
	cclog::log(error) << message;

	std::string log_level_tag = " log_level=\"error\" ";
	std::string default_log_level_tag = " log_level=\"info\" ";
	bool result = Utility::is_text_in_file(message, "cclog.log", {log_level_tag}, {default_log_level_tag});
	CONFIRM_TRUE(result);
}

TEST("Log needs no namespace when used with LogLevel") {
	log(error) << "no namespace";
}

TEST("Default tags set in main appear in log") {
	std::string message = "default tag ";
	message += Utility::random_string();
	cclog::log() << message;

	std::string log_level_tag = " log_level=\"info\" ";
	std::string color_tag = " color=\"green\" ";
	bool result = Utility::is_text_in_file(message, "cclog.log", {log_level_tag, color_tag});
	CONFIRM_TRUE(result);
}

TEST("Multiple tags can be used in log") {
	std::string message = "multi tags ";
	message += Utility::random_string();

	cclog::log(debug, red, large) << message;

	std::string log_level_tag = " log_level=\"debug\" ";
	std::string color_tag = " color=\"red\" ";
	std::string size_tag = " size=\"large\" ";
	bool result = Utility::is_text_in_file(message, "cclog.log", {log_level_tag, color_tag, size_tag});
	CONFIRM_TRUE(result);
}

TEST("Tags can be streamed to log") {
	std::string message_base = " 1 type ";
	std::string message = message_base + Utility::random_string();
	cclog::log(info) << cclog::Count(1) << message;

	std::string count_tag = " count=1 ";
	bool result = Utility::is_text_in_file(message, "cclog.log", {count_tag});
	CONFIRM_TRUE(result);

	message_base = " 2 type ";
	message = message_base + Utility::random_string();
	cclog::log(info) << cclog::Identity(123456789012345) << message;

	std::string id_tag = " id=123456789012345 ";
	result = Utility::is_text_in_file(message, "cclog.log", {id_tag});
	CONFIRM_TRUE(result);

	message_base = " 3 type ";
	message = message_base + Utility::random_string();
	cclog::log(info) << cclog::Scale(1.5) << message;

	std::string scale_tag = " scale=1.500000 ";
	result = Utility::is_text_in_file(message, "cclog.log", {scale_tag});
	CONFIRM_TRUE(result);

	message_base = " 4 type ";
	message = message_base + Utility::random_string();
	cclog::log(info) << cache_miss << message;

	std::string cache_tag = " cache_hit=false ";
	result = Utility::is_text_in_file(message, "cclog.log", {cache_tag});
	CONFIRM_TRUE(result);
}

TEST("Tags can be used to filter message") {
	ccunit::SetupAndTeardown<TempFilterClause> filter;
	cclog::add_filter_literal(filter.get_id(), error);

	std::string message = "filter ";
	message += Utility::random_string();
	cclog::log(info) << message;
	bool result = Utility::is_text_in_file(message, "cclog.log");
	CONFIRM_FALSE(result);

	cclog::clear_filter_clause(filter.get_id());
	cclog::log(info) << message;
	result = Utility::is_text_in_file(message, "cclog.log");
	CONFIRM_TRUE(result);
}

TEST("Overridden default tag not used to filter messages") {
	ccunit::SetupAndTeardown<TempFilterClause> filter;
	cclog::add_filter_literal(filter.get_id(), info);

	std::string message = "override default ";
	message += Utility::random_string();
	cclog::log(debug) << message;

	bool result = Utility::is_text_in_file(message, "cclog.log");
	CONFIRM_FALSE(result);
}

TEST("Inverted tag can be used to filter messages") {
	ccunit::SetupAndTeardown<TempFilterClause> filter;
	cclog::add_filter_literal(filter.get_id(), green, false);

	std::string message = "inverted ";
	message += Utility::random_string();
	cclog::log(info) << message;

	bool result = Utility::is_text_in_file(message, "cclog.log");
	CONFIRM_FALSE(result);
}

TEST("Tag values can be used to filter messages") {
	ccunit::SetupAndTeardown<TempFilterClause> filter;
	cclog::add_filter_literal(filter.get_id(), cclog::Count(100, cclog::TagOperation::kGreaterThan));

	std::string message = "values ";
	message += Utility::random_string();

	cclog::log(cclog::Count(1)) << message;

	bool result = Utility::is_text_in_file(message, "cclog.log");
	CONFIRM_FALSE(result);

	cclog::log() << cclog::Count(101) << message;

	result = Utility::is_text_in_file(message, "cclog.log");
	CONFIRM_FALSE(result);


	cclog::log(cclog::Count(101)) << message;

	result = Utility::is_text_in_file(message, "cclog.log");
	CONFIRM_TRUE(result);
}
