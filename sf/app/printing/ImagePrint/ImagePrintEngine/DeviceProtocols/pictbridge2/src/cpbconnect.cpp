/*
* Copyright (c) 2002-2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Contains the CPBConnect class definition.
*
*/


#include <e32std.h>
#include <pictbridge.h>

#include "cpbconnect.h"
#include "mpbobserver.h"
#include "cpbevent.h"
#include "clog.h"

CPBConnect* CPBConnect::NewL( CDpsEngine& aEngine, MPBObserver& aObserver )
    {
    LOG("CPBConnect::NewL begin");
    CPBConnect* self = new (ELeave) CPBConnect( aEngine, aObserver );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop();	// self
    LOG("CPBConnect::NewL end");
    return self;
    }    
    
CPBConnect::CPBConnect(CDpsEngine& aEngine, MPBObserver& aObserver) :
CActive( CActive::EPriorityStandard ), iDpsEngine(aEngine), iObserver( aObserver )
    {
    LOG("CPBConnect::CPBConnect begin");
    CActiveScheduler::Add( this );
    LOG("CPBConnect::CPBConnect end");
    }    
    
CPBConnect::~CPBConnect()
    {
    LOG("CPBConnect::~CPBConnect begin");
    Cancel();
    delete iEvent;
    LOG("CPBConnect::~CPBConnect end");
    }    
    
void CPBConnect::ConstructL()
    {
    LOG("CPBConnect::ConstructL begin");
    iEvent = CPBEvent::NewL( iDpsEngine, iObserver );
    iDpsEngine.SetPrintMode( iStatus );
    SetActive(); 
    LOG("CPBConnect::ConstructL end");
    }
    
    
void CPBConnect::RunL()        
    {
    LOG1("CPBConnect::RunL begin, status %d", iStatus.Int() );
    switch ( iStatus.Int() )
        {
         case CDpsEngine::ENotConnected:
    		iObserver.ConnectionNotifyL( iStatus.Int() );
            iDpsEngine.ConnectStateNotify( iStatus );
            SetActive();
        	break;        
        case CDpsEngine::EPrinterConnected:
        	iObserver.ConnectionNotifyL( iStatus.Int() );
            iDpsEngine.ConnectStateNotify( iStatus );
            SetActive();            
            iEvent->Listen();
        	break;        
        case CDpsEngine::EPrinterDisconnected:
            iEvent->Cancel();
            iObserver.ConnectionNotifyL( iStatus.Int() );
            iDpsEngine.SetPrintMode( iStatus );
            SetActive();    
	        break;        
        case CDpsEngine::EOtherConnected:
        	iObserver.ConnectionNotifyL( iStatus.Int() );
            iDpsEngine.ConnectStateNotify( iStatus );
            SetActive();
        	break;                
        case CDpsEngine::EWrongPrintModeConnected:   
            iObserver.ConnectionNotifyL( iStatus.Int() );
            iDpsEngine.ConnectStateNotify( iStatus );
            SetActive();
            iEvent->Listen();
	        break;
        default:
	        break;
        }
    LOG("CPBConnect::RunL end");
    }
    
void CPBConnect::DoCancel()
    {
    LOG("CPBConnect::DoCancel begin");
    iDpsEngine.CancelPrintMode();
    LOG("CPBConnect::DoCancel end");
    }    

//  End of File
