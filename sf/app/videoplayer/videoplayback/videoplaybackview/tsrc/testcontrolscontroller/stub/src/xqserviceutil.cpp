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
* Description:   ?Description
*
*/

#include "xqserviceutil.h"
#include "xqserviceutilxtra.h"
#include "mpxvideo_debug.h"


// ----------------------------------------------------------------------------
// XQServiceUtil::toBackground()
// ----------------------------------------------------------------------------
//
void XQServiceUtil::toBackground( bool value )
{
    MPX_DEBUG(_L("XQServiceUtil::toBackground( %d )"), value );
}

// ----------------------------------------------------------------------------
// XQServiceUtil::isEmbedded()
// ----------------------------------------------------------------------------
//
bool XQServiceUtil::isEmbedded()
{
    MPX_DEBUG(_L("XQServiceUtil::isEmbedded()"));

    return false;
}

// ----------------------------------------------------------------------------
// XQServiceUtil::isService()
// ----------------------------------------------------------------------------
//
bool XQServiceUtil::isService()
{
    bool service;
    XQServiceUtilXtra* ptr = XQServiceUtilXtra::instance();
    service = ptr->currentService();
    ptr->decreaseReferenceCount();
    
    MPX_DEBUG(_L("XQServiceUtil::isEmbedded() ret %d"), service);
    return service;
}

