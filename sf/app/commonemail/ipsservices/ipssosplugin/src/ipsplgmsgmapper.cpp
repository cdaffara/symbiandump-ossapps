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
* Description: This file implements class CIpsPlgMsgMapper.
*
*/



#include "emailtrace.h"
#include "ipsplgheaders.h"

_LIT( KMimeTypeTextPlain, "text/plain" );
_LIT( KMimeTypeTextHtml, "text/html" );
_LIT( KMimeTypeTextRtf, "text/rtf" );
_LIT( KSlash, "/" );

_LIT( KCharSemicolon, ";" );
_LIT( KCharEquals, "=" );
_LIT( KCharsetTag, "CHARSET" );
_LIT( KMessageExtension, ".eml" );

// Supported multipart content types
_LIT( KMimeTypeMultipartRelated, "multipart/related" );
_LIT( KMimeTypeMultipartMixed, "multipart/mixed" );
_LIT( KMimeTypeMultipartAlternative, "multipart/alternative" );
_LIT( KMimeTypeMultipartDigest, "multipart/digest" );
_LIT( KMimeTypeMultipartRfc822, "message/rfc822" );
_LIT( KMimeTypeMultipartPartial, "message/partial" );
_LIT( KMimeTypeMultipartDirectory, "text/directory" );
_LIT( KMimeTypeMultipartExternalBody, "message/external-body" );
_LIT( KMimeTypeMultipartParallel, "multipart/parallel" );

const TInt KCharEscape       = 0x5C;
const TInt KCharNonPrintable = 0x1F;
const TInt KCharAt           = 0x40;
const TInt KMaxContentTypeLength = 200;
const TInt KEmbeddedMsgExtensionLength  = 4;

// ======== CONSTRUCTORS & DESTRUCTOR ========

