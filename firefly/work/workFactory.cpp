
#include "workFactory.h"


// Implementation
#include <radioUseCase/radioUseCase.h>


// dummy workpayload represents version of code
// #define WORK_VERSION 34

MailContents WorkFactory::make() {


	MailContents value;

	// debugging

	// ID of sender
	//#include <services/system.h>
	// value = System::shortID();

	// xmit power of sender
	value = static_cast<unsigned char> (RadioUseCase::getXmitPower());

	// FUTURE: an amount of work

	return value;
}
