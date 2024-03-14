#include "cclog.h"
#include "utility.h"
#include "ccunit.h"
#include <thread>

TEST("Log can be called from multiple threads") {
	std::vector<std::string> messages;
	for(int i = 0; i < 150; ++i) {
		std::string message = std::to_string(i);
		message += " thread-safe message ";
		message += Utility::random_string();
		messages.push_back(message);
	}

	std::vector<std::thread> threads;
	for(int c = 0; c < 3; ++c) {
		threads.emplace_back([c, &messages]() {
			int index_start = c * 50;
			for(int i = 0; i < 50; ++i) {
				cclog::log() << messages[index_start + i];
			}
		});
	}

	for(auto& t : threads) {
		t.join();
	}

	for(auto const& message : messages) {
		bool result = Utility::is_text_in_file(message, "cclog.log");
		CONFIRM_TRUE(result);
	}
}
