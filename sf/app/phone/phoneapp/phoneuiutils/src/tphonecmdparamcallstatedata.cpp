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
* Description: Implementation of TPhoneCmdParamCallStateData class.
*
*/


// INCLUDE FILES
#include "tphonecmdparamcallstatedata.h"   

// ================= MEMBER FUNCTIONS =======================

// -----------------------------------------------------------------------------
// TPhoneCmdParamCallStateData::TPhoneCmdParamCallStateData
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
EXPORT_C TPhoneCmdParamCallStateData::TPhoneCmdParamCallStateData() :
    TPhoneCommandParam(),
    iCallState( 0 ),
    iCallId( KErrNotFound )
    {
    iParamId = EPhoneParamIdCallStateData;
    }


// ---------------------------------------------------------
// TPhoneCmdParamCallStateData::SetCallState
// Sets the call header call state
// (other items were commented in a header).
// ---------------------------------------------------------
//
EXPORT_C void TPhoneCmdParamCallStateData::SetCallState( TInt aCallState )
   {
   iCallState = aCallState;
   }

// ---------------------------------------------------------
// TPhoneCmdParamCallStateData::CallState
// Returns the call header call state
// (other items were commented in a header).
// ---------------------------------------------------------
//
EXPORT_C TInt TPhoneCmdParamCallStateData::CallState() const
   {
   return iCallState;
   }

// ---------------------------------------------------------
// TPhoneCmdParamCallStateData::SetCallState
// Sets the call id
// (other items were commented in a header).
// ---------------------------------------------------------
//
EXPORT_C void TPhoneCmdParamCallStateData::SetCallId( TInt aCallId )
   {
   iCallId = aCallId;
   }

// ---------------------------------------------------------
// TPhoneCmdParamCallStateData::CallId
// Returns the call Id
// (other items were commented in a header).
// ---------------------------------------------------------
//
EXPORT_C TInt TPhoneCmdParamCallStateData::CallId() const
   {
   return iCallId;
   }
   
//  End of File
