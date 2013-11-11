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
* Description:  
*
*/


#include <avkon.hrh>

#include "cbtpasswordquery.h"

CBtPasswordQuery::CBtPasswordQuery( MConfirmationNotify& aNotify ) : CActive( CActive::EPriorityStandard),
																	 iNotify (aNotify)
	{
	CActiveScheduler::Add( this );
	}

CBtPasswordQuery::~CBtPasswordQuery()
	{
	Cancel();
	iNotifier.Close();
	}

// ============================ MEMBER FUNCTIONS ==============================

// ----------------------------------------------------------------------------
// CBtPasswordQuery::NewL()
// Symbian two-phased constructor.
// ----------------------------------------------------------------------------
//
CBtPasswordQuery* CBtPasswordQuery::NewL( MConfirmationNotify& aNotify )  
	{
	CBtPasswordQuery* self = CBtPasswordQuery::NewLC( aNotify );
    CleanupStack::Pop( self );
    return self;
	}

// ----------------------------------------------------------------------------
// CBtPasswordQuery::NewLC()
// Symbian two-phased constructor.
// ----------------------------------------------------------------------------
//
CBtPasswordQuery* CBtPasswordQuery::NewLC( MConfirmationNotify& aNotify )
    {
    CBtPasswordQuery* self;
    self = new ( ELeave ) CBtPasswordQuery( aNotify );
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
   	}

// ----------------------------------------------------------------------------
// CBtPasswordQuery::ConstructL()
// Perform second phase construction of this object.
// ----------------------------------------------------------------------------
//
void CBtPasswordQuery::ConstructL()
    {
    	
    }

void CBtPasswordQuery::StartActiveObjectL()
	{
	
    //RNotifier notifier;
    User::LeaveIfError(iNotifier.Connect());
	
    // start asynchronic notifier -> prompt user passsword
    iNotifier.StartNotifierAndGetResponse(iStatus, KBTObexPasskeyQueryNotifierUid, iPckg, iResultPckg);
    
	// Start active object and set iStatus = KRequestPending
	SetActive();	
	}

void CBtPasswordQuery::RunL()
	{
    if( iStatus.Int() == KErrNone || iStatus.Int() == KErrCancel)
        {
      	iNotifier.CancelNotifier( KPowerModeSettingNotifierUid );
        // close connection to the notify server  
		iNotifier.Close();
		iNotify.PasswordQueryCompleteL( iResultPckg );
		}
	}

void CBtPasswordQuery::RunError()
	{
	
	}

void CBtPasswordQuery::DoCancel()
	{
  	iNotifier.CancelNotifier( KPowerModeSettingNotifierUid );
		
	}

// End of File
