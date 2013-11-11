/*
 * Copyright (c) 2009-2010 Nokia Corporation and/or its subsidiary(-ies).
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

#include <hbsplashscreen.h>
#include "nmuiheaders.h"
#include "nmapplication.h"

/*!
    The main function.
 */
int main(int argc, char *argv[])
{
    // Decide which splash screen to show.
    if (!XQServiceUtil::isService(argc,argv)) {
        HbSplashScreen::setScreenId("messagelist_screen");        
    } else {
        QString interfaceName = XQServiceUtil::interfaceName(argc,argv);
        
        if (interfaceName == XQI_EMAIL_MESSAGE_SEND || 
            interfaceName == XQI_URI_VIEW ||
            interfaceName == XQI_FILE_SHARE) {
            HbSplashScreen::setScreenId("editor_screen");
        }
        else if (interfaceName == XQI_EMAIL_MESSAGE_VIEW) {
            HbSplashScreen::setScreenId("viewer_screen");
        }
        else if (interfaceName == XQI_EMAIL_INBOX_VIEW) {
            HbSplashScreen::setScreenId("messagelist_screen");
        }
    }
    
    NmApplication app(argc,argv);
    NM_TIMESTAMP("Application started.");
    int ret = app.exec();
    return ret;
}
