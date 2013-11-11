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

#include "emailattachment.h"
#include "emailcontent.h"
#include "emailclientapi.hrh"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------/
CEmailAttachment::CEmailAttachment( const TDataOwner aOwner ) : iOwner( aOwner )
    {
    }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
CEmailAttachment::~CEmailAttachment()
    {
    delete iEmailMsgContent;
    }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
CEmailAttachment* CEmailAttachment::NewLC(
        CPluginData& aPluginData,
        const TMessageContentId& aMsgContentId,
        CFSMailMessagePart *aAtt,
        const TDataOwner aOwner )
    {    
    CEmailAttachment* self = new (ELeave) CEmailAttachment( aOwner );
    CleanupStack::PushL( self );
    self->ConstructL( aPluginData, aMsgContentId, aAtt );

    return self;
    }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
CEmailAttachment* CEmailAttachment::NewL(
        CPluginData& aPluginData,
        const TMessageContentId& aMsgContentId,
        CFSMailMessagePart *aAtt,
        const TDataOwner aOwner )
    {
    CEmailAttachment* self = CEmailAttachment::NewLC( aPluginData, aMsgContentId, aAtt, aOwner );
    CleanupStack::Pop( self );

    return self;
    }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void CEmailAttachment::ConstructL(
        CPluginData& aPluginData,
        const TMessageContentId& aMsgContentId,
        CFSMailMessagePart *aAtt)
    {
    iEmailMsgContent = CEmailMessageContent::NewL( aPluginData, aMsgContentId, aAtt );
    }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
TInt CEmailAttachment::AvailableSize() const
{
    return iEmailMsgContent->AvailableSize();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void CEmailAttachment::CancelFetch()
{
    iEmailMsgContent->CancelFetch();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
TPtrC CEmailAttachment::ContentL() const
{
    User::Leave(KErrNotSupported);
    return iEmailMsgContent->ContentL();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
TPtrC CEmailAttachment::ContentClass() const
{
    return iEmailMsgContent->ContentClass();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
TPtrC CEmailAttachment::ContentDescription() const
{
    return iEmailMsgContent->ContentDescription();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
TPtrC CEmailAttachment::ContentDisposition() const
{
    return iEmailMsgContent->ContentDisposition();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
TPtrC CEmailAttachment::ContentId() const
{
    return iEmailMsgContent->ContentId();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
TPtrC CEmailAttachment::ContentType() const
{
    return iEmailMsgContent->ContentType();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void CEmailAttachment::FetchL( MEmailFetchObserver & aObserver )
{
    iEmailMsgContent->FetchL( aObserver );
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
TMessageContentId CEmailAttachment::Id() const
{
    return iEmailMsgContent->Id();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
TEmailTypeId CEmailAttachment::InterfaceId() const
    {
    return KEmailIFUidAttachment;
    }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void CEmailAttachment::Release()
    {
    if ( iOwner == EClientOwns )
        {    
        delete this;
        }
    }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void CEmailAttachment::SaveToFileL( const TDesC& aPath )
{
    iEmailMsgContent->SaveToFileL( aPath );
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void CEmailAttachment::SetContentClass( const TDesC& aContentClass )
{
    iEmailMsgContent->SetContentClass( aContentClass );
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void CEmailAttachment::SetContentDescription( const TDesC& aContentDescription )
{
    iEmailMsgContent->SetContentDescription(aContentDescription);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void CEmailAttachment::SetContentDisposition( const TDesC& aContentDisposition )
{
    iEmailMsgContent->SetContentDisposition( aContentDisposition );
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void CEmailAttachment::SetContentId( const TDesC &aContentId )
{
    iEmailMsgContent->SetContentId( aContentId );
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void CEmailAttachment::SetContentL( const TDesC &aContent )
{
    iEmailMsgContent->SetContentL( aContent );
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void CEmailAttachment::SetContentType( const TDesC &aContentType )
{
    iEmailMsgContent->SetContentType( aContentType );
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
TInt CEmailAttachment::TotalSize() const
{
    return iEmailMsgContent->TotalSize();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
MEmailMultipart* CEmailAttachment::AsMultipartOrNull() const
    {
    return NULL;
    }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
MEmailTextContent* CEmailAttachment::AsTextContentOrNull() const
    {
    return NULL;
    }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
MEmailAttachment* CEmailAttachment::AsAttachmentOrNull() const
    {
    const MEmailAttachment* ptr = this;
    return const_cast<MEmailAttachment *>( ptr );
    }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
RFile CEmailAttachment::FileL() const
    {
    return iEmailMsgContent->Part().GetContentFileL();
    }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------/
void CEmailAttachment::SetFileNameL( const TDesC& aFileName )
    {
    iEmailMsgContent->Part().SetAttachmentNameL( aFileName );
    }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
TPtrC CEmailAttachment::FileNameL() const
    {
    return iEmailMsgContent->Part().AttachmentNameL();
    }

// End of file
