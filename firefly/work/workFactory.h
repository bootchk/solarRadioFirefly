
#pragma once

#include <platformTypes.h>   // WorkPayload


/*
 * Creates payloads for work.
 *
 * Work message carries a value.
 * This knows what the value represents.
 * Common use case for work value:
 * - it means nothing and is ignored by receiver, the fact of msg received is all that matters
 * - it is debug info for the sniffer.
 * - it tells how much work to do
 */
class WorkFactory {
public:
	static WorkPayload make();
};
