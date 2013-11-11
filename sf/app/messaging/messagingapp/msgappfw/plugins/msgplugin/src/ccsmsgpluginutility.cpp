/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  CS Msg Plugin Utility class
 *
*/


// USER INCLUDES
#include "ccsmsgpluginutility.h"
#include "ccsdebug.h"

//SYSTEM INCLUDES
#include <csmsemailfields.h>
#include <e32cmn.h>
#include <SendUiConsts.h>
#include <pushentry.h>
#include <msvids.h>

// ----------------------------------------------------------------------------
// CCsMsgPluginUtility::NewL
// Two Phase Construction
// ----------------------------------------------------------------------------
CCsMsgPluginUtility* CCsMsgPluginUtility::NewL( )
    {
    CCsMsgPluginUtility* self = new ( ELeave ) CCsMsgPluginUtility();
    return self;
    }

// ----------------------------------------------------------------------------
// CCsMsgPluginUtility::~CCsMsgPluginUtility
// Destructor
// ----------------------------------------------------------------------------
CCsMsgPluginUtility::~CCsMsgPluginUtility()
    {
    }

// ----------------------------------------------------------------------------
// Constructor
// ----------------------------------------------------------------------------
CCsMsgPluginUtility::CCsMsgPluginUtility()
    {
    }

// ----------------------------------------------------------------------------
// CCsMsgPluginUtility::CreateConversationEntryL
// Creates CCsConversationEntry
// ----------------------------------------------------------------------------
//
CCsConversationEntry* CCsMsgPluginUtility::CreateConversationEntryL(
        const HBufC* aContact,
        TMsvId aEnryId,
        TInt64 aTimeStamp,
        TMsvId aDir,
        const HBufC* aDescription,
        TCsSendState aSendState,
        TCsAttribute aMsgAttribs,
        TCsType aCsType) const 
    {
    // create CCsConversationEntry
    CCsConversationEntry *conversationEntry = CCsConversationEntry::NewL();
    CleanupStack::PushL(conversationEntry);
        
    //set the values
    if(aContact)
        conversationEntry->SetContactL( *aContact );
    conversationEntry->SetEntryId( aEnryId );
    conversationEntry->SetTimeStampL( aTimeStamp );
    if (aDescription) {
    conversationEntry->SetDescriptionL( *aDescription );
    }
    conversationEntry->SetConversationDir( MapDirection( aDir ) );
    conversationEntry->SetSendState( aSendState );
    conversationEntry->ChangeAttributes( aMsgAttribs, ECsAttributeNone );
    conversationEntry->SetType(aCsType);
        
    //pop and return
    CleanupStack::Pop(conversationEntry);
    return conversationEntry;
    }

// ----------------------------------------------------------------------------
// CCsMsgPluginUtility::MapDirection
// Map the msg direction to TCsDirection
// ----------------------------------------------------------------------------
//
TCsDirection CCsMsgPluginUtility::MapDirection( TMsvId aDir ) const
    {
    TCsDirection direction = ECsDirectionUnknown;    
    switch(aDir)
        {
        case KMsvSentEntryIdValue:
        case KMsvGlobalOutBoxIndexEntryIdValue:  // Fall-through
        case KMsvDraftEntryIdValue:
            {
            direction = ECsDirectionOutgoing;
            break;
            }
        case KMsvGlobalInBoxIndexEntryIdValue:
            {
            direction = ECsDirectionIncoming;
            break;
            }
        }
    return direction;
    }

// ----------------------------------------------------------------------------
// CCsMsgPluginUtility::GetMsgAttributes
// Returns the message attributes as a bitmask of TCsAttribute values.
// ----------------------------------------------------------------------------
//
TUint16 CCsMsgPluginUtility::GetMsgAttributes( const TMsvEntry& aEntry )
    {
    TUint16 msgAttributes = ECsAttributeNone;
    TMsvId parent = aEntry.Parent();

    // New
    if( aEntry.New() )
        {
        msgAttributes |= ECsAttributeNew;
        }

    // Drafts
    if( KMsvDraftEntryIdValue == parent )
        {
        msgAttributes |= ECsAttributeDraft;
        }

    // Attachments
    if( aEntry.Attachment() )
        {
        msgAttributes |= ECsAttributeAttachment;
        }

    // Priority
    TMsvPriority priority = aEntry.Priority();
    if( EMsvHighPriority == priority )
        {
        msgAttributes |= ECsAttributeHighPriority;
        }
    else if( EMsvLowPriority == priority )
        {
        msgAttributes |= ECsAttributeLowPriority;
        }

    // Read/Unread
    if( KMsvGlobalInBoxIndexEntryIdValue == parent )
        {
        if( EFalse == aEntry.Unread() )
            {
            // Read
            msgAttributes &= ~ECsAttributeUnread;
            }
        else
            {
            // Unread
            msgAttributes |= ECsAttributeUnread;
            }
        }

    // Sent
    if( KMsvSentEntryIdValue == parent )
        {
        msgAttributes |= ECsAttributeSent;
        }

    return msgAttributes;
    }

