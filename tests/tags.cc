#include "cclog.h"
#include "utility.h"
#include "ccunit.h"

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
