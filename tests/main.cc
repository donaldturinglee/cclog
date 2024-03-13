#include <iostream>
#include "cclog.h"
#include "ccunit.h"

int main(int argc, char* argv[]) {
	
	cclog::add_default_tag(info);
	cclog::add_default_tag(green);

	return ccunit::run_tests(std::cout);
}