// ----------------------------------------------------------------------------
// CCsMsgPluginUtility::GetSendState
// Returns the send status.
// ----------------------------------------------------------------------------
//
TCsSendState CCsMsgPluginUtility::GetSendState( const TMsvEntry& aContext )
    {
    TUint sendingState = aContext.SendingState();
    TCsSendState convSendState = ECsSendStateUnknown;

    switch( sendingState )
        {
        case KMsvSendStateUponRequest:
            {
            convSendState = ECsSendStateUponRequest;
            break;
            }
        case KMsvSendStateWaiting:
            {
            convSendState = ECsSendStateWaiting;
            break;
            }
        case KMsvSendStateSending:
            {
            convSendState = ECsSendStateSending;
            break;
            }
        case KMsvSendStateScheduled:
            {
            convSendState = ECsSendStateScheduled;
            break;
            }
        case KMsvSendStateResend:
            {
            convSendState = ECsSendStateResend;
            break;
            }
        case KMsvSendStateSuspended:
            {
            convSendState = ECsSendStateSuspended;
            break;
            }
        case KMsvSendStateFailed:
            {
            convSendState = ECsSendStateFailed;
            break;
            }
        case KMsvSendStateSent:
            {
            convSendState = ECsSendStateSent;
            break;
            }
        case KMsvSendStateNotApplicable:
            {
            convSendState = ECsSendStateNotApplicable;
            break;
            }
        }
    return convSendState;
    }

// ----------------------------------------------------------------------------
// CCsMsgPluginUtility::GetContact
// Get the Contact (From/To) of the message
// ----------------------------------------------------------------------------
//
void CCsMsgPluginUtility::CreateContactL(CMsvSession* aSession,
					const TMsvEntry& aEntry,
					RPointerArray<HBufC>& addressList)
	{
    if ( aEntry.iMtm.iUid == KSenduiMtmSmsUidValue )      
        {
        CPlainText* nullString = CPlainText::NewL();
        CleanupStack::PushL( nullString );
    
        CSmsHeader* smsHeader = NULL;
        if ( aEntry.Parent() == KMsvGlobalInBoxIndexEntryIdValue ) 
            {
            smsHeader = CSmsHeader::NewL( CSmsPDU::ESmsDeliver, *nullString );
            }
        else if ( aEntry.Parent() == KMsvSentEntryIdValue || 
                  aEntry.Parent() == KMsvGlobalOutBoxIndexEntryIdValue )
            {
            smsHeader = CSmsHeader::NewL( CSmsPDU::ESmsSubmit, *nullString );
            }
        else
            {
            CleanupStack::PopAndDestroy(nullString);
            return;
            }
        CleanupStack::PushL( smsHeader );
    
        CMsvEntry *cEntry = CMsvEntry::NewL(*aSession,aEntry.Id(),
            TMsvSelectionOrdering() );
        CleanupStack::PushL( cEntry );
    
        CMsvStore* store = cEntry->ReadStoreL();
        CleanupStack::PushL(store);
    
        TRAPD(err, smsHeader->RestoreL( *store ));    
        if ( err == KErrNone )
            {
            if ( aEntry.Parent() == KMsvGlobalOutBoxIndexEntryIdValue ||
                 aEntry.Parent() == KMsvSentEntryIdValue )
                {
                const CArrayPtrFlat<CSmsNumber>& rcpts = smsHeader->Recipients();
                const CSmsNumber& rcpt = *rcpts.At(0);

                HBufC* fromAddress = (rcpt.Address()).AllocL();
                addressList.Append(fromAddress);
                }
            else
                {
                // Check and add if any email addresses                
                const CSmsEmailFields& emailFields = smsHeader->EmailFields();
                if( emailFields.HasAddress() )
                    {                  
                    TPtrC name;
                    TPtrC address;

                    const MDesCArray& emailRecipients = emailFields.Addresses();
                    for( TInt id = 0; id < emailRecipients.MdcaCount(); id++ )
                        {
                        NameAndAddress( emailRecipients.MdcaPoint( id ), name, address);
                        
                        // Add address to list
                        HBufC* addressBuf = HBufC::NewL( address.Length() );
                        TPtr addressPtr( 0, 0 );
                        addressPtr.Set( addressBuf->Des() );
                        addressPtr.Copy( address );
                        addressList.Append(addressBuf);
                        }
                    }
                else
                    { 
                    HBufC* fromAddress = (smsHeader->FromAddress()).AllocL();   
                    
                    // Try iDetails if empty
                    if ( fromAddress->Length() == 0 )
                        fromAddress = aEntry.iDetails.AllocL();
                    
                    addressList.Append(fromAddress);
                    }
                }
            }
        
        // Cleanup 
        CleanupStack::PopAndDestroy(store);
        CleanupStack::PopAndDestroy(cEntry);
        CleanupStack::PopAndDestroy(smsHeader);
        CleanupStack::PopAndDestroy(nullString);
        }
    else if ( aEntry.iMtm.iUid == KSenduiMtmBtUidValue  ||  
			  aEntry.iMtm.iUid == KSenduiMtmBioUidValue ||
			  aEntry.iMtm == KUidMtmWapPush )
        {
        HBufC* fromAddress = aEntry.iDetails.AllocL();
        addressList.Append(fromAddress);
        }
    }

