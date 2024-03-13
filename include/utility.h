#ifndef UTILITY_H
#define UTILITY_H

#include <string>
#include <string_view>

class Utility {
public:
	static std::string random_string();

	static bool is_text_in_file(std::string_view text, std::string_view filename);
};

#endif // UTILITY_H
