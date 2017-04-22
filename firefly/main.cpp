
#include <inttypes.h>


void powerManagedMain() __attribute__ ((noreturn));


// Doesn't return, but compiler requires it to return int
// so get a spurious warning "has a return statement."
__attribute__((noreturn))
int main() {
	// assert embedded system startup is done and calls main.

	// Write to odd address
	*(uint32_t *)(1) = 0;

	// Choose corresponding build configuration
	powerManagedMain();
}


#ifdef FOO
extern "C" {

_start() {
	main();
}
}
#endif
