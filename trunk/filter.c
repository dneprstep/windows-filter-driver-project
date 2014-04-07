#include "filter.h"

FILTERDATA FilterData;
PHideFileMasks hMask = NULL;
PCHAR masks = NULL;

const UNICODE_STRING HIDE_ALL_FILES = RTL_CONSTANT_STRING(L"*.");

int masksLen;

const UNICODE_STRING rFileName = RTL_CONSTANT_STRING(L"\\??\\C:\\hideMaskFile");

BOOLEAN InHideList(IN PUNICODE_STRING fileName)
{
	BOOLEAN status=FALSE;
	PHideFileMasks tmpMask;	
//	DbgPrint("Before fileName:%wZ - hMask - %wZ", fileName, &hMask->pMask);

	if(hMask != NULL)
	{
		tmpMask = hMask;
	//	DbgPrint("Before fileName:%wZ - hMask - %wZ", fileName, &tmpMask->pMask);


		while(tmpMask->nextMask != NULL)
		{
	//		DbgPrint(" Inside fileName:%wZ - mask - %wZ", fileName, &tmpMask->pMask);
			status = FsRtlIsNameInExpression(&tmpMask->pMask, fileName, TRUE, 0);
			if(status)
				break;
			tmpMask = (PHideFileMasks)tmpMask->nextMask;
		} 
	}
	else
	{
		status = FsRtlIsNameInExpression((PUNICODE_STRING)&HIDE_ALL_FILES, fileName, TRUE, 0);
	}
	return status;
}
VOID maskList()
{
	PHideFileMasks tmpMask;
	if(hMask == NULL)
		return;
	tmpMask = hMask;
	do
	{		
		DbgPrint("tmpMask:%wZ\n",&tmpMask->pMask);
		tmpMask = tmpMask->nextMask;
	}	while(tmpMask != NULL);
}
BOOLEAN readHideFileMaskFromFile()
{
	NTSTATUS status;
	OBJECT_ATTRIBUTES objectAttributes;
	IO_STATUS_BLOCK     ioStatus;
	HANDLE FileHandle;
	FILE_STANDARD_INFORMATION fileInfo;
	
	int byteOffset;

	ANSI_STRING temp;
	
	
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
			masks = ExAllocatePool(NonPagedPool, byteOffset+1);
			if(masks == NULL)
			{
				ZwClose(FileHandle);
				return FALSE;
			}
			RtlZeroMemory(masks,byteOffset+1);
			
			
			////////////////////////////////////
			//Read file
			status = ZwReadFile(FileHandle,
						NULL,
						NULL,
						NULL,
						&ioStatus,
						masks,byteOffset,
						0,
						NULL);
			//if Read file success
			if(NT_SUCCESS( status ) )
			{
				//close file
				ZwClose(FileHandle);
				
				//null-terminated string
				masksLen = byteOffset+1/sizeof(PCHAR);
	//			DbgPrint("masksLen = %i\n",masksLen);
				masks[masksLen] = '\0';
	//			DbgPrint("MiniFilter: masks - %s -\n", masks);	
			
				return TRUE;
								
			}
			else
			{			
				DbgPrint("MiniFilter: SetHideFileMask ZwReadFile error - status: %08x\n", status);	
				ExFreePool(masks);
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
	NTSTATUS status;
	
	if(aStroka == NULL)
		return FALSE;

	if(hMask == NULL)
	{			
		hMask = ExAllocatePool(NonPagedPool, sizeof(HideFileMasks));
		if(hMask == NULL)
		{
			DbgPrint("WinMiniFilter: Add_Mask - hMask - memory alloc ERROR");
			return FALSE;
		}
		RtlZeroMemory(hMask, sizeof(HideFileMasks));

		status = RtlAnsiStringToUnicodeString(&hMask->pMask, aStroka,  TRUE);
		if (!NT_SUCCESS(status))
		{
			ExFreePool(hMask);
			DbgPrint("MiniFilter: Add_Mask RtlAnsiStringToUnicodeString error  - status: %08x\n", status);
			return FALSE;
		}
		
		hMask->nextMask = NULL;
//		DbgPrint("MiniFilter: Add_Mask hMask - %wZ -\n", &hMask->pMask);
	}
	else
	{	
		PHideFileMasks tmpMask = hMask;

		while(tmpMask->nextMask != NULL)
		{
			tmpMask = (PHideFileMasks)tmpMask->nextMask;
		}


		tmpMask->nextMask = ExAllocatePool(NonPagedPool, sizeof(HideFileMasks));
		if(tmpMask == NULL)
		{
			DbgPrint("WinMiniFilter: Add_Mask - tmpMask - memory alloc ERROR");
			return FALSE;
		}
		RtlZeroMemory(tmpMask->nextMask, sizeof(HideFileMasks));
		


		tmpMask = tmpMask->nextMask;

		status = RtlAnsiStringToUnicodeString(&tmpMask->pMask, aStroka,  TRUE);
		if (!NT_SUCCESS(status))
		{
			ExFreePool(tmpMask->nextMask);
			DbgPrint("MiniFilter: Add_Mask RtlAnsiStringToUnicodeString error  - status: %08x\n", status);
			return FALSE;
		}

		tmpMask->nextMask = NULL;

//		DbgPrint("MiniFilter: Add_Mask tmpMask - %wZ -\n", &tmpMask->pMask);
	}


	return TRUE;
}

VOID cleanUpMasks()
{
	PHideFileMasks tmpMask;
	if(hMask == NULL)
		return;
	tmpMask = hMask;
	do
	{		
		DbgPrint("MiniFilter: cleanUpMasks tmpMask - %wZ -\n", &tmpMask->pMask);
		tmpMask = hMask->nextMask;
		RtlFreeUnicodeString(&hMask->pMask);
		ExFreePool(hMask);		

		hMask = tmpMask;
	}	while(tmpMask != NULL);

	hMask = NULL;
}
BOOLEAN parseMasksString()
{	
	PCHAR pMasks;
	PCHAR tmpMaskStr = NULL;
	ANSI_STRING tmpAnsi;
	int count = 0;

	if ( masks == NULL)
		return FALSE;
	
	pMasks = masks;
//	DbgPrint("parseMasks pMasks  - %s -",pMasks);

	while (*pMasks !='\0')
	{		
//		DbgPrint("pMasks char[%i] = %c -",count, *pMasks);
		count++;
		if(*pMasks == ';')
		{
			int poolSize = count*sizeof(PCHAR);
			tmpMaskStr = ExAllocatePool(NonPagedPool, poolSize);
			if(tmpMaskStr == NULL)
			{
				DbgPrint("WinMiniFilter: parseMasks - tmpMaskStr - memory alloc ERROR");
				return FALSE;
			}
			RtlZeroMemory(tmpMaskStr, poolSize);
			
			RtlCopyMemory(tmpMaskStr, pMasks-count+1, count*sizeof(PCHAR));
			tmpMaskStr[poolSize/sizeof(PCHAR)-1]='\0';
			
			RtlInitAnsiString(&tmpAnsi, tmpMaskStr);
//			DbgPrint("ANSI_STRING - %Z - \n",  &tmpAnsi);
			Add_Mask(&tmpAnsi);
			
			

//			DbgPrint("tmpMaskStr - ; - %s",tmpMaskStr);
			ExFreePool(tmpMaskStr);
			tmpMaskStr = NULL;
			count = 0;
		}
		
		pMasks++;
	}
	return TRUE;
}

ULONG GetNextEntryOffset(IN PVOID pData,IN FILE_INFORMATION_CLASS fileInfo)   
 {   
        ULONG result = 0;   
        switch(fileInfo)
		{   
			case FileDirectoryInformation:   
				result = ((PFILE_DIRECTORY_INFORMATION)pData)->NextEntryOffset;   
				break;   
			case FileFullDirectoryInformation:   
				result = ((PFILE_FULL_DIR_INFORMATION)pData)->NextEntryOffset;   
				break;   
			case FileIdFullDirectoryInformation:   
				result = ((PFILE_ID_FULL_DIR_INFORMATION)pData)->NextEntryOffset;   
				break;   
			case FileBothDirectoryInformation:   
				result = ((PFILE_BOTH_DIR_INFORMATION)pData)->NextEntryOffset;   
				break;   
			case FileIdBothDirectoryInformation:   
				result = ((PFILE_ID_BOTH_DIR_INFORMATION)pData)->NextEntryOffset;   
				break;   
			case FileNamesInformation:   
				result = ((PFILE_NAMES_INFORMATION)pData)->NextEntryOffset;   
				break;   
        }   
        return result;   
 }   
 
VOID SetNextEntryOffset(IN PVOID pData,IN FILE_INFORMATION_CLASS fileInfo,IN ULONG Offset)   
{   
        switch(fileInfo)
		{   
			case FileDirectoryInformation:   
				((PFILE_DIRECTORY_INFORMATION)pData)->NextEntryOffset = Offset;   
				break;   
			case FileFullDirectoryInformation:   
				((PFILE_FULL_DIR_INFORMATION)pData)->NextEntryOffset = Offset;   
				break;   
			case FileIdFullDirectoryInformation:   
				((PFILE_ID_FULL_DIR_INFORMATION)pData)->NextEntryOffset = Offset;   
				break;   
			case FileBothDirectoryInformation:   
				((PFILE_BOTH_DIR_INFORMATION)pData)->NextEntryOffset = Offset;   
				break;   
			case FileIdBothDirectoryInformation:   
				((PFILE_ID_BOTH_DIR_INFORMATION)pData)->NextEntryOffset = Offset;   
				break;   
			case FileNamesInformation:   
				((PFILE_NAMES_INFORMATION)pData)->NextEntryOffset = Offset;   
				break;   
        }   
}   

PWSTR  GetEntryFileName(IN PVOID pData,IN FILE_INFORMATION_CLASS fileInfo)   
{   
        PWSTR  result = 0;   
        switch(fileInfo)
		{   
			case FileDirectoryInformation:   
				result = ((PFILE_DIRECTORY_INFORMATION)pData)->FileName;   
				break;   
			case FileFullDirectoryInformation:   
				result =((PFILE_FULL_DIR_INFORMATION)pData)->FileName;   
				break;   
			case FileIdFullDirectoryInformation:   
				result =((PFILE_ID_FULL_DIR_INFORMATION)pData)->FileName;   
				break;   
			case FileBothDirectoryInformation:   
				result =((PFILE_BOTH_DIR_INFORMATION)pData)->FileName;   
				break;   
			case FileIdBothDirectoryInformation:   
				result =((PFILE_ID_BOTH_DIR_INFORMATION)pData)->FileName;   
				break;   
			case FileNamesInformation:   
				result =((PFILE_NAMES_INFORMATION)pData)->FileName;   
				break;   
        }   
        return result;   
}   

ULONG GetEntryFileNameLength (IN PVOID pData,IN FILE_INFORMATION_CLASS fileInfo) 
{
        ULONG result = 0;   

        switch(fileInfo)
		{   
			case FileDirectoryInformation:   
				result = ((PFILE_DIRECTORY_INFORMATION)pData)->FileNameLength;   
				break;   
			case FileFullDirectoryInformation:   
				result =((PFILE_FULL_DIR_INFORMATION)pData)->FileNameLength;   
				break;   
			case FileIdFullDirectoryInformation:   
				result =((PFILE_ID_FULL_DIR_INFORMATION)pData)->FileNameLength;   
				break;   
			case FileBothDirectoryInformation:   
				result =((PFILE_BOTH_DIR_INFORMATION)pData)->FileNameLength;   
				break;   
			case FileIdBothDirectoryInformation:   
				result =((PFILE_ID_BOTH_DIR_INFORMATION)pData)->FileNameLength;   
				break;   
			case FileNamesInformation:   
				result =((PFILE_NAMES_INFORMATION)pData)->FileNameLength;   
				break;   
        }   
	return result;
}
 
 
 


const FLT_OPERATION_REGISTRATION Callbacks[] = 
{
//  Calbacks routins for our filter
	{ IRP_MJ_DIRECTORY_CONTROL,
      0,
      NULL,
      FilterPostDirectoryControl 
	},

    { IRP_MJ_OPERATION_END }
};

CONST FLT_REGISTRATION FilterRegistration = 
{

    sizeof( FLT_REGISTRATION ),         //  Size
    FLT_REGISTRATION_VERSION,           //  Version
    0,                                  //  Flags

    NULL,                               //  Context
    Callbacks,                          //  Operation callbacks
    FilterUnload,                     	//  FilterUnload
    NULL,                    		//  FilterInstanceSetup
    QueryTeardown,            //  InstanceQueryTeardown
    NULL,            //  InstanceTeardownStart
    NULL,            //  InstanceTeardownComplete

    NULL,           //  GenerateFileName
    NULL            //  NormalizeNameComponent
};

NTSTATUS
QueryTeardown (
    __in PCFLT_RELATED_OBJECTS FltObjects,
    __in FLT_INSTANCE_QUERY_TEARDOWN_FLAGS Flags
    )
{
    UNREFERENCED_PARAMETER( FltObjects );
    UNREFERENCED_PARAMETER( Flags );

    PAGED_CODE();

    return STATUS_SUCCESS;
}


NTSTATUS
FilterUnload (
    __in FLT_FILTER_UNLOAD_FLAGS Flags
    )
{
    UNREFERENCED_PARAMETER( Flags );

    //
    //  Unregister the filter
    //
	
	//Free global masks string    PCHAR masks
	if(masks != NULL)
		ExFreePool(masks);
	////////////////////////////////////////

	cleanUpMasks();
	
    FltUnregisterFilter( FilterData.pFilter );

    return STATUS_SUCCESS;
}

NTSTATUS
DriverEntry 
(
    __in PDRIVER_OBJECT DriverObject,
    __in PUNICODE_STRING RegistryPath
)
{

	NTSTATUS status;

	
	UNREFERENCED_PARAMETER( RegistryPath );

	DbgPrint("WinMiniFilter: Start.");

	status = FltRegisterFilter
	(
		DriverObject,
		&FilterRegistration,
		&FilterData.pFilter
	);

	if (!NT_SUCCESS(status))
	{
		DbgPrint("MiniFilter: Driver not started. ERROR FltRegisterFilter - %08x\n", status);
		return status;
	}

	if(readHideFileMaskFromFile())
	{
//		DbgPrint("MiniFilter: fileMaskInfo - %s -\n", masks);
		if(parseMasksString())
			maskList();
	}
	maskList();
	
	status = FltStartFiltering( FilterData.pFilter );

	if (!NT_SUCCESS( status )) 
	{
         FltUnregisterFilter( FilterData.pFilter );
         DbgPrint("MiniFilter:  Driver not started. ERROR FltStartFiltering - %08x\n", status);
         return status;
	}

	DbgPrint("MiniFilter:  Driver was started success.");

	return STATUS_SUCCESS;


}


FLT_POSTOP_CALLBACK_STATUS
	FilterPostDirectoryControl 
(
	__inout PFLT_CALLBACK_DATA Data,
	__in PCFLT_RELATED_OBJECTS FltObjects,
	__in_opt PVOID CompletionContext,
	__in FLT_POST_OPERATION_FLAGS Flags
)
{	
	FILE_INFORMATION_CLASS fileInfo;

	NTSTATUS status = FLT_POSTOP_FINISHED_PROCESSING;

	if (!NT_SUCCESS( Data->IoStatus.Status ) ||
	    Data->Iopb->MinorFunction != IRP_MN_QUERY_DIRECTORY ||
		Data->Iopb->Parameters.DirectoryControl.QueryDirectory.DirectoryBuffer == NULL ||
		KeGetCurrentIrql() != PASSIVE_LEVEL
		)
	{
		return FLT_POSTOP_FINISHED_PROCESSING;
	}
	
	
	fileInfo = Data->Iopb->Parameters.DirectoryControl.QueryDirectory.FileInformationClass;
	if
	(  
		fileInfo == FileDirectoryInformation ||   
		fileInfo == FileFullDirectoryInformation ||   
		fileInfo == FileIdFullDirectoryInformation ||   
		fileInfo == FileBothDirectoryInformation ||   
		fileInfo == FileIdBothDirectoryInformation ||   
		fileInfo == FileNamesInformation    
	)   
	{		
		PVOID curEntry = Data->Iopb->Parameters.DirectoryControl.QueryDirectory.DirectoryBuffer;

		PVOID prevEntry = NULL;

		ULONG len=0;
		ULONG BufferPosition=0;
		
		UNICODE_STRING curFileName;

		
		ULONG curEntryNextEntryOffset = 0;

		for(;;)
		{
			curEntryNextEntryOffset = GetNextEntryOffset(curEntry, fileInfo);
			BufferPosition += curEntryNextEntryOffset; //GetNextEntryOffset(curEntry, fileInfo); 	//curEntry->NextEntryOffset;

			curFileName.Buffer = GetEntryFileName(curEntry, fileInfo);	//curEntry -> FileName;
			curFileName.Length = curFileName.MaximumLength = (USHORT)GetEntryFileNameLength(curEntry, fileInfo);		//curEntry->FileNameLength;
			

			if(InHideList(&curFileName))
			{			
				DbgPrint("Hide File - %wZ -\n",&curFileName);

				if( curEntryNextEntryOffset > 0)
				{									
					len = Data->Iopb->Parameters.DirectoryControl.QueryDirectory.Length - BufferPosition; 
					
					RtlMoveMemory(curEntry, 
					(PVOID)((PUCHAR)curEntry + curEntryNextEntryOffset), 
					len + curEntryNextEntryOffset);		
					continue;
				}	
				else
				{				
					if(prevEntry == NULL)
					{
						status = STATUS_NO_MORE_FILES;
						break;
					}

					SetNextEntryOffset(prevEntry, fileInfo, 0);
					break;
				}					
			}

			if(curEntryNextEntryOffset == 0)
			{
				break;
			}
			prevEntry = curEntry;
			curEntry = (PVOID) ( (PUCHAR) curEntry + curEntryNextEntryOffset );
		}

	}
	return status;
}
