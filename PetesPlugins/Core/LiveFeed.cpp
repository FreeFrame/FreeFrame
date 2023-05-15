/* Copyright (C) 2002 Pete Warden

Effect plugin

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU Lesser General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU Lesser General Public License for more details.

You should have received a copy of the Lesser GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.

*/

#include "LiveFeed.h"
#include "PeteHelpers.h"

#include <windows.h>
#include <atlbase.h>
#include <streams.h>
#include <qedit.h>

#include "ImageScale.h"

const int nMaxDevices=16;

static int g_nCaptureWidths[nMaxDevices];
static int g_nCaptureHeights[nMaxDevices];

struct SSampleGrabberCB : public ISampleGrabberCB {

	U32* m_pCaptureBuffer;

	STDMETHODIMP SampleCB( double SampleTime, IMediaSample * pSample ) {
        return NULL;
    }

    STDMETHODIMP BufferCB( double dblSampleTime, BYTE * pBuffer, long lBufferSize ) {

        if (m_pCaptureBuffer==NULL) {
			m_pCaptureBuffer = (U32*)Pete_NewHandle(lBufferSize);
		}

        if (m_pCaptureBuffer!=NULL) {
            memcpy(m_pCaptureBuffer, pBuffer, lBufferSize);
		}

        return NULL;
    }

    STDMETHODIMP_(U32) AddRef(void) {
		return 2;
	}

    STDMETHODIMP_(U32) Release(void) {
		return 1;
	}

    STDMETHODIMP QueryInterface(REFIID RefIID,void** ppOutputInterface) {

		if ((RefIID==IID_ISampleGrabberCB)||
			(RefIID==IID_IUnknown)) {

            *ppOutputInterface =
				(void *)static_cast<ISampleGrabberCB*>(this);

            return NOERROR;
        }

        return E_NOINTERFACE;
    }

};

SSampleGrabberCB g_CallbackObjects[nMaxDevices];

CComPtr< IGraphBuilder > g_pGraphs[nMaxDevices]={NULL};
CComPtr< ISampleGrabber > g_pGrabbers[nMaxDevices]={NULL};

bool g_bDeviceInitialised[nMaxDevices]={0};

void Pete_LiveFeed_ClearGraph(SPete_LiveFeed_Data* pInstanceData);
bool Pete_LiveFeed_InitGraph(SPete_LiveFeed_Data* pInstanceData);
IBaseFilter* Pete_LiveFeed_GetCaptureDevice(int nDeviceIndex);
void Pete_LiveFeed_Error(char* pMessage);
IPin* Pete_LiveFeed_GetFirstPin(IBaseFilter* pinFilter,bool bIsInput);

void Pete_LiveFeed_GetParameters(SPete_Parameter** poutpParameters,int* poutnParametersCount) {
	*poutpParameters=NULL;
	*poutnParametersCount=0;
}

int Pete_LiveFeed_Init(SPete_LiveFeed_Data* pInstanceData,int nWidth, int nHeight,int nDeviceIndex) {

	pInstanceData->nDeviceIndex=nDeviceIndex;

	Pete_LiveFeed_DeInit(pInstanceData);

	if (!g_bDeviceInitialised[nDeviceIndex]) {

		CoInitialize( NULL );

		bool bInitGraphSucceeded=Pete_LiveFeed_InitGraph(pInstanceData);
		if (!bInitGraphSucceeded) {
			Pete_LiveFeed_Error("PeteLiveFeed: Couldn't setup video capture in DirectX");
		}

		g_bDeviceInitialised[nDeviceIndex]=true;

	}

	pInstanceData->nWidth=nWidth;
	pInstanceData->nHeight=nHeight;

	return 1;

}

void Pete_LiveFeed_DeInit(SPete_LiveFeed_Data* pInstanceData) {

//	Pete_LiveFeed_ClearGraph(pInstanceData);

//	CoUninitialize();

}

