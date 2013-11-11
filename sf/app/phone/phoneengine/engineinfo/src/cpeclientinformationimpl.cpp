/*
* Copyright (c) 2008-2008 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Phone client information for a single call
*
*/

#include <talogger.h>
#include "cpeclientinformationimpl.h"

// ---------------------------------------------------------------------------
// Two-phased constructor.
// ---------------------------------------------------------------------------
//
CPEClientInformationImpl* CPEClientInformationImpl::NewL()
    {
    CPEClientInformationImpl* self = new ( ELeave ) CPEClientInformationImpl;
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------------------------
// Constructor.
// ---------------------------------------------------------------------------
//
CPEClientInformationImpl::CPEClientInformationImpl()
    {
    Reset();
    }

// ---------------------------------------------------------------------------
// Second-phase constructor.
// ---------------------------------------------------------------------------
//
void CPEClientInformationImpl::ConstructL()
    {
    }

// ---------------------------------------------------------------------------
// Destructor.
// ---------------------------------------------------------------------------
//
CPEClientInformationImpl::~CPEClientInformationImpl()
    {
    iContactLink.Close();
    }

// ---------------------------------------------------------------------------
// Resets the state of this client information.
// ---------------------------------------------------------------------------
//
void CPEClientInformationImpl::Reset()
    {
    TEFLOGSTRING( KTAINT,"PE CPEClientInformationImpl::Reset" );
    iContactLink.Close();
    iName = KNullDesC;
    iNumber = KNullDesC;
    iCallType = EPECallTypeUninitialized;
    iAllowMatch = ETrue;    
    iShowNumber = EFalse;
    iEndOtherCalls = EFalse;    
    }

// ---------------------------------------------------------------------------
// From class MPEClientInformation.
// ---------------------------------------------------------------------------
//
void CPEClientInformationImpl::Set( const MPEClientInformation& aSource )
    {
    SetContactLink( aSource.ContactLink() );
    iName = aSource.Name();
    iNumber = aSource.Number();
    iCallType = aSource.CallType();
    iAllowMatch = aSource.AllowMatch();
    TEFLOGSTRING2( KTAINT,"PE CPEClientInformationImpl::Set, iAllowMatch: %d", iAllowMatch );    
    iShowNumber = aSource.ShowNumber();
    TEFLOGSTRING2( KTAINT,"PE CPEClientInformationImpl::Set, iShowNumber: %d", iShowNumber );
    iEndOtherCalls = aSource.EndOtherCalls();
    TEFLOGSTRING2( KTAINT,"PE CPEClientInformationImpl::Set, iEndOtherCalls: %d", iEndOtherCalls );    
    }

// ---------------------------------------------------------------------------
// From class MPEClientInformation.
// ---------------------------------------------------------------------------
//
void CPEClientInformationImpl::SetContactLink( const TDesC8& aLink )
    {
    iContactLink.Close();
    HBufC8* contactLink = aLink.Alloc(); // No leaving allowed here due to emergency call cases.
    if ( contactLink )
        {
        iContactLink.Assign( contactLink );
        }
    }

// ---------------------------------------------------------------------------
// From class MPEClientInformation.
// ---------------------------------------------------------------------------
//
const TDesC8& CPEClientInformationImpl::ContactLink() const
    {
    return iContactLink;
    }

// ---------------------------------------------------------------------------
// From class MPEClientInformation.
// ---------------------------------------------------------------------------
//
void CPEClientInformationImpl::SetName( const TName& aName )
    {
    iName = aName;
    }

// ---------------------------------------------------------------------------
// From class MPEClientInformation.
// ---------------------------------------------------------------------------
//
const TName& CPEClientInformationImpl::Name() const
    {
    return iName;
    }

// ---------------------------------------------------------------------------
// From class MPEClientInformation.
// ---------------------------------------------------------------------------
//
void CPEClientInformationImpl::SetNumber( const TPEPhoneNumber& aNumber )
    {
    iNumber = aNumber;
    }

// ---------------------------------------------------------------------------
// From class MPEClientInformation.
// ---------------------------------------------------------------------------
//
const TPEPhoneNumber& CPEClientInformationImpl::Number() const
    {
    return iNumber;
    }

// ---------------------------------------------------------------------------
// From class MPEClientInformation.
// ---------------------------------------------------------------------------
//
void CPEClientInformationImpl::SetCallType( TPECallType aType )
    {
    iCallType = aType;
    }

// ---------------------------------------------------------------------------
// From class MPEClientInformation.
// ---------------------------------------------------------------------------
//
TPECallType CPEClientInformationImpl::CallType() const
    {
    return iCallType;
    }

// ---------------------------------------------------------------------------
// From class MPEClientInformation.
// ---------------------------------------------------------------------------
//
void CPEClientInformationImpl::SetAllowMatch( TBool aAllow )
    {
    iAllowMatch = aAllow;
    TEFLOGSTRING2( KTAINT,
        "PE CPEClientInformationImpl::SetAllowMatch, iAllowMatch: %d", iAllowMatch );
    }

// ---------------------------------------------------------------------------
// From class MPEClientInformation.
// ---------------------------------------------------------------------------
//
TBool CPEClientInformationImpl::AllowMatch() const
    {
    TEFLOGSTRING2( KTAINT,
        "PE CPEClientInformationImpl::AllowMatch, iAllowMatch: %d", iAllowMatch );
    return iAllowMatch;
    }

// ---------------------------------------------------------------------------
// From class MPEClientInformation.
// ---------------------------------------------------------------------------
//
void CPEClientInformationImpl::SetShowNumber( TBool aShow )
    {
    iShowNumber = aShow;
    }

// ---------------------------------------------------------------------------
// From class MPEClientInformation.
// ---------------------------------------------------------------------------
//
TBool CPEClientInformationImpl::ShowNumber() const
    {
    return iShowNumber;
    }

// ---------------------------------------------------------------------------
// From class MPEClientInformation.
// ---------------------------------------------------------------------------
//
void CPEClientInformationImpl::SetEndOtherCalls( TBool aEndOtherCalls )
    {
    iEndOtherCalls = aEndOtherCalls;
    TEFLOGSTRING2( KTAINT,
        "PE CPEClientInformationImpl::SetEndOtherCalls, iEndOtherCalls: %d", 
        iEndOtherCalls );
    }

// ---------------------------------------------------------------------------
// From class MPEClientInformation.
// ---------------------------------------------------------------------------
//
TBool CPEClientInformationImpl::EndOtherCalls() const
    {
    TEFLOGSTRING2( KTAINT,
        "PE CPEClientInformationImpl::EndOtherCalls, iEndOtherCalls: %d", 
        iEndOtherCalls );
    return iEndOtherCalls;
    }
