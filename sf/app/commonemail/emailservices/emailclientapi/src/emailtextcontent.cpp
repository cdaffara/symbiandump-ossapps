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
#include "emailmessage.h"
#include "emailclientapi.hrh"

// CEmailTextContent

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
CEmailTextContent* CEmailTextContent::NewL( 
        CPluginData& aPluginData,
        const TMessageContentId& aMsgContentId,
        CFSMailMessagePart* aPart,
        const TDataOwner aOwner ) 
    {
    CEmailTextContent* self = new ( ELeave ) CEmailTextContent( aOwner );
    CleanupStack::PushL( self );
    self->ConstructL( aPluginData, aMsgContentId, aPart );
    CleanupStack::Pop( self );
    return self;
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
void CEmailTextContent::ConstructL(
        CPluginData& aPluginData,
        const TMessageContentId& aMsgContentId,
        CFSMailMessagePart* aPart )
    {
    iEmailMsgContent = CEmailMessageContent::NewL( aPluginData, aMsgContentId, aPart );
    TContentType contentType( aPart->GetContentType() );
    if ( contentType.Equals( KFSMailContentTypeTextHtml ) )
        {
        iTextType = EHtmlText;
        }
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
CEmailTextContent::~CEmailTextContent()
    {
    delete iEmailMsgContent;
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
CEmailTextContent::CEmailTextContent( TDataOwner aOwner ) : 
    iTextType( EPlainText ), 
    iOwner( aOwner )
    {
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
TEmailTypeId CEmailTextContent::InterfaceId() const
    {
    return KEmailIFUidTextContent;
    }
    
// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
void CEmailTextContent::Release()
    {
    if ( iOwner == EClientOwns )
        {
        delete this;
        }
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
MEmailTextContent::TTextType CEmailTextContent::TextType() const
    {
    return iTextType;
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
void CEmailTextContent::SetTextL(
            const TTextType aPlainOrHtml,
            const TDesC& aText )
    {    
    iTextType = aPlainOrHtml;

    if( aPlainOrHtml == EPlainText )
        {
        SetContentType( KContentTypeTextPlain );
        }
    else if( aPlainOrHtml == EHtmlText )
        {
        SetContentType( KContentTypeTextHtml );
        }
    SetContentL( aText );
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
TMessageContentId CEmailTextContent::Id() const
    {
    return iEmailMsgContent->Id(); 
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
TPtrC CEmailTextContent::ContentType() const
    {
    return iEmailMsgContent->ContentType();
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
void CEmailTextContent::SetContentType( const TDesC& aContentType )
    {
    iEmailMsgContent->SetContentType( aContentType );
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
TPtrC CEmailTextContent::ContentId() const
    {
    return iEmailMsgContent->ContentId();
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
void CEmailTextContent::SetContentId( const TDesC& aContentId )
    {
    iEmailMsgContent->SetContentId( aContentId );
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
TPtrC CEmailTextContent::ContentDescription() const
    {
    return iEmailMsgContent->ContentDescription();
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
void CEmailTextContent::SetContentDescription( const TDesC& aContentDescription )
    {
    iEmailMsgContent->SetContentDescription( aContentDescription );
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
TPtrC CEmailTextContent::ContentDisposition() const
    {
    return iEmailMsgContent->ContentDisposition();
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
void CEmailTextContent::SetContentDisposition( const TDesC& aContentDisposition )
    {
    iEmailMsgContent->SetContentDisposition( aContentDisposition );
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
TPtrC CEmailTextContent::ContentClass() const
    {
    return iEmailMsgContent->ContentClass();
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
void CEmailTextContent::SetContentClass( const TDesC& aContentClass )
    {
    iEmailMsgContent->SetContentClass( aContentClass );
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
TInt CEmailTextContent::AvailableSize() const
    {
    return iEmailMsgContent->AvailableSize();
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
TInt CEmailTextContent::TotalSize() const
    {
    return iEmailMsgContent->TotalSize();
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
TPtrC CEmailTextContent::ContentL() const
    {
    return iEmailMsgContent->ContentL();
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
void CEmailTextContent::SetContentL( const TDesC& aContent )
    {
    iEmailMsgContent->SetContentL( aContent );
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
void CEmailTextContent::FetchL( MEmailFetchObserver& aObserver )
    {
    iEmailMsgContent->FetchL( aObserver );
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
void CEmailTextContent::CancelFetch()
    {
    iEmailMsgContent->CancelFetch();
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
void CEmailTextContent::SaveToFileL( const TDesC& aPath )
    {
    iEmailMsgContent->SaveToFileL( aPath );
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
MEmailMultipart* CEmailTextContent::AsMultipartOrNull() const
    {    
    return NULL;
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
MEmailTextContent* CEmailTextContent::AsTextContentOrNull() const
    {
    const MEmailTextContent* ptr = this;
    return const_cast<MEmailTextContent *>(ptr);
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
MEmailAttachment* CEmailTextContent::AsAttachmentOrNull() const
    {
    return NULL;
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
void CEmailTextContent::SetOwner( const TDataOwner aOwner )
    {
    iOwner = aOwner;
    }

// End of file
