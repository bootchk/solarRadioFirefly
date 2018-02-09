

void testMain();
void powerManagedMain() __attribute__ ((noreturn));


// Doesn't return, but compiler requires it to return int
// so get a spurious warning "has a return statement."

//__attribute__((noreturn))

int main() {
	// assert embedded system startup is done and calls main.

	// Choose a main
	//testMain();	// test harness
	powerManagedMain();	// main for Firefly app
}
