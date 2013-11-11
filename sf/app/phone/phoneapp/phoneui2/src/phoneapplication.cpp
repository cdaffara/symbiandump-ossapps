/*
* Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Phone application class
*
*/

#include "phoneapplication.h"

#ifdef Q_OS_SYMBIAN
#include <QSymbianEvent>
#include <w32std.h>
#endif

PhoneApplication::PhoneApplication(
        int &argc, 
        char *argv[], 
        Hb::ApplicationFlags flags):
    HbApplication(argc, argv, flags)
{
}

#ifdef Q_OS_SYMBIAN
bool PhoneApplication::symbianEventFilter(const QSymbianEvent *event)
{
    if (event->type() == QSymbianEvent::WindowServerEvent){
        const TWsEvent *wsevent = event->windowServerEvent();
        if ( wsevent->Type() == EEventWindowVisibilityChanged ){
            TUint visible = wsevent->VisibilityChanged()->iFlags;
            if ( visible & TWsVisibilityChangedEvent::EFullyVisible ){
                emit focusGained();
            }else if (visible & TWsVisibilityChangedEvent::ENotVisible){
                emit focusLost();
            }
        }
    }
    return HbApplication::symbianEventFilter(event);
}
#endif
