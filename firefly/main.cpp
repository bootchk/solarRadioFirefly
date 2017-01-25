

void powerManagedMain() __attribute__ ((noreturn));


int main() {
	// assert embedded system startup is done and calls main.

	// Choose corresponding build configuration
	powerManagedMain();
	return 0;
}

#ifdef FOO
extern "C" {

_start() {
	main();
}
}
#endif
