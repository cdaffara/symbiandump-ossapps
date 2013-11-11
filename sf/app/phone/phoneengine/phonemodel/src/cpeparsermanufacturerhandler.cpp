/*
* Copyright (c) 2006-2008 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Battery state monitoring class methods implementation
*
*/
 

//  ==================== INCLUDE FILES  ====================
#include <mpecallhandling.h>
#include <mpedatastore.h>
#include <talogger.h>
#include "cpeparsermanufacturerhandler.h"
#include "mpephonemodelinternal.h"

// ================= MEMBER FUNCTIONS =======================

// C++ default constructor can NOT contain any code, that
// might leave.
//
CPEParserManufacturerHandler::CPEParserManufacturerHandler( 
        MPEPhoneModelInternal& aModel,
        MPECallHandling& aCallHandling,
        RFs& aFsSession )
        : iModel( aModel ),
            iCallHandling( aCallHandling ),
            iFsSession( aFsSession )
    {
    TEFLOGSTRING( KTAOBJECT, "PE CPEParserManufacturerHandler::CPEParserManufacturerHandler()" );
    }

// Destructor
CPEParserManufacturerHandler::~CPEParserManufacturerHandler()
    {    
    TEFLOGSTRING( KTAOBJECT, "PE CPEParserManufacturerHandler::~CPEParserManufacturerHandler()" );
    }

// -----------------------------------------------------------------------------
// CPhoneManufacturerHandler::ProcessCommandL
// -----------------------------------------------------------------------------
//
void CPEParserManufacturerHandler::ProcessCommandL( 
        TUint aCommand )       // aCommand command enumeration.
    {
    TEFLOGSTRING( KTAINT, "PE CPEParserManufacturerHandler::ProcessCommandL" );

    switch ( aCommand )
        {
        case EBadPinChange:    
            {
            iModel.SendMessage( MEngineMonitor::EPEMessageBadPin1Change );
            break;
            }
        case EBadPin2Change:
            {
            iModel.SendMessage( MEngineMonitor::EPEMessageBadPin2Change );
            break;
            }
        case EBadPinUnblock:    
            {
            iModel.SendMessage( MEngineMonitor::EPEMessageBadPin1Unblock );
            break;
            }
        case EBadPin2Unblock:
            {
            iModel.SendMessage( MEngineMonitor::EPEMessageBadPin2Unblock );
            break;
            }
        case EActivateRfsNormal:
            {
            iModel.SendMessage( MEngineMonitor::EPEMessageActivateRfsNormal );
            break;
            }
        case EActivateRfsDeep:    
            {
            iModel.SendMessage( MEngineMonitor::EPEMessageActivateRfsDeep );
            break;
            }
        case EShowBtAddress: // *#2820#
            {
            iModel.SendMessage( MEngineMonitor::EPEMessageShowBTDeviceAddress );
            break;
            }
        case EBTLoopback: // *#9990#
            {
            iModel.SendMessage( MEngineMonitor::EPEMessageShowBTLoopback );
            break;
            }
        case EShowWlanMac: // *#62209526#
            {
            iModel.SendMessage( MEngineMonitor::EPEMessageShowWlanMacAddress );
            break;
            }
        default:
            {
            TEFLOGSTRING( KTAERROR, "PE CPEGSMPARSERMANUFACTURERHANDLER::PROCESSCOMMANDL: UNEXPECTED COMMAND" );
            break;
            }
        }
    }

// -----------------------------------------------------------------------------
// CPEParserManufacturerHandler::ProcessDebugL
// -----------------------------------------------------------------------------
//
void CPEParserManufacturerHandler::ProcessDebugL(
        const TDesC& aCode
        )
    {
    TEFLOGSTRING( KTAINT, "PE CPEParserManufacturerHandler::ProcessDebugL" );
    
    if ( aCode.Length() )
        {
        TInt cmd = KErrNotFound;
        TLex( aCode.Left( 1 ) ).Val( cmd ); // first character.

        TInt param = KErrNotFound;
        TLex( aCode.Mid( 1 ) ).Val( param ); // rest

        switch ( cmd )
            {
            case EPhoneCmdDebugPrint:
                {
                //Copied from S60 Telephony. To be migrated to CoTe
                TEFLOGSTRING( KTAERROR, "PE CPEGSMPARSERMANUFACTURERHANDLER::PROCESSDEBUGL EPHONECMDDEBUGPRINT" );
                }
                break;

            case EPhoneCmdHeapFailure:
                {
#ifdef PHONE_HEAP_FAILURE_SUPPORT
                if ( param <= 0 )
                    {
                    // Reset alloc failure
                    User::__DbgSetAllocFail(
                        RHeap::EUser,
                        RHeap::ENone,
                        1 );
                    }
                else
                    {
                    // Set heap failure
                    User::__DbgSetAllocFail(
                        RHeap::EUser,
                        RHeap::ERandom,
                        param );
                    }
#endif // PHONE_HEAP_FAILURE_SUPPORT
                } 
                break;

            default:
                break;
            }
        }
    }

// End of File
