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
* Description: This file implements class CIpsPlgSmtpService.
*
*/


#include "emailtrace.h"
#include "ipsplgheaders.h"

#include <featmgr.h>
#include <bldvariant.hrh> // for feature definitions

// <cmail> const TInt KIpsSetUtilsCharAt = '@'; </cmail>
const TInt KIpsSetUtilsCharLessThan = '<';
const TInt KIpsSetUtilsCharMoreThan = '>';
const TInt KIpsSetUtilsCntrCharacters = 2;

// do we need smtp object instead set of static functions

// ---------------------------------------------------------------------------
// CIpsPlgSmtpService::CIpsPlgSmtpService
// ---------------------------------------------------------------------------
//
CIpsPlgSmtpService::CIpsPlgSmtpService(
    CMsvSession& aSession,
    CIpsPlgSosBasePlugin& aPlugin ) :
    iPlugin( aPlugin ),
    iSession( aSession), 
    iMsgMapper( NULL )
    {
    FUNC_LOG;
    }
  
// ---------------------------------------------------------------------------
// CIpsPlgSmtpService::~CIpsPlgSmtpService
// ---------------------------------------------------------------------------
//    
CIpsPlgSmtpService::~CIpsPlgSmtpService() 
    {
    FUNC_LOG;
    delete iMsgMapper;
    
    if( iFeatureManagerInitialized )
    	{
    	FeatureManager::UnInitializeLib();  
    	}
    }    

