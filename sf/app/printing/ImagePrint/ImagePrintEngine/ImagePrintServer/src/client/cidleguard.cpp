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


#include "cidleguard.h"
#include "rimageprintclient.h"
#include "imageprint.h"
#include "tidleguarddata.h"
#include "clog.h"

EXPORT_C CIdleGuard* CIdleGuard::NewL( RImagePrintClient& aClient )
    {	
	CIdleGuard* self = new ( ELeave ) CIdleGuard( aClient );
    CleanupStack::PushL( self );
    self->ConstructL();
	CleanupStack::Pop();    // self
    return self;
    }

CIdleGuard::CIdleGuard( RImagePrintClient& aClient ) : 
							CActive( CActive::EPriorityStandard ),
							iClient( aClient )
    {
    CActiveScheduler::Add( this );
    }

EXPORT_C CIdleGuard::~CIdleGuard()
    {  
    Cancel();
    delete iData;
    }

void CIdleGuard::ConstructL()
    {
    iData = new (ELeave) TIdleGuardData;
    }

EXPORT_C void CIdleGuard::Guard( MIdleObserver* aObserver )
	{
	Cancel();
	iObserver = aObserver;
	iClient.RegisterIdleObserver( *iData, iStatus );
	SetActive();
	}		
	
void CIdleGuard::RunL()
	{
	LOG1("CIdleGuard::RunL iStatus.Int(): %d", iStatus.Int());
	LOG1("CIdleGuard::RunL iData->iEvent.iProtocol: %d", iData->iEvent.iProtocol);
	LOG1("CIdleGuard::RunL iData->iEvent.iSeverity: %d", iData->iEvent.iSeverity);
	LOG1("CIdleGuard::RunL iData->iEvent.iEventType: %d", iData->iEvent.iEventType);		
	LOG1("CIdleGuard::RunL iData->iError: %d", iData->iError);
	LOG1("CIdleGuard::RunL iData->iMsgCode: %d", iData->iMsgCode);	
	if( iStatus == KErrNone && iObserver )
		{
		iObserver->StatusEvent( iData->iEvent, iData->iError, iData->iMsgCode );
		iClient.RegisterIdleObserver( *iData, iStatus );	
		SetActive();
		}	
	}

void CIdleGuard::DoCancel()
	{
	iClient.CancelRegisterIdleObserver();		
	}

//  End of File
