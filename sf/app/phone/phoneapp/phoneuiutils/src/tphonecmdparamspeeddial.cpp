/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description: Implementation of TPhoneCmdParamSpeedDial class.
*
*/


// INCLUDE FILES
#include "tphonecmdparamspeeddial.h"   
#include "phoneui.pan"

// ================= MEMBER FUNCTIONS =======================

// -----------------------------------------------------------------------------
// TPhoneCmdParamSpeedDial::TPhoneCmdParamSpeedDial
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
EXPORT_C TPhoneCmdParamSpeedDial::TPhoneCmdParamSpeedDial() :
    TPhoneCommandParam(),
    iPhoneNumber( NULL ),
    iNumberType( EPhoneNumberTypeNotFound ),
    iLocation( KErrNotFound ),
    iServiceId( 0 )
    {
    iParamId = EPhoneParamIdSpeedDial;
    }

// ---------------------------------------------------------
// TPhoneCmdParamSpeedDial::SetPhoneNumber
// Sets the phone number.
// (other items were commented in a header).
// ---------------------------------------------------------
//
EXPORT_C void TPhoneCmdParamSpeedDial::SetPhoneNumber( TPtr* aPhoneNumber )
    {
    __ASSERT_DEBUG( aPhoneNumber, 
        Panic( EPhoneUtilsParameterNotInitialized ) );
    iPhoneNumber = aPhoneNumber;   
    }

// ---------------------------------------------------------
// TPhoneCmdParamSpeedDial::PhoneNumber
// Returns the phone number.
// (other items were commented in a header).
// ---------------------------------------------------------
//
EXPORT_C TPtr* TPhoneCmdParamSpeedDial::PhoneNumber()
    {
    __ASSERT_DEBUG( iPhoneNumber, 
        Panic( EPhoneUtilsInvariant ) );
    return iPhoneNumber;
    }

// ---------------------------------------------------------
// TPhoneCmdParamSpeedDial::SetNumberType
// Sets the phone number's type.
// (other items were commented in a header).
// ---------------------------------------------------------
//
EXPORT_C void TPhoneCmdParamSpeedDial::SetNumberType( TPhoneNumberType aNumberType )
   {
   iNumberType = aNumberType;   
   }

// ---------------------------------------------------------
// TPhoneCmdParamSpeedDial::NumberType
// Returns the phone number's type.
// (other items were commented in a header).
// ---------------------------------------------------------
//
EXPORT_C TPhoneNumberType TPhoneCmdParamSpeedDial::NumberType()
   {
   return iNumberType;
   }

// ---------------------------------------------------------
// TPhoneCmdParamSpeedDial::SetLocation
// Sets the speed dial location.
// (other items were commented in a header).
// ---------------------------------------------------------
//
EXPORT_C void TPhoneCmdParamSpeedDial::SetLocation( TInt aLocation )
    {
    __ASSERT_DEBUG( aLocation >= 0, 
        Panic( EPhoneUtilsParameterNotInitialized ) );
    iLocation = aLocation;   
    }

// ---------------------------------------------------------
// TPhoneCmdParamSpeedDial::Location
// Returns the speed dial location.
// (other items were commented in a header).
// ---------------------------------------------------------
//
EXPORT_C TInt TPhoneCmdParamSpeedDial::Location()
    {
    __ASSERT_DEBUG( iLocation >= 0, 
        Panic( EPhoneUtilsInvariant ) );
    return iLocation;
    }

// ---------------------------------------------------------
// TPhoneCmdParamSpeedDial::SetServiceId
// Sets the serviceId
// ---------------------------------------------------------
//
EXPORT_C void TPhoneCmdParamSpeedDial::SetServiceId( TUint aServiceId )
    {
    iServiceId = aServiceId;
    }

// ---------------------------------------------------------
// TPhoneCmdParamSpeedDial::ServiceId
// Returns the serviceId
// ---------------------------------------------------------
//
EXPORT_C TUint TPhoneCmdParamSpeedDial::ServiceId() const
    {
    return iServiceId;
    }

//  End of File
