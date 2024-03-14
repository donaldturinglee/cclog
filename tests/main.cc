#include <fstream>
#include <iostream>
#include "cclog.h"
#include "ccunit.h"

int main(int argc, char* argv[]) {
	
	cclog::FileOutput output_file("logs");
	//file.name_pattern() = "cclog-{}.log";
	//file.max_size() = 10'000'000;
	//file.rollover_count = 5;
	cclog::add_log_output(output_file);

	cclog::StreamOutput console_stream(std::cout);
	cclog::add_log_output(console_stream);

	//std::fstream fs("stream.log", std::ios::app);
	//cclog::StreamOutput file_stream(fs);
	//cclog::add_log_output(file_stream);

	cclog::add_default_tag(info);
	cclog::add_default_tag(green);

	return ccunit::run_tests(std::cout);
}
