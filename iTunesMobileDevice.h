/* File: iTunesMobileDevice.c
 * Creation Date: December 17th, 2009
 * Last Modified Date: January 9th, 2010
 * Version: 0.0.27
 * Contact: Adam Lamers <adam@millenniumsoftworks.com>
*/

#ifndef ITUNESMOBILEDEVICE_H_INCLUDED
#define ITUNESMOBILEDEVICE_H_INCLUDED

typedef unsigned int uint32;
typedef long long int64_t;
typedef unsigned long long uint64_t;

typedef enum
{
    Connected = 1,
    Disconnected = 2,
    Unknown = 3,
} NotificationMessage;

typedef struct AMDeviceNotificationCallbackInfo_t
{
    void *dev;
    NotificationMessage msg;
    
} AMDeviceNotificationCallbackInfo;

typedef void (*DeviceNotificationCallback)(AMDeviceNotificationCallbackInfo*);

typedef struct AMRecoveryDevice_t
{
    char unk0[8];
    
} AMRecoveryDevice;

typedef void (*DeviceRestoreNotificationCallback)(AMRecoveryDevice*);

#ifdef __cplusplus
extern "C"
{
#endif

/* iTunesMobileDevice.dll */
int __declspec(dllimport) AMDeviceNotificationSubscribe(DeviceNotificationCallback callback, uint32 unused1, uint32 unused2, uint32 unused3, void  **hNotification);
int __declspec(dllimport) AMRestoreRegisterForDeviceNotifications(DeviceRestoreNotificationCallback dfu_connect,
                                                                  DeviceRestoreNotificationCallback recovery_connect,
                                                                  DeviceRestoreNotificationCallback dfu_disconnect,
                                                                  DeviceRestoreNotificationCallback recovery_disconnect,
                                                                  uint32 unknown0,
                                                                  void* user_info);
int __declspec(dllimport) AMDeviceConnect(void* hPhone);
int __declspec(dllimport) AMDeviceDisconnect(void* hPhone);
int __declspec(dllimport) AMDeviceIsPaired(void* hPhone);
int __declspec(dllimport) AMDeviceValidatePairing(void* hPhone);
int __declspec(dllimport) AMDeviceStartSession(void* hPhone);
int __declspec(dllimport) AMDeviceStartService(void* hPhone, void* service_name, void **serviceHandle, void* unknown);

int __declspec(dllimport) AFCConnectionOpen(void* hAFC, uint32 io_timeout, void* conn);

int __declspec(dllimport) AFCDirectoryOpen(void* hAFC, char *path, void **dir);
int __declspec(dllimport) AFCDirectoryRead(void *hAFC, void *hDir, char **dirent);
int __declspec(dllimport) AFCDirectoryClose(void *hAFC, void *hDir);
int __declspec(dllimport) AFCDirectoryCreate(void *hAFC, char *path);

int __declspec(dllimport) AFCFileInfoOpen(void *hAFC, char *path, void *data);
int __declspec(dllimport) AFCKeyValueRead(void *data, void **key, void **val);
int __declspec(dllimport) AFCKeyValueClose(void *val);

int __declspec(dllimport) AFCFileRefOpen(void *hAFC, char *path, int mode, int unknown, int64_t *handle);
int __declspec(dllimport) AFCFileRefRead(void *hAFC, int64_t handle, void *buffer, uint32 *len);
int __declspec(dllimport) AFCFileRefWrite(void *hAFC, int64_t handle, void *buffer, uint32 len);
int __declspec(dllimport) AFCFileRefTell(void *hAFC, int64_t handle, long *position);
int __declspec(dllimport) AFCFileRefSeek(void *hAFC, int64_t handle, int pos, int org);
int __declspec(dllimport) AFCFlushData(void *hAFC, int64_t handle);
int __declspec(dllimport) AFCFileRefClose(void *hAFC, int64_t handle);

/*CoreFoundations.dll */
void* __declspec(dllimport) __CFStringMakeConstantString(char* s);

#ifdef __cplusplus
}
#endif

#endif // ITUNESMOBILEDEVICE_H_INCLUDED
