/*
* Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
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
* Description: LocationPickerService implementation
*
*/

#include <hbapplication.h>
#include "locationpickerservice.h"
#include "locationpickerappwindow.h"

// ----------------------------------------------------------------------------
// LocationPickerService::LocationPickerService()
// ----------------------------------------------------------------------------
//
LocationPickerService::LocationPickerService(LocationPickerAppWindow* parent)
: XQServiceProvider(QLatin1String("com.nokia.symbian.ILocationPick"),parent),mServiceApp(parent)
{
    mAsyncReqId = 0;
    publishAll();
}

// ----------------------------------------------------------------------------
// LocationPickerService::~LocationPickerService()
// ----------------------------------------------------------------------------
//
LocationPickerService::~LocationPickerService()
{
}


// ----------------------------------------------------------------------------
// LocationPickerService::complete()
// ----------------------------------------------------------------------------
void LocationPickerService::complete( QLocationPickerItem aLm )
{
    if ( isActive() )
    {
        mReturn=aLm;
        connect( this, SIGNAL( returnValueDelivered() ), qApp, SLOT( quit() ) );
        bool ok = completeRequest(mAsyncReqId,mReturn);
		mAsyncReqId = 0;
    }
}
// ----------------------------------------------------------------------------
// isActive()
// ----------------------------------------------------------------------------
//
bool LocationPickerService::isActive()
{
    return mAsyncReqId>0;
}


// ----------------------------------------------------------------------------
// LocationPickerService::pick()
// ----------------------------------------------------------------------------
//
void LocationPickerService::pick()
{
    connect( this, SIGNAL( clientDisconnected() ), qApp, SLOT( quit() ) );
    mAsyncReqId = setCurrentRequestAsync();
}



