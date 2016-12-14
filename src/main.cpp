

void powerManagedMain();


int main() {
	// assert embedded system startup is done and calls main.
	powerManagedMain();	// never returns
	return 0;
}
