#ifndef INCLUDE_PETECHAIN_H
#define INCLUDE_PETECHAIN_H

typedef struct SPete_ChainBinImageInfo {
	int m_nImageStartOffset;
	int m_nImageByteCount;
} SPete_ChainBinImageInfo;

typedef struct SPete_ChainBinHeader {
	int m_nPluginCount;
	int m_nImageTableOffset;
} SPete_ChainBinHeader;

/*-----------------------------------------*/
//
// All members with the suffix 'Offset' are
// byte offsets from the start of the data file.
// The base address of the loaded file must be
// added to get the correct pointer address.
//
// Binary data structure is:
//
// Header:
// SPete_ChainBinHeader
//
// Image Table:
// SPete_ChainBinImageInfo * nPluginCount
//
// Image Data:
// DLL image raw data * nPluginCount 
//
/*-----------------------------------------*/

#define CHAIN_DATA_RESOURCE_ID (2323)

#endif // INCLUDE_PETECHAIN_H