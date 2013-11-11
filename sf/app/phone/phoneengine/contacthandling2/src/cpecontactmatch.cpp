/*
* Copyright (c) 2002-2008 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  This module contains the implementation of CPEContactMatch class
*
*/

// INCLUDE FILES
#include "cpecontacthandling.h"
#include "cpecontactmatch.h"
#include <cphonecntfactory.h>
#include <cphcntmatcher.h>
#include <mpeclientinformation.h>
#include <mpedatastore.h>
#include <mphcntmatch.h>
#include <pepanic.pan>
#include <pevirtualengine.h>
#include <talogger.h>

// CONSTANTS

// Space - used as a separator between concatenated first and last name
_LIT( KPESpace," ");

// For parsing protocol prefix and domain part out of a VoIP call URI
_LIT( KPhoneColon, ":" );
_LIT( KPhoneAt, "@" );

// Contact group array granularity
const TInt KPEContactGroupArrayGranularity = 5;

// ================= MEMBER FUNCTIONS =======================

// ---------------------------------------------------------------------------
// CPEContactMatch::NewL
// ---------------------------------------------------------------------------
//
CPEContactMatch* CPEContactMatch::NewL(
        CPEContactHandling& aOwner,
        MPEDataStore& aDataStore
        )
    {
    CPEContactMatch* self = 
        new ( ELeave ) CPEContactMatch( aOwner, aDataStore );

    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );

    return self;
    }

// ---------------------------------------------------------------------------
// Destructor
// ---------------------------------------------------------------------------
//
CPEContactMatch::~CPEContactMatch()
    {

    delete iContactMatcher;
    delete iContactFactory;
    iLibrary.Close();
    }


// -----------------------------------------------------------------------------
// CPEContactMatch::CreateContactIdL
// -----------------------------------------------------------------------------
//
CPhCntContactId* CPEContactMatch::CreateContactIdL( const TDesC8& aContactId )
    {  
    return iContactFactory->CreateContactIdL( aContactId );
    }

// -----------------------------------------------------------------------------
// CPEContactMatch::CreateContactIdL
// -----------------------------------------------------------------------------
//
CPhCntContactId* CPEContactMatch::CreateContactIdL(
    TContactItemId aContactId )
    {  
    return iContactFactory->CreateContactIdL( aContactId );
    }

// -----------------------------------------------------------------------------
// CPEContactMatch::CPEContactMatch
// -----------------------------------------------------------------------------
//
CPEContactMatch::CPEContactMatch(
        CPEContactHandling& aOwner,
        MPEDataStore& aDataStore )
        : iOwner( aOwner ), iDataStore( aDataStore )
    {
    }

// -----------------------------------------------------------------------------
// CPEContactMatch::ConstructL
// -----------------------------------------------------------------------------
//
void CPEContactMatch::ConstructL()
    {
    //Mandatory services for providing contact match functionality
    TEFLOGSTRING( KTAOBJECT, "CNT CPEContactMatch::ConstructL > RLibrary::Load" );
    iContactFactory = CreateCntFactoryL();
    TEFLOGSTRING( KTAOBJECT, "CNT CPEContactMatch::ConstructL > CPhCntFactory::CreateContactMatcherL" );
    iContactMatcher = iContactFactory->CreateContactMatcherL();

    }


