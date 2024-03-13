#include "cclog.h"
#include "ccunit.h"
#include "utility.h"

TEST("Simple message can be logged") {
	std::string message = "simple ";
	message += Utility::random_string();

	cclog::log(message);

	bool result = Utility::is_text_in_file(message, "cclog.log");
	CONFIRM_TRUE(result);
}
