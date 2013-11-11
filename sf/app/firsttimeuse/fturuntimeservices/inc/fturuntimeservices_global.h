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


#ifndef FTURUNTIMESERVICES_GLOBAL_H
#define FTURUNTIMESERVICES_GLOBAL_H

#include <Qt/qglobal.h>


// services uri; services key names used when the service is passed as property in state
const char FTU_CONTENT_SERVICE_KEY[]  = "http://ftu.nokia.com/runtimeservices/contentservice";

// property name used to register for required services in state
// runtime assigns requested service based on value of the key in state, service keys should be used.
const char FTU_SERVICES_REGISTRATION_KEY[]  = "_ftu_services_";

#ifdef FTURUNTIMESERVICES_LIB
#define FTURUNTIMESERVICES_EXPORT Q_DECL_EXPORT
#else
#ifdef FTURUNTIMESERVICES_TEST
        #define FTURUNTIMESERVICES_EXPORT
    #else
        #define FTURUNTIMESERVICES_EXPORT Q_DECL_IMPORT
    #endif	
#endif

#endif // FTURUNTIMESERVICES_GLOBAL_H
