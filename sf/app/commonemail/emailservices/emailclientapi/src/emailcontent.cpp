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
* Description: This file implements class CEmailContent.
*
*/

#include <apmstd.h>

#include "emailcontent.h"
#include "CFSMailClient.h"
#include "emailclientapi.hrh"
#include "emailapiutils.h"

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
CEmailMessageContent* CEmailMessageContent::NewL( 
        CPluginData& aPluginData,
        const TMessageContentId& aMsgContentId,
        CFSMailMessagePart* aPart )    
    {

    CEmailMessageContent* self = new ( ELeave ) 
        CEmailMessageContent( aPluginData, aMsgContentId, aPart );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
CFSMailMessagePart& CEmailMessageContent::Part()
    {
    return *iPart;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
void CEmailMessageContent::SetPart( CFSMailMessagePart* aPart )
    {
    iPart = aPart;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
CPluginData& CEmailMessageContent::PluginData()
    {
    return iPluginData;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
void CEmailMessageContent::ConstructL()
    {
    iPlugin = iPluginData.ClaimInstanceL();
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
CEmailMessageContent::~CEmailMessageContent()
    {
    iPluginData.ReleaseInstance();
    delete iPart;
    delete iBuf;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
CEmailMessageContent::CEmailMessageContent(
        CPluginData& aPluginData,
        const TMessageContentId& aMsgContentId,
        CFSMailMessagePart* aPart):
        iPluginData( aPluginData ), 
        iPart( aPart ), 
        iMsgContentId( aMsgContentId ), 
        iBuf( NULL ), 
        iUsed( 0 )
    {
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
TEmailTypeId CEmailMessageContent::InterfaceId() const
    {
    return KEmailIFUidMessageContent;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
void CEmailMessageContent::Release()
    {
    delete this;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
TMessageContentId CEmailMessageContent::Id() const
    {
    return iMsgContentId;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
TPtrC CEmailMessageContent::ContentType() const
    {
    if (iPart)
        {
        return iPart->GetContentType();
        } 
    else
        {
        return TPtrC(0,0);
        }
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
void CEmailMessageContent::SetContentType( const TDesC& aContentType )
    {
    if (iPart)
        iPart->SetContentType( aContentType );
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
TPtrC CEmailMessageContent::ContentId() const
    {
    if (iPart)
        {
        return iPart->ContentID();
        }
    else
        {
        return TPtrC(0,0);
        }
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
void CEmailMessageContent::SetContentId( const TDesC& aContentId )
    {
    if (iPart)
        TRAP_IGNORE( iPart->SetContentIDL( aContentId ) );
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
TPtrC CEmailMessageContent::ContentDescription() const
    {
    if (iPart)
        {
        return iPart->ContentDescription();
        }
    else
        {
        return TPtrC(0,0);
        }
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
void CEmailMessageContent::SetContentDescription( 
    const TDesC& aContentDescription )
    {
    if (iPart)
        iPart->SetContentDescription( aContentDescription );
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
TPtrC CEmailMessageContent::ContentDisposition() const
    {
    if (iPart)
        {
        return iPart->ContentDisposition();
        }
    else
        {
        return TPtrC(0,0);
        }
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
void CEmailMessageContent::SetContentDisposition( 
    const TDesC& aContentDisposition )
    {
    if (iPart)
        iPart->SetContentDisposition( aContentDisposition );
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
TPtrC CEmailMessageContent::ContentClass() const
    {
    if (iPart)
        {
        return iPart->GetContentClass();
        }
    else
        {
        return TPtrC(0,0);
        }
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
void CEmailMessageContent::SetContentClass( const TDesC& aContentClass )
    {
    if (iPart)
        iPart->SetContentClass( aContentClass );
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
TInt CEmailMessageContent::AvailableSize() const
    {
    if (iPart)
        {
        return iPart->FetchedContentSize();
        }
    else
        {
        return 0;
        }
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
TInt CEmailMessageContent::TotalSize() const
    {
    if (iPart)
        {
        return iPart->ContentSize();
        }
    else
        {
        return 0;
        }
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
TPtrC CEmailMessageContent::ContentL() const
    {
    TInt size = AvailableSize();
    TPtr16 ptr( 0, size );
    if ( size != 0 )
        {
        if ( !iBuf )
            {
            iBuf = HBufC::NewL( size );
            }
        
        if ( size > ptr.MaxLength() )
            {
            iBuf = iBuf->ReAlloc( size );
            }
        ptr.Set( iBuf->Des() );
        iPart->GetContentToBufferL( ptr, iUsed );
        iUsed += size;
        }
    return ptr;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
void CEmailMessageContent::SetContentL( const TDesC& aContent )
    {
    User::LeaveIfNull( iPart );
    iPart->SetContentSize( aContent.Length() );
    iPart->SetFetchedContentSize( aContent.Length() );
    iPlugin->SetContentL( aContent, 
            FsMsgId( iPluginData, iMsgContentId.iMessageId.iFolderId.iMailboxId ),
            FsMsgId( iPluginData, iMsgContentId.iMessageId.iFolderId ),
            FsMsgId( iPluginData, iMsgContentId.iMessageId ),
            FsMsgId( iPluginData, iMsgContentId ) );
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
void CEmailMessageContent::FetchL( MEmailFetchObserver&  aObserver )
    {
    User::LeaveIfNull( iPart );
    const TFSMailMsgId fsId = FsMsgId(iPluginData, iMsgContentId);
    if ( !iFetchObserver )
        {
        iFetchObserver = new ( ELeave ) CContentRequestObserver( *this );
        }
    iFetchObserver->SetObserverL( &aObserver );
    iRequestId = iPart->FetchMessagePartL( fsId, *iFetchObserver, TUint( 0 ) );

    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
void CEmailMessageContent::CancelFetch()
    {
    TRAP_IGNORE( iPlugin->CancelL( iRequestId ) );
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
void CEmailMessageContent::SaveToFileL( const TDesC& aPath )
    {
    iPlugin->CopyMessagePartFileL(
        FsMsgId( iPluginData, iMsgContentId.iMessageId.iFolderId.iMailboxId ),
        FsMsgId( iPluginData, iMsgContentId.iMessageId.iFolderId ),
        FsMsgId( iPluginData, iMsgContentId.iMessageId ),
        FsMsgId( iPluginData, iMsgContentId ),
        aPath );
    }

/* Dummy implementations, not ever called */
MEmailMultipart* CEmailMessageContent::AsMultipartOrNull() const 
    {
    return NULL;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
MEmailTextContent* CEmailMessageContent::AsTextContentOrNull() const
    {
    return NULL;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
MEmailAttachment* CEmailMessageContent::AsAttachmentOrNull() const
    {
    return NULL;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
CEmailMessageContent::CContentRequestObserver::CContentRequestObserver( 
    CEmailMessageContent& aParent ) : iObserver( NULL ), iParent( aParent )
    {
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
void CEmailMessageContent::CContentRequestObserver::RequestResponseL( 
    TFSProgress aEvent, TInt /* aRequestId */ )
    {
    if ( aEvent.iProgressStatus == TFSProgress::EFSStatus_RequestComplete )
        {
        delete iParent.iPart;
        iParent.iPart = NULL;
        CFSMailMessagePart *part = iParent.iPlugin->MessagePartL( 
            FsMsgId( iParent.iPluginData, iParent.iMsgContentId.iMessageId.iFolderId.iMailboxId ),
            FsMsgId( iParent.iPluginData, iParent.iMsgContentId.iMessageId.iFolderId ),
            FsMsgId( iParent.iPluginData, iParent.iMsgContentId.iMessageId ),
            FsMsgId( iParent.iPluginData, iParent.iMsgContentId ) );
        iParent.SetPart( part );

        if ( iObserver ) 
            iObserver->DataFetchedL( aEvent.iError );
        }
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
void CEmailMessageContent::CContentRequestObserver::SetObserverL( MEmailFetchObserver* aObserver )
    {
    iObserver = aObserver;
    }

// End of file
