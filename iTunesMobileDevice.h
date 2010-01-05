#ifndef ITUNESMOBILEDEVICE_H_INCLUDED
#define ITUNESMOBILEDEVICE_H_INCLUDED
typedef unsigned int uint;

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
int __declspec(dllimport) AMDeviceNotificationSubscribe(DeviceNotificationCallback callback, uint unused1, uint unused2, uint unused3, void* am_device_notification_ptr);
int __declspec(dllimport) AMRestoreRegisterForDeviceNotifications(DeviceRestoreNotificationCallback dfu_connect,
                                                                  DeviceRestoreNotificationCallback recovery_connect,
                                                                  DeviceRestoreNotificationCallback dfu_disconnect,
                                                                  DeviceRestoreNotificationCallback recovery_disconnect,
                                                                  uint unknown0,
                                                                  void* user_info);
int __declspec(dllimport) AMDeviceConnect(void* phone);
int __declspec(dllimport) AMDeviceIsPaired(void* phone);
int __declspec(dllimport) AMDeviceValidatePairing(void* phone);
int __declspec(dllimport) AMDeviceStartSession(void* phone);
int __declspec(dllimport) AMDeviceStartService(void* phone, void* service_name, void* handle, void* unknown);

int __declspec(dllimport) AFCConnectionOpen(void* handle, uint io_timeout, void* conn);
int __declspec(dllimport) AFCDirectoryOpen(void* handle, char *path, void *dir);
int __declspec(dllimport) AFCDirectoryRead(void *handle, void *hDir, char *buffer);

/*CoreFoundations.dll */
void* __declspec(dllimport) __CFStringMakeConstantString(char* s);

#ifdef __cplusplus
}
#endif

#endif // ITUNESMOBILEDEVICE_H_INCLUDED
