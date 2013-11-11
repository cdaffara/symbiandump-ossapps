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
* Description:  Panics definitions for HtiAdmin
*
*/


#ifndef __HTIADMIN_PAN__
#define __HTIADMIN_PAN__

/** HtiAdmin application panic codes */
enum THtiAdminPanics
    {
    EHtiAdminBasicUi = 1
    // add further panics here
    };

inline void Panic(THtiAdminPanics aReason)
    {
    _LIT(applicationName,"HtiAdmin");
    User::Panic(applicationName, aReason);
    }

#endif // __HTIADMIN_PAN__
