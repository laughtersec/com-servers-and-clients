# COM Servers and Clients

Sample COM servers and a client.

## Instructions

### Building
- Build the UserInfo in-process server first (select "Release" build instead of "Debug"), so that the IDL can generate the appropriate source and header files.
- Two generated files `UserInfo_i.c` and `UserInfo_i.h` should appear in the project.
- Copy those two files to the COMClient project's "Header Files".
- You may now build COMClient.

### Registering COM server
- Open cmd.exe with Administrator privileges.
- Use regsvr32.exe to register the COM server.

### Usage
After registering the COM server, you may use it via the COM client. Simply run COMClient.exe
