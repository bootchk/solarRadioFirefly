

void powerManagedMain();


int main() {
	// assert embedded system startup is done and calls main.

	// Choose corresponding build configuration
	powerManagedMain();	// never returns
	return 0;
}
