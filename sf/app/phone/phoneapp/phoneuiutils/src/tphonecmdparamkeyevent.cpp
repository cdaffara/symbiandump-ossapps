/*
* Copyright (c) 2005 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description: Implementation of TPhoneCmdParamKeyEvent class.
*
*/


// INCLUDE FILES

#include "tphonecmdparamkeyevent.h"   

// ================= MEMBER FUNCTIONS =======================

// C++ default constructor can NOT contain any code, that
// might leave.
//
EXPORT_C TPhoneCmdParamKeyEvent::TPhoneCmdParamKeyEvent() :
    TPhoneCommandParam(),
    iEventCode( EEventNull )
    {
    iParamId = EPhoneParamIdKeyEvent;
    }

// ---------------------------------------------------------
// TPhoneCmdParamKeyEvent::SetKeyEvent
// Sets the key event
// (other items were commented in a header).
// ---------------------------------------------------------
//
EXPORT_C void TPhoneCmdParamKeyEvent::SetKeyEvent( const TKeyEvent& aKeyEvent )
   {
   iKeyEvent = aKeyEvent;   
   }

// ---------------------------------------------------------
// TPhoneCmdParamKeyEvent::SetEventCode
// Sets the key event code
// (other items were commented in a header).
// ---------------------------------------------------------
//
EXPORT_C void TPhoneCmdParamKeyEvent::SetEventCode( TEventCode aEventCode )
   {
   iEventCode = aEventCode;   
   }

// ---------------------------------------------------------
// TPhoneCmdParamKeyEvent::KeyEvent
// Returns the key event
// (other items were commented in a header).
// ---------------------------------------------------------
//
EXPORT_C const TKeyEvent TPhoneCmdParamKeyEvent::KeyEvent() const
   {
   return iKeyEvent;
   }

// ---------------------------------------------------------
// TPhoneCmdParamKeyEvent::EventCode
// Returns the key event code
// (other items were commented in a header).
// ---------------------------------------------------------
//
EXPORT_C TEventCode TPhoneCmdParamKeyEvent::EventCode() const
   {
   return iEventCode;
   }

//  End of File  
