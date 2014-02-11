#include "Os.h"

void OsTest() {
	assemble("eos0.asm1", "eos0.obj1");
	execute("eos0.obj1", 10);
}


