#include "cclog.h"
#include "utility.h"
#include "ccunit.h"

TEST("Message can be tagged in log") {
	std::string message = "simple tag ";
	message += Utility::random_string();
	log(error) << message;
	std::string tagged_message = " log_level=\"error\" ";
	tagged_message += message;
	bool result = Utility::is_text_in_file(tagged_message, "cclog.log");
	CONFIRM_TRUE(result);
}

TEST("Log needs no namespace when used with LogLevel") {
	log(error) << "no namespace";
}
