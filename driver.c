#include "driver.h"

PDEVICE_OBJECT deviceObject = NULL; 
UNICODE_STRING deviceName = RTL_CONSTANT_STRING(L"\\Device\\Zero"); 
UNICODE_STRING dosDeviceName = RTL_CONSTANT_STRING(L"\\??\\Zero");

NTSTATUS DriverEntry(IN PDRIVER_OBJECT DriverObject, IN PUNICODE_STRING RegistryPath)
{
	UNREFERENCED_PARAMETER(RegistryPath);
	NTSTATUS status = STATUS_SUCCESS; 

	DbgPrint("DevZero: DriverEntry: initializing driver");

	status = IoCreateDevice(
		DriverObject,
		0,
		&deviceName,
		FILE_DEVICE_UNKNOWN,
		FILE_READ_ONLY_DEVICE,
		FALSE,
		&deviceObject);
	if (!NT_SUCCESS(status)) {
		DbgPrint("DevZero: DriverEntry: IoCreateDevice failed: %08X", status);
		return status;
	}

	status = IoCreateSymbolicLink(&dosDeviceName, &deviceName);
	if (!NT_SUCCESS(status)) {
		DbgPrint("DevZero: DriverEntry: IoCreateSymbolicLink failed: %08X", status);
		return status;
	}

	// Set dispatch routine
	DriverObject->MajorFunction[IRP_MJ_CREATE] = DriverDispatch;
	DriverObject->MajorFunction[IRP_MJ_READ] = DriverDispatch;
	DriverObject->MajorFunction[IRP_MJ_CLEANUP] = DriverDispatch;
	DriverObject->MajorFunction[IRP_MJ_CLOSE] = DriverDispatch;

	DriverObject->DriverUnload = DriverUnload;

	// Specify I/O buffering type
	deviceObject->Flags |= DO_BUFFERED_IO;

	return status;
}

NTSTATUS DriverDispatch(IN PDEVICE_OBJECT DeviceObject, IN PIRP irp)
{
	UNREFERENCED_PARAMETER(DeviceObject);
	NTSTATUS status = STATUS_UNSUCCESSFUL;
	PIO_STACK_LOCATION irpStack = NULL;

	irp->IoStatus.Information = 0;
	irpStack = IoGetCurrentIrpStackLocation(irp);

	switch (irpStack->MajorFunction)
	{
	case IRP_MJ_CREATE:
		status = STATUS_SUCCESS;
		break;

	case IRP_MJ_READ:
		memset(irp->AssociatedIrp.SystemBuffer, 0, irpStack->Parameters.Read.Length);
		irp->IoStatus.Information = irpStack->Parameters.Read.Length;
		status = STATUS_SUCCESS;
		break;

	case IRP_MJ_CLEANUP:
		status = STATUS_SUCCESS;
		break;

	case IRP_MJ_CLOSE:
		status = STATUS_SUCCESS;
		break;

	default:
		status = STATUS_NOT_IMPLEMENTED;
		break;
	}

	irp->IoStatus.Status = status;
	IoCompleteRequest(irp, IO_NO_INCREMENT);

	return status;
}

void DriverUnload(IN PDRIVER_OBJECT DriverObject)
{
	UNREFERENCED_PARAMETER(DriverObject);
	NTSTATUS status = STATUS_UNSUCCESSFUL;

	DbgPrint("DevZero: DriverUnload: unloading driver");

	status = IoDeleteSymbolicLink(&dosDeviceName);
	if (!NT_SUCCESS(status)) {
		DbgPrint("DevZero: DriverUnload: IoDeleteSymbolicLink failed: %08X", status);
	}

	IoDeleteDevice(deviceObject);
	deviceObject = NULL;
}