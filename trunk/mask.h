#ifndef __MASK_H__
#define __MASK_H__

#define MASK_POOL 'mP'

typedef struct _HIDEFILEMASKS
{
	UNICODE_STRING pMask;
	PVOID nextMask;
} HideFileMasks, *PHideFileMasks;



extern PHideFileMasks g_hMask;
/////////////////////////////////////////////////////////
//
//Prototypes for the mask routins
//
// implementation in mask.c
/////////////////////////////////////////////////////////

BOOLEAN InHideList(IN PUNICODE_STRING);

BOOLEAN readHideFileMaskFromFile();
BOOLEAN parseMasksString();

BOOLEAN Add_Mask(PANSI_STRING);

VOID cleanUpMasks();
VOID printMaskList();


#endif  __MASK_H__ 