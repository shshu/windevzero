# \\Device\\Zero Driver

This driver is a Windows implementation of the Unix [/dev/zero](https://en.wikipedia.org/wiki//dev/zero) device.

Read operations from `\Device\Zero` (available from user space as `\??\Zero`) return as many zero characters (0x00) as requested in the read operation.


## Installation

```
sc create DevZero type= kernel binPath= "C:\path\to\DevZero.sys"
sc start DevZero
```


## Development

Disable driver signature enforcement for [testing purposes](https://msdn.microsoft.com/en-us/windows/hardware/drivers/install/installing-an-unsigned-driver-during-development-and-test):
```
bcdedit.exe -set testsigning on
```

Set the Component Filter Mask (in order to [read debug messages](https://msdn.microsoft.com/en-us/library/windows/hardware/ff553545(v=vs.85).aspx) in Windows Vista and later):
```
reg.exe ADD "HKEY_LOCAL_MACHINE\SYSTEM\CurrentControlSet\Control\Session Manager\Debug Print Filter" /v "DEFAULT" /t REG_DWORD /d 0xffffffff
```

Restart for the changes to take effect.


## Usage

To access the device from user space, open the file `\??\Zero`.

**Example code:**
```
#include <Windows.h>
#include <stdio.h>

int main(int argc, char *argv[])
{
	HANDLE hFile = INVALID_HANDLE_VALUE;
	DWORD dwBytesRead;
	char buffer[16];
    int i;

	FillMemory(buffer, sizeof(buffer), 0x01);

	hFile = CreateFile("\\??\\Zero", GENERIC_READ, 0, NULL, OPEN_EXISTING, 0, NULL);
	if (INVALID_HANDLE_VALUE == hFile) {
        fprintf(stderr, "error: CreateFile: %d\n", GetLastError());
		return 1;
	}

	if (!ReadFile(hFile, buffer, sizeof(buffer), &dwBytesRead, NULL)) {
        CloseHandle(hFile);
        fprintf(stderr, "error: ReadFile: %d\n", GetLastError());
		return 1;
    }

    CloseHandle(hFile);

    printf("Read %d bytes:\n", dwBytesRead);
    for (i = 0; i < dwBytesRead; i++) {
        printf("buffer[%d] = 0x%02x\n", i, buffer[i]);
    }

	return 0;
}
```