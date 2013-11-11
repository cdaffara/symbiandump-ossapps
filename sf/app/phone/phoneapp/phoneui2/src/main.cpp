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
* Description:  Main.
*
*/

#include <QGraphicsWidget>
#include <hbapplication.h>
#include <hbmainwindow.h>
#include <hbtoolbar.h>
#include <xqserviceutil.h>

#include "hbphonemainwindow.h"
#include "phoneapplication.h"


int main (int argc, char *argv[])
{
    PhoneApplication app(argc, argv, Hb::NoSplash);
    HbPhoneMainWindow window;
    QObject::connect(&app,SIGNAL(focusLost()),&window,SLOT(onFocusLost()));
    QObject::connect(&app,SIGNAL(focusGained()),&window,SLOT(onFocusGained()));
    XQServiceUtil::toBackground( true );
    return app.exec ();
}
