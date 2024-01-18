#include <windows.h>
#include <stdio.h>
#include <conio.h>

int x = 0;
int y = 1;
HANDLE S1, S2;

DWORD WINAPI T1(LPVOID lpParameter)
{
    while (1)
    {
        WaitForSingleObject(S1, INFINITE);
        x = y + 1;
        ReleaseSemaphore(S2, 1, NULL);
        printf("%4d\n", x);
    }

    return 0;
}

DWORD WINAPI T2(LPVOID lpParameter)
{
    while (1)
    {
        y = 2;
        ReleaseSemaphore(S1, 1, NULL);
        WaitForSingleObject(S2, INFINITE);
        y = 2 * y;
    }

    return 0;
}

int main()
{
    HANDLE h1, h2;
    DWORD ThreadId;

    S1 = CreateSemaphore(NULL, 0, 1, NULL);
    S2 = CreateSemaphore(NULL, 0, 1, NULL);

    h1 = CreateThread(NULL, 0, T1, NULL, 0, &ThreadId);
    h2 = CreateThread(NULL, 0, T2, NULL, 0, &ThreadId);

    getch(); // Wait for a key press before exiting

    return 0;
}