// ---------------------------------------------------------------------------
// CIpsPlgSmtpService::NewL
// ---------------------------------------------------------------------------
//
EXPORT_C CIpsPlgSmtpService* CIpsPlgSmtpService::NewL(
    CMsvSession& aSession,
    CIpsPlgSosBasePlugin& aPlugin )
    {
    FUNC_LOG;
    CIpsPlgSmtpService* self = CIpsPlgSmtpService::NewLC( 
        aSession, aPlugin  );
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------------------------
// CIpsPlgSmtpService::NewLC
// ---------------------------------------------------------------------------
//
EXPORT_C CIpsPlgSmtpService* CIpsPlgSmtpService::NewLC(
    CMsvSession& aSession,
    CIpsPlgSosBasePlugin& aPlugin )
    {
    FUNC_LOG;
    CIpsPlgSmtpService* self = new( ELeave ) CIpsPlgSmtpService( 
        aSession, aPlugin );
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }
    
// ---------------------------------------------------------------------------
// CIpsPlgSmtpService::ConstructL
// ---------------------------------------------------------------------------
//
void CIpsPlgSmtpService::ConstructL()
    {
    FUNC_LOG;
    iMsgMapper = CIpsPlgMsgMapper::NewL( iSession, iPlugin );
    
    FeatureManager::InitializeLibL();
    iFeatureManagerInitialized = ETrue;
    }

// ---------------------------------------------------------------------------
// CIpsPlgSmtpService::CreateDisplayStringLC
// ---------------------------------------------------------------------------
//
HBufC* CIpsPlgSmtpService::CreateDisplayStringLC(
    const TDesC& aDisplayName,
    const TDesC& aEmailName )
    {
    FUNC_LOG;
    TInt length( aDisplayName.Length() );
    HBufC* fullName( NULL );
    
    length += aEmailName.Length() + KIpsSetUtilsCntrCharacters;
    fullName = HBufC::NewLC( length );
    fullName->Des().Append( aDisplayName );
    
    if ( fullName->Length() )
        {
        // We have display name, add angle brackets to address
        fullName->Des().Append( KIpsSetUtilsCharLessThan );
        fullName->Des().Append( aEmailName );
        fullName->Des().Append( KIpsSetUtilsCharMoreThan );
        }
    else
        {
        fullName->Des().Append( aEmailName );
        }
    return fullName;
    }

// ---------------------------------------------------------------------------
// CIpsPlgSmtpService::StoreMessageL
// ---------------------------------------------------------------------------
//
void CIpsPlgSmtpService::StoreMessageL( 
    TFSMailMsgId /* aMailBoxId */, CFSMailMessage& aMessage )
    {
    FUNC_LOG;

    // Update flags i.e. Priority
    iMsgMapper->UpdateMessageFlagsL( aMessage.GetMessageId().Id(), aMessage );
    
    // get message from store
    CMsvEntry* cEntry = CMsvEntry::NewL(
        iSession, aMessage.GetMessageId().Id(), TMsvSelectionOrdering() );
    CleanupStack::PushL( cEntry );  // << cEntry
    TMsvEntry tEntry = cEntry->Entry();    
    
    // creating store object for editing message
    CMsvStore* store = cEntry->EditStoreL();
    CleanupStack::PushL( store );   // << store
    
    // create symbian email header object
    CImHeader* header = CImHeader::NewLC(); // << header
    header->RestoreL( *store );

    // From field, check that sender is set
    if ( aMessage.GetSender() )
        {
        header->SetFromL( aMessage.GetSender()->GetEmailAddress() );
        }
    
    // To Field
// <qmail> return by value, because Qt types are used for internal CFSMailAddress data storage
    RPointerArray<CFSMailAddress> toArray = aMessage.GetToRecipients();
// </qmail>
    // Reseting recipient list, because our client calls us many times
    // and we want to avoid duplicates
    header->ToRecipients().Reset();
    // build new 'to' header
    for( TInt i = 0; i < toArray.Count(); i++ )
        {
        TPtrC displayName = toArray[i]->GetDisplayName();
        TPtrC emailName = toArray[i]->GetEmailAddress();
        HBufC* fullName = CreateDisplayStringLC( displayName, emailName );  // << fullName
        header->ToRecipients().AppendL( *fullName ); // copy created
        // only first receiver 
        if( i == 0 && fullName )
            {
            tEntry.iDetails.Set( *fullName );
            cEntry->ChangeL( tEntry );
            // Load tEntry again
            // because old instance's iDetails member gets wiped
            // in PopAndDestroy below
            tEntry = cEntry->Entry();
            }
        CleanupStack::PopAndDestroy( fullName );    // >>> fullName
        }   
    
    // CC field
// <qmail> return by value, because Qt types are used for internal CFSMailAddress data storage
    RPointerArray<CFSMailAddress> ccArray 
        = aMessage.GetCCRecipients();
// </qmail>
    header->CcRecipients().Reset();
    for( TInt i = 0; i < ccArray.Count(); i++ )
        {
        TPtrC displayName = ccArray[i]->GetDisplayName();
        TPtrC emailName = ccArray[i]->GetEmailAddress();
        HBufC* fullName = CreateDisplayStringLC( displayName, emailName );  // << fullName
        header->CcRecipients().AppendL( *fullName );
        CleanupStack::PopAndDestroy( fullName );    // >>> fullName
        }
    
    // BCC field
// <qmail> return by value, because Qt types are used for internal CFSMailAddress data storage
    RPointerArray<CFSMailAddress> bccArray 
        = aMessage.GetBCCRecipients();
// </qmail>
    header->BccRecipients().Reset();
    for( TInt i = 0; i < bccArray.Count(); i++ )
        {
        TPtrC displayName = bccArray[i]->GetDisplayName();
        TPtrC emailName = bccArray[i]->GetEmailAddress();
        HBufC* fullName = CreateDisplayStringLC( displayName, emailName );  // << fullName
        header->BccRecipients().AppendL( *fullName ); // copy created
        CleanupStack::PopAndDestroy( fullName );    // >>> fullName
        }
    
    // Subject, check null pointer
    if ( &aMessage.GetSubject() )
        {
        header->SetSubjectL( aMessage.GetSubject() );
        }
        
    // store header in cEntry's store
    header->StoreWithoutCommitL(*store);
    // commit header to store
    store->CommitL();
    
    //change preparation state and set subject to description
    tEntry.SetInPreparation( EFalse );
    tEntry.iDescription.Set( aMessage.GetSubject() );
    
    cEntry->ChangeL( tEntry );
    
    // release store
    CleanupStack::PopAndDestroy( 3, cEntry );   // >>> cEntry, header, store
    }

// <qmail> removing unused functions
// CreateNewSmtpMessageL
// CreateForwardSmtpMessageL
// CreateReplySmtpMessageL
// </qmail>

// ---------------------------------------------------------------------------
// CIpsPlgSmtpService::ChangeServiceIdL
// ---------------------------------------------------------------------------
//
void CIpsPlgSmtpService::ChangeServiceIdL( TMsvEntry& aEntry )
    {
    FUNC_LOG;
    TMsvEntry serviceEntry;
    TMsvId service;
    CMsvEntry* cEntry = iSession.GetEntryL( aEntry.iServiceId );
    CleanupStack::PushL( cEntry );
    User::LeaveIfError( iSession.GetEntry( cEntry->EntryId(), service, serviceEntry ) );
    if( aEntry.iServiceId != serviceEntry.iRelatedId )
        {
        aEntry.iServiceId = serviceEntry.iRelatedId;
        cEntry->SetEntryL( aEntry.Id() );
        cEntry->ChangeL( aEntry );
        }
    CleanupStack::PopAndDestroy( cEntry );
    }

// ---------------------------------------------------------------------------
// CIpsPlgSmtpService::ChangeServiceIdL
// ---------------------------------------------------------------------------
//
void CIpsPlgSmtpService::ChangeServiceIdL( TMsvEntry& aEntry, TMsvId aServiceId )
    {
    FUNC_LOG;
    CMsvEntry* cEntry = iSession.GetEntryL( aEntry.Id() );
    CleanupStack::PushL( cEntry );
    aEntry.iServiceId = aServiceId;
    cEntry->ChangeL( aEntry );
    CleanupStack::PopAndDestroy( cEntry );
    }

// ---------------------------------------------------------------------------
// CIpsPlgSmtpService::CreateFSMessageAndSetFlagsL
// ---------------------------------------------------------------------------
//
CFSMailMessage* CIpsPlgSmtpService::CreateFSMessageAndSetFlagsL(
       TMsvId aMessageId,
       TMsvId aOriginalMsgId,
       TMsvId aMailboxId,
       TBool aCopyOriginalMsgProperties )
    {
    FUNC_LOG;

    
   
    // tp teleca change for korean specific email encoding
    // Default charset
    TUid charset;
    charset.iUid = KCharacterSetIdentifierUtf8;

      // korea specific charset
      if ( FeatureManager::FeatureSupported( KFeatureIdKorean ))
      {
      
      //const TUid KCRUidEMailCharsets = {0x20018441};    // korea needs different default charset
          const TInt KoreanMib = 38; // magic number to read from cen rep would be better
            
		  CCnvCharacterSetConverter* charConv = NULL;
		  TRAPD( err, charConv = CCnvCharacterSetConverter::NewL() );
		  if( err == KErrNone )
			  {
			  TUint characterSetId = KCharacterSetIdentifierUtf8;
			  RFs fs;
			  TInt err = fs.Connect();
			  
			  if( err == KErrNone )
				  {
				  TRAP( err, characterSetId = charConv->ConvertMibEnumOfCharacterSetToIdentifierL( KoreanMib, fs ));
				   if( err == KErrNone )
					   { // set korean specific charset
					   charset = TUid::Uid( characterSetId );
					   }
				   fs.Close();
				  }
			  }

         delete charConv; 
     
      }
      // tp teleca change END

    // set in preparation flag, altought 
    // not known where this affects
    TMsvId dummy;
    TMsvEmailEntry newEmailMsg;
    iSession.GetEntry( aMessageId, dummy, newEmailMsg );
    newEmailMsg.SetInPreparation( ETrue );
    
    // when creating new message from scratch theres no 
    // orginal message, so it is set to KErrNotFound
    if ( aOriginalMsgId != KErrNotFound )
        {
        TMsvEntry orgMsg;
        //get entry errors not handled
        iSession.GetEntry( aOriginalMsgId, dummy, orgMsg );
        const TMsvEmailEntry& orgEmailMsg(orgMsg);
        
        // symbian not mark same priority to reply / forward messages
// <qmail>
        if ( aCopyOriginalMsgProperties && orgEmailMsg.Priority() != newEmailMsg.Priority() )
// </qmail>
            {
            newEmailMsg.SetPriority( orgEmailMsg.Priority() );
            }
        //get charset from original message
        charset = GetOriginalCharsetL( aOriginalMsgId );
        }
    
    // set in preparation
    CMsvEntry* cEntry = iSession.GetEntryL( aMessageId );
    CleanupStack::PushL( cEntry );
    cEntry->ChangeL( newEmailMsg );

    //no need to set if we for some reason don't have it.
    //system default will be used.
    if ( KNullUidValue != charset.iUid )
        {
        SetCharactersetL( *cEntry, charset );
        }
    // delete operations we don't need anymore
    CleanupStack::PopAndDestroy( cEntry );
    
    // create a Freestyle mail message object and set to it all
    // needed data from just created symbian mail message
    CFSMailMessage* fsMsg = CFSMailMessage::NewL(
        TFSMailMsgId( iPlugin.PluginId(), aMessageId ) );
    fsMsg->SetMailBoxId( TFSMailMsgId( iPlugin.PluginId(), aMailboxId ) );
    fsMsg->SetFolderId( 
        TFSMailMsgId( iPlugin.PluginId(), KMsvDraftEntryId ) );

// <qmail>
    // update subject
    fsMsg->SetSubject( newEmailMsg.iDescription );
// </qmail>
    
    //update flags
    iMsgMapper->SetFSMessageFlagsL( newEmailMsg, *fsMsg );
    
    return fsMsg;
    }

// ---------------------------------------------------------------------------
// CIpsPlgSmtpService::GetIdFromProgressL
// inline static
// ---------------------------------------------------------------------------
//
inline TMsvId CIpsPlgSmtpService::GetIdFromProgressL( const TDesC8& aProg )
    {
    FUNC_LOG;
    // Taken from symbian os help example code
    // Create and initialise a temporary TPckg object that 
    // can hold a message Id.
    TMsvId msgId;
    TPckg<TMsvId> param(msgId);
    
    // Copy the message Id returned from the ProgressL() call into the
    // temporary TPckg object.
    param.Copy( aProg ); 

    // Check the value of the returned message Id in the TPckg object.
    if ( msgId == KMsvNullIndexEntryId )
        {
        // Email is not correctly created, so leave
        User::Leave( KErrNotFound );
        }
    return msgId;
    }

// ---------------------------------------------------------------------------
// CIpsPlgSmtpService::GetOriginalCharsetL
// ---------------------------------------------------------------------------
//
TUid CIpsPlgSmtpService::GetOriginalCharsetL( TMsvId aOriginalMsgId )
    {
    FUNC_LOG;
    TUid ret;
    CMsvEntry* oEntry = iSession.GetEntryL( aOriginalMsgId );
    CleanupStack::PushL( oEntry );
    
    CImEmailMessage* cMessage = CImEmailMessage::NewLC( *oEntry );
    
    // <cmail> Added check for body text because GetCharacterSetL will
    // hang if there is no body text
    CMsvStore* store = oEntry->EditStoreL();
    CleanupStack::PushL(store);

    TUint charId;
    
    if( store->HasBodyTextL() )
        {
        TBool override=EFalse;
        cMessage->GetCharacterSetL( aOriginalMsgId, charId, override );
        ret.iUid = charId;
        }
    else
        {
        ret.iUid = KCharacterSetIdentifierUtf8;
        }
    
    CleanupStack::PopAndDestroy( 3, oEntry );//oEntry, cMessage, store
    // </cmail>
      
    return ret;
    }

// ---------------------------------------------------------------------------
// CIpsPlgSmtpService::SetCharactersetL
// ---------------------------------------------------------------------------
//
void CIpsPlgSmtpService::SetCharactersetL( CMsvEntry& aEntry, TUid aCharset )
    {
    FUNC_LOG;
    //Set character set for outgoing message. We use the one 
    //that was in the original message.     
    CMsvStore* store = aEntry.EditStoreL();
    CleanupStack::PushL(store);

    CImHeader* header = CImHeader::NewLC();
    header->RestoreL( *store );
    
    TImEmailTransformingInfo info;
    info.SetToDefault(ESendAsMimeEmail);    
    info.SetHeaderAndBodyCharset( aCharset.iUid );
    info.StoreL( *store );
    
    header->StoreL( *store );
    store->CommitL();
    CleanupStack::PopAndDestroy( 2, store ); // store, header       
    }
// End of File

