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

XQServiceUtilXtra* XQServiceUtilXtra::mInstance = 0;
int  XQServiceUtilXtra::mReferenceCount = 0;
bool XQServiceUtilXtra::service = false;

// -----------------------------------------------------------------------------
// XQServiceUtilXtra::instance()
// -----------------------------------------------------------------------------
//
XQServiceUtilXtra* XQServiceUtilXtra::instance()
{
    if(!mInstance)
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
    if(mInstance)
    {
        if(--mInstance->mReferenceCount == 0)
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
}

// ----------------------------------------------------------------------------
// ~XQServiceUtilXtra()
// ----------------------------------------------------------------------------
//
XQServiceUtilXtra::~XQServiceUtilXtra()
{
}


// ----------------------------------------------------------------------------
// currentService()
// ----------------------------------------------------------------------------
//
bool XQServiceUtilXtra::currentService()
{
	return mInstance->service;
}


// ----------------------------------------------------------------------------
// setCurrentService()
// ----------------------------------------------------------------------------
//
void XQServiceUtilXtra::setCurrentService(bool aservice)
{
	mInstance->service = aservice;
}

