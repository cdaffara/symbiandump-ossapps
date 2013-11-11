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


#include "cidlemode.h"
#include "muimodeobserver.h"
#include "muimode.h"
#include "mprintsettings.h"
#include "cuiprotocolcontroller.h"
#include "ciffactory.h"
#include "mdiscoveryobserver.h"
#include "clog.h"

CIdleMode* CIdleMode::NewL( MUIModeObserver* aUIModeObserver,
                            CIFFactory& aIFFactory )
    {
    CIdleMode* self = NewLC( aUIModeObserver, aIFFactory );
    CleanupStack::Pop( self );
    return self;
    }

CIdleMode* CIdleMode::NewLC( MUIModeObserver* aUIModeObserver,
                             CIFFactory& aIFFactory )
    {
    CIdleMode* self = new ( ELeave ) CIdleMode( aUIModeObserver );
    CleanupStack::PushL( self );
    self->ConstructL( aIFFactory );
    return self;
    }

// Default constructor
CIdleMode::CIdleMode( MUIModeObserver* aUIModeObserver )
	{
    iUIModeObserver = aUIModeObserver;
    }

// 2nd phase constructor
void CIdleMode::ConstructL( CIFFactory& aIFFactory )
    {
    iIFFactory = &aIFFactory;
    iUIProtocolController = CUIProtocolController::NewL( this, aIFFactory );
    }

// Destructor
CIdleMode::~CIdleMode()
    {
    delete iUIProtocolController;
	}

void CIdleMode::SearchDefaultPrinterL()
    {
    LOG("void CIdleMode::SearchDefaultPrinterL() START");
    TBool printerFound = iUIProtocolController->SearchDefaultPrinterL();
    // If printer was found change to settings view
    if( printerFound )
        {  
        LOG("void CIdleMode::SearchDefaultPrinterL() Printer found");
        iUIModeObserver->ChangeViewL( MUIModeObserver::SettingsView );
        }
    // Printer was not found or discovery cancelled
    else 
        {
        LOG("void CIdleMode::SearchDefaultPrinterL() Printer not found");
        iUIModeObserver->ChangeViewL( MUIModeObserver::SettingsView );
        }
    LOG("void CIdleMode::SearchDefaultPrinterL() END");
    }

TBool CIdleMode::ChangeDefaultPrinterL( TInt aProtocol )
    {
    TBool printerFound = iUIProtocolController->
                                ChangeDefaultPrinterL( aProtocol );

    return printerFound;
    }


void CIdleMode::UsbCableDisconnected()
    {
    if ( iIFFactory->SettingsIF()->GetCurrentPrinterProtocol() == 
         MDiscoveryObserver::EUSB )
    	{
    	// No leaving function. Ignore leave. 
	    TRAP_IGNORE( iUIModeObserver->ChangeViewL( MUIModeObserver::ImageSelectionView ));    	
    	}
    }

//  End of File
