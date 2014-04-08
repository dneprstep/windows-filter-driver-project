#ifndef __FILTER_H__
#define __FILTER_H__



typedef struct _FILTERDATA
{
    PDRIVER_OBJECT pDriverObject;
    PFLT_FILTER pFilter;
} FILTERDATA, *PFILTERDATA;


extern FILTERDATA FilterData;


/////////////////////////////////////////////////////////
//
//Prototypes for the start and unload routins for filter
//
// implementation in filter.c
/////////////////////////////////////////////////////////

NTSTATUS
DriverEntry 
(
    __in PDRIVER_OBJECT DriverObject,
    __in PUNICODE_STRING RegistryPath
);
	
NTSTATUS
FilterUnload 
(
    __in FLT_FILTER_UNLOAD_FLAGS Flags
);

NTSTATUS
QueryTeardown 
(
    __in PCFLT_RELATED_OBJECTS FltObjects,
    __in FLT_INSTANCE_QUERY_TEARDOWN_FLAGS Flags
);


//////////////////////////////////////
// IRP_MJ_DIRECTORY_CONTROL routins
//

FLT_POSTOP_CALLBACK_STATUS
	FilterPostDirectoryControl 
(
	__inout PFLT_CALLBACK_DATA Data,
	__in PCFLT_RELATED_OBJECTS FltObjects,
	__in_opt PVOID CompletionContext,
	__in FLT_POST_OPERATION_FLAGS Flags
);



/////////////////////////////////////////////
#endif  __FILTER_H__ 

