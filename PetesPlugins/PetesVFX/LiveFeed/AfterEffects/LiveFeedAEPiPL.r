#ifndef MSWindows
#include "AE_General.r"
#endif
resource 'PiPL' (16000) {
	{	/* array properties: 12 elements */
		/* [1] */
		Kind {
			AEEffect
		},
		/* [2] */
		Name {
			"LiveFeed"
		},
		/* [3] */
		Category {
			"Pete's Plugins"
		},
		
#ifdef MSWindows
		CodeWin32X86 {"main"},
#else
		/* [4] */
		CodeCarbonPowerPC {
			0,
			0,
			""
		},
		CodePowerPC {
			0,
			0,
			""
		},
#endif

		/* [6] */
		AE_PiPL_Version {
			2,
			0
		},
		/* [7] */
		AE_Effect_Spec_Version {
			12,
			2
		},
		/* [8] */
		AE_Effect_Version {	
			655360
		},
		/* [9] */
		AE_Effect_Info_Flags {
			0
		},
		/* [10] */
		AE_Effect_Global_OutFlags {
			0
		},
		/* [11] */
		AE_Effect_Match_Name {
			"PETE LiveFeed"
		},
		/* [12] */
		AE_Reserved_Info {
			0
		}
	}
};

