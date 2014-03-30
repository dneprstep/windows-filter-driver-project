#include "filter.h"

FILTERDATA FilterData;

WCHAR			sFile[256]			= {0};
#define POOL_TAG_TEMPORARY_BUFFER   'BpmT' 

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


	status = FltStartFiltering( FilterData.pFilter );

	if (!NT_SUCCESS( status )) 
	{
         FltUnregisterFilter( FilterData.pFilter );
         DbgPrint("MiniFilter:  Driver not started. ERROR FltStartFiltering - %08x\n", status);
         return status;
	}

	DbgPrint("MiniFilter:  Driver was started success.");

	wcscat(sFile,L"Eula.txt");

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
	NTSTATUS status;

	PFILE_BOTH_DIR_INFORMATION curEntry = NULL;
	PVOID tempBuf = NULL;
    PMDL newMdl = NULL;
	ULONG len=0;
	ULONG BufferPosition=0;
	ULONG prevOffset=0;



	if (!NT_SUCCESS( Data->IoStatus.Status ) ||
	    Data->Iopb->MinorFunction != IRP_MN_QUERY_DIRECTORY ||
		Data->Iopb->Parameters.DirectoryControl.QueryDirectory.DirectoryBuffer == NULL ||
		KeGetCurrentIrql() != PASSIVE_LEVEL
		)
	{
		return FLT_POSTOP_FINISHED_PROCESSING;
	}
	
	
	switch (Data->Iopb->Parameters.DirectoryControl.QueryDirectory.FileInformationClass)
	{
		case FileBothDirectoryInformation:
			{
				curEntry = (PFILE_BOTH_DIR_INFORMATION)Data->Iopb->Parameters.DirectoryControl.QueryDirectory.DirectoryBuffer;
				BufferPosition += curEntry->NextEntryOffset;

				for(;;)
				{				
					DbgPrint("Full name cur -  %wS*** size:%i\n",curEntry->FileName, curEntry->FileNameLength);

					if(wcscmp(sFile, curEntry->FileName)==0)
					{
						DbgPrint("sFile - %ws  - Compare - %wS",sFile, curEntry->FileName);
						
						if( curEntry->NextEntryOffset >0)
						{
							len=Data->Iopb->Parameters.DirectoryControl.QueryDirectory.Length - BufferPosition; 
							tempBuf = ExAllocatePoolWithTag( NonPagedPool, len, POOL_TAG_TEMPORARY_BUFFER);

							RtlCopyMemory(tempBuf, ((PUCHAR)curEntry + curEntry->NextEntryOffset), len); 
							RtlZeroMemory(curEntry, len + curEntry->NextEntryOffset); 
							RtlCopyMemory(curEntry, tempBuf, len);

							ExFreePoolWithTag(tempBuf, POOL_TAG_TEMPORARY_BUFFER); 
							FltSetCallbackDataDirty(Data); 
						}
						else
						{
							if(curEntry->NextEntryOffset ==0)
							{ 
								curEntry=(PFILE_BOTH_DIR_INFORMATION)((PCHAR) curEntry - prevOffset); 
								curEntry->NextEntryOffset=0; 
								FltSetCallbackDataDirty(Data); 
							} 
						}

					}
					
					
					
					if(curEntry->NextEntryOffset == 0)
					{
						break;
					}
					prevOffset = curEntry->NextEntryOffset;
					curEntry = (PFILE_BOTH_DIR_INFORMATION) ( (PUCHAR) curEntry + curEntry->NextEntryOffset );
					
				}
			}
			break;
		}
	
	return FLT_POSTOP_FINISHED_PROCESSING;
}
