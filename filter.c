#include "filter.h"

FILTERDATA FilterData;

UNICODE_STRING			sFile;

void HideFile(
	PFILE_BOTH_DIR_INFORMATION fileInfo,
	PFLT_CALLBACK_DATA Data
);

void HideFile(
	PFILE_BOTH_DIR_INFORMATION fileInfo,
	PFLT_CALLBACK_DATA Data

)
{
	NTSTATUS status;

	PFILE_BOTH_DIR_INFORMATION curEntry = fileInfo;
	PFILE_BOTH_DIR_INFORMATION prevEntry = NULL;
	ULONG len=0;
	ULONG BufferPosition=0;
	ULONG prevOffset=0;
	UNICODE_STRING curFileName;

				for(;;)
				{
					BufferPosition += curEntry->NextEntryOffset;
		
					curFileName.Buffer = curEntry -> FileName;
					curFileName.Length = curFileName.MaximumLength = (USHORT)curEntry->FileNameLength;
									

					if(FsRtlIsNameInExpression(&sFile, &curFileName, TRUE, 0))
					{
						if( curEntry->NextEntryOffset > 0)
						{			
							len=Data->Iopb->Parameters.DirectoryControl.QueryDirectory.Length - BufferPosition; 
							RtlMoveMemory(curEntry, 
							(PFILE_BOTH_DIR_INFORMATION)((PUCHAR)curEntry + curEntry->NextEntryOffset), 
							len + curEntry->NextEntryOffset);
						}	
						else
							if(curEntry->NextEntryOffset ==0)
							{
								RtlZeroMemory(curEntry, len + curEntry->NextEntryOffset); 
								((PFILE_BOTH_DIR_INFORMATION)((PCHAR) curEntry - prevOffset))->NextEntryOffset=0; 
								break;
							}					
						continue;
					}


					if(curEntry->NextEntryOffset == 0)
					{ break; }
					prevOffset = curEntry->NextEntryOffset;
					prevEntry = curEntry;
					curEntry = (PFILE_BOTH_DIR_INFORMATION) ( (PUCHAR) curEntry + curEntry->NextEntryOffset );
				}
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

	RtlInitUnicodeString(&sFile,L"*.TXT");

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
	PFILE_BOTH_DIR_INFORMATION prevEntry = NULL;
	PFILE_BOTH_DIR_INFORMATION tempBuf = NULL;
	ULONG len=0;
	ULONG BufferPosition=0;
	ULONG prevOffset=0;
	UNICODE_STRING curFileName;



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
				HideFile(curEntry, Data);
			}
			break;
			case FileDirectoryInformation:
			{
			} 
			break;
			case FileFullDirectoryInformation:
			{
			} 
			break;
			case FileIdBothDirectoryInformation:
			{
			} 
			break;
			case FileIdFullDirectoryInformation:
			{
			} 
			break;
			case FileNamesInformation:
			{
			} 
			break;

		}
	
	return FLT_POSTOP_FINISHED_PROCESSING;
}
