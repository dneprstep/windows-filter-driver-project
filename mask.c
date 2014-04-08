#include "main.h"

const UNICODE_STRING rFileName = RTL_CONSTANT_STRING(L"\\??\\C:\\hideMaskFile.mask");

PHideFileMasks g_hMask = NULL;
PCHAR g_masksStr = NULL;


BOOLEAN InHideList(IN PUNICODE_STRING fileName)
{
	BOOLEAN status = FALSE;
	PHideFileMasks tmpMask = NULL;	
	
	if(fileName == NULL)
		return FALSE;
	
	////////////////////////
	//compare with mask list 
	if(g_hMask != NULL)
	{
		tmpMask = g_hMask;

		do
		{
			status = FsRtlIsNameInExpression(&tmpMask->pMask, fileName, TRUE, 0);
			if(status)
				break;
			if(tmpMask->nextMask == NULL)
				break;
			tmpMask = (PHideFileMasks)tmpMask->nextMask;
		} while(tmpMask != NULL);
	}
	return status;
}

VOID printMaskList()
{
//for Debug
//////////////////////////////////
//show all mask from the mask list

	PHideFileMasks tmpMask = NULL;
	if(g_hMask == NULL)
		return;
	tmpMask = g_hMask;
	do
	{		
		DbgPrint("Mask:%wZ\n",&tmpMask->pMask);
		tmpMask = tmpMask->nextMask;
	}	while(tmpMask != NULL);
}
BOOLEAN readHideFileMaskFromFile()
{
	NTSTATUS 				  status = FALSE;
	OBJECT_ATTRIBUTES 		  objectAttributes;
	IO_STATUS_BLOCK     	  ioStatus;
	HANDLE 					  FileHandle = NULL;
	FILE_STANDARD_INFORMATION fileInfo;

	int masksLen = 0;
	int byteOffset = 0;
	
	
	//////////////////////////////////////
	//InitializeAttributes
	InitializeObjectAttributes (&objectAttributes,
                        (PUNICODE_STRING)&rFileName,
                        OBJ_CASE_INSENSITIVE | OBJ_OPENIF,
                        NULL,
                        NULL );       
	///////////////////////////////////////
  
	//////////////////////////////////////
	//OpenFile
	status = ZwCreateFile(&FileHandle,
                          SYNCHRONIZE | FILE_READ_DATA,
                          &objectAttributes,
                          &ioStatus,
                          NULL, 
                          FILE_ATTRIBUTE_NORMAL,
                          FILE_SHARE_READ,
                          FILE_OPEN,
                          FILE_SYNCHRONOUS_IO_NONALERT,
                          NULL,     
                          0 );
	//if OpenFile success					  
	if(NT_SUCCESS( status ) )
	{
		DbgPrint("MiniFilter: SetHideFileMask open file %ls- status: %08x\n", rFileName.Buffer, status);
		
		/////////////////////////////////////////
		//File information
		status = ZwQueryInformationFile( FileHandle,
            &ioStatus,
            &fileInfo,
            sizeof(FILE_STANDARD_INFORMATION),
            FileStandardInformation);
		//if File information success
		if(NT_SUCCESS( status ) )
		{
			//file size
			byteOffset = fileInfo.EndOfFile.LowPart;
			
			//alloc pool 
			if(g_masksStr != NULL)
			{
				ExFreePoolWithTag (g_masksStr, MASK_POOL);
				g_masksStr = NULL;
			}
			g_masksStr = ExAllocatePoolWithTag(NonPagedPool, byteOffset+sizeof(PCHAR), MASK_POOL);
			if(g_masksStr == NULL)
			{
				ZwClose(FileHandle);
				return FALSE;
			}
			RtlZeroMemory(g_masksStr,byteOffset+sizeof(PCHAR));
			
			
			////////////////////////////////////
			//Read file
			status = ZwReadFile(FileHandle,
						NULL,
						NULL,
						NULL,
						&ioStatus,
						g_masksStr,byteOffset,
						0,
						NULL);
			//if Read file success
			if(NT_SUCCESS( status ) )
			{
				//close file
				ZwClose(FileHandle);
				
				//null-terminated string
				masksLen = byteOffset+sizeof(PCHAR)/sizeof(PCHAR);
				g_masksStr[masksLen] = '\0';			
				return TRUE;							
			}
			else
			{			
				DbgPrint("MiniFilter: SetHideFileMask ZwReadFile error - status: %08x\n", status);	
				ExFreePoolWithTag (g_masksStr, MASK_POOL);
				g_masksStr = NULL;
				ZwClose(FileHandle);
				return FALSE;
			}
		}
		else
		{
			DbgPrint("MiniFilter: SetHideFileMask ZwQueryInformationFile error - status: %08x\n", status);
			ZwClose(FileHandle);
			return FALSE;
		}
		
	}
	else
	{
		DbgPrint("MiniFilter: SetHideFileMask cant open file %ls- status: %08x\n", rFileName.Buffer, status);
		return FALSE;
	}
}

