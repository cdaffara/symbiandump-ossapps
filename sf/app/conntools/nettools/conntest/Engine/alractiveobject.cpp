/*
* Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description: conntest application
*
*/

/**
@file alractiveobject.cpp
ALR application active object.
*/

// INCLUDE FILES
#include <e32cons.h>
#include <comms-infras/mobilitymessagesecom.h>
#include <mobilitymessages.h>
#include "alractiveobject.h"
#include "uinotify.h"

using namespace ESock;

const TInt KCommsMobilityMaxMsgLength = 256;  //Max mobility message size is much less than that

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CALRActiveObject::NewL
// -----------------------------------------------------------------------------
//
CALRActiveObject* CALRActiveObject::NewL(
            MUINotify& aConsole, 
            MMobilityProtocolResp* aApplication)
    {
    CALRActiveObject* self =
        new ( ELeave ) CALRActiveObject(aConsole, aApplication);
    CleanupStack::PushL( self );
    self->ConstructL(aConsole);
    CleanupStack::Pop( self );
    return self;
    }

// -----------------------------------------------------------------------------
// CALRActiveObject::ConstructL
// -----------------------------------------------------------------------------
//
void CALRActiveObject::ConstructL(MUINotify& /*aConsole*/)
    {
    iResponseBuf.CreateL( KCommsMobilityMaxMsgLength );
    }

// -----------------------------------------------------------------------------
// CALRActiveObject::CALRActiveObject
// -----------------------------------------------------------------------------
//
CALRActiveObject::CALRActiveObject(MUINotify& aConsole, MMobilityProtocolResp* aApplication ) 
    : CActive( CActive::EPriorityStandard ),
    iConsole(aConsole), iApplication(aApplication)
    {
    CActiveScheduler::Add( this );
    }

// -----------------------------------------------------------------------------
// CALRActiveObject::~CALRActiveObject
// -----------------------------------------------------------------------------
//
CALRActiveObject::~CALRActiveObject()
    {
    Cancel();
	iExtApi.Close();
	iResponseBuf.Close();
    }

// -----------------------------------------------------------------------------
// CALRActiveObject::OpenL
// -----------------------------------------------------------------------------
//
void CALRActiveObject::OpenL( RCommsSubSession& aExtensionProvider )
    {
    User::LeaveIfError( iExtApi.Open( aExtensionProvider ) );
    
    //iConsole.PrintNotify(_L("\r\nRegister for mobility notification "));

    iExtApi.RegisterForMobilityNotification( iResponseBuf, iStatus );
    
    //iConsole.PrintNotify(_L("\r\nRegistration successful "));

    SetActive();
    }

// -----------------------------------------------------------------------------
// CALRActiveObject::MigrateToPreferredCarrier
// -----------------------------------------------------------------------------
//
void CALRActiveObject::MigrateToPreferredCarrier()
	{
	iExtApi.MigrateToPreferredCarrier();
	}

// -----------------------------------------------------------------------------
// CALRActiveObject::IgnorePreferredCarrier
// -----------------------------------------------------------------------------
//
void CALRActiveObject::IgnorePreferredCarrier()
	{
    iExtApi.IgnorePreferredCarrier();
	}

// -----------------------------------------------------------------------------
// CALRActiveObject::NewCarrierAccepted
// -----------------------------------------------------------------------------
//
void CALRActiveObject::NewCarrierAccepted()
	{
    iExtApi.NewCarrierAccepted();
	}

// -----------------------------------------------------------------------------
// CALRActiveObject::NewCarrierRejected
// -----------------------------------------------------------------------------
//
void CALRActiveObject::NewCarrierRejected()
	{
    iExtApi.NewCarrierRejected();
	}

// -----------------------------------------------------------------------------
// CALRActiveObject::RunL
// -----------------------------------------------------------------------------
//
void CALRActiveObject::RunL()
    {
    if( iStatus != KErrNone )
        { 
      //  iConsole.Printf(_L("\niStatus: %i\n"), iStatus.Int() );
        iApplication->Error( iStatus.Int() );
//          virtual void Error(TInt aError) = 0;

        return;
        }

	//iConsole.PrintNotify(_L("\r\nReceived notification"));
    CCommsApiExtensionMsg* msg = CCommsApiExtensionMsg::NewL( iResponseBuf );

    //check types
    STypeId preferredCarrierAvailableTID  = STypeId::CreateSTypeId( ESock::KMobilityMessagesImplementationUid, ESock::EPreferredCarrierAvailable );
    STypeId newCarrierActiveTID = STypeId::CreateSTypeId( ESock::KMobilityMessagesImplementationUid, ESock::ENewCarrierActive );
    
    if (msg->IsTypeOf(preferredCarrierAvailableTID))
        {
        CPreferredCarrierAvailable& pca = reinterpret_cast<CPreferredCarrierAvailable&>( *msg );
        iApplication->PreferredCarrierAvailable( pca.OldAPInfo(),
                                                pca.NewAPInfo(),
                                                pca.IsUpgrade(),
                                                pca.IsSeamless() );         
//        virtual void PreferredCarrierAvailable(TAccessPointInfo aOldAP, TAccessPointInfo aNewAP, TBool aIsUpgrade, TBool aIsSeamless) = 0;

        //MigrateToPreferredCarrier();
        }
    else if (msg->IsTypeOf(newCarrierActiveTID))
        {
        CNewCarrierActive& nca = reinterpret_cast<CNewCarrierActive&>( *msg );
        iApplication->NewCarrierActive( nca.NewAPInfo(), nca.IsSeamless() );
//        virtual void NewCarrierActive(TAccessPointInfo aNewAP, TBool aIsSeamless) = 0;
      	//NewCarrierAccepted();
        }
  
    delete msg;

    iExtApi.RegisterForMobilityNotification( iResponseBuf, iStatus );

    SetActive();
    }

// -----------------------------------------------------------------------------
// CALRActiveObject::DoCancel
// -----------------------------------------------------------------------------
//
void CALRActiveObject::DoCancel()
    {
    iExtApi.CancelMobilityNotification();
    }

// -----------------------------------------------------------------------------
// CALRActiveObject::RunError
// -----------------------------------------------------------------------------
//
TInt CALRActiveObject::RunError(TInt aError)
    {
    //The notification has not been sent to the client because of KErrNoMemory
    //We may need to provide an inplace construction for the messages to exclude this possibility.
    return aError;
    }

//  End of File
