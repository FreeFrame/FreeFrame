#include "FreeFramePlugin.h"

TPluginInfoStruct pInfo;
TVideoInfoStruct tInfo;
DWORD bpp;



/*
 *  ==========================
 *   Function Code Table
 *   ==========================
 *   Code           Function
 *  =============  ============
 *  0              GetInfo
 *  1              Initialize
 *  2              DeInitialize
 *  3              ProcessFrame
 *  4              GetNumParameters
 * 5              GetParameterName
 * 6              GetParameterDefault
 * 7              GetParameterDisplay
 * 8              SetParameter
 * 9              GetParameter
 * 10             GetPluginCaps
 *  =============  ============
 */

void *pluginMain(DWORD functionCode,DWORD *pParam) {
  void *retval;

  switch(functionCode) {
  case 0:
    retval = (void *)GetInfo();
    break;
  case 1:
    retval = (void *)InitPlugin((TVideoInfoStruct *)pParam);
    break;
  case 2:
    retval = (void *)DeInitPlugin();
    break;
  case 3:
    retval = (void *)ProcessFrame(((ProcessFrameStruct *)pParam)->src,
			  ((ProcessFrameStruct *)pParam)->dst);
    break;
  case 4:
    retval = (void *)GetNumParameters();
    break;
  case 5:
    retval = (void *)GetParameterName((DWORD)pParam);
    break;
  case 6:
    retval = (void *)GetParameterDefault((DWORD)pParam);
    break;
  case 7:
    retval = (void *)GetParameterDisplay((DWORD)pParam);
    break;
  case 8:
    retval = (void *)SetParameter(((TSetParam *)pParam)->index,
				   ((TSetParam *)pParam)->value);
    break;
  case 9:
    retval = (void *)GetParameter((DWORD) pParam);
    break;
  case 10:
    retval = (void *)GetPluginCaps((DWORD) pParam);
    break;
  default:
    retval= (void *)0;
  }
  return retval;
}

#ifndef HAVE_INIT

DWORD InitPlugin(TVideoInfoStruct *tVidInfo) {
	tInfo.FrameWidth = tVidInfo->FrameWidth;
	tInfo.FrameHeight = tVidInfo->FrameHeight;
	bpp = (tVidInfo->BitDepth==0?16:tVidInfo->BitDepth==1?24:32);
	return 1;
}
#endif

#ifndef HAVE_DEINIT

DWORD DeInitPlugin() {
  /* Move along, nothing to see here */
};
#endif

#ifndef HAVE_GETINFO

TPluginInfoStruct *GetInfo() {
  char name[16];
  sprintf(name,"No Effect");
  pInfo.APIMajorVersion=0;
  pInfo.APIMinorVersion=1022;
  pInfo.PluginUniqueID[0]='D';
  pInfo.PluginUniqueID[1]='u';
  pInfo.PluginUniqueID[2]='m';
  pInfo.PluginUniqueID[3]='b';
  pInfo.PluginName= name;
  pInfo.PluginType = 0; /* Effect */
  return &pInfo;
}
#endif

#ifndef HAVE_PROCESSFRAME

DWORD ProcessFrame(DWORD *src,DWORD *dst) {
	memcpy(dst,src,sizeof(DWORD)*(bpp/8));
	return 0;
}
#endif

#ifndef HAVE_PARAMFUNCS

DWORD GetNumParameters() {

	return 0;

}

char *GetParameterName(DWORD i) {
  char *nope="None";
	return nope;
}

FloatStruct *GetParameterDefault(DWORD i) {
  static FloatStruct foo;
  foo.value = 0.0;
	return &foo;
}
char *GetParameterDisplay(DWORD i) {
  char *nope="Nothing";
  return nope;
}
DWORD SetParameter(DWORD i,float f) {

  return 0;
}
FloatStruct *GetParameter(DWORD i) {
  static FloatStruct foo;
  foo.value = 0.0;
  return &foo;
}
#endif

#ifdef HAVE_GETPLUGINCAPS

DWORD GetPluginCaps(DWORD i) {
  switch(i) {
  case 0:
    return 1;
    break;
  case 1:
    return 1;
    break;
  case 2:
    return 1;
    break;
  default:
    return 0;
  }
}
#endif