void Pete_LiveFeed_Render(SPete_LiveFeed_Data* pInstanceData,SPete_LiveFeed_Settings* pSettings,U32* pSource,U32* pOutput) {

	const int nWidth=pInstanceData->nWidth;
	const int nHeight=pInstanceData->nHeight;
	const int nNumPixels=(nWidth*nHeight);
	const int nNumBytes=(nNumPixels*sizeof(U32));

	const int nDeviceIndex=pInstanceData->nDeviceIndex;

	const int nCaptureWidth=g_nCaptureWidths[nDeviceIndex];
	const int nCaptureHeight=g_nCaptureHeights[nDeviceIndex];
	U32* pCaptureBuffer=(U32*)(g_CallbackObjects[nDeviceIndex].m_pCaptureBuffer);

	if (pCaptureBuffer==NULL) {
		return;
	}

	if ((nWidth==nCaptureWidth)&&(nHeight==nCaptureHeight)) {

		memcpy(pOutput, pCaptureBuffer, nNumBytes);

	} else {

		Pete_ImageCopyAndScale(pCaptureBuffer,pOutput,
			nCaptureWidth,nCaptureHeight,
			nWidth,nHeight);

	}

}

void Pete_LiveFeed_ClearGraph(SPete_LiveFeed_Data* pInstanceData) {

	const int nDeviceIndex=pInstanceData->nDeviceIndex;

    if (g_pGraphs[nDeviceIndex]!=NULL) {

        CComQIPtr< IMediaControl, &IID_IMediaControl > pMediaControl=
			g_pGraphs[nDeviceIndex];

        if (pMediaControl!=NULL) {
            pMediaControl->Stop();
		}

        g_pGraphs[nDeviceIndex].Release( );
        g_pGrabbers[nDeviceIndex].Release( );

    }

}

bool Pete_LiveFeed_InitGraph(SPete_LiveFeed_Data* pInstanceData) {

	const int nDeviceIndex=pInstanceData->nDeviceIndex;

    HRESULT GraphCoCreateResult = g_pGraphs[nDeviceIndex].CoCreateInstance( CLSID_FilterGraph );
    if ((g_pGraphs[nDeviceIndex]==NULL)||
		(FAILED(GraphCoCreateResult))) {

        Pete_LiveFeed_Error("PeteLiveFeed: Graph creation failed");
		return false;

    }

    CComPtr< IBaseFilter > pCaptureDevice;
    pCaptureDevice=Pete_LiveFeed_GetCaptureDevice(nDeviceIndex);
    if (pCaptureDevice==NULL) {

        Pete_LiveFeed_Error("PeteLiveFeed: Failed to find a video capture device");
        return false;

    }

    HRESULT AddFilterResult=g_pGraphs[nDeviceIndex]->AddFilter(pCaptureDevice,L"Capture Device");
    if (FAILED(AddFilterResult)) {

        Pete_LiveFeed_Error("PeteLiveFeed: Adding capture device to graph failed");
        return false;

    }

    HRESULT GrabberCoCreateResult=g_pGrabbers[nDeviceIndex].CoCreateInstance(CLSID_SampleGrabber);
    if ((g_pGrabbers[nDeviceIndex]==NULL)||FAILED(GrabberCoCreateResult)) {

		// Pete-This needs QEdit.dll, is this standard?
        Pete_LiveFeed_Error("PeteLiveFeed: Didn't find SampleGrabber");
        return false;

    }

    CMediaType VideoType;
    VideoType.SetType( &MEDIATYPE_Video );
    VideoType.SetSubtype( &MEDIASUBTYPE_RGB32 );
    HRESULT SetMediaResult= g_pGrabbers[nDeviceIndex]->SetMediaType( &VideoType );
    if (FAILED(SetMediaResult)) {

        Pete_LiveFeed_Error("PeteLiveFeed: SetMediaType() failed");
        return false;

    }


    CComQIPtr< IBaseFilter, &IID_IBaseFilter > pGrabberBaseFilter(g_pGrabbers[nDeviceIndex]);
    HRESULT GrabberAddFilterResult=g_pGraphs[nDeviceIndex]->AddFilter(
		pGrabberBaseFilter,L"Grabber");
    if (FAILED(GrabberAddFilterResult)) {

        Pete_LiveFeed_Error("PeteLiveFeed: Grabber AddFilter() failed");
        return false;

    }

    IPin* pCaptureOutput=Pete_LiveFeed_GetFirstPin(pCaptureDevice,false);
    IPin* pGrabberInput=Pete_LiveFeed_GetFirstPin(pGrabberBaseFilter,true);

    HRESULT ConnectResult=g_pGraphs[nDeviceIndex]->Connect(pCaptureOutput,pGrabberInput);
    if(FAILED(ConnectResult)) {

        Pete_LiveFeed_Error("PeteLiveFeed: Failed to connect to capture device, is it already in use?");
        return false;

    }

    AM_MEDIA_TYPE MediaType;
    HRESULT GetConnectedMediaTypeResult=g_pGrabbers[nDeviceIndex]->GetConnectedMediaType(&MediaType);
    if (FAILED(GetConnectedMediaTypeResult)) {

        Pete_LiveFeed_Error("PeteLiveFeed: GetConnectedMediaType() failed");
        return false;

    }

    VIDEOINFOHEADER* pVideoInfoHeader=reinterpret_cast<VIDEOINFOHEADER*>(MediaType.pbFormat);
    g_nCaptureWidths[nDeviceIndex]=pVideoInfoHeader->bmiHeader.biWidth;
    g_nCaptureHeights[nDeviceIndex]=pVideoInfoHeader->bmiHeader.biHeight;
    FreeMediaType(MediaType);

    g_pGrabbers[nDeviceIndex]->SetBufferSamples(FALSE);
    g_pGrabbers[nDeviceIndex]->SetOneShot(FALSE);
    g_pGrabbers[nDeviceIndex]->SetCallback(&g_CallbackObjects[nDeviceIndex],1);

	g_CallbackObjects[nDeviceIndex].m_pCaptureBuffer=NULL;

    CComQIPtr< IMediaControl, &IID_IMediaControl > pGraphControl(g_pGraphs[nDeviceIndex]);
    HRESULT GraphControlRunResult=pGraphControl->Run();
    if (FAILED(GraphControlRunResult)) {

        Pete_LiveFeed_Error("PeteLiveFeed: Video capture graph Run() failed");
        return false;

    }

    return true;

}

