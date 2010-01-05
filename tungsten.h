#ifndef __TUNGSTEN_H__
#define __TUNGSTEN_H__

#include <windows.h>
#include "iTunesMobileDevice.h"

#ifdef BUILD_DLL
    #define DLL_EXPORT __declspec(dllexport)
#else
    #define DLL_EXPORT __declspec(dllimport)
#endif

#ifdef __cplusplus
extern "C"
{
#endif

void DLL_EXPORT iPhone_init();
bool ConnectToPhone();
void NotifyCallback(AMDeviceNotificationCallbackInfo*);
bool GetFiles(char*);

#ifdef __cplusplus
}
#endif

#endif // __TUNGSTEN_H__
