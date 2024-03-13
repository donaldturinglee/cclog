#include "utility.h"

#include <chrono>
#include <fstream>
#include <random>

std::string Utility::random_string() {
	static bool first_call{true};
	static std::mt19937 gen;
	if(first_call) {
		first_call = false;
		unsigned int seed = static_cast<int>(std::chrono::system_clock::now().time_since_epoch().count());
		gen.seed(seed);
	}
	std::uniform_int_distribution<std::mt19937::result_type>dist(1, 10000);
	return std::to_string(dist(gen));
}

bool Utility::is_text_in_file(std::string_view text, std::string_view filename, std::vector<std::string> const& wanted_tags, std::vector<std::string> const& unwanted_tags) {
	std::ifstream log_file(filename.data());
	std::string line;
	while(getline(log_file, line)) {
		if(line.find(text) != std::string::npos) {
			for(auto const& tag : wanted_tags) {
				if(line.find(tag) == std::string::npos) {
					return false;
				}
			}
			for(auto const& tag : unwanted_tags) {
				if(line.find(tag) != std::string::npos) {
					return false;
				}
			}
			return true;
		}
	}
	return false;
}
