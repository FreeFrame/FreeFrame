#ifndef INCLUDE_FREEFRAMEHELPERS_H
#define INCLUDE_FREEFRAMEHELPERS_H

#include "FreeFrame.h"

typedef PlugInfoStruct*			(*FF_GetInfo_FuncPtr)				(void);
typedef int						(*FF_Initialise_FuncPtr)			(struct VideoInfoStruct* pVideoInfoStruct);
typedef int						(*FF_DeInitialise_FuncPtr)			(void);
typedef void					(*FF_ProcessFrame_FuncPtr)			(struct FrameInfoStruct* pFrame);
typedef int						(*FF_GetNumParameters_FuncPtr)		(void);
typedef char*					(*FF_GetParameterName_FuncPtr)		(int n);
typedef float					(*FF_GetParameterDefault_FuncPtr)	(int n);
typedef char*					(*FF_GetParameterDisplay_FuncPtr)	(int n);
typedef int						(*FF_SetParameter_FuncPtr)			(int n,float Value);
typedef float					(*FF_GetParameter_FuncPtr)			(int n);

typedef int 					(__stdcall *FF_Main_FuncPtr)		(int nFuncIndex,void* pArgument);

typedef struct FF_FunctionTableStruct {
	FF_GetInfo_FuncPtr				getInfo;
	FF_Initialise_FuncPtr			initialise;
	FF_DeInitialise_FuncPtr			deInitialise;
	FF_ProcessFrame_FuncPtr			processFrame;
	FF_GetNumParameters_FuncPtr		getNumParameters;
	FF_GetParameterName_FuncPtr		getParameterName;
	FF_GetParameterDefault_FuncPtr	getParameterDefault;
	FF_GetParameterDisplay_FuncPtr	getParameterDisplay;
	FF_SetParameter_FuncPtr			setParameter;
	FF_GetParameter_FuncPtr			getParameter;
};

inline void FF_FillFunctionTable(FF_FunctionTableStruct* poutFuncTable,FF_Main_FuncPtr pMain) {

	poutFuncTable->getInfo=(FF_GetInfo_FuncPtr)(pMain(FF_GETINFO));
	poutFuncTable->initialise=(FF_Initialise_FuncPtr)(pMain(FF_INITIALIZE));
	poutFuncTable->deInitialise=(FF_DeInitialise_FuncPtr)(pMain(FF_DEINITIALIZE));
	poutFuncTable->processFrame=(FF_ProcessFrame_FuncPtr)(pMain(FF_PROCESSFRAME));
	poutFuncTable->getNumParameters=(FF_GetNumParameters_FuncPtr)(pMain(FF_GETNUMPARAMETERS));
	poutFuncTable->getParameterName=(FF_GetParameterName_FuncPtr)(pMain(FF_GETPARAMETERNAME));
	poutFuncTable->getParameterDefault=(FF_GetParameterDefault_FuncPtr)(pMain(FF_GETPARAMETERDEFAULT));
	poutFuncTable->getParameterDisplay=(FF_GetParameterDisplay_FuncPtr)(pMain(FF_GETPARAMETERDISPLAY));
	poutFuncTable->setParameter=(FF_SetParameter_FuncPtr)(pMain(FF_SETPARAMETER));
	poutFuncTable->getParameter=(FF_GetParameter_FuncPtr)(pMain(FF_GETPARAMETER));

}


#endif // INCLUDE_PETEHELPERS_H