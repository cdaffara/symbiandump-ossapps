/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies). 
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

#include <QSymbianEvent>

#include "application.h"

#ifdef Q_OS_SYMBIAN
#include <w32std.h>

#endif

Application::Application(int &argc, char *argv[]) :
        HbApplication(argc, argv)
{
}

#ifdef Q_OS_SYMBIAN
bool Application::symbianEventFilter(const QSymbianEvent * event)
{
    if (event->type() == QSymbianEvent::WindowServerEvent)
    {
        const TWsEvent *aEvent = event->windowServerEvent();

        if (aEvent->Type() == EEventFocusLost) {
            emit foregroundEvent(false);
        }

        if (aEvent->Type() == EEventFocusGained) {
            emit foregroundEvent(true);
        }
    }
    return HbApplication::symbianEventFilter(event);
}
#endif
