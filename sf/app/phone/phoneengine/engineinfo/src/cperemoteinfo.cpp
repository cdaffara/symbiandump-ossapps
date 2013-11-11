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
* Description:  This file contains the implementation of the CPERemoteInfo object
*
*/

#include <cphcntcontactid.h>
#include "cperemoteinfo.h"
#include "talogger.h"

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// CPERemoteInfo::NewL
// Two-phased constructor.
// ---------------------------------------------------------------------------
//
CPERemoteInfo* CPERemoteInfo::NewL()
    {
    TEFLOGSTRING( KTAOBJECT, "CPERemoteInfo::NewL" );
    CPERemoteInfo* remoteInfo = new ( ELeave ) CPERemoteInfo();
    return remoteInfo;
    }

// ---------------------------------------------------------------------------
// Constructor
// ---------------------------------------------------------------------------
//
CPERemoteInfo::CPERemoteInfo()
    {
    TEFLOGSTRING( KTAOBJECT, "CPERemoteInfo::CPERemoteInfo" );
    ResetRemoteInfo();
    }

// ---------------------------------------------------------------------------
// Destructor
// ---------------------------------------------------------------------------
//
CPERemoteInfo::~CPERemoteInfo()
    {
    TEFLOGSTRING( KTAOBJECT, "CPERemoteInfo::~CPERemoteInfo" );
    delete iTextToSpeechText;
    delete iCallerText;
    delete iCallerImage;
    delete iContactLink;
    if ( iContactId )
        {
        if ( iContactId->IsValid() )
            {
            iContactId->Invalidate();
            delete iContactId;
            }
        }
    iGroupId.Reset();
    delete iCallerThumbnail;
    delete iPredefinedDtmfStrings;
    }

TInt CPERemoteInfo::CallId() const
    {
    TEFLOGSTRING2( KTAINT, "CPERemoteInfo::CallId, id: %d", iCallId );
    return iCallId;
    }

const CPhCntContactId& CPERemoteInfo::ContactId() const
    {
    TEFLOGSTRING2( KTAINT, "CPERemoteInfo::ContactId, ptr: %x", iContactId );
    return *iContactId;
    }

const TDesC8& CPERemoteInfo::ContactLink() const
    {
    if( iContactLink )
        {
        return *iContactLink;
        }
    else
        {
        return KNullDesC8;
        }
    }

const TPEPhoneNumber& CPERemoteInfo::PhoneNumber() const
    {
    TEFLOGSTRING2( KTAINT, "CPERemoteInfo::PhoneNumber, number: '%S'", &iPhoneNumber );
    return iPhoneNumber;
    }

const TPEPhoneNumber& CPERemoteInfo::ColpNumber() const
    {
    TEFLOGSTRING2( KTAINT, "CPERemoteInfo::ColpNumber, number: '%S'", &iColpNumber );
    return iColpNumber;
    }

const TPEContactName& CPERemoteInfo::Name() const
    {
    TEFLOGSTRING2( KTAINT, "CPERemoteInfo::Name, name: '%S'", &iName );
    return iName;
    }

const TPEContactCompany& CPERemoteInfo::CompanyName() const
    {
    TEFLOGSTRING2( KTAINT, "CPERemoteInfo::CompanyName, name: '%S'", &iCompanyName );
    return iCompanyName;
    }

const TPEContactFileName& CPERemoteInfo::PersonalRingingTone() const
    {
    TEFLOGSTRING2( KTAINT, "CPERemoteInfo::PersonalRingingTone, path: '%S'", &iPersonalRingingTone );
    return iPersonalRingingTone;
    }

const TPEPhoneNumberIdType& CPERemoteInfo::PhoneNumberType() const
    {
    TEFLOGSTRING2( KTAINT, "CPERemoteInfo::PhoneNumberType, type: %d", iPhoneNumberType );
    return iPhoneNumberType;
    }

const TPtrC CPERemoteInfo::TextToSpeechText() const
    {
    if( iTextToSpeechText )
        {
        TEFLOGSTRING2( KTAINT, "CPERemoteInfo::TextToSpeechText, path: '%S'", iTextToSpeechText );
        return *iTextToSpeechText;
        }
    else
        {
        return KNullDesC();
        }
    }

const TArray< TContactItemId > CPERemoteInfo::ContactGroups() const
    {
    TEFLOGSTRING2( KTAINT, "CPERemoteInfo::ContactGroups, entries: %d", iGroupId.Count() );
    return iGroupId.Array();
    }

