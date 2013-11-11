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
* Description:  Library header.
*
*/


#ifndef HSDOMAINMODEL_GLOBAL_H
#define HSDOMAINMODEL_GLOBAL_H

#include <qglobal.h>

// services uri; services key names used when the service is passed as property in state
const char CONTENT_SERVICE_KEY[]  = "http://homescreen.nokia.com/runtimeservices/contentservice";
const char SHORTCUT_SERVICE_KEY[] = "http://homescreen.nokia.com/runtimeservices/shortcutservice";

// property name used to register for required services in state
// runtime assigs requested service based on value of the key in state, service keys should be used.
const char HS_SERVICES_REGISTRATION_KEY[]  = "_hs_services_";

#ifdef HSDOMAINMODEL_LIB
    #define HSDOMAINMODEL_EXPORT Q_DECL_EXPORT
#else
    #ifdef HSDOMAINMODEL_TEST
        #define HSDOMAINMODEL_EXPORT
    #else
        #define HSDOMAINMODEL_EXPORT Q_DECL_IMPORT
    #endif
#endif

#endif // HSDOMAINMODEL_GLOBAL_H
