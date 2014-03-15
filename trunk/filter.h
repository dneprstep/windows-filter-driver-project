#ifndef __FILTER_H__
#define __FILTER_H__

#include <Fltkernel.h>


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
FilterLoad 
(
    __in PCFLT_RELATED_OBJECTS FltObjects,
    __in FLT_INSTANCE_SETUP_FLAGS Flags,
    __in DEVICE_TYPE VolumeDeviceType,
    __in FLT_FILESYSTEM_TYPE VolumeFilesystemType
);

#endif /* __FILTER_H__ */
