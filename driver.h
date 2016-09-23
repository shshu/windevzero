#pragma once
#include <Ntddk.h>
#include <devioctl.h>

NTSTATUS DriverEntry(IN PDRIVER_OBJECT DriverObject, IN PUNICODE_STRING RegistryPath);
NTSTATUS DriverDispatch(IN PDEVICE_OBJECT DeviceObject, IN PIRP irp);
void DriverUnload(IN PDRIVER_OBJECT DriverObject);