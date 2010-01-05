#include "tungsten.h"
#include <stdio.h>
#include <windows.h>

DeviceNotificationCallback        dnc;
DeviceRestoreNotificationCallback drn1;
DeviceRestoreNotificationCallback drn2;
DeviceRestoreNotificationCallback drn3;
DeviceRestoreNotificationCallback drn4;

void* iPhoneHandle;
void* hAFC = NULL;
void* hService = NULL;
bool  connected;
char* current_directory;
bool  wasAFC2;

void iPhone_init()
{
    dnc = &NotifyCallback;
    
    void* notification = malloc(sizeof(void*));
    int ret = AMDeviceNotificationSubscribe(dnc, 0, 0, 0, notification);
    if(ret != 0)
        fprintf(stderr, "AMDeviceNotificationSubscribe failed with error %d", ret);
    
    ret = AMRestoreRegisterForDeviceNotifications(drn1, drn2, drn3, drn4, 0, NULL);
    if(ret != 0)
        fprintf(stderr, "AMRestoreRegisterForDeviceNotifications failed with error %d", ret);
    free(notification);
}

void NotifyCallback(AMDeviceNotificationCallbackInfo* callback)
{
    switch(callback->msg)
    {
        case Connected:
            iPhoneHandle = callback->dev;
            if(ConnectToPhone() == TRUE)
            {
                //MessageBox(NULL, "Connected", "iPhone Event", MB_OK);
                GetFiles("/bin");
            }
        break;
        case Disconnected:
            connected = FALSE;
            //MessageBox(NULL, "Disconnected", "iPhone Event", MB_OK);
        break;
        default:
        break;
    }
}

bool FileExists(char *path)
{
    void *data = NULL;
    int ret = AFCFileInfoOpen(hAFC, path, &data);
    if(ret == 0) 
    {
        AFCKeyValueClose(data);
        return TRUE;
    }
    else
        return FALSE;
}

bool GetFiles(char *path)
{
    if(connected == FALSE){ fprintf(stderr, "GetFiles() failed, device not connected."); return FALSE; } //If we aren't connected, we can't get files.
    void *hDir = NULL;
    if(AFCDirectoryOpen(hAFC, path, &hDir) != 0){ fprintf(stderr, "AFCDirectoryOpen failed"); return FALSE; }
    char *buffer = NULL;
    while(1)
    {
        AFCDirectoryRead(hAFC, hDir, &buffer);
        if(buffer == NULL) break;
        printf("%s\n", buffer);
    }
    AFCDirectoryClose(hAFC, hDir);
    return TRUE;
}

bool ConnectToPhone()
{
    if(AMDeviceConnect(iPhoneHandle) == 1)   fprintf(stderr, "Device is in recovery mode.");
    if(AMDeviceIsPaired(iPhoneHandle) == 0){ fprintf(stderr, "AMDeviceIsPaired failed."); return FALSE; }
    if(AMDeviceValidatePairing(iPhoneHandle) != 0){ fprintf(stderr, "AMDeviceValidatePairing failed."); return FALSE; }
    if(AMDeviceStartSession(iPhoneHandle) == 1){ fprintf(stderr, "AMDeviceStartSession failed."); return FALSE; }
    bool isAfc2 = 0 == AMDeviceStartService(iPhoneHandle, __CFStringMakeConstantString("com.apple.afc2"), &hService, NULL);
    if(!isAfc2)
    {
        bool isAfc = 0 == AMDeviceStartService(iPhoneHandle, __CFStringMakeConstantString("com.apple.afc"), &hService, NULL);
        if(!isAfc) return FALSE;
    }
    else
        wasAFC2 = TRUE;
    if(AFCConnectionOpen(hService, 0, &hAFC) != 0){ fprintf(stderr, "AFCConnectionOpen failed."); return FALSE; }
    connected = TRUE;
    return TRUE;
}

int main(int argc, char **argv)
{
    iPhone_init();
    getchar();
    return 0;
}
