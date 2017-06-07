/*
 * Simplified version of C++ stdlib
 *
 * In build config, use -nostdlib.
 *
 * See "Working Without Standard Library" avabodh.com
 *
 * Also requires mods to the linker script.
 */

#ifdef NO_USE_STDLIB

void callConstructors();
extern "C" int main();
extern "C" void _start();
extern "C" void __cxa_pure_virtual ();
extern "C" void  __cxa_atexit();


extern "C" {
  extern void (*__CTOR_LIST__)() ;
  // extern void (*__DTOR_LIST__)() ;
}


void callConstructors()
{
	void (**constructor)() = &__CTOR_LIST__ ;
	int total = *(int *)constructor ;
	constructor++ ;
	while(total){
		(*constructor)() ;
		total-- ;
		constructor++ ;
	}
}

extern "C" {

// gcc_startup.S has start code that initializes vars
void _start() {
	callConstructors();
	main();
	// never returns, no need callDestructors
}


// No heap => no new(), delete()


extern "C" void __cxa_pure_virtual (){}
void * __dso_handle=0;
extern "C" void  __cxa_atexit(){}

}

#endif