const TPtrC CPERemoteInfo::CallerText() const
    {
    if( iCallerText )
        {
        TEFLOGSTRING2( KTAINT, "CPERemoteInfo::CallerText, path: '%S'", iCallerText );
        return *iCallerText;
        }
    else
        {
        return KNullDesC();
        }
    }

const TPtrC CPERemoteInfo::CallerImage() const
    {
    if( iCallerImage )
        {
        TEFLOGSTRING2( KTAINT, "CPERemoteInfo::CallerImage, path: %S", iCallerImage );
        return *iCallerImage;
        }
    else
        {
        return KNullDesC();
        }
    }

CFbsBitmap* CPERemoteInfo::CallerThumbnail() const
    {
    TEFLOGSTRING2( KTAINT, "CPERemoteInfo::CallerThumbnail, ptr: %x", iCallerThumbnail );
    return iCallerThumbnail;
    }

TBool CPERemoteInfo::HasCallerThumbnail() const
    {
    return iHasCallerThumbnail;
    }    

void CPERemoteInfo::SetCallId( const TInt aCallId )
    {
    TEFLOGSTRING2( KTAINT, "CPERemoteInfo::SetCallId, id: %d", iCallId );
    iCallId = aCallId;
    }

void CPERemoteInfo::SetContactId( CPhCntContactId* aContactId )
    {
    TEFLOGSTRING2( KTAINT, "CPERemoteInfo::SetContactId, ptr: %x", iContactId );
    iContactId = aContactId;
    
    delete iContactLink;
    iContactLink = NULL;
    TRAP_IGNORE( iContactLink = iContactId->PackLC();
        CleanupStack::Pop( iContactLink ); );

    }

void CPERemoteInfo::SetPhoneNumber( const TPEPhoneNumber& aPhoneNumber )
    {
    iPhoneNumber = aPhoneNumber;
    TEFLOGSTRING2( KTAINT, "CPERemoteInfo::SetPhoneNumber, number: '%S'", &iPhoneNumber );
    }

void CPERemoteInfo::SetColpNumber( const TPEPhoneNumber& aColpNumber )
    {
    iColpNumber = aColpNumber;
    TEFLOGSTRING2( KTAINT, "CPERemoteInfo::SetColpNumber, number: '%S'", &iColpNumber );
    }

void CPERemoteInfo::SetName( const TPEContactName& aContactName )
    {
    iName = aContactName;
    TEFLOGSTRING2( KTAINT, "CPERemoteInfo::SetName, name: '%S'", &iName );
    }

void CPERemoteInfo::SetCompanyName( const TPEContactName& aCompanyName )
    {
    iCompanyName = aCompanyName;
    TEFLOGSTRING2( KTAINT, "CPERemoteInfo::SetCompanyName, company name: '%S'", &iCompanyName );
    }

void CPERemoteInfo::SetPersonalRingingTone(
        const TPEContactFileName& aPersonalRingingTone )
    {
    iPersonalRingingTone = aPersonalRingingTone;
    TEFLOGSTRING2( KTAINT, "CPERemoteInfo::SetPersonalRingingTone, path: '%S'", &iPersonalRingingTone );
    }

void CPERemoteInfo::SetPhoneNumberType( const TPEPhoneNumberIdType aPhoneNumberType )
    {
    iPhoneNumberType = aPhoneNumberType;
    TEFLOGSTRING2( KTAINT, "CPERemoteInfo::SetPhoneNumberType, type: %d", iPhoneNumberType );
    }

void CPERemoteInfo::SetTextToSpeechText( HBufC* aText )
    {
    iTextToSpeechText = aText;
    TEFLOGSTRING2( KTAINT, "CPERemoteInfo::SetTextToSpeechText, path: '%S'", &iTextToSpeechText );
    }

void CPERemoteInfo::SetContactGroups(
        const TArray< TContactItemId >& aGroupIdArray )
    {
    TEFLOGSTRING( KTAINT, "CPERemoteInfo::SetContactGroups" );
    TInt errorCode( KErrNone );

    iGroupId.Reset();
    for ( TInt i = 0; i < aGroupIdArray.Count(); i++ )
        {
        errorCode = iGroupId.Append( aGroupIdArray[ i ] );
        TEFLOGSTRING3( 
            KTAINT, 
            "PE CPERemoteInfo::SetContactGroups, index: %d, group id: %d", 
            i, 
            aGroupIdArray[ i ] );
        }
    if ( errorCode != KErrNone )
        {
        TEFLOGSTRING2( KTAERROR,
            "CPERemoteInfo::SetContactGroups ! RARRAY::APPEND FAILED WITH EXIT CODE: &d",
            errorCode );
        iGroupId.Reset();
        }
    TEFLOGSTRING2( KTAINT, "PE CPERemoteInfo::SetContactGroups, entries: %d", iGroupId.Count() );
    }

