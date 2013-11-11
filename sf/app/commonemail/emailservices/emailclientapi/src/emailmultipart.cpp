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
* Description: This file implements class CEmailAttachment.
*
*/

#include "emailcontent.h"
#include "emailtextcontent.h"
#include "emailattachment.h"
#include "emailmultipart.h"
#include "emailmessage.h"
#include "emailclientapi.hrh"

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
CEmailMultipart* CEmailMultipart::NewL( 
        CPluginData& aPluginData,
        const TMessageContentId& aMsgContentId,
        CFSMailMessagePart* aPart,
        const TDataOwner aOwner ) 
    {
    CEmailMultipart* self = new ( ELeave ) CEmailMultipart( aOwner );
    CleanupStack::PushL( self );
    self->ConstructL( aPluginData, aMsgContentId, aPart );
    CleanupStack::Pop( self );
    return self;
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
void CEmailMultipart::ConstructL(
        CPluginData& aPluginData,
        const TMessageContentId& aMsgContentId,
        CFSMailMessagePart* aPart )
    {
    iEmailMsgContent = CEmailMessageContent::NewL( aPluginData, aMsgContentId, aPart );
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
CEmailMultipart::~CEmailMultipart()
    {
    delete iEmailMsgContent;
    iChildParts.Reset();
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
CEmailMultipart::CEmailMultipart( const TDataOwner aOwner ) : iOwner( aOwner )
    {
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
TInt CEmailMultipart::PartCountL()
    {
    iChildParts.Reset();
    RPointerArray<CFSMailMessagePart> childParts;
    CleanupResetAndDestroyPushL( childParts );
    iEmailMsgContent->Part().ChildPartsL( childParts );
    iChildPartCount = childParts.Count();

    for ( TInt i = 0; i < iChildPartCount; i++ )
        {
        TFSMailMsgId id = childParts[i]->GetPartId();
        iChildParts.AppendL( id );
        }
    CleanupStack::PopAndDestroy( &childParts );
    return iChildPartCount;
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
MEmailMessageContent* CEmailMultipart::PartByIndexL( const TUint aIndex ) const
    {
    if ( aIndex >= iChildPartCount )
        {
        User::Leave( KErrArgument );
        }
    MEmailMessageContent* content = NULL;
    TFSMailMsgId partId = iChildParts[aIndex];
    CFSMailMessagePart* copy = iEmailMsgContent->Part().ChildPartL( partId );
    if ( !copy )
        return content;

    TContentType contentType( copy->GetContentType() );
    TMessageContentId msgContentId = TMessageContentId( 
            copy->GetPartId().Id(),
            Id().iMessageId.iId,
            Id().iMessageId.iFolderId.iId,
            Id().iMessageId.iFolderId.iMailboxId );

    if ( contentType.Equals( KFSMailContentTypeTextPlain ) || 
         contentType.Equals( KFSMailContentTypeTextHtml ) )
        {
        content = CEmailTextContent::NewL( 
            iEmailMsgContent->PluginData(), msgContentId, copy, EClientOwns );
        }
    else if ( contentType.Equals( KFSMailContentTypeMultipartMixed ) ||
              contentType.Equals( KFSMailContentTypeMultipartAlternative ) ||
              contentType.Equals( KFSMailContentTypeMultipartDigest ) ||
              contentType.Equals( KFSMailContentTypeMultipartRelated ) ||
              contentType.Equals( KFSMailContentTypeMultipartParallel ) )
        {
        content = CEmailMultipart::NewL( 
            iEmailMsgContent->PluginData(), msgContentId, copy, EClientOwns );
        }
    else 
        {
        content = CEmailAttachment::NewL( 
            iEmailMsgContent->PluginData(), msgContentId, copy, EClientOwns );
        } 
    return content;
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
void CEmailMultipart::DeletePartL( const TUint aIndex )
    {
    if ( aIndex >= iChildPartCount )
        {
        User::Leave( KErrArgument );
        }
    
    TFSMailMsgId partId = iChildParts[aIndex];
    iEmailMsgContent->Part().RemoveChildPartL( partId );
    iChildParts.Remove( aIndex );
    iChildPartCount--;
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
void CEmailMultipart::AddPartL(
        const MEmailMessageContent& aPart,
        const TUint aPos )
    {
    if ( aPos > iChildPartCount )
        {
        User::Leave( KErrArgument );
        }

    TFSMailMsgId insertBefore = TFSMailMsgId();
    if ( aPos <  iChildPartCount )
        {
        insertBefore = iChildParts[aPos];
        }
    const TDesC& contentType = aPart.ContentType();

    CFSMailMessagePart* newPart = 
        iEmailMsgContent->Part().NewChildPartL( insertBefore, contentType );
    CleanupStack::PushL( newPart );
    TFSMailMsgId newPartId = newPart->GetPartId(); 
    iChildParts.InsertL( newPartId, aPos );
    iChildPartCount = iChildParts.Count();
    CleanupStack::Pop( newPart );
    


    return;
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
TEmailTypeId CEmailMultipart::InterfaceId() const
    {
    return KEmailIFUidMultipart;
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
void CEmailMultipart::Release()
    {
    if ( iOwner == EClientOwns )
        {
        delete this;
        }
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
TMessageContentId CEmailMultipart::Id() const
    {
    return iEmailMsgContent->Id(); 
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
TPtrC CEmailMultipart::ContentType() const
    {
    return iEmailMsgContent->ContentType();
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
void CEmailMultipart::SetContentType( const TDesC& aContentType )
    {
    iEmailMsgContent->SetContentType( aContentType );
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
TPtrC CEmailMultipart::ContentId() const
    {
    return iEmailMsgContent->ContentId();
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
void CEmailMultipart::SetContentId( const TDesC& aContentId )
    {
    iEmailMsgContent->SetContentId( aContentId );
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
TPtrC CEmailMultipart::ContentDescription() const
    {
    return iEmailMsgContent->ContentDescription();
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
void CEmailMultipart::SetContentDescription( const TDesC& aContentDescription )
    {
    iEmailMsgContent->SetContentDescription( aContentDescription );
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
TPtrC CEmailMultipart::ContentDisposition() const
    {
    return iEmailMsgContent->ContentDisposition();
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
void CEmailMultipart::SetContentDisposition( const TDesC& aContentDisposition )
    {
    iEmailMsgContent->SetContentDisposition( aContentDisposition );
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
TPtrC CEmailMultipart::ContentClass() const
    {
    return iEmailMsgContent->ContentClass();
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
void CEmailMultipart::SetContentClass( const TDesC& aContentClass )
    {
    iEmailMsgContent->SetContentClass( aContentClass );
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
TInt CEmailMultipart::AvailableSize() const
    {
    return iEmailMsgContent->AvailableSize();
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
TInt CEmailMultipart::TotalSize() const
    {
    return iEmailMsgContent->TotalSize();
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
TPtrC CEmailMultipart::ContentL() const
    {
    User::Leave(KErrNotSupported);
    return iEmailMsgContent->ContentL();
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
void CEmailMultipart::SetContentL( const TDesC& aContent )
    {
    iEmailMsgContent->SetContentL( aContent );
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
void CEmailMultipart::FetchL( MEmailFetchObserver& aObserver )
    {
    iEmailMsgContent->FetchL( aObserver );
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
void CEmailMultipart::CancelFetch()
    {
    iEmailMsgContent->CancelFetch();
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
void CEmailMultipart::SaveToFileL( const TDesC& aPath )
    {
    iEmailMsgContent->SaveToFileL( aPath );
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
MEmailMultipart* CEmailMultipart::AsMultipartOrNull() const
    {
    const MEmailMultipart* ptr = this;
    return const_cast<MEmailMultipart*>( ptr );
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
MEmailTextContent* CEmailMultipart::AsTextContentOrNull() const
    {
    return NULL;
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
MEmailAttachment* CEmailMultipart::AsAttachmentOrNull() const
    {
    return NULL;
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
void CEmailMultipart::SetOwner( const TDataOwner aOwner )
    {
    iOwner = aOwner;
    }

// End of file