// -----------------------------------------------------------------------------
// CPEContactMatch::MatchWithContactIdL
// Synchronous method for finding contact data specified 
// with aContactId.
// -----------------------------------------------------------------------------
//
void CPEContactMatch::MatchWithContactIdL(
        const TInt aCallId
        )

    {
    TEFLOGSTRING( KTAINT, "CNT CPEContactMatch::MatchWithContactIdL" );
    MPhCntMatch* match( NULL );
    CPhCntContactId* contactId = NULL;
    contactId = CreateContactIdL( iDataStore.CallClientInformation( aCallId ).ContactLink() );
    CleanupStack::PushL( contactId );
    if ( iDataStore.CallType( aCallId ) == EPECallTypeVoIP )
        {
        if ( contactId && contactId->IsValid() )
            {
            TEFLOGSTRING( KTAINT, "CNT CPEContactMatch::MatchWithContactIdL > CPhCntMatcher::MatchVoipNumber" );
            // Get contact based on contact id.
            iContactMatcher->MatchVoipNumber( match, 
                                              *contactId );
            }
        else
            {
            User::Leave( KErrArgument );
            }
        }
    else
        {
        if ( contactId && contactId->IsValid() )
            {
            TEFLOGSTRING( KTAINT, "CNT CPEContactMatch::MatchWithContactIdL > CPhCntMatcher::MatchNumber" );
            iContactMatcher->MatchNumber( match, 
                                          iDataStore.RemotePhoneNumber( aCallId ),
                                          *contactId );
            }
        else
            {
            User::Leave( KErrArgument );
            }
        }

    if ( match != NULL )
        {
        TEFLOGSTRING( KTAINT, "CNT CPEContactMatch::MatchWithContactIdL, Contact match found" );
        CopyContactFieldsDataL( *match, aCallId );

        if ( match->HasThumbnailImage() )
            {
            TEFLOGSTRING( KTAINT, "CNT CPEContactMatch::MatchWithContactIdL, Thumbnail image found" );
            // Start loading thumbnail for this contact
            // Error code may be ignored since this is not a critical function
            //LoadThumbnail( iDataStore.ContactId( aCallId ), aCallId );
            }
        match->Release();
        match = NULL;
        }
    CleanupStack::PopAndDestroy( contactId ); //contactId 

    }

// -----------------------------------------------------------------------------
// CPEContactMatch::MatchWithNumberL
// Synchronous method for finding contact data specified with phone number.
// -----------------------------------------------------------------------------
//
void CPEContactMatch::MatchWithNumberL(
        const TInt aCallId
        )
    {
    MPhCntMatch* match( NULL );

    if ( iDataStore.CallType( aCallId ) == EPECallTypeVoIP )
        {
        TEFLOGSTRING2( KTAINT, "CNT CPEContactMatch::MatchWithNumberL > CPhCntMatcher::MatchVoipNumber, call id: %d", aCallId );
        TPEPhoneNumber remoteNumber = iDataStore.RemotePhoneNumber( aCallId );        
        

        // TODO: Determine whether domain part of the URI should be ignored
        // This could be done via RCSE (which is in my understanding: depricated)
        // but needs to be studied if SP Settings need to support this service
        // setting instead
        
        // Parse protocol prefix and domain part out of a VoIP call URI
        TInt pos( remoteNumber.Find( KPhoneColon ) );
        if ( pos > KErrNotFound )
            {
            remoteNumber.Delete( 0, pos+1 );
            }                
            
        pos = remoteNumber.Find( KPhoneAt );
        if ( pos > KErrNotFound )
            {
            remoteNumber.Delete( pos, remoteNumber.Length() - pos );
            }

        TEFLOGSTRING3( KTAINT, "CNT CPEContactMatch::MatchWithNumberL > CPhCntMatcher::MatchNumber, number: %S, call id: %d",
            &remoteNumber, aCallId );
        iContactMatcher->MatchVoipNumber( match, remoteNumber, ETrue, remoteNumber.Length() );

                
        }
    else
        {
        TPEPhoneNumber remoteNumber = iDataStore.RemotePhoneNumber( aCallId );
        
        TEFLOGSTRING3( KTAINT, "CNT CPEContactMatch::MatchWithNumberL > CPhCntMatcher::MatchNumber, number: %S, call id: %d",
            &remoteNumber, aCallId );

         TInt id = KErrNotFound;
         id = iDataStore.ContactId2();
         id == KErrNotFound? iContactMatcher->MatchNumber( match, remoteNumber ) :
                                     iContactMatcher->MatchNumber(match, remoteNumber, id);
         //reset contact-id
         iDataStore.SetContactId2(KErrNotFound);
         

        }

    if ( match != NULL )
        {
        TEFLOGSTRING( KTAINT, "CNT CPEContactMatch::MatchWithNumberL, Contact match found" );
        CopyContactFieldsDataL( *match, aCallId );

        if ( match->HasThumbnailImage() )
            {
            TEFLOGSTRING( KTAINT, "CNT CPEContactMatch::MatchWithNumberL, Thumbnail image found" );
            // Start loading thumbnail for this contact
            // Error code may be ignored since this is not a critical function
            iDataStore.SetHasCallerThumbnail( ETrue, aCallId );
            //LoadThumbnail( iDataStore.ContactId( aCallId ), aCallId );
            }
        match->Release();
        match = NULL;
        }
    }