void CPERemoteInfo::SetCallerText( const TPtrC aCallerText )
    {
    delete iCallerText;
    if ( aCallerText.Length() > 0 )
        {
        iCallerText = aCallerText.Alloc();
        TEFLOGSTRING2( KTAINT, "CPERemoteInfo::SetCallerText, text: '%S'", &iCallerText );
        }
    else
        {
        TEFLOGSTRING( KTAERROR, "CPEREMOTEINFO::SETCALLERTEXT ! NO CALLER TEXT" );
        }
    }

void CPERemoteInfo::SetCallerImage( const TPtrC aCallerImage )
    {
    delete iCallerImage;
    if ( aCallerImage.Length() > 0 )
        {
        iCallerImage = aCallerImage.Alloc();
        TEFLOGSTRING2( KTAINT, "CPERemoteInfo::SetCallerImage, path: '%S'", &iCallerImage );
        }
    else
        {
        TEFLOGSTRING( KTAERROR, "CPEREMOTEINFO::SETCALLERIMAGE ! NO CALLER IMAGE PATH" );
        }
    }

void CPERemoteInfo::SetCallerThumbnail( CFbsBitmap* aCallerThumbnail )
    {
    delete iCallerThumbnail;
    iCallerThumbnail = aCallerThumbnail;
    TEFLOGSTRING2( KTAINT, "CPERemoteInfo::SetCallerThumbnail, ptr: %x", iCallerThumbnail );
    }


void CPERemoteInfo::SetHasCallerThumbnail( TBool aStatus )
    {
    iHasCallerThumbnail = aStatus;
    TEFLOGSTRING2( KTAINT, "CPERemoteInfo::SetHasCallerThumbnail, %d", aStatus );
    }

// -----------------------------------------------------------------------------
// CPERemoteInfo::SetPredefinedDtmfStrings()
//
//  
// -----------------------------------------------------------------------------
//
void CPERemoteInfo::SetPredefinedDtmfStrings( 
            const CDesCArray& aArray )           
    {
    if ( iPredefinedDtmfStrings )
        {
        delete iPredefinedDtmfStrings;
        iPredefinedDtmfStrings = NULL;
        }
    
    TInt count = aArray.Count();
  
    if ( count )
        {
        TRAPD( err, iPredefinedDtmfStrings = new ( ELeave ) CDesCArrayFlat( count ) );
        
        if( err != KErrNone )
            {
            TEFLOGSTRING2( KTAINT, "CPERemoteInfo::SetPredefinedDtmfStrings, err: %d", err );
            return;
            }
         
        for ( TInt i = 0; i < count; i++ )
            {
            TRAP_IGNORE( iPredefinedDtmfStrings->AppendL( aArray.MdcaPoint(i) ) );    
            }
        }
    }


// -----------------------------------------------------------------------------
// CPERemoteInfo::PredefinedDtmfStrings()
//
//  
// -----------------------------------------------------------------------------
//
CDesCArray& CPERemoteInfo::PredefinedDtmfStrings()
    {
    if ( !iPredefinedDtmfStrings )
        {
        // Create dymmy array to help post handling
        TRAP_IGNORE( iPredefinedDtmfStrings = new ( ELeave ) CDesCArrayFlat( 1 ) );        
        }
    return *iPredefinedDtmfStrings;    
    }


void CPERemoteInfo::ResetRemoteInfo()
    {
    TEFLOGSTRING( KTAINT, "CPERemoteInfo::ResetRemoteInfo" );
    iPhoneNumber = KNullDesC;
    iColpNumber = KNullDesC;
    iName = KNullDesC;
    iCompanyName = KNullDesC;
    iPersonalRingingTone = KNullDesC;
    iPhoneNumberType = EPEUnknownNumber;
    delete iTextToSpeechText;
    iTextToSpeechText = NULL;
    delete iCallerText;
    iCallerText = NULL;
    delete iCallerImage;
    iCallerImage = NULL;
    if ( iContactId )
        {
        if ( iContactId->IsValid() )
            {
            iContactId->Invalidate();
            delete iContactId;
            iContactId = NULL;
            }
        }
    
    delete iContactLink;
    iContactLink = NULL;
    
    iGroupId.Reset();
    delete iCallerThumbnail;
    iCallerThumbnail = NULL;
    delete iPredefinedDtmfStrings;
    iPredefinedDtmfStrings = NULL;
    iHasCallerThumbnail = EFalse;
    }
