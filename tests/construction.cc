#include "cclog.h"
#include "ccunit.h"

TEST("Simple message can be logged") {
	cclog::log("simple");
}
