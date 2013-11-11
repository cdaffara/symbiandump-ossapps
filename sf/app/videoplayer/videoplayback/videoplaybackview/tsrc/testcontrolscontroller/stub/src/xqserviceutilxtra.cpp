/**
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

#include "xqserviceutilxtra.h"
#include "mpxvideo_debug.h"

XQServiceUtilXtra* XQServiceUtilXtra::mInstance = 0;
int  XQServiceUtilXtra::mReferenceCount = 0;
bool XQServiceUtilXtra::service = false;

// -----------------------------------------------------------------------------
// XQServiceUtilXtra::instance()
// -----------------------------------------------------------------------------
//
XQServiceUtilXtra* XQServiceUtilXtra::instance()
{
    MPX_DEBUG(_L("XQServiceUtilXtra::instance()"));

    if ( !mInstance )
    {
        mInstance = new XQServiceUtilXtra();
    }
    mInstance->mReferenceCount++;

    return mInstance;
}

// -----------------------------------------------------------------------------
// XQServiceUtilXtra::decreaseReferenceCount()
// -----------------------------------------------------------------------------
//
void XQServiceUtilXtra::decreaseReferenceCount()
{
    MPX_DEBUG(_L("XQServiceUtilXtra::decreaseReferenceCount()"));

    if ( mInstance )
    {
        if (--mInstance->mReferenceCount == 0 )
        {
            delete mInstance;
            mInstance = NULL;
        }
    }
}


// ----------------------------------------------------------------------------
// XQServiceUtilXtra()
// ----------------------------------------------------------------------------
//
XQServiceUtilXtra::XQServiceUtilXtra()
{
    MPX_DEBUG(_L("XQServiceUtilXtra::XQServiceUtilXtra()"));
}

// ----------------------------------------------------------------------------
// ~XQServiceUtilXtra()
// ----------------------------------------------------------------------------
//
XQServiceUtilXtra::~XQServiceUtilXtra()
{
    MPX_DEBUG(_L("XQServiceUtilXtra::~XQServiceUtilXtra()"));
}


// ----------------------------------------------------------------------------
// currentService()
// ----------------------------------------------------------------------------
//
bool XQServiceUtilXtra::currentService()
{
    MPX_DEBUG(_L("XQServiceUtilXtra::currentService() ret %d"), mInstance->service);

    return mInstance->service;
}


// ----------------------------------------------------------------------------
// setCurrentService()
// ----------------------------------------------------------------------------
//
void XQServiceUtilXtra::setCurrentService( bool service )
{
    MPX_DEBUG(_L("XQServiceUtilXtra::setCurrentService( %d )"), service);

    mInstance->service = service;
}

