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
* Description:  Implementation for controlling S60 device lights.
*
*/


// INCLUDE FILES
#include <HtiLogging.h>
#include "HtiPropertySubscriber.h"

// CONSTANTS

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CHtiPropertySubscriber::CHtiPropertySubscriber
// -----------------------------------------------------------------------------
//
CHtiPropertySubscriber::CHtiPropertySubscriber( TCallBack aCallBack,
    RProperty& aProperty) : CActive( EPriorityNormal ),
                            iCallBack( aCallBack ),
                            iProperty( aProperty )
    {
    HTI_LOG_TEXT( "CHtiPropertySubscriber construct" );
    CActiveScheduler::Add( this );
    }


// -----------------------------------------------------------------------------
// CHtiPropertySubscriber::~CHtiPropertySubscriber
// -----------------------------------------------------------------------------
//
CHtiPropertySubscriber::~CHtiPropertySubscriber()
    {
    HTI_LOG_TEXT( "CHtiPropertySubscriber destroy" );
    Cancel();
    }


// -----------------------------------------------------------------------------
// CHtiPropertySubscriber::Subscribe
// -----------------------------------------------------------------------------
//
void CHtiPropertySubscriber::Subscribe()
    {
    if ( !IsActive() )
        {
        iProperty.Subscribe( iStatus );
        SetActive();
        }
    }


// -----------------------------------------------------------------------------
// CHtiPropertySubscriber::Unsubscribe
// -----------------------------------------------------------------------------
//
void CHtiPropertySubscriber::Unsubscribe()
    {
    Cancel();
    }


// -----------------------------------------------------------------------------
// CHtiPropertySubscriber::RunL
// -----------------------------------------------------------------------------
//
void CHtiPropertySubscriber::RunL()
    {
    if ( iStatus.Int() == KErrNone )
        {
        iCallBack.CallBack();
        Subscribe();
        }
    }


// -----------------------------------------------------------------------------
// CHtiPropertySubscriber::DoCancel
// -----------------------------------------------------------------------------
//
void CHtiPropertySubscriber::DoCancel()
    {
    iProperty.Cancel();
    }


// End of file