// ---------------------------------------------------------------------------
// CIpsPlgMsgMapper::NewL()
// ---------------------------------------------------------------------------
//
CIpsPlgMsgMapper* CIpsPlgMsgMapper::NewL(
    CMsvSession& aSession,
    CIpsPlgSosBasePlugin& aPlugin )
    {
    FUNC_LOG;
    CIpsPlgMsgMapper* self = CIpsPlgMsgMapper::NewLC( aSession, aPlugin );
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------------------------
// CIpsPlgMsgMapper::NewLC()
// ---------------------------------------------------------------------------
//
CIpsPlgMsgMapper* CIpsPlgMsgMapper::NewLC(
    CMsvSession& aSession,
    CIpsPlgSosBasePlugin& aPlugin )
    {
    FUNC_LOG;
    CIpsPlgMsgMapper* self =
        new( ELeave ) CIpsPlgMsgMapper( aSession, aPlugin );
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }

// ---------------------------------------------------------------------------
// CIpsPlgMsgMapper::~CIpsPlgMsgMapper()
// ---------------------------------------------------------------------------
//
CIpsPlgMsgMapper::~CIpsPlgMsgMapper()
    {
    FUNC_LOG;
    }

// ---------------------------------------------------------------------------
// CIpsPlgMsgMapper::CIpsPlgMsgMapper()
// ---------------------------------------------------------------------------
//
CIpsPlgMsgMapper::CIpsPlgMsgMapper(
    CMsvSession& aSession,
    CIpsPlgSosBasePlugin& aPlugin )
    : iSession( aSession ), iPlugin( aPlugin )
    {
    FUNC_LOG;
    }

// ---------------------------------------------------------------------------
// CIpsPlgMsgMapper::ConstructL()
// ---------------------------------------------------------------------------
//
void CIpsPlgMsgMapper::ConstructL()
    {
    FUNC_LOG;
    }

// ---------------------------------------------------------------------------
// CIpsPlgMsgMapper::GetMailMessageL()
// ---------------------------------------------------------------------------
//
CFSMailMessage* CIpsPlgMsgMapper::GetMailMessageL(
    CMsvEntry& aEntry )
    {
    FUNC_LOG;
    TFSMailMsgId msgId( iPlugin.PluginId(), aEntry.Entry().Id() );
    CFSMailMessage* fsMsg = CFSMailMessage::NewLC( msgId );

    fsMsg->SetFolderId(
        TFSMailMsgId( iPlugin.PluginId(), aEntry.Entry().Parent() ) );
    // ERLN-7YUEX3
    fsMsg->SetMailBoxId(TFSMailMsgId( iPlugin.PluginId(), aEntry.Entry().iServiceId ));
    // End ERLN-7YUEX3
    
    CMsvStore* store( NULL );

    if ( aEntry.HasStoreL() )
        {
        store = aEntry.ReadStoreL();
        CleanupStack::PushL( store );
        }

    SetEnvelopeL( &aEntry, store, *fsMsg );

    // Apparently, this should be done only with
    // EFSMsgDataStructure, but EFSMsgDataEnvelope is currently
    // used by assuming that it reads also the content-type of
    // the message
    SetStructureL( &aEntry, *fsMsg );

    TMsvEmailEntry tEntry( aEntry.Entry() );

    SetFlags( tEntry, *fsMsg );

    if ( store )
        {
        CleanupStack::PopAndDestroy( store );
        }

    CleanupStack::Pop( fsMsg );
    return fsMsg;
    }

// ---------------------------------------------------------------------------
// CIpsPlgMsgMapper::GetMailMessageL()
// ---------------------------------------------------------------------------
//
CFSMailMessage* CIpsPlgMsgMapper::GetMailMessageL(
    const TFSMailMsgId& aMailboxId,
    const TMsvEmailEntry& aEntry,
    const TFSMailDetails& aDetails )
    {
    FUNC_LOG;
    TFSMailMsgId msgId( iPlugin.PluginId(), aEntry.Id() );
    CFSMailMessage* result = CFSMailMessage::NewLC( msgId );

    result->SetMailBoxId( aMailboxId );
    result->SetFolderId(
        TFSMailMsgId( iPlugin.PluginId(), aEntry.Parent() ) );

    SetFlags( aEntry, *result );
    SetFetchStateL( aEntry, aEntry.Id(), EFalse, *result );

    switch( aDetails )
        {
        case EFSMsgDataDate:
            SetDateL( aEntry, *result );
            break;
        case EFSMsgDataSubject:
            SetSubjectL( aEntry, *result );
            break;
        case EFSMsgDataSender:
            SetSenderL( aEntry.iDetails, *result );
            break;
        case EFSMsgDataEnvelope:
        case EFSMsgDataStructure:
            {
            CMsvStore* store( NULL );
            CMsvEntry* cEntry = iSession.GetEntryL( aEntry.Id() );

            if ( cEntry )
            	{
                CleanupStack::PushL( cEntry );
            	if ( cEntry->HasStoreL() )
					{
					store = cEntry->ReadStoreL();
					CleanupStack::PushL( store );
					}

				SetEnvelopeL( cEntry, store, *result );
	
				// Apparently, this should be done only with
				// EFSMsgDataStructure, but EFSMsgDataEnvelope is currently
				// used by assuming that it reads also the content-type of
				// the message
				SetStructureL( cEntry, *result );
	
				if ( store )
					{
					CleanupStack::PopAndDestroy( store );
					}
				CleanupStack::PopAndDestroy( cEntry );
            	}
            break;
            }
        case EFSMsgDataIdOnly:
        default:
            break;
        }

    CleanupStack::Pop( result );
    return result;
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
void CIpsPlgMsgMapper::GetChildPartsL(
    const TFSMailMsgId& aMailBoxId,
    const TFSMailMsgId& aMessageId,
    const TFSMailMsgId& aParentId,
    RPointerArray<CFSMailMessagePart>& aParts )
    {
    FUNC_LOG;

    if ( aParentId.IsNullId() )
        // the caller wants the children of the message
        {
        GetChildPartsOfMessageEntryL( aMailBoxId, aMessageId, aParts );
        }
    else
        // the caller wants the parts of the 'real' subpart
        {
        GetChildPartsOfFolderEntryL( aMailBoxId, aMessageId, aParentId.Id(),
            aParts );
        }
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
CFSMailMessagePart* CIpsPlgMsgMapper::GetMessagePartL(
    TMsvId aEntryId,
    const TFSMailMsgId& aMailBoxId,
    const TFSMailMsgId& aMessageId )
    {
    FUNC_LOG;
    CFSMailMessagePart* result( NULL );
    TInt status;
    TMsvId serviceId;
    TMsvEmailEntry tEntry;

    status = iSession.GetEntry( aEntryId, serviceId, tEntry );

    if ( status == KErrNone )
        {
        switch ( tEntry.iType.iUid )
            {
            case KUidMsvFolderEntryValue:
                {
                result = ConvertFolderEntry2MessagePartL( tEntry,
                    aMailBoxId, aMessageId );
                break;
                }
            case KUidMsvAttachmentEntryValue:
            case KUidMsvMessageEntryValue:
                {
                result = ConvertAttachmentEntry2MessagePartL( tEntry,
                    aMailBoxId, aMessageId );
                break;
                }
            case KUidMsvEmailTextEntryValue:
            case KUidMsvEmailHtmlEntryValue:
            case KUidMsvEmailExternalBodyEntryValue:
            case KUidMsvEmailRtfEntryValue:
                {
                result = ConvertBodyEntry2MessagePartL( tEntry, aMailBoxId,
                    aMessageId );
                break;
                }
            }
        }

    return result;
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
TBool CIpsPlgMsgMapper::ChangeTEntryFlagsL(
    TMsvEmailEntry& aEmlEntry,
    const CFSMailMessage& aMessage )
    {
    FUNC_LOG;
    TInt msgFlags = aMessage.GetFlags();
    TBool modified ( EFalse );
    TBool unread( aEmlEntry.Unread() );

// <qmail>
    if ( LogicalXor( unread, msgFlags & EFSMsgFlag_Read ) ||
         LogicalXor( unread, msgFlags & EFSMsgFlag_Read_Locally ))
// </qmail>
        {
        aEmlEntry.SetUnread( !unread );
        modified = ETrue;
        }

    // EFSMsgFlag_Low
    // EFSMsgFlag_Important
    TMsvPriority msgPriority;

    if ( msgFlags & EFSMsgFlag_Important )
        {
        msgPriority = EMsvHighPriority;
        }
    else if ( msgFlags & EFSMsgFlag_Low )
        {
        msgPriority = EMsvLowPriority;
        }
    else
        {
        msgPriority = EMsvMediumPriority;
        }

    if ( aEmlEntry.Priority() != msgPriority )
        {
        aEmlEntry.SetPriority( msgPriority );
        modified = ETrue;
        }

    // EFSMsgFlag_FollowUpComplete: no counterpart in Symbian message

    // EFSMsgFlag_FollowUp: supported only with IMAP4 (see below)

    // EFSMsgFlag_Attachments: It does not seem reasonable to update
    // this flag here (but when attachments are added)

    // EFSMsgFlag_Multiple: no counterpart in Symbian message

    // EFSMsgFlag_CalendarMsg
    if( ( aEmlEntry.iMtm == KSenduiMtmSmtpUid ) && ( msgFlags & EFSMsgFlag_CalendarMsg ) )
        {
        if( !aEmlEntry.ICalendar() )
            {
            aEmlEntry.SetICalendar( ETrue );
            modified = ETrue;
            }
        }


    // EFSMsgFlag_Answered: supported only with IMAP4 (see below)

    // EFSMsgFlag_Forwarded: no counterpart in Symbian message in S60 3.1

    // EFSMsgFlag_OnlyToMe: no counterpart in Symbian message

    // EFSMsgFlag_RemoteDeleted: no counterpart in Symbian message

    // EFSMsgFlag_HasMsgSender: no counterpart in Symbian message


    //this applies to POP too. Just ignore the misleading naming
    // moved to fix EANA-7HUD2G, pop doesn't support permanent flags
    // EFSMsgFlag_Answered

    // IMAP flags
    if ( aEmlEntry.iMtm == KSenduiMtmImap4Uid )
        {
            // EFSMsgFlag_FollowUp
        if ( LogicalXor( aEmlEntry.FlaggedIMAP4Flag(),
                         ( msgFlags & EFSMsgFlag_FollowUp ) ) )
            {
            aEmlEntry.SetFlaggedIMAP4Flag( !aEmlEntry.FlaggedIMAP4Flag() );
            modified = ETrue;
            }
        // EFSMsgFlag_Answered
        if ( LogicalXor( aEmlEntry.AnsweredIMAP4Flag(),
                         ( msgFlags & EFSMsgFlag_Answered ) ) )
            {
            aEmlEntry.SetAnsweredIMAP4Flag( !aEmlEntry.AnsweredIMAP4Flag() );
            modified = ETrue;
            }
        }
    return modified;
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
void CIpsPlgMsgMapper::UpdateMessageFlagsL(
    TMsvId aEntryId,
    const CFSMailMessage& aMessage )
    {
    FUNC_LOG;
    // This function updates tEntry from aMessage
    CMsvEntry* cEntry = iSession.GetEntryL( aEntryId );
    CleanupStack::PushL( cEntry );

    TMsvEmailEntry tEntry( cEntry->Entry() );
    TBool isModified = ChangeTEntryFlagsL( tEntry, aMessage );
    // <qmail>
    if ( isModified )
        {
        cEntry->ChangeL( tEntry );
        }

    CleanupStack::PopAndDestroy( cEntry );
    // </qmail>
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
CMsvOperation* CIpsPlgMsgMapper::UpdateMessageFlagsAsyncL(
    TMsvId aEntryId,
    const CFSMailMessage& aMessage,
    TRequestStatus& aStatus )
    {
    FUNC_LOG;
    // This function updates tEntry from aMessage
    CMsvEntry* cEntry = iSession.GetEntryL( aEntryId );
    CleanupStack::PushL( cEntry );
    TMsvEmailEntry tEntry( cEntry->Entry() );

    TBool isModified = ChangeTEntryFlagsL( tEntry, aMessage );
    CMsvOperation* ops = NULL;
    if ( isModified )
        {
        ops = cEntry->ChangeL( tEntry, aStatus );
        }

    CleanupStack::PopAndDestroy( cEntry );
    return ops;
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
void CIpsPlgMsgMapper::SetFSMessageFlagsL(
    const TMsvEmailEntry& aEntry, CFSMailMessage& aMsg )
    {
    FUNC_LOG;
    // this is just stupid through call, but now not need to
    // move SetFlags to public
    SetFlags( aEntry, aMsg );
    }

// ---------------------------------------------------------------------------
// CIpsPlgMsgMapper::NewChildPartL
// The method supports currently only the multipart/alternative structure
// construction (which is needed in the meeting request messages).
// The creation of the multipart structure happens by changing the folder
// type of the (SMTP) message entry itself. The folder entry is created
// by Symbian MTM later when new parts are added below the multipart
// structure. This causes some problems because there is no new entry
// in this phase and so the new CFSMailMessagePart instance do not
// ---------------------------------------------------------------------------
CFSMailMessagePart* CIpsPlgMsgMapper::NewChildPartL(
    const TFSMailMsgId& /* aMailBoxId */,
    const TFSMailMsgId& aMessageId,
    const TFSMailMsgId& /* aParentPartId */,
    const TDesC& /*aContentType*/ )
    {
    FUNC_LOG;
    CFSMailMessagePart* result( NULL );
    result = CFSMailMessagePart::NewLC( aMessageId,
                TFSMailMsgId( aMessageId.PluginId().iUid,
                    KMsvNullIndexEntryIdValue ) );
    CleanupStack::Pop( result );
    return result;
    }

// ---------------------------------------------------------------------------
// CIpsPlgMsgMapper::SetDateL
// ---------------------------------------------------------------------------
//
void CIpsPlgMsgMapper::SetDateL(
    const TMsvEntry& aEntry,
    CFSMailMessage& aMsg )
    {
    FUNC_LOG;
    const TTime date =  aEntry.iDate;
    aMsg.SetDate( date );
    }

// ---------------------------------------------------------------------------
// CIpsPlgMsgMapper::SetSubjectL
// ---------------------------------------------------------------------------
//
void CIpsPlgMsgMapper::SetSubjectL(
    const TMsvEntry& aEntry,
    CFSMailMessage& aMsg )
    {
    FUNC_LOG;
    aMsg.SetSubject( aEntry.iDescription );
    }

// ---------------------------------------------------------------------------
// CIpsPlgMsgMapper::SetSenderL
// ---------------------------------------------------------------------------
//
void CIpsPlgMsgMapper::SetSenderL(
    TPtrC aSender,
    CFSMailMessage& aMsg )
    {
    FUNC_LOG;
    CFSMailAddress* addr = CFSMailAddress::NewLC();
    ConvertAddressL( aSender, *addr );
    aMsg.SetSender( addr );
    CleanupStack::Pop( addr );
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
void CIpsPlgMsgMapper::SetEnvelopeL(
    const CMsvEntry* aEntry,
    CMsvStore* aStore,
    CFSMailMessage& aMsg )
    {
    FUNC_LOG;
    CFSMailAddress* addr = NULL;

    TMsvEmailEntry tEntry( aEntry->Entry() );

    SetDateL( tEntry, aMsg );
    SetSubjectL( tEntry, aMsg );

    if ( aStore && aStore->IsPresentL( KUidMsgFileIMailHeader ) )
        {
        CImHeader* header = CImHeader::NewLC();
        header->RestoreL( *aStore );

        SetSenderL( header->From(), aMsg );

        const CDesCArray& toRecs = header->ToRecipients();

        for( TInt i=0; i<toRecs.Count(); i++ )
            {
            addr = CFSMailAddress::NewLC();
            ConvertAddressL( toRecs[i], *addr );
            aMsg.AppendToRecipient( addr );
            CleanupStack::Pop( addr );
            }

        const CDesCArray& ccRecs = header->CcRecipients();

        for( TInt i=0; i<ccRecs.Count(); i++ )
            {
            addr = CFSMailAddress::NewLC();
            ConvertAddressL( ccRecs[i], *addr );
            aMsg.AppendCCRecipient( addr );
            CleanupStack::Pop( addr );
            }

        const CDesCArray& bccRecs = header->BccRecipients();

        for( TInt i=0; i< bccRecs.Count(); i++ )
            {
            addr = CFSMailAddress::NewLC();
            ConvertAddressL( bccRecs[i], *addr );
            aMsg.AppendBCCRecipient( addr );
            CleanupStack::Pop( addr );
            }

        CleanupStack::PopAndDestroy( header );
        }
    else
        {
        // if the email header stream is not available, the sender
        // information is taken from the index entry
        SetSenderL( aEntry->Entry().iDetails, aMsg );
        }
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
void CIpsPlgMsgMapper::SetStructureL(
    const CMsvEntry* aEntry,
    CFSMailMessage& aMessage )
    {
    FUNC_LOG;
    // Find out the content type by studying the child entry:
    // If there are no children, the message contents are not fetched and
    // thus the content type cannot be inferred. On the other hand,
    // the message entry should never have more than one child entry
    // (otherwise, it should be a multipart message)
    if ( aEntry->Count() == 1 )
        {
        TMsvEmailEntry tEntry = (*aEntry)[0];
        if ( tEntry.iType == KUidMsvFolderEntry )
            {
            aMessage.SetContentType(
                ConvertMultipartMimeType( tEntry.MessageFolderType() ) );
            }
        else
            // The case in which the content entry is directly below
            // the message entry..
            {
            aMessage.SetContentType( KMimeTypeMultipartMixed );
            }
        }
    else
        {
        // mark message structure state unknown if we don't have any
        // child parts. This needed to ui start fetch
        aMessage.SetMessagePartsStatus( EFSEmailStructureUnknown );
        }

    aMessage.SetContentSize( aEntry->Entry().iSize );
    }

// ---------------------------------------------------------------------------
// CIpsPlgMsgMapper::SetFlags
// FS framework's flag definitions and methods are somewhat clumsy and thus
// there is quite much code for a pretty simple function..
// ---------------------------------------------------------------------------
void CIpsPlgMsgMapper::SetFlags(
    const TMsvEmailEntry& aEntry,
    CFSMailMessage& aMsg )
    {
    FUNC_LOG;

    TBool forwardedMeetingRequest = EFalse;
    
    // EFSMsgFlag_Read
    // EFSMsgFlag_Read_Locally
    if ( aEntry.Unread() )
        {
            aMsg.ResetFlag( EFSMsgFlag_Read );
            aMsg.ResetFlag( EFSMsgFlag_Read_Locally );
            }
    else
        {
        aMsg.SetFlag( EFSMsgFlag_Read );
        aMsg.SetFlag( EFSMsgFlag_Read_Locally );
        }

    // EFSMsgFlag_Low
    // EFSMsgFlag_Important
    switch ( aEntry.Priority() )
        {
        case EMsvHighPriority:
            {
            aMsg.SetFlag( EFSMsgFlag_Important );
            aMsg.ResetFlag( EFSMsgFlag_Low );
            break;
            }
        case EMsvMediumPriority:
            {
            aMsg.ResetFlag( EFSMsgFlag_Important );
            aMsg.ResetFlag( EFSMsgFlag_Low );
            break;
            }
        case EMsvLowPriority:
            {
            aMsg.ResetFlag( EFSMsgFlag_Important );
            aMsg.SetFlag( EFSMsgFlag_Low );
            break;
            }
        }

    // EFSMsgFlag_FollowUpComplete: cannot be supported with Symbian messages?

    // EFSMsgFlag_FollowUp: supported only with IMAP4 messages (see below)
    aMsg.ResetFlag( EFSMsgFlag_FollowUp );

// <cmail>
    //only for incomplete POP3 messages
    if ( aEntry.iMtm.iUid == KSenduiMtmPop3UidValue &&
            ( !aEntry.Complete() || aEntry.PartialDownloaded () ) )
        {
        TRAP_IGNORE( AttaCheckForIncompleteMsgL( aEntry, aMsg ) );
        }
// </cmail>
    else
        {
        // EFSMsgFlag_Attachments
        if ( aEntry.Attachment() )
            {
            aMsg.SetFlag( EFSMsgFlag_Attachments );

            if ( aEntry.ICalendar() )
                {
                // <cmail> implementation changed due to cs warning
                TInt attCount = 0;
                TRAPD ( err, attCount = GetAttachmentCountL( aEntry ) );
                if ( err == KErrNone && attCount == 1 )
                	{
                	CMsvEntry* cEntry = NULL;
                	TRAPD ( err2, cEntry = iSession.GetEntryL( aEntry.Id() ) );
                	if ( err2 == KErrNone && cEntry->Count() == 1 )
                		{
                		TMsvEmailEntry tEntry = (*cEntry)[0];
                		TImEmailFolderType ft = tEntry.MessageFolderType();
                		if ( tEntry.iType == KUidMsvFolderEntry && ft == EFolderTypeMixed  )
                			{
                           	// Message with calendar object. But based on content type
                			// (multipart/mixed) we know that this is meeting request
                			// forwarded as email, so it must be seen as normal email.
                   			forwardedMeetingRequest = ETrue;
                   			aMsg.ResetFlag( EFSMsgFlag_CalendarMsg );
                   			aMsg.SetFlag( EFSMsgFlag_Attachments );
                        	}
						else
							{
							// Only text/calendar part included as attachment
							aMsg.ResetFlag( EFSMsgFlag_Attachments );
							// <qmail> remove call to SetAttachmentFlagL(), because shouln't be needed any more
							}
                		}
                	delete cEntry;
                	cEntry = NULL;
                	}
                // </cmail>
                }
            }
        else
            {
            aMsg.ResetFlag( EFSMsgFlag_Attachments );
            }
        }

    // EFSMsgFlag_Multiple: currently this is not used anywhere and
    // to evaluate the value, the CMsvStore of the message should be
    // accessed which is relatively slow (compared to the access of
    // the index entry)

    // EFSMsgFlag_CalendarMsg
    if ( aEntry.ICalendar() && !forwardedMeetingRequest ) // <cmail>
        {
        aMsg.SetFlag( EFSMsgFlag_CalendarMsg );
        }
    else if( !( aMsg.GetFlags() & EFSMsgFlag_CalendarMsg ) )
        {
        // Flag not set by mrui, we can reset it
        aMsg.ResetFlag( EFSMsgFlag_CalendarMsg );
        }

    // EFSMsgFlag_Answered: supported only with IMAP4 messages (see below)
    aMsg.ResetFlag( EFSMsgFlag_Answered );

    // EFSMsgFlag_Forwarded: not supported in S60 3.1
    aMsg.ResetFlag( EFSMsgFlag_Forwarded );

    // EFSMsgFlag_OnlyToMe: like EFSMsgFlag_Multiple

    // EFSMsgFlag_RemoteDeleted: no reasonable use in IPS

    // EFSMsgFlag_HasMsgSender: currently not used anywhere. Could be
    // probably be supported by checking the contents of TmsvEntry::iDetails
    // but this should be tested

    // Additional logic for IMAP4 messages
    if ( aEntry.iMtm == KSenduiMtmImap4Uid )
        {
        if ( aEntry.FlaggedIMAP4Flag() )
            {
            aMsg.SetFlag( EFSMsgFlag_FollowUp );
            }
        // moved to fix EANA-7HUD2G, pop doesn't support permanent flags
        if ( aEntry.AnsweredIMAP4Flag() )
            {
            aMsg.SetFlag( EFSMsgFlag_Answered );
            }
        }
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
void CIpsPlgMsgMapper::SetFetchStateL(
    const TMsvEmailEntry& aEntry,
    TMsvId aMsgMainId,
    TBool aIsAtta,
    CFSMailMessagePart& aMessage )
    {
    FUNC_LOG;
    if ( aEntry.iMtm.iUid == KSenduiMtmPop3UidValue && 
            aEntry.Id() != aMsgMainId &&
            !aIsAtta )
        {
        TInt error = KErrNone;
        TMsvEmailEntry emlEntry;
        TMsvId dummy;

        error = iSession.GetEntry( aMsgMainId , dummy, emlEntry );

        if ( error == KErrNone )
            {
            DoSetFetchStateL( emlEntry, aMsgMainId, aIsAtta, aMessage );
            }
        else
            {
            aMessage.SetMessagePartsStatus( EFSEmailStructureUnknown );
            }
        }
    else
        {
        DoSetFetchStateL( aEntry, aMsgMainId, aIsAtta, aMessage );
        }
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
void CIpsPlgMsgMapper::DoSetFetchStateL(
    const TMsvEmailEntry& aEntry,
    TMsvId aMsgMainId,
    TBool aIsAtta,
    CFSMailMessagePart& aMessage )
    {
    FUNC_LOG;
    if ( aEntry.PartialDownloaded() )
        {
        aMessage.SetMessagePartsStatus( EFSPartial );
        }
    else if ( aEntry.BodyTextComplete() && !aIsAtta )
        {
        aMessage.SetMessagePartsStatus( EFSFull );
        }
    else if ( aEntry.Complete() && !aIsAtta )
        {
        aMessage.SetMessagePartsStatus( EFSFull );
        }
    else if ( aEntry.Complete() && aIsAtta )
        {
        CMsvEntry* cEntry = iSession.GetEntryL( aEntry.Id() );

        CleanupStack::PushL( cEntry );

        TBool hasStore = cEntry->HasStoreL();

        if ( hasStore )
            {
            CMsvStore* store = cEntry->EditStoreL();
    
            CleanupStack::PushL( store );

            MMsvAttachmentManager& attMgr = store->AttachmentManagerL();
        
            if ( attMgr.AttachmentCount() )
                {
                aMessage.SetMessagePartsStatus( EFSFull );
                }
            else
                {
                aMessage.SetMessagePartsStatus( EFSNone );
                }
            
            CleanupStack::PopAndDestroy( store );
            }

        CleanupStack::PopAndDestroy( cEntry );
        }
    else if ( aEntry.Id() != aMsgMainId )
        {
        // fetch state of some message part, then we sure know
        // structure, but its not complete nor partial so state must be
        // none
        aMessage.SetMessagePartsStatus( EFSNone );
        }
    else
        {
        aMessage.SetMessagePartsStatus( EFSEmailStructureUnknown );
        }
    }


// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
TPtrC CIpsPlgMsgMapper::ConvertMultipartMimeType(
    TImEmailFolderType aFolderType ) const
    {
    FUNC_LOG;
    TPtrC result;

    switch ( aFolderType )
    {
    case EFolderTypeRelated:
        {
        result.Set( KMimeTypeMultipartRelated );
        break;
        }
    case EFolderTypeMixed:
        {
        result.Set( KMimeTypeMultipartMixed );
        break;
        }
    case EFolderTypeParallel:
        {
        result.Set( KMimeTypeMultipartParallel );
        break;
        }
    case EFolderTypeAlternative:
        {
        result.Set( KMimeTypeMultipartAlternative );
        break;
        }
    case EFolderTypeDigest:
        {
        result.Set( KMimeTypeMultipartDigest );
        break;
        }
    case EFolderTypeRFC822:
        {
        result.Set( KMimeTypeMultipartRfc822 );
        break;
        }
    case EFolderTypePartial:
        {
        result.Set( KMimeTypeMultipartPartial );
        break;
        }
    case EFolderTypeDirectory:
        {
        result.Set( KMimeTypeMultipartDirectory );
        break;
        }
    case EFolderTypeExternal:
        {
        result.Set( KMimeTypeMultipartExternalBody );
        break;
        }
    case EFolderTypeUnknown:
    default:
        {
        result.Set( KNullDesC16 );
        break;
        }
    }

    return result;
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
// <cmail>
TInt CIpsPlgMsgMapper::ConvertBodyPartMimeType(
    const TMsvEmailEntry& aEntry,
    TDes& aMimeType )
    {
    FUNC_LOG;
    TInt result( KErrNone );

    switch ( aEntry.iType.iUid )
    {
    case KUidMsvEmailTextEntryValue:
        {
        if( aEntry.ICalendar() )
            {
            aMimeType.Append( KFSMailContentTypeTextCalendar );
            }
        else
            {
            aMimeType.Append( KMimeTypeTextPlain );
            }
        break;
        }
    case KUidMsvEmailHtmlEntryValue:
        {
            aMimeType.Append( KMimeTypeTextHtml );
        break;
        }
    case KUidMsvEmailExternalBodyEntryValue:
        {
        // This seems a bit suspicious, but it is improbable that this case
        // is ever used..
            aMimeType.Append( KMimeTypeMultipartExternalBody );
        break;
        }
    case KUidMsvEmailRtfEntryValue:
        {
            aMimeType.Append( KMimeTypeTextRtf );
        break;
        }
    default:
        {
        result = KErrNotFound;
        }
    }
    return result;
    }
// </cmail>

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
void CIpsPlgMsgMapper::ConvertAddressL(
    TPtrC aSourceAddress,
    CFSMailAddress& aTargetAddress )
    {
    FUNC_LOG;
    TInt status( KErrNone );
    TImMessageField mailField;

    aTargetAddress.SetEmailAddress(
        mailField.GetValidInternetEmailAddressFromString( aSourceAddress ) );

// <cmail>
    // If previous validation fails because of invalid email address.
    // e.g '@' is missing. Then address between '<' and '>' marks is used.
    TInt start( aSourceAddress.LocateReverse('<') );
    if ( start != KErrNotFound && start > 0)
    	{
    	TInt end( aSourceAddress.LocateReverse('>') );
    	start += 1;
    	aTargetAddress.SetEmailAddress( aSourceAddress.Mid( start, ( end - start ) ) );
    	}
// </cmail>

    TPtrC alias = mailField.GetValidAlias( aSourceAddress, status );
    if ( status == KErrNone )
        {
        TPtr alias2 = alias.AllocLC()->Des(); // have to create copy because alias might change
        for ( TInt ii = 0; ii < alias2.Length(); ii++ )
            {
            if ( alias2[ii] <= KCharNonPrintable )
                {
                alias2.Delete( ii, 1 );
                ii--;
                }
            else if ( ii < ( alias2.Length() - 1 ) &&
                      alias2[ii] == KCharEscape )
                {
                if( alias2[ii+1] == KCharAt )
                    {
                    alias2.Delete( ii, 1 );
                    ii--;
                    }
                }
            }
        aTargetAddress.SetDisplayName( alias2 ); // creates its own copy
        CleanupStack::PopAndDestroy();
        }
    else
        {
// <cmail>
       	aTargetAddress.SetDisplayName( aTargetAddress.GetEmailAddress() );
// </cmail>
        }
    }


// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
void CIpsPlgMsgMapper::SetContentTypeL(
    CImMimeHeader& aMimeHeader,
    CFSMailMessagePart& aMessagePart )
    {
    FUNC_LOG;
    HBufC8* buffer8 = HBufC8::NewLC(
        aMimeHeader.ContentType().Length() +
        aMimeHeader.ContentSubType().Length() + 1 );
    buffer8->Des().Append( aMimeHeader.ContentType() );
    buffer8->Des().Append( KSlash );
    buffer8->Des().Append( aMimeHeader.ContentSubType() );
    HBufC* buffer = HBufC::NewLC( buffer8->Length() );
    buffer->Des().Copy( buffer8->Des() );
    aMessagePart.SetContentType( *buffer );
    CleanupStack::PopAndDestroy( 2, buffer8 ); // buffer
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
void CIpsPlgMsgMapper::GetChildPartsOfMessageEntryL(
    const TFSMailMsgId& aMailBoxId,
    const TFSMailMsgId& aMessageId,
    RPointerArray<CFSMailMessagePart>& aParts )
    {
    FUNC_LOG;
    CFSMailMessagePart* childPart( NULL );
    CMsvEntry* cEntry = iSession.GetEntryL( aMessageId.Id() );
    CleanupStack::PushL( cEntry );

    if ( cEntry->Count() > 0 )
        {
        // Note: it is assumed that there is only one child which is
        // either the body part (simple message) or the folder entry
        // representing the main level multipart structure
        if ( (*cEntry)[0].iType == KUidMsvFolderEntry )
            {
            GetChildPartsOfFolderEntryL( aMailBoxId, aMessageId,
                (*cEntry)[0].Id(), aParts);
            }
        else
            {
            childPart = GetMessagePartL( (*cEntry)[0].Id(), aMailBoxId, aMessageId );
            if( childPart )
                {
                aParts.AppendL( childPart );
                }
            }
        }
    CleanupStack::PopAndDestroy( cEntry );
    }

// ---------------------------------------------------------------------------
// CIpsPlgMsgMapper::GetChildPartsOfFolderEntryL
// Note: if the aParentId does not refer to a folder entry, childCount
// will be zero and the method does not do anything.
// It assumed that the method is called mainly for the folder entries
// and thus CMsvEntry is constructed without checking the entry type
// beforehand. If the assumption is not correct, the performance can be
// optimized by checking the entry type first.
// ---------------------------------------------------------------------------
void CIpsPlgMsgMapper::GetChildPartsOfFolderEntryL(
    const TFSMailMsgId& aMailBoxId,
    const TFSMailMsgId& aMessageId,
    TMsvId aParentId,
    RPointerArray<CFSMailMessagePart>& aParts )
    {
    FUNC_LOG;
    CMsvEntry* cEntry = iSession.GetEntryL( aParentId );
    CleanupStack::PushL( cEntry );
    CFSMailMessagePart* childPart( NULL );
    TInt childCount( cEntry->Count() );
    TInt i;
    TInt position;
    TBool textBodyPartFound( EFalse );

    for (i = 0; i < childCount; i++ )
        {
        childPart = GetMessagePartL( (*cEntry)[i].Id(), aMailBoxId, aMessageId );

        // Child parts have to be ordered so that the plain text body
        // part is first (if such exists) and the HTML body is next.
        // The rest of the children can be in any order.
        if ( (*cEntry)[i].iType == KUidMsvEmailTextEntry )
            {
            position = 0;
            textBodyPartFound = ETrue;
            }
        else if ( (*cEntry)[i].iType == KUidMsvEmailHtmlEntry )
            {
            if ( textBodyPartFound )
                {
                position = 1;
                }
            else
                {
                position = 0;
                }
            }
        else
            {
            position = i;
            }

        // Insert the new child part to the result array
        if ( childPart )
            {
            aParts.InsertL( childPart, position );
            childPart = NULL;
            }
        }
    CleanupStack::PopAndDestroy( cEntry );
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
CFSMailMessagePart* CIpsPlgMsgMapper::ConvertBodyEntry2MessagePartL(
    const TMsvEmailEntry& aEntry,
    const TFSMailMsgId& aMailBoxId,
    const TFSMailMsgId& aMessageId )
    {
    FUNC_LOG;
    CFSMailMessagePart* result( NULL );
    TInt status;
    HBufC* buf = HBufC::NewLC( KMaxContentTypeLength );
    TPtr contentType = buf->Des();

    status = ConvertBodyPartMimeType( aEntry, contentType );
    
    __ASSERT_DEBUG( ( status == KErrNone ),
        User::Panic( KIpsPlgPanicCategory, EIpsPlgInvalidEntry ) );
    if ( status == KErrNone )
        {
        result = CFSMailMessagePart::NewLC(
            aMessageId, TFSMailMsgId( iPlugin.PluginId(), aEntry.Id() ) );

        // If mimetype is 'text/html' add charset parameter
        if ( aEntry.iType.iUid == KUidMsvEmailHtmlEntryValue )
            {
            GetCharsetParameterL( aEntry, contentType );
            }
        result->SetContentType( contentType );
        result->SetMailBoxId( aMailBoxId );

        // Size
        result->SetContentSize( aEntry.iSize );

        if ( aEntry.Complete() )
            {
            TMsvEmailEntry parent;
            TMsvId dummy;
            if ( aEntry.PartialDownloaded() )
                {
                CMsvEntry* cEntry = iSession.GetEntryL( aEntry.Id() );
                CleanupStack::PushL( cEntry );
                if ( cEntry->HasStoreL() )
                    {
                    CMsvStore* store = cEntry->ReadStoreL();
                    CleanupStack::PushL( store );
                    result->SetFetchedContentSize( store->SizeL() );
                    CleanupStack::PopAndDestroy( store );
                    }

                // sometimes store size is bigger than msg size
                // even if msg is partially downloaded, then set
                // fetched content size smaller than msg size
                TUint fetchedSize = result->FetchedContentSize();
                TUint contentSize = result->ContentSize();
                if ( (contentSize > 0) && (fetchedSize > contentSize) )
                    {
                    result->SetFetchedContentSize( contentSize - 1 );
                    }

                CleanupStack::PopAndDestroy( cEntry );
                }
            else if ( aEntry.iMtm == KSenduiMtmPop3Uid &&
                    iSession.GetEntry( aEntry.Parent(), dummy, parent )
                    == KErrNone && parent.PartialDownloaded() )
                {
                // we cant know remote size of pop body part, but
                // we know it is not completely fetched at this point
                // set content size fetchedSize+1 to make framework know that
                // this message is not completely fetched

                result->SetContentSize( aEntry.iSize+1 );
                result->SetFetchedContentSize( aEntry.iSize );

                }
            else
                {
                result->SetFetchedContentSize( aEntry.iSize );
                }
            // This is "gludge-fix" for situation when
            // content size is zero and message is complete
            // mark sizes to 1 then ui not assume that something
            // missing and not try to fetch it
            if ( aEntry.iSize == 0 )
                {
                result->SetContentSize(1);
                result->SetFetchedContentSize(1);
                }
            }
        else
            {
            result->SetFetchedContentSize( 0 );
            }

        CleanupStack::Pop( result );
        }
    if(result) // coverity error fix: result might be null
        {
        SetFetchStateL( aEntry, aMessageId.Id(), EFalse, *result );
        }

    CleanupStack::PopAndDestroy( buf );
    return result;
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
CFSMailMessagePart* CIpsPlgMsgMapper::ConvertAttachmentEntry2MessagePartL(
    const TMsvEmailEntry& aEntry,
    const TFSMailMsgId& aMailBoxId,
    const TFSMailMsgId& aMessageId )
    {
    FUNC_LOG;
    CFSMailMessagePart* result( NULL );
    HBufC* buffer;
    CMsvEntry* cEntry;

    __ASSERT_DEBUG( ( aEntry.iType == KUidMsvAttachmentEntry || 
    	aEntry.iType == KUidMsvMessageEntry ),
        User::Panic( KIpsPlgPanicCategory, EIpsPlgInvalidEntry ) );

    cEntry = iSession.GetEntryL( aEntry.Id() );
    CleanupStack::PushL( cEntry );

    if ( cEntry->HasStoreL() )
        {
        result = CFSMailMessagePart::NewLC( aMessageId,
            TFSMailMsgId( iPlugin.PluginId(), aEntry.Id() ) );

        CMsvStore* store = cEntry->ReadStoreL();
        CleanupStack::PushL( store );

        if ( store->IsPresentL( KUidMsgFileMimeHeader ) )
            {
            CImMimeHeader* mimeHeader = CImMimeHeader::NewLC();
            mimeHeader->RestoreL( *store );

            // Content-type
            if ( aEntry.iType.iUid == KUidMsvMessageEntryValue )
                {
                result->SetContentType( KMimeTypeMultipartRfc822 );
                }
            else
                {
                SetContentTypeL( *mimeHeader, *result );
                }

            // Content-description
            buffer = HBufC::NewLC(
                mimeHeader->ContentDescription().Length ( ) );
            buffer->Des().Copy ( mimeHeader->ContentDescription ( ) );
            result->SetContentDescription ( *buffer );
            CleanupStack::PopAndDestroy ( buffer );
            buffer = NULL;

            // Content-disposition
            buffer = HBufC::NewLC(
                mimeHeader->ContentDisposition().Length ( ) );
            buffer->Des().Copy( mimeHeader->ContentDisposition() );
            result->SetContentDisposition( *buffer );
            CleanupStack::PopAndDestroy( buffer );
            buffer = NULL;

            // Content-type parameters

            CDesC8Array& sourceArray( mimeHeader->ContentTypeParams() );
            CDesCArray& targetArray( result->ContentTypeParameters() );
            for ( TInt i=0; i<sourceArray.Count(); i++ )
                {
                buffer = HBufC::NewLC( sourceArray[i].Length() );
                buffer->Des().Copy( sourceArray[i] );
                targetArray.AppendL( buffer->Des() );
                CleanupStack::PopAndDestroy( buffer );
                buffer = NULL;
                i++;
                }

            // Content ID
            buffer = HBufC::NewLC(
                mimeHeader->ContentID().Length ( ) );
            buffer->Des().Copy( mimeHeader->ContentID() );
            result->SetContentIDL( *buffer );
            CleanupStack::PopAndDestroy( buffer );
            buffer = NULL;

            // Content-class: not supported by Symbian (non-standard field)

            CleanupStack::PopAndDestroy( mimeHeader );
            }
        // Name
        if ( aEntry.iType.iUid == KUidMsvMessageEntryValue )
            {
            HBufC* att = HBufC::NewLC( aEntry.iDescription.Length() + KEmbeddedMsgExtensionLength );
            att->Des().Copy( aEntry.iDescription );
            att->Des().Append( KMessageExtension );
            result->SetAttachmentNameL( att->Des() );
            CleanupStack::PopAndDestroy( att );
            }
        else
            {
            result->SetAttachmentNameL( aEntry.iDetails );
            }

        // Size
        result->SetContentSize( aEntry.iSize );

        if ( aEntry.Complete() )
            {
            result->SetFetchedContentSize( aEntry.iSize );
            }
        else
            {
            result->SetFetchedContentSize( 0 );
            }
        result->SetMailBoxId( aMailBoxId );
        CleanupStack::PopAndDestroy(store);
        
        SetFetchStateL( aEntry, aMessageId.Id(), ETrue, *result );
        CleanupStack::Pop( result );
        }
    CleanupStack::PopAndDestroy( cEntry );

    return result;
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
CFSMailMessagePart* CIpsPlgMsgMapper::ConvertFolderEntry2MessagePartL(
    const TMsvEmailEntry& aEntry,
    const TFSMailMsgId& aMailBoxId,
    const TFSMailMsgId& aMessageId )
    {
    FUNC_LOG;
    CFSMailMessagePart* result( NULL );

    __ASSERT_DEBUG( ( aEntry.iType == KUidMsvFolderEntry ),
        User::Panic( KIpsPlgPanicCategory, EIpsPlgInvalidEntry ) );

    TPtrC mimeType = ConvertMultipartMimeType( aEntry.MessageFolderType() );

    if ( mimeType.Length() > 0 )
        {
        result = CFSMailMessagePart::NewLC(
            aMessageId, TFSMailMsgId( iPlugin.PluginId(), aEntry.Id() ) );

        result->SetContentType( mimeType );
        result->SetMailBoxId( aMailBoxId );

        CleanupStack::Pop( result );
        }

    return result;
    }


// ---------------------------------------------------------------------------
//
//
// ---------------------------------------------------------------------------
// <cmail> implemented to get rid of cs warning
TInt CIpsPlgMsgMapper::GetAttachmentCountL(
    const TMsvEmailEntry& aEntry )
    {
    FUNC_LOG;
    TInt result( KErrNone );

    CMsvEntry *cEntry = iSession.GetEntryL( aEntry.Id() );
    CleanupStack::PushL( cEntry );
    CImEmailMessage* message = CImEmailMessage::NewLC( *cEntry );

    message->GetAttachmentsListL( cEntry->Entry().Id( ),
        CImEmailMessage::EAllAttachments, CImEmailMessage::EThisMessageOnly );

    result = message->Selection().Count();

    CleanupStack::PopAndDestroy( 2, cEntry ); // message, cEntry

    return result;
    }
// </cmail>

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
void CIpsPlgMsgMapper::AttaCheckForIncompleteMsgL(
        const TMsvEmailEntry& aEntry,
        CFSMailMessage& aMsg )
    {
    aMsg.ResetFlag( EFSMsgFlag_Attachments );

    TImEmailFolderType ft = aEntry.MessageFolderType();

    if ( ft == EFolderTypeUnknown )
        {
        CMsvEntry* cEntry = iSession.GetEntryL( aEntry.Id() );
        CleanupStack::PushL( cEntry );

        if ( cEntry->HasStoreL() )
            {
            CMsvStore* store = cEntry->ReadStoreL();
            CleanupStack::PushL( store );

            if ( store->IsPresentL( KUidMsgFileMimeHeader ) )
                {
                CImMimeHeader* mimeHeader = CImMimeHeader::NewLC();
                mimeHeader->RestoreL( *store );

                HBufC8* buffer8 = HBufC8::NewLC(
                mimeHeader->ContentType().Length() +
                mimeHeader->ContentSubType().Length() + 1 );

                buffer8->Des().Append( mimeHeader->ContentType() );
                buffer8->Des().Append( KSlash );
                buffer8->Des().Append( mimeHeader->ContentSubType() );

                HBufC* buffer = HBufC::NewLC( buffer8->Length() );
                buffer->Des().Copy( buffer8->Des() );

                if ( buffer->CompareF( KMimeTypeMultipartMixed ) == 0 ||
                     buffer->CompareF( KMimeTypeMultipartRelated ) == 0 )
                    {
                    aMsg.SetFlag( EFSMsgFlag_Attachments );
                    if ( !aEntry.Attachment() )
                    	{
                    	SetAttachmentFlagL( aEntry, ETrue );
                    	}
                    }

                CleanupStack::PopAndDestroy( 3, mimeHeader );
                }

            CleanupStack::PopAndDestroy( store );
            }
        CleanupStack::PopAndDestroy( cEntry );
        }
    else if ( ft == EFolderTypeMixed || ft == EFolderTypeRelated )
        {
        aMsg.SetFlag( EFSMsgFlag_Attachments );
        if ( !aEntry.Attachment() )
        	{
        	SetAttachmentFlagL( aEntry, ETrue );
        	}
        }

    if ( !aMsg.IsFlagSet( EFSMsgFlag_Attachments ) && aEntry.Attachment() )
    	{
    	SetAttachmentFlagL( aEntry, EFalse );
    	}
    }
// </cmail>


// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
// <cmail>
void CIpsPlgMsgMapper::SetAttachmentFlagL( const TMsvEmailEntry& /*aEntry*/,
										   TBool /*aHasAttachment*/ )
	{
	FUNC_LOG;
	// <qmail> commented out, causing freeze in message list
	/*CMsvEntry* cEntry = iSession.GetEntryL( aEntry.Id() );
	CleanupStack::PushL( cEntry );
	// Only text/calendar part included as attachment
	TMsvEmailEntry entryToBeChanged( aEntry );
	entryToBeChanged.SetAttachment( aHasAttachment );

	cEntry->ChangeL( entryToBeChanged );
	CleanupStack::PopAndDestroy( cEntry );*/
// </qmail>
	}
// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
void CIpsPlgMsgMapper::GetCharsetParameterL(
    const TMsvEmailEntry& aEntry, TDes& aContentType )
    {
    FUNC_LOG;

    CMsvEntry* cEntry = iSession.GetEntryL( aEntry.Id() );
    CleanupStack::PushL( cEntry );
    if ( cEntry->HasStoreL() )
        {
        CMsvStore* store = cEntry->ReadStoreL();
        CleanupStack::PushL( store );
        if ( store->IsPresentL( KUidMsgFileMimeHeader ) )
            {
            CImMimeHeader* mimeHeader = CImMimeHeader::NewLC();
            mimeHeader->RestoreL( *store );

            TInt count = mimeHeader->ContentTypeParams().MdcaCount();
            //INFO_1("# of CT params: %d", count);
            for ( TInt i = 0; i < count; i++ )
                {
                TPtrC8 key8 = mimeHeader->ContentTypeParams().MdcaPoint( i );
                //INFO_1("%S", &key8);
                TPtr16 keyUppercase16 = HBufC::NewLC( key8.Length() )->Des();
                keyUppercase16.Copy( key8 );
                keyUppercase16.UpperCase();
                if ( keyUppercase16.Compare( KCharsetTag ) == 0 &&
                     count >= i+1 ) // prevent possible indexing over array limits
                    {
                    // Starting to append text to the current content-type header..
                    // First, add ';'
                    aContentType.Append( KCharSemicolon );

                    // Then, 'CHARSET'
                    aContentType.Append( keyUppercase16 );

                    // '='
                    aContentType.Append( KCharEquals );

                    // Finally, the actual charset value (e.g. 'utf-8' or similar)
                    TPtrC8 value8 = mimeHeader->ContentTypeParams().MdcaPoint( i+1 );
                    TPtr16 value16 = HBufC::NewLC( value8.Length() )->Des();
                    value16.Copy( value8 );
                    aContentType.Append( value16 );
                    CleanupStack::PopAndDestroy(); // value16
                    }
                CleanupStack::PopAndDestroy(); // keyUppercase16
                } // for loop
            CleanupStack::PopAndDestroy( mimeHeader );
            }
        CleanupStack::PopAndDestroy( store );
        }
    CleanupStack::PopAndDestroy( cEntry );
    }
// </cmail>
