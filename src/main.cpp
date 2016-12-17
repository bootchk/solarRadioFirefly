

void powerManagedMain();
void snifferMain();


int main() {
	// assert embedded system startup is done and calls main.

	// Choose one, and use corresponding build configuration
	//powerManagedMain();	// never returns
	snifferMain();
	return 0;
}
