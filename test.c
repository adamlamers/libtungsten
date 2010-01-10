/* File: ${filename}.c
 * Creation Date: ${date} ${time}
 * Last Modified Date: ${date} ${time}
 * Version: 0.0.1
 * Contact: Adam Lamers <adam@millenniumsoftworks.com>
*/

/********************************************************************************
* This file is part of Tungsten.                                                *
*                                                                               *
* Tungsten is free software: you can redistribute it and/or modify              *
* it under the terms of the GNU General Public License as published by          *
* the Free Software Foundation, either version 3 of the License, or             *
* (at your option) any later version.                                           *
*                                                                               *
* Tungsten is distributed in the hope that it will be useful,                   *
* but WITHOUT ANY WARRANTY; without even the implied warranty of                *
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the                 *
* GNU General Public License for more details.                                  *
*                                                                               *
* You should have received a copy of the GNU General Public License             *
* along with Tungsten.  If not, see <http://www.gnu.org/licenses/>.             *
********************************************************************************/

#include "tungsten.h"
#include "iTunesMobileDevice.h"
#include <stdio.h>

iPhone *phone;

void NotifyCallback(AMDeviceNotificationCallbackInfo* callback)
{
    switch(callback->msg)
    {
        case Connected:
            phone->handle = callback->dev;
            if(iPhone_Connect(phone) == TRUE)
            {
                //GetFiles(phone, "/private/var/mobile/Media/iTunes_Control/iTunes/");
            }
        break;
        case Disconnected:
            phone->connected = FALSE;
        break;
        default:
        break;
    }
}

int main(int argc, char **argv)
{
    phone = malloc(sizeof(iPhone));
    memset(phone, 0, sizeof(iPhone));
    phone->dnc = &NotifyCallback;
    iPhone_init(phone);
    iPhone_WaitForConnect(phone);
    iPhone_CopyFileFromDevice(phone, "/private/var/mobile/Media/iTunes_Control/iTunes/iTunesCDB", "iTunesCDB");
    getchar();
    return 0;
}
