#include "FreeFramePlugin.h"

TPluginInfoStruct pInfo;
TVideoInfoStruct tInfo;
DWORD bpp;

DWORD InitPlugin(TVideoInfoStruct *tVidInfo) {
	tInfo.FrameWidth = tVidInfo->FrameWidth;
	tInfo.FrameHeight = tVidInfo->FrameHeight;
	bpp = (tVidInfo->BitDepth==0?16:tVidInfo->BitDepth==1?24:32);
	return 1;
}

DWORD DeInitPlugin(TVideoInfoStruct *tVidInfo) {
  /* Move along, nothing to see here */
};

DWORD ProccessFrame(DWORD *src,DWORD *dst) {
	memcpy(dst,src,sizeof(DWORD)*(bpp/8));
}

DWORD GetNumParameters() {

	return 0;

}

DWORD GetParameterName(DWORD i) {
	return 0;
}

DWORD GetParameterDefault(DWORD i) {
	return 0;
}
DWORD GetParameterDisplay(DWORD i) {

}
DWORD SetParameter(DWORD i) {
}
DWORD GetParameter(DWORD i) {
}

