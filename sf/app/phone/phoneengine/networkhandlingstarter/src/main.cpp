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

#include <networkhandlingstarter.h>
#include <xqserviceutil.h>
#include <hbapplication.h>

/*!
    main.
 */
int main(int argc, char **argv)
{
    HbApplication app(argc, argv, Hb::NoSplash);
    NetworkHandlingStarter service;
    XQServiceUtil::toBackground(true);
    return app.exec();
}