// ---------------------------------------------------------
// CCsMsgPluginUtility::CompareEntry
//
// ---------------------------------------------------------
TBool CCsMsgPluginUtility::CompareEntry( const TMsvEntry& aOldContext,  
	const TMsvEntry& aNewContext, TMsvId aDir ) const
    {
    if(	(aOldContext.Id() == aNewContext.Id()) 
        &&   (aOldContext.Parent() == aNewContext.Parent())
        &&   (0 == aOldContext.iDescription.Compare(aNewContext.iDescription)) )
    {
    // incase of outbox, check sending state also
    if(aDir == KMsvGlobalOutBoxIndexEntryIdValue)
        {
        if(aOldContext.SendingState() == aNewContext.SendingState())
            return ETrue;
        else
            return EFalse;
        }
    return ETrue;
    }

    return EFalse;
    }

// ----------------------------------------------------------------------------
// CCsMsgPluginUtility::NameAndAddress
// Extracts name and address from the aMsvAddress
// which is of the form name<address>
// ----------------------------------------------------------------------------
void CCsMsgPluginUtility::NameAndAddress( const TPtrC& aMsvAddress, 
				TPtrC& aName, 
				TPtrC& aAddress )
    {
    // For address information separation (start)
    const TUint KMsgSmsAddressStartChar         ('<');

    // For address information separation (end)
    const TUint KMsgSmsAddressEndChar           ('>');

    TInt addressStart = aMsvAddress.LocateReverse( KMsgSmsAddressStartChar );
    TInt addressEnd = aMsvAddress.LocateReverse( KMsgSmsAddressEndChar );

    if ( addressStart != KErrNotFound && addressEnd != KErrNotFound
            && addressEnd > addressStart )
        {
        // verified address, will be used as selected from contacts manager
        aName.Set( aMsvAddress.Ptr(), addressStart );
        aAddress.Set(
                aMsvAddress.Mid( addressStart + 1 ).Ptr(),
                ( addressEnd - addressStart ) -1 );
        if ( !aAddress.Length())
            {
            aAddress.Set( aName );
            aName.Set( KNullDesC ); // empty string
            }
        }
    else
        {
        // unverified string, will be used as entered in the header field
        aName.Set( KNullDesC ); // empty string
        aAddress.Set( aMsvAddress.Ptr(), aMsvAddress.Length() ); // a whole string to address
        }

    if ( aName.CompareF( aAddress ) == 0 )
        {
        aName.Set( KNullDesC ); // empty string
        }
    }

// ---------------------------------------------------------
// CCsMsgPluginUtility::PureAddress
// Extract the phone number for MMS
// ---------------------------------------------------------
//
TPtrC CCsMsgPluginUtility::PureAddress( const TDesC& aAddress )
    {
    _LIT( KOpen, "<" );
    _LIT( KClose, ">" );
	
    // syntax is :
    // <alias><separator1><pure_address><separator2> |
    // <pure_address>
    TInt firstPos = 0;
    TInt lastPos = 0;
    TInt length = aAddress.Length();
    TInt sepaLen1 = KOpen().Length();
    TInt sepaLen2 = KClose().Length();
    TInt firstSeparatorPosition = 0;

    while( firstSeparatorPosition >= 0 )
        {
        firstSeparatorPosition = aAddress.Mid( firstPos ).Find( KOpen );
        if ( firstSeparatorPosition >= 0 )
            {
            firstPos += firstSeparatorPosition + sepaLen1;
            }
        }
    if ( firstPos <= 0 )
        {
        // No alias
        return aAddress;
        }

    // Check if the second separator ends the address
    TPtrC last = aAddress.Right( sepaLen2 );
    lastPos = length - sepaLen2;

    if ( !last.Compare( KClose ) )
        {
        // Alias part found
        if ( lastPos > firstPos )
            {
            return aAddress.Mid( firstPos, lastPos - firstPos );
            }
        }
    // No alias defined - return the original string as pure address
    // If syntax is weird, namely 
    // alias <>
    // with nothing between the separators, we return the original string as is
    return aAddress;
    }

//End of File
