/*
* Copyright (c) 2004-2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Contains the CBtOnQuery class definition. This Implementation is used to avoid
*  				 User::WaitForRequest() function and ViewServer 11 panic when BT naming query is activated. 
*
*/


#include <e32base.h>
#include <btnotifierapi.h>

#include "cbtonquery.h"
#include "mbtonnotify.h"
#include "clog.h"

// ============================ MEMBER FUNCTIONS ==============================

// ----------------------------------------------------------------------------
// CObjectExchangeClient::NewL()
// Symbian two-phased constructor.
// ----------------------------------------------------------------------------
//
CBtOnQuery* CBtOnQuery::NewL( MBTOnNotify& aNotify )  
	{
	CBtOnQuery* self = CBtOnQuery::NewLC( aNotify );
    CleanupStack::Pop( self );
    return self;
	}

// ----------------------------------------------------------------------------
// CBtOnQuery::NewLC()
// Symbian two-phased constructor.
// ----------------------------------------------------------------------------
//
CBtOnQuery* CBtOnQuery::NewLC( MBTOnNotify& aNotify )
    {
    CBtOnQuery* self;
    self = new ( ELeave ) CBtOnQuery( aNotify );
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
   	}

// ----------------------------------------------------------------------------
// CBtOnQuery::CBtOnQuery()
// Constructor.
// ----------------------------------------------------------------------------
//
CBtOnQuery::CBtOnQuery( MBTOnNotify& aNotify ) : CActive( CActive::EPriorityLow ),
                                                 iNotify( aNotify )
    {
	CActiveScheduler::Add( this );
    }

// ----------------------------------------------------------------------------
// CBtOnQuery::~CBtOnQuery()
// Destructor.
// ----------------------------------------------------------------------------
//
CBtOnQuery::~CBtOnQuery()
    {
	Cancel();
    }

// ----------------------------------------------------------------------------
// CBtOnQuery::ConstructL()
// Perform second phase construction of this object.
// ----------------------------------------------------------------------------
//
void CBtOnQuery::ConstructL()
    {
    iResultPckg = EFalse;
    }


// ----------------------------------------------------------------------------
// CBtOnQuery::IsBluetoothOn()
// Starts Notifier to ask user to BT activation. 
// ----------------------------------------------------------------------------
//
void CBtOnQuery::IsBluetoothOn()
    {
    LOG("CBtOnQuery::IsBluetoothOn, start");
	TPckgBuf<TBool> pckg( ETrue ); // Input parameter

	if( iNotif.Connect() == KErrNone )
		{		
	    LOG("CBtOnQuery::IsBluetoothOn() iNotif.Connect() == KErrNone");
		iNotif.StartNotifierAndGetResponse( iStatus, KPowerModeSettingNotifierUid, pckg, iResultPckg );
	    LOG1("CBtOnQuery::IsBluetoothOn %d", iResultPckg());
		}

	SetActive();
    LOG("CBtOnQuery::IsBluetoothOn, end");
    }

// ----------------------------------------------------------------------------
// CBtOnQuery::IsBluetoothOn()
// Respond to an event. 
// ----------------------------------------------------------------------------
//
void CBtOnQuery::RunL()
    {
    LOG1("CBtOnQuery::RunL() %d", iStatus.Int());
    if( iStatus.Int() == KErrNone || iStatus.Int() == KErrCancel || iStatus.Int() == KErrGeneral )
        {
      	iNotif.CancelNotifier( KPowerModeSettingNotifierUid );
		iNotif.Close();
		LOG1("CBtOnQuery::RunL() %d", iResultPckg());
        iNotify.BTOnCompleteL( iResultPckg() );  // iResultPckg contains user answer 
        }   
	LOG("CBtOnQuery::RunL()  End");		// Activate BT -> iResultPckg = ETrue 
    }                                           // No BT Activation -> iResultPckg  EFalse) 

// ----------------------------------------------------------------------------
// CBtOnQuery::RunError()
// Handles errors which is throwed by RunL() function. 
// ----------------------------------------------------------------------------
//
void CBtOnQuery::RunError()
    {
	// No implementation required										
    }

// ----------------------------------------------------------------------------
// CBtOnQuery::RunError()
// Cancel any outstanding requests.
// ----------------------------------------------------------------------------
//
void CBtOnQuery::DoCancel()
    {
    // No implementation required
    }

// End of file