// ---------------------------------------------------------
// CPEContactMatch::ContactThumbnail
// Returns contact thumbnail image, NULL if not found
// ---------------------------------------------------------
//
CFbsBitmap* CPEContactMatch::ContactThumbnail()
    {
    return NULL;

    }

// -----------------------------------------------------------------------------
// CPEContactMatch::CopyContactFieldsDataL
// Copies contact information to remote info struct
// -----------------------------------------------------------------------------
//
void CPEContactMatch::CopyContactFieldsDataL(
        const MPhCntMatch& aMatch,
        const TInt aCallId
        )
    {
    TEFLOGSTRING( KTAINT, "CNT CPEContactMatch::CopyContactFieldsDataL: start" );

    // Set ringing tone's filename
    iDataStore.SetPersonalRingingTone( aMatch.PersonalRingingTone(), aCallId );

    TPEContactName finalName;

    if ( aMatch.GroupName().Length() == 0 )
        {
        GetName( aMatch, finalName );
        }
    else 
        {
        // Group name found, set it as remote party name
        finalName.Append( aMatch.GroupName().Left( KCntMaxTextFieldLength ) );
        }
    
    // Replace paragraph separator with space character
    const TText ps(0x2029);
    
    while ( finalName.Locate( ps ) != KErrNotFound )
        {
        finalName[ finalName.Locate( ps )] = ' ';
        }
    
    TEFLOGSTRING3( KTAINT, "CNT CPEContactMatch::iDataStore.SetRemoteName( finalName, aCallId ): finalName: %S callid: %d ", &finalName, aCallId);
    iDataStore.SetRemoteName( finalName, aCallId );

    iDataStore.SetRemoteTextToSpeechText( aMatch.TextToSpeechTextL(), aCallId );

    TInt phcntfinderNumberType = aMatch.NumberType();
            
    switch ( phcntfinderNumberType )
        {
        case MPhCntMatch::EMobileNumber:
            iDataStore.SetRemotePhoneNumberType( EPEMobileNumber, aCallId );
            break;
        case MPhCntMatch::EStandardNumber:
            iDataStore.SetRemotePhoneNumberType( EPETelephoneNumber, aCallId );
            break;
        case MPhCntMatch::EFaxNumber:
            iDataStore.SetRemotePhoneNumberType( EPEFaxNumber, aCallId );
            break;
        case MPhCntMatch::EPagerNumber:
            iDataStore.SetRemotePhoneNumberType( EPEPager, aCallId );
            break;
        case MPhCntMatch::EVoipNumber:
            iDataStore.SetRemotePhoneNumberType( EPEVoipNumber, aCallId );
            break;
        case MPhCntMatch::EVideoNumber:
            iDataStore.SetRemotePhoneNumberType( EPEVideoNumber, aCallId );
            break;        
        case MPhCntMatch::EAssistantNumber:
            iDataStore.SetRemotePhoneNumberType( EPEAssistantNumber, aCallId );
            break;        
        case MPhCntMatch::ECarNumber:
            iDataStore.SetRemotePhoneNumberType( EPECarNumber, aCallId );
            break;        
        case MPhCntMatch::EHomeNumber:
            iDataStore.SetRemotePhoneNumberType( EPEHomeNumber, aCallId );
            break;        
        case MPhCntMatch::EWorkNumber:
            iDataStore.SetRemotePhoneNumberType( EPEWorkNumber, aCallId );
            break;        
        case MPhCntMatch::ENone: // fall through
        default:
            iDataStore.SetRemotePhoneNumberType( EPEUnknownNumber, aCallId );
            break;
        }

    // Replace paragraph separator with space character
    TPEContactName companyName;
    companyName = aMatch.CompanyName();
    while ( companyName.Locate( ps ) != KErrNotFound )
        {
        companyName[ companyName.Locate( ps )] = ' ';
        }
    // Set Company name
    iDataStore.SetRemoteCompanyName( companyName, aCallId );

    // Set Call text
    iDataStore.SetCallerText( aMatch.CallText(), aCallId );

    // Set Call image
    iDataStore.SetCallerImage( aMatch.CallImage(), aCallId );
    
    // Set Contact ID
    iDataStore.SetContactId( aMatch.ContactId()->CloneL(), aCallId );

    CArrayFixFlat< TContactItemId >* groupList
        = new ( ELeave ) CArrayFixFlat< TContactItemId >
        ( KPEContactGroupArrayGranularity );
 
    TArray<TContactItemId> alertFor = iDataStore.AlertForGroup();
    const TInt groupLength = alertFor.Count();
    for ( TInt i = 0; i < groupLength; i++ ) 
        {
        groupList->AppendL( alertFor[ i ] );
        }
   
    if ( aMatch.BelongsToGroups( *groupList ) )
        {
        // Convert array type here to ease further handling
        TArray<TContactItemId> list = groupList->Array();
        // Set group information
        iDataStore.SetContactGroups( list, aCallId );
        }
    delete groupList;
    groupList = NULL;
    
    // Set dtmf array
    iDataStore.SetPredefinedDtmfStrings( aMatch.AllDtmfNumbers(), aCallId );
    TEFLOGSTRING( KTAINT, "CNT CPEContactMatch::CopyContactFieldsDataL: complete" );  
    }

