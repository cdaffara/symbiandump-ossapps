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


#include "cserveridleguard.h"
#include "cimageprintbody.h"
#include "tidleevent.h"
#include "clog.h"
#include "tmessagewrp2.h"

CServerIdleGuard* CServerIdleGuard::NewL( CImagePrintBody& aEngine )
    {	
	CServerIdleGuard* self = new ( ELeave ) CServerIdleGuard( aEngine );
    CleanupStack::PushL( self );
    self->ConstructL();
	CleanupStack::Pop();    // self
    return self;
    }

CServerIdleGuard::CServerIdleGuard( CImagePrintBody& aEngine ) : iEngine( aEngine ),
														iRequestActive( EFalse )
    {
    }

CServerIdleGuard::~CServerIdleGuard()
    { 
    LOG("CServerIdleGuard::~CServerIdleGuard begin");
    iBuffer.Close();
    LOG("CServerIdleGuard::~CServerIdleGuard end");  
    }

void CServerIdleGuard::ConstructL()
    {
    }

void CServerIdleGuard::StatusEvent( const TEvent &aEvent, TInt aError, TInt aMsgCode )
	{
	LOG("CServerIdleGuard::StatusEvent begin");		
	LOG1("CServerIdleGuard::StatusEvent aError: %d", aError);
	LOG1("CServerIdleGuard::StatusEvent aMsgCode: %d", aMsgCode);	
	LOG1("CServerIdleGuard::StatusEvent aEvent.iProtocol: %d", aEvent.iProtocol);
	LOG1("CServerIdleGuard::StatusEvent aEvent.iSeverity: %d", aEvent.iSeverity);
	LOG1("CServerIdleGuard::StatusEvent aEvent.iEventType: %d", aEvent.iEventType);				
	TRAPD( err, DoStatusEventL( aEvent, aError, aMsgCode ) );					
	LOG1("CServerIdleGuard::StatusEvent DoStatusEventL's TRAP err: %d", err);					
	Process( err );
	LOG("CServerIdleGuard::StatusEvent end");
	}
		
void CServerIdleGuard::Guard( TMessageWrp2& aMessage )
	{
	LOG("CServerIdleGuard::Guard begin");
	if( iMessage ) iMessage->SetDisposable( ETrue );			
	iMessage = &aMessage;			
	iRequestActive = ETrue;
	iEngine.RegisterIdleObserver( this );
	if( iBuffer.Count() )
		{
		LOG("CServerIdleGuard::Guard before Process");
		Process();
		LOG("CServerIdleGuard::Guard after Process");	
		}
	LOG("CServerIdleGuard::Guard end");	
	}
	
void CServerIdleGuard::Stop()
	{
	LOG("CServerIdleGuard::Stop begin");		
	if( iRequestActive )
		{
		LOG("CServerIdleGuard::Stop cancelling...");
		iMessage->Complete( KErrCancel );
		iRequestActive = EFalse;
		}
	iEngine.RegisterIdleObserver( NULL );		
	LOG("CServerIdleGuard::Stop end");
	}	
		
void CServerIdleGuard::DoStatusEventL( const TEvent &aEvent, TInt aError, TInt aMsgCode )
	{
	LOG("CServerIdleGuard::DoStatusEventL begin");
	TIdleGuardData data;
	data.iEvent = aEvent;
	data.iError = aError;
	data.iMsgCode = aMsgCode;
	User::LeaveIfError( iBuffer.Append( data ) );
	LOG("CServerIdleGuard::DoStatusEventL end");				
	}
		
void CServerIdleGuard::Process( TInt aErr )
	{
	LOG("CServerIdleGuard::Process begin");
	if( aErr )
		{
		LOG1("CServerIdleGuard::Process aErr: %d", aErr);
		iMessage->Complete( aErr );
		iRequestActive = EFalse;
		}
	else
		{
		LOG("CServerIdleGuard::Process calling DoProcessL");
		TRAPD( err, DoProcessL() );
		LOG1("CServerIdleGuard::Process DoProcessL's TRAP err: %d", err);
		if( err )
			{
			iMessage->Complete( err );
			iRequestActive = EFalse;
			}	
		}
	LOG("CServerIdleGuard::Process end");	
	}

void CServerIdleGuard::DoProcessL()
	{
	LOG("CServerIdleGuard::DoProcessL begin");
	if( iBuffer.Count() && iRequestActive )
		{
		LOG("CServerIdleGuard::DoProcessL step 1");																
		TPtr8 ptr(reinterpret_cast<TUint8*>(&(iBuffer[0])), sizeof(iBuffer[0]), sizeof(iBuffer[0]));
		LOG("CServerIdleGuard::DoProcessL step 2");
		iMessage->WriteL( 0, ptr );
		LOG("CServerIdleGuard::DoProcessL step 3");				
		iMessage->Complete( KErrNone );
		LOG("CServerIdleGuard::DoProcessL step 4");
		iRequestActive = EFalse;
		LOG("CServerIdleGuard::DoProcessL step 5");
		iBuffer.Remove( 0 );
		LOG("CServerIdleGuard::DoProcessL step 6");
		}
	LOG("CServerIdleGuard::DoProcessL end");	
	}

//  End of File
