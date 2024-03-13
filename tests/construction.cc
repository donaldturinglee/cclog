#include "cclog.h"
#include "ccunit.h"
#include "utility.h"

TEST("Simple message can be logged") {
	std::string message = "simple ";
	message += Utility::random_string();

	cclog::log() << message << " with more text.";

	bool result = Utility::is_text_in_file(message, "cclog.log");

	CONFIRM_TRUE(result);
}

TEST("Complicated message can be logged") {
	std::string message = "complicated ";
	message += Utility::random_string();
	cclog::log() << message
				<< " double=" << 3.14
				<< " quoted=" << std::quoted("in quotes");
	bool result = Utility::is_text_in_file(message, "cclog.log");
	CONFIRM_TRUE(result);
}