BOOLEAN Add_Mask(PANSI_STRING aStroka)
{
//////////////////////////////////////
//create structure(list) for our g_masksStr

	NTSTATUS status;
	
	if(aStroka == NULL)
		return FALSE;
	
	///////////////////
	//if list are empty
	if(g_hMask == NULL)
	{			
		//alloc memory
		g_hMask = ExAllocatePoolWithTag(NonPagedPool, sizeof(HideFileMasks), MASK_POOL);
		if(g_hMask == NULL)
		{
			DbgPrint("WinMiniFilter: Add_Mask - g_hMask - memory alloc ERROR");
			return FALSE;
		}
		RtlZeroMemory(g_hMask, sizeof(HideFileMasks));

		//convert ANSI to UNICODE
		status = RtlAnsiStringToUnicodeString(&g_hMask->pMask, aStroka,  TRUE);
		if (!NT_SUCCESS(status))
		{
			ExFreePoolWithTag (g_hMask, MASK_POOL);
			g_hMask = NULL;
			DbgPrint("MiniFilter: Add_Mask RtlAnsiStringToUnicodeString error  - status: %08x\n", status);
			return FALSE;
		}
		
		g_hMask->nextMask = NULL;
	}
	////////////////////////
	//not head add
	else
	{
	    PHideFileMasks currMask = NULL;
		PHideFileMasks tmpMask = g_hMask;

		//till the end of the list
		while(tmpMask->nextMask != NULL)
		{
			tmpMask = (PHideFileMasks)tmpMask->nextMask;
		}

		//alloc memory
		currMask = ExAllocatePoolWithTag(NonPagedPool, sizeof(HideFileMasks), MASK_POOL);
		if(currMask == NULL)
		{
			DbgPrint("WinMiniFilter: Add_Mask - tmpMask - memory alloc ERROR");
			return FALSE;
		}
		RtlZeroMemory(currMask, sizeof(HideFileMasks));
		
		//convert ANSI to UNICODE
		status = RtlAnsiStringToUnicodeString(&currMask->pMask, aStroka,  TRUE);
		if (!NT_SUCCESS(status))
		{
			ExFreePoolWithTag (currMask, MASK_POOL);
			DbgPrint("MiniFilter: Add_Mask RtlAnsiStringToUnicodeString error  - status: %08x\n", status);
			return FALSE;
		}
		
		//end of the list
		currMask->nextMask = NULL;
		tmpMask->nextMask = currMask;

	}

	return TRUE;
}

VOID cleanUpMasks()
{
	PHideFileMasks tmpMask = NULL;

	//Free global g_masksStr string    PCHAR g_masksStr
	if(g_masksStr != NULL)
		ExFreePoolWithTag (g_masksStr, MASK_POOL);
	////////////////////////////////////////


////////////////////////////
//clean up our mask list
	if(g_hMask == NULL)
		return;
	tmpMask = g_hMask;
	do
	{		
		DbgPrint("MiniFilter: cleanUpMasks tmpMask - %wZ -\n", &tmpMask->pMask);
		tmpMask = g_hMask->nextMask;
		
		RtlFreeUnicodeString(&g_hMask->pMask);
		ExFreePoolWithTag (g_hMask, MASK_POOL);		

		g_hMask = tmpMask;
	}	while(g_hMask != NULL);

	g_hMask = NULL;
}
BOOLEAN parseMasksString()
{	
///////////////////////////////////////
//parse g_masksStr to masks
//  separator ";"

	PCHAR 		pMasks = NULL;
	PCHAR 		tmpMaskStr = NULL;
	ANSI_STRING tmpAnsi;

	//if our g_masksStr exist
	if ( g_masksStr == NULL)
		return FALSE;
	
	pMasks = g_masksStr;
	tmpMaskStr = g_masksStr;

	//till the end of the string
	while (*pMasks !='\0')
	{		
		//separator
		if(*pMasks == ';')
		{
		        *pMasks = '\0';

				//Init AnsiString
				RtlInitAnsiString(&tmpAnsi, tmpMaskStr);
				
				//Add new mask to list
				if(Add_Mask(&tmpAnsi) == FALSE)
				{
					DbgPrint("WinMiniFilter: parseMasks - Add_Mask - ERROR");
				}
				
				tmpMaskStr = pMasks + 1;
		}
		
		pMasks++;
	}
	return TRUE;
}