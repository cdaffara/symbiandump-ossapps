/*
* Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
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
* Description: Implementation for server side session object of
*              Screensaver Display Control Server
*
*/

#include <power_save_display_mode.h>
#include <hal.h>

#include "snsrdisplaycontrolsession.h"
#include "snsrdisplaycontrolcommon.h"


// ======== MEMBER FUNCTIONS ========


// ---------------------------------------------------------------------------
// CSnsrDisplayControlSession
// ---------------------------------------------------------------------------
//
CSnsrDisplayControlSession::CSnsrDisplayControlSession() : 
    CSession2()
    {
    }


// ---------------------------------------------------------------------------
// ConstructL
// ---------------------------------------------------------------------------
//
void CSnsrDisplayControlSession::ConstructL()
    {
    iPowerSave = CPowerSaveDisplayMode::NewL();
    
    // Create pixel buffer that can hold the full screen picture of our 
    // display device.
    TInt x = 0;
    TInt y = 0;
    HAL::Get(HALData::EDisplayXPixels, x);
    HAL::Get(HALData::EDisplayYPixels, y);
    iPowerSavePixelBuffer = HBufC::NewL(x*y);
    iPowerSavePixelBuffer->Des().FillZ();
    }


// ---------------------------------------------------------------------------
// NewL
// ---------------------------------------------------------------------------
//
EXPORT_C CSnsrDisplayControlSession* CSnsrDisplayControlSession::NewL()
    {
    CSnsrDisplayControlSession* self = new (ELeave) CSnsrDisplayControlSession();
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }


// ---------------------------------------------------------------------------
// ~CSnsrDisplayControlSession
// ---------------------------------------------------------------------------
//
CSnsrDisplayControlSession::~CSnsrDisplayControlSession()
    {
    delete iPowerSave;
    delete iPowerSavePixelBuffer;
    }

// ---------------------------------------------------------------------------
// ServiceL
// ---------------------------------------------------------------------------
//
void CSnsrDisplayControlSession::ServiceL( const RMessage2& aMessage )
    {
    TInt func = aMessage.Function();
    RDEBUG1( "func", func )
    
    
    TInt err( KErrNone );
    
    switch ( func )
        {
        case ESnsrDispCtrlSrvDisplayOff:
            {
            RDEBUG( "Switching display off" )
            
            // Disable touch
            TInt err1 = HAL::Set( HALData::EPenState, 0 );
            RDEBUG1( "disable touch err", err1 )
            
            // Display off
            TInt err2 = HAL::Set( HALData::EDisplayState, 0 );
            RDEBUG1( "display off err", err2 )
            
            err = err1 || err2;
            break;
            }
        case ESnsrDispCtrlSrvDisplayLowPower:
            {
            RDEBUG( "Switching display to low power mode" )
            
            // Disable touch
            TInt err1 = HAL::Set( HALData::EPenState, 0 );
            RDEBUG1( "disable touch err", err1 )
            
            // Enable low power mode
            TInt startRow = aMessage.Int0();
            TInt endRow = aMessage.Int1();
            RDEBUG1( "first active row", startRow )
            RDEBUG1( "last active row", endRow )
            
            // TODO: for now, we pass a zero-filled pixel buffer to power save API.
            // This works fine with our reference hardware but some types of displays
            // might require passing the actual screen contents in this buffer.
            TUint16* ptr = const_cast<TUint16*>( iPowerSavePixelBuffer->Ptr() );
            TInt err2 = iPowerSave->Set(startRow, endRow, ptr);
            RDEBUG1( "enable low power err", err2 )
            
            // Display on
            TInt err3 = HAL::Set( HALData::EDisplayState, 1 );
            RDEBUG1( "display on err", err3 );
            
            err = err1 || err2 || err3;
            break;
            }
        case ESnsrDispCtrlSrvDisplayFullPower:
            {
            RDEBUG( "Switching display to full power mode" )
            
            // Enable touch
            TInt err1 = HAL::Set( HALData::EPenState, 1 );
            RDEBUG1( "enable touch err", err1 )
            
            // Low power off
            TInt err2 = iPowerSave->Exit();
            RDEBUG1( "disable low power err", err2 );
            
            // Display on
            TInt err3 = HAL::Set( HALData::EDisplayState, 1 );
            RDEBUG1( "display on err", err3 )
            
            err = err1 || err2 || err3;
            break;
            }
        default:
            {
            err = KErrNotSupported;
            }
        }
    
    aMessage.Complete(err);
    }