// -----------------------------------------------------------------------------
// CPEContactMatch::CopyContactFieldsDataL
// Copies contact information to remote info struct
// -----------------------------------------------------------------------------
//
void CPEContactMatch::GetName(
        const MPhCntMatch& aMatch,
        TPEContactName& aName
        )
    {
    // Set name
    // The following logic determines if one of the languages that require
    // swapping of first/last name order is selected and performs the operation.
    HBufC* lastname( NULL );
    HBufC* firstname( NULL );
    
    if ( aMatch.LastName().Length() > 0 )
        {
        TEFLOGSTRING( KTAINT, "CNT CPEContactMatch::CopyContactFieldsDataL, alloc last name" );
        lastname = aMatch.LastName().Alloc();
        }
    if ( aMatch.FirstName().Length() > 0 )
        {
        TEFLOGSTRING( KTAINT, "CNT CPEContactMatch::CopyContactFieldsDataL, alloc first name" );
        firstname = aMatch.FirstName().Alloc();
        }

    if ( ( lastname ) && ( firstname ) )
        {
        //Swap name order, if required by the current language setting
        const TLanguage language = User::Language();
    
        for ( TInt index = 0; index < KPENumberOfSwappedLanguages; index++ )
            {
            if ( KPESwappedLanguages[index] == language )
                {
                HBufC* tempfirstname( firstname );
                firstname = lastname;
                lastname = tempfirstname;
                break;
                }
            }
        }
    // Parse name by concatenating lastname and firstname
    if ( firstname )
        {
        aName = *firstname;
        if ( aName.Length() < KCntMaxTextFieldLength && lastname )
            {
            TEFLOGSTRING( KTAINT, "CNT CPEContactMatch::CopyContactFieldsDataL, append with space" );
            aName.Append( KPESpace );
            }
        delete firstname;
        firstname = NULL;
        }
    if ( lastname )
        {
        if ( aName.Length() < KCntMaxTextFieldLength )
            {
            // Check how long last name can be added to the iName string. 
            TInt lastNameMaxLength = KCntMaxTextFieldLength - aName.Length();
            if ( lastname->Length() > lastNameMaxLength )
                {
                TEFLOGSTRING( KTAINT, "CNT CPEContactMatch::CopyContactFieldsDataL, append with part of last name" );
                // Cut extra chars from the string.
                aName.Append( lastname->Left( lastNameMaxLength ) );          
                }
            else 
                {
                TEFLOGSTRING( KTAINT, "CNT CPEContactMatch::CopyContactFieldsDataL, append with last name" );
                aName.Append( *lastname );
                }
            }
        delete lastname;
        lastname = NULL;
        }    
    }

//  End of File  
