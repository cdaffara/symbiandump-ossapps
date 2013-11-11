/*
* Copyright (c) 2005-2007 Nokia Corporation and/or its subsidiary(-ies).
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


#include "caiwmode.h"
#include "muimodeobserver.h"
#include "muimode.h"
#include "mprintsettings.h"
#include "cuiprotocolcontroller.h"
#include "ciffactory.h"
#include "mdiscoveryobserver.h" 

CAiwMode* CAiwMode::NewL( MUIModeObserver* aUIModeObserver,
                          CIFFactory& aIFFactory )
    {
    CAiwMode* self = NewLC( aUIModeObserver, aIFFactory );
    CleanupStack::Pop( self );
    return self;
    }

CAiwMode* CAiwMode::NewLC( MUIModeObserver* aUIModeObserver,
                           CIFFactory& aIFFactory )
    {
    CAiwMode* self = new ( ELeave ) CAiwMode( aUIModeObserver );
    CleanupStack::PushL( self );
    self->ConstructL( aIFFactory  );
    return self;
    }

// Default constructor
CAiwMode::CAiwMode( MUIModeObserver* aUIModeObserver  )
	{
    iUIModeObserver = aUIModeObserver;
    }

// 2nd phase constructor
void CAiwMode::ConstructL( CIFFactory& aIFFactory )
    {
    iIFFactory = &aIFFactory;
    iUIProtocolController = CUIProtocolController::NewL( this, aIFFactory );
    }

// Destructor
CAiwMode::~CAiwMode()
    {
    delete iUIProtocolController;
	}

void CAiwMode::SearchDefaultPrinterL()
    {
    iUIProtocolController->SearchDefaultPrinterL();  
    iUIModeObserver->ChangeViewL( MUIModeObserver::SettingsView );
    }

TBool CAiwMode::ChangeDefaultPrinterL( TInt aProtocol )
    {
    TBool printerFound = iUIProtocolController->
                                ChangeDefaultPrinterL( aProtocol );
 
    return printerFound;
    }

void CAiwMode::UsbCableDisconnected()
    {
    if ( iIFFactory->SettingsIF()->GetCurrentPrinterProtocol() == 
         MDiscoveryObserver::EUSB )
    	{    
    	iUIModeObserver->ExitApplication();
    	}
    }

//  End of File
