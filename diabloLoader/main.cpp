#include <windows.h>

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
                    LPTSTR lpCmdLine, int nCmdShow)
{
    // Structures for creating the process
    STARTUPINFO si = {0};
    PROCESS_INFORMATION pi = {0};
    BOOL result = FALSE;

    char exeString[] = "Diablo.exe";
    si.cb = sizeof(STARTUPINFO);

    result = CreateProcess(NULL, exeString, NULL, NULL, FALSE,
                           0, NULL, NULL, &si, &pi);
    if(!result)
    {
        MessageBox(0, "Couldn't start Diablo.exe", "Error", MB_ICONERROR);
        return -1;
    }

	HANDLE hThread;
	char    szLibPath[_MAX_PATH] = "diabloext.dll";
	void*   pLibRemote;
	DWORD   hLibModule;
	HMODULE hKernel32 = GetModuleHandle("Kernel32");
	pLibRemote = VirtualAllocEx( pi.hProcess, NULL, sizeof(szLibPath),
								   MEM_COMMIT, PAGE_READWRITE );

	WriteProcessMemory( pi.hProcess, pLibRemote, (void*)szLibPath,
						  sizeof(szLibPath), NULL );

	hThread = CreateRemoteThread( pi.hProcess, NULL, 0,
				(LPTHREAD_START_ROUTINE) GetProcAddress( hKernel32,
										   "LoadLibraryA" ),
				 pLibRemote, 0, NULL );
	WaitForSingleObject( hThread, INFINITE );
	GetExitCodeThread( hThread, &hLibModule );

	// Clean up

	CloseHandle( hThread );
	VirtualFreeEx( pi.hProcess, pLibRemote, sizeof(szLibPath), MEM_RELEASE );
    return 0;
}