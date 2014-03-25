#include "filter.h"

FILTERDATA FilterData;

const FLT_OPERATION_REGISTRATION Callbacks[] = 
{
//  Calbacks routins for our filter
    { IRP_MJ_CREATE,
      0,
      NULL,
      NULL},
	{ IRP_MJ_DIRECTORY_CONTROL,
      0,
      FilterPreDirectoryControl,
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
    FilterLoad,                    		//  FilterInstanceSetup
    NULL,            //  InstanceQueryTeardown
    NULL,            //  InstanceTeardownStart
    NULL,            //  InstanceTeardownComplete

    NULL,           //  GenerateFileName
    NULL            //  NormalizeNameComponent
};


NTSTATUS
FilterLoad 
(
    __in PCFLT_RELATED_OBJECTS FltObjects,
    __in FLT_INSTANCE_SETUP_FLAGS Flags,
    __in DEVICE_TYPE VolumeDeviceType,
    __in FLT_FILESYSTEM_TYPE VolumeFilesystemType
)
{
    UNREFERENCED_PARAMETER( FltObjects );
    UNREFERENCED_PARAMETER( Flags );
    UNREFERENCED_PARAMETER( VolumeFilesystemType );

    PAGED_CODE();

    ASSERT( FltObjects->Filter == FilterData.pFilter );

    //
    //  Don't attach to network volumes.
    //

    if (VolumeDeviceType == FILE_DEVICE_NETWORK_FILE_SYSTEM) 
	{

       return STATUS_FLT_DO_NOT_ATTACH;
    }

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
	return STATUS_SUCCESS;


}


FLT_PREOP_CALLBACK_STATUS
	FilterPreDirectoryControl (
	__inout PFLT_CALLBACK_DATA Data,
	__in PCFLT_RELATED_OBJECTS FltObjects,
	__deref_out_opt PVOID *CompletionContext
	)
{
	PFLT_FILE_NAME_INFORMATION nameInfo;

	
	if (!NT_SUCCESS( Data->IoStatus.Status )) 
	{
		return FLT_PREOP_SUCCESS_NO_CALLBACK;
	}
	if( KeGetCurrentIrql() != PASSIVE_LEVEL )
	{
		return FLT_PREOP_SUCCESS_NO_CALLBACK;
	}
	if( Data->Iopb->MinorFunction != IRP_MN_QUERY_DIRECTORY)
	{
		return FLT_PREOP_SUCCESS_NO_CALLBACK;
	}
	

	 if (Data->Iopb->Parameters.DirectoryControl.QueryDirectory.FileInformationClass == 
			FileBothDirectoryInformation)
    {
        PFILE_BOTH_DIR_INFORMATION curEntry = NULL;
        ULONG          nextEntryOffset;

		if(Data->Iopb->Parameters.DirectoryControl.QueryDirectory.DirectoryBuffer == NULL)
		{
			return FLT_PREOP_SUCCESS_NO_CALLBACK;		
		}
		curEntry = 
		(PFILE_BOTH_DIR_INFORMATION) Data->Iopb->Parameters.DirectoryControl.QueryDirectory.DirectoryBuffer;


		if( Data->Iopb->Parameters.DirectoryControl.QueryDirectory.Length>0)
		{

			for(;;)
			{
			if(curEntry->NextEntryOffset == 0)
				break;

			curEntry = (PFILE_BOTH_DIR_INFORMATION) ( (PUCHAR) curEntry + curEntry->NextEntryOffset );
			}
        }   
	}

	return FLT_PREOP_SUCCESS_NO_CALLBACK;
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
	
	return FLT_POSTOP_FINISHED_PROCESSING;
	
}
