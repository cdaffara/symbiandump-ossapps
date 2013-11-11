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


#include <imageprintapp.rsg>
#include <StringLoader.h>
#include <aknnotewrappers.h>

#include "tidleevent.h"
#include "cidleprinteventcatcher.h"
#include "crealfactory.h"
#include "midleprinteventobserver.h"
#include "cprinteventcatcher.h"
#include "mprintsettings.h"
#include "clog.h"
#include "mdiscoveryobserver.h"
#include "midleprinteventregisterif.h"
#include "mdiscoveryobserver.h"

CIdlePrintEventCatcher* CIdlePrintEventCatcher::NewL(
    CRealFactory* aFactory /*,
    MIdlePrintEventObserver* aIdleUIObserver*/ )
    {
    CIdlePrintEventCatcher* self = CIdlePrintEventCatcher::NewLC( aFactory/*, aIdleUIObserver*/ );
    CleanupStack::Pop();    // self
    return self;
    }

CIdlePrintEventCatcher* CIdlePrintEventCatcher::NewLC(
    CRealFactory* aFactory/*,
    MIdlePrintEventObserver* aIdleUIObserver*/ )
    {
    CIdlePrintEventCatcher* self =
        new ( ELeave ) CIdlePrintEventCatcher( aFactory/*, aIdleUIObserver*/ );
    CleanupStack::PushL( self );
    self->ConstructL();

    return self;
    }

// Constructor
CIdlePrintEventCatcher::CIdlePrintEventCatcher(
    CRealFactory* aFactory/*,
    MIdlePrintEventObserver* aIdleUIObserver*/ ) :
      iFactory( aFactory )/*,
      iIdleUIObserver( aIdleUIObserver )*/
    {
    }

// Destructor
CIdlePrintEventCatcher::~CIdlePrintEventCatcher()
    {
    iObservers.Reset();//)AndDestroy();
    }

// 2nd phase constructor
void CIdlePrintEventCatcher::ConstructL()
    {
    }

void CIdlePrintEventCatcher::StatusEvent( const TEvent &aEvent,
                                               TInt aError,
                                               TInt aMsgCode )
    {
    LOG1("CIdlePrintEventCatcher::StatusEvent, aEventType: %d", aEvent.iEventType);
    LOG2("CIdlePrintEventCatcher::StatusEvent, aError: %d, aMsgCode: %d", aError, aMsgCode );
    TInt obsCount = iObservers.Count();
    switch ( aEvent.iEventType )
        {
        ///The protocol has made a connection to the printer, device or transport
        case EConnect:
            {
            LOG( "CIdlePrintEventCatcher::StatusEvent CONNECT " );
            if ( (TUint)aEvent.iProtocol == KImagePrint_PrinterProtocol_PictBridge )
                {
                for( TInt i=0; i<obsCount; i++ )
                    {
                    iObservers[i]->UsbPrinterConnected();                    
                    }
                iFactory->SettingsIF()->SetUsbState( ETrue );
                }
            break;
            }
		///The protocol has been disconnected from the printer, device or transport
		case EDisconnect:
            {
            if ( (TUint)aEvent.iProtocol == KImagePrint_PrinterProtocol_PictBridge )
                {
                // If idle print event is called while usb printing is still ongoing,
                // printing is cancelled.
                if ( iFactory->IsPrinting() &&
                	 ( iFactory->SettingsIF()->GetCurrentPrinterProtocol() ==
                	   MDiscoveryObserver::EUSB ) && aError != KErrDisconnected)
                    {
                    LOG2("printing pictbridge, aError = %d, aMsgCode = %d", aError, aMsgCode );
                    iFactory->PrintEventObserverIF()->PrintJobErrorEvent( KErrDisconnected, KErrNone );
                    iFactory->SettingsIF()->SetUsbState( EFalse );
                    }
                else
                    {
                    LOG2("not printing or not pictbridge, aError = %d, aMsgCode = %d", aError, aMsgCode );                    
                    if ( ( aError == KErrDisconnected) &&
                       	( iFactory->SettingsIF()->GetCurrentPrinterProtocol() == 
                       	  MDiscoveryObserver::EUSB ) )                   
                    	{
                    	LOG( "CIdlePrintEventCatcher::StatusEvent 'Connection to printer lost' " );
                        
                    	TInt error ( KErrNone );                    	
                    	TRAP( error, iBuf = StringLoader::LoadL( R_NOTE_DISCONNECT_PRINT_ERROR ));
                    	CAknErrorNote* errornote = new CAknErrorNote( ETrue );  
						TInt err ( KErrNone );
		        		TRAP( err, errornote->ExecuteLD( *iBuf ));		        				        			
											
                    	}
                    for( TInt i=0; i<obsCount; i++ )
                        {
                        iObservers[i]->UsbPrinterDisconnected();                    
                        }
                    if ( iBuf )
                    	{
                    	delete iBuf;
                    	iBuf = NULL;
                    	}
                    
                    }
                }
            break;
            }
	
		///The event is notifying a change in the device status,
        ///or is reporting an error sent from the device
		case EPrinterStatus:
            {
            if ( EWarning==aEvent.iSeverity || EFatal==aEvent.iSeverity )
                {
                for( TInt i=0; i<obsCount; i++ )
                    {
                    iObservers[i]->UsbStateChanged( ENOUSBEvent, aMsgCode );                    
                    }
                }
            break;
            }
		
		///The event is notifying that an asynchronous leave has occurred
		///in server and user should be notified in some way            
		case EAsynchronousLeave:
            {
            LOG( "CIdlePrintEventCatcher::StatusEvent EAsynchronousLeave!");
            LOG2( "CIdlePrintEventCatcher::StatusEvent protocol: %d, error: %d",
            		aEvent.iProtocol, aError );
            for( TInt i=0; i<obsCount; i++ )
            	{
                iObservers[i]->AsynchronousServerLeave( aError );                    
                }
            CCoeEnv::Static()->HandleError( aError );                
            break;
            }
        default:
            {
            // Should not be never called
            break;
            }
        }
    LOG("CIdlePrintEventCatcher::StatusEvent END");    
    }

void CIdlePrintEventCatcher::RegisterObserver( MIdlePrintEventObserver* aIdleObserver )
    {
    iObservers.Append( aIdleObserver );
    }

void CIdlePrintEventCatcher::UnRegisterObserver( MIdlePrintEventObserver* aIdleObserver )
    {
    TInt observerPosition = iObservers.Find( aIdleObserver );
    if( observerPosition != KErrNotFound )
        {
        iObservers.Remove( observerPosition );
        }
    }

//  End of File
