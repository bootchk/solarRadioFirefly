
#include "workFactory.h"

#include <services/system.h>
#include <radioUseCase/radioUseCase.h>


// dummy workpayload represents version of code
// #define WORK_VERSION 34

WorkPayload WorkFactory::make() {


	WorkPayload value;

	// debugging

	// ID of sender
	// value = System::shortID();

	// xmit power of sender
	value = static_cast<unsigned char> (RadioUseCase::getXmitPower());

	// FUTURE: an amount of work

	return value;
}
