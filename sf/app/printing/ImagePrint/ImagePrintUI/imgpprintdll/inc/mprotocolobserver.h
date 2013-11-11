/*
* Copyright (c) 2004-2007 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:  
*
*/


#ifndef MPROTOCOLBSERVER_H
#define MPROTOCOLBSERVER_H

#include <e32std.h>

/**
 *
 *  Observer class that will get notification message when the print preview
 *  is ready
 *
 */
class MProtocolObserver
    {
    public:
        
        /**
         * Called by CUIProtocol when usb cable is disconnected and application
         * need to closed or view changed. This method is only called when USB
         * printer is in use
         */
        virtual void UsbCableDisconnected() = 0;
    };

#endif  //  MPROTOCOLBSERVER_H

//  End of File
