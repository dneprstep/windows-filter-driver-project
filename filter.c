#include "main.h"

FILTERDATA FilterData;



ULONG GetNextEntryOffset(IN PVOID pData,IN FILE_INFORMATION_CLASS fileInfo)   
 {   
        ULONG result = 0;   
		if(pData == NULL)
			return result;
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
		if(pData == NULL)
			return;
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
        PWSTR  result = NULL; 
		
		if(pData == NULL)
			return result;
		
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

		if(pData == NULL)
			return result;

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

	////////////////////
	//parse hideMaskFile
	if(readHideFileMaskFromFile())
	{
		if(parseMasksString())
			printMaskList();
	}
	////////////////////
	
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

		PVOID prevEntry = NULL;
		PVOID curEntry = NULL;

		ULONG BufferPosition=0;
		ULONG moveEntryOffset=0;
		ULONG curEntryNextEntryOffset = 0;
		
		UNICODE_STRING curFileName;

		
		//get current directory buffer
		curEntry = Data->Iopb->Parameters.DirectoryControl.QueryDirectory.DirectoryBuffer;


		for(;;)
		{
			//calculate NextEntryOffset
			curEntryNextEntryOffset = GetNextEntryOffset(curEntry, fileInfo);
			
			//get current file name
			curFileName.Buffer = GetEntryFileName(curEntry, fileInfo);
			if(curFileName.Buffer == NULL)
				break;
			
			//get current file name length
			curFileName.Length = curFileName.MaximumLength = (USHORT)GetEntryFileNameLength(curEntry, fileInfo);

			//check if this file in mask list
			if(InHideList(&curFileName) == TRUE)
			{
				
				//if file not in the end of the buffer list
				if( curEntryNextEntryOffset > 0)
				{
                    BufferPosition = 
					((ULONG)curEntry) 
					- (ULONG)Data->Iopb->Parameters.DirectoryControl.QueryDirectory.DirectoryBuffer;
					
					//moved entry
                    moveEntryOffset = 
					(ULONG)Data->Iopb->Parameters.DirectoryControl.QueryDirectory.Length 
					- BufferPosition 
					- curEntryNextEntryOffset;   
				
					
					//move memory
					RtlMoveMemory
					(
						curEntry,
						(PVOID)((PCHAR)curEntry + curEntryNextEntryOffset),
						(ULONG)moveEntryOffset
					);

					continue;
				}
				else
				{

					//if single file in directory
					if(prevEntry == NULL)
					{
						Data->IoStatus.Status = STATUS_NO_MORE_FILES;
						status = FLT_POSTOP_FINISHED_PROCESSING;
					}
					else
					{
						//if last file in the list set NextEntryOffset = 0
						SetNextEntryOffset(prevEntry, fileInfo, 0);
					}
					break;
				}
			}
			
			//end of directory buffer
			if(GetNextEntryOffset(curEntry, fileInfo) == 0)
			{	break;	}
			
			prevEntry = curEntry;
			
			//next file in buffer
			curEntry = (PVOID)((PCHAR)curEntry + GetNextEntryOffset(curEntry, fileInfo));
		}

	}
	return status;
}