IBaseFilter* Pete_LiveFeed_GetCaptureDevice(int nDeviceIndex) {

    CComPtr< ICreateDevEnum > pCreateDeviceEnum;
    pCreateDeviceEnum.CoCreateInstance(CLSID_SystemDeviceEnum);
    if (pCreateDeviceEnum==NULL) {
        return NULL;
	}

    CComPtr< IEnumMoniker > pEnumMoniker;
    pCreateDeviceEnum->CreateClassEnumerator(CLSID_VideoInputDeviceCategory,&pEnumMoniker,0);
    if (pEnumMoniker==NULL) {
        return NULL;
	}
    pEnumMoniker->Reset();

    IBaseFilter* pResult=NULL;

	int nCurrentDeviceIndex=0;

    while (true) {

        U32 Dummy=0;
        CComPtr< IMoniker > pMoniker;
        HRESULT NextResult=pEnumMoniker->Next(1,&pMoniker,&Dummy);
        if (NextResult!=S_OK) {
            break;
		}

        HRESULT BindToObjectResult=
			pMoniker->BindToObject(0,0,IID_IBaseFilter,(void**)&pResult);

        if (pResult!=NULL) {

			if (nCurrentDeviceIndex==nDeviceIndex) {
	            break;
			}

		}

		nCurrentDeviceIndex+=1;

    }

	return pResult;

}

void Pete_LiveFeed_Error(char* pMessage) {

#ifdef DEBUG
    MessageBox(NULL,pMessage,"Error!",(MB_OK|MB_TASKMODAL|MB_SETFOREGROUND));
#endif // DEBUG

}

IPin* Pete_LiveFeed_GetFirstPin(IBaseFilter* pinFilter,bool bIsInput) {

    CComPtr< IPin > pResult=NULL;

	PIN_DIRECTION RequiredDirection;
	if (bIsInput) {
		RequiredDirection=PINDIR_INPUT;
	} else {
		RequiredDirection=PINDIR_OUTPUT;
	}

    CComPtr< IEnumPins > pEnum;

    HRESULT EnumPinsResult = pinFilter->EnumPins(&pEnum);
    if(FAILED(EnumPinsResult)) {
        return NULL;
	}

    while(true) {

		U32 Dummy;
		IPin* pCurrentPin;
		HRESULT EnumNextResult=
			pEnum->Next(1, &pCurrentPin, &Dummy);

		if (EnumNextResult!=S_OK) {
			break;
		}

        PIN_DIRECTION PinDirection=(PIN_DIRECTION)3;
        pCurrentPin->QueryDirection(&PinDirection);

        if(PinDirection==RequiredDirection) {

			pResult=pCurrentPin;

            break;
        }

        pCurrentPin->Release();
    }

    return pResult;
}
