#include "PeteProcessor.h"

#ifdef PETE_MAC_OSX

#include <Gestalt.h>

bool Pete_IsAltiVecAvailable(void) {

	long cpuAttributes;
	bool hasAltiVec=false;
	OSErr err=Gestalt(gestaltPowerPCProcessorFeatures,&cpuAttributes); 

	if (noErr==err) {
		hasAltiVec=(1<< gestaltPowerPCHasVectorInstructions)&cpuAttributes;
	}

	return hasAltiVec;

}

#endif // PETE_MAC_OSX