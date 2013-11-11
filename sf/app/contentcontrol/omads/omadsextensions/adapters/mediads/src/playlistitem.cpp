/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  CPlaylistItem class implementation
*
*/



#include "playlistitem.h"

#include "logger.h"

_LIT( KLineFeed, "\r\n" );
_LIT( KUrl, "#EXTURL:" );
_LIT( KTitle, "#EXTTITLE:" );

_LIT( KCommentChar, "#" );

const TInt KDefaultGranularity( 5 );

CPlaylistItem* CPlaylistItem::NewLC()
    {
    CPlaylistItem* self = new (ELeave) CPlaylistItem();
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }

CPlaylistItem::~CPlaylistItem()
    {
    TRACE_FUNC;
    delete iTitle;
    delete iUrl;
    delete iDescArray;
    }

CPlaylistItem::CPlaylistItem()
    {
    }

void CPlaylistItem::ConstructL()
    {
    TRACE_FUNC;
    iTitle = KNullDesC().AllocL();
    iUrl = KNullDesC().AllocL();
    iDescArray = new (ELeave) CDesCArrayFlat(KDefaultGranularity);
    }

void CPlaylistItem::SetId( TInt aId )
    {
    iId = aId;
    }

TInt CPlaylistItem::Id() const
    {
    return iId;
    }

void CPlaylistItem::SetTitleL( const TDesC& aTitle )
    {
    delete iTitle;
    iTitle = NULL;
    iTitle = aTitle.AllocL();
    }

const TDesC& CPlaylistItem::Title() const
    {
    return *iTitle;
    }

void CPlaylistItem::SetUrlL( const TDesC& aUrl )
    {
    delete iUrl;
    iUrl = NULL;
    iUrl = aUrl.AllocL();
    }

const TDesC& CPlaylistItem::Url() const
    {
    return *iUrl;
    }

TInt CPlaylistItem::ItemCount() const
    {
    return iDescArray->MdcaCount();
    }

TPtrC16 CPlaylistItem::ItemAt( TInt aIndex ) const
    {
    return iDescArray->MdcaPoint( aIndex );
    }

TInt CPlaylistItem::FindItem( const TDesC16& aPtr, TInt& aPos, TKeyCmpText aTextComparisonType ) const
    {
    return iDescArray->Find( aPtr, aPos, aTextComparisonType );
    }

void CPlaylistItem::ExportL( RBufWriteStream& aStream ) const
    {
    TRACE_FUNC_ENTRY;
    
    aStream.WriteL( KUrl );
    aStream.WriteL( *iUrl, iUrl->Length() );
    aStream.WriteL( KLineFeed );
    
    aStream.WriteL( KTitle );
    aStream.WriteL( *iTitle, iTitle->Length() );
    aStream.WriteL( KLineFeed );
    
    TInt count = iDescArray->MdcaCount();
    for ( TInt i=0; i<count; i++ )
        {
        TPtrC16 ptr16 = iDescArray->MdcaPoint( i );
        aStream.WriteL( ptr16, ptr16.Length() );
        aStream.WriteL( KLineFeed );
        }
    
    aStream.CommitL();
    TRACE_FUNC_EXIT;
    }

void CPlaylistItem::ImportL( const TDesC& aBuffer )
    {
    TRACE_FUNC_ENTRY;
    iDescArray->Reset();
    TPtrC tag;
    TPtrC data;
    TInt currentPos(0);
    TInt lineLen(0);
    TBool eof(EFalse);
    while( !eof )
        {
        lineLen = ReadNextLine( aBuffer.Mid(currentPos), data );
        if ( lineLen >= 0)
            {
            if ( data.FindF( KTitle ) == 0 )
                {
                TPtrC title = data.Mid( KTitle().Length() );
                SetTitleL( title );
                }
            else if ( data.FindF( KCommentChar ) == 0 )
                {
                // ignore comment
                }
            else if ( data.Length() > 0 )
                {
                // must be song url
                AddItemL( data );
                }
            }
        else
            {
            eof = ETrue;
            }
        currentPos += lineLen;
        if ( currentPos >= aBuffer.Length() )
            {
            eof = ETrue;
            }
        }
    TRACE_FUNC_EXIT;
    }

void CPlaylistItem::AddItemL( const TDesC& aSongUri )
    {
    //LOGGER_WRITE_1("AddItem: %S", &aSongUri);
    TFileName uri(aSongUri);
    uri.LowerCase();
    uri.TrimAll();
    iDescArray->InsertIsqAllowDuplicatesL( uri );
    }

TInt CPlaylistItem::ReadNextLine( const TDesC& aBuffer, TPtrC& aLine )
    {
    //TRACE_FUNC_ENTRY;
    TInt lineLen = aBuffer.FindF( KLineFeed );
    
    if ( lineLen == KErrNotFound )
        {
        lineLen = aBuffer.Length();
        if ( lineLen == 0 )
            {
            //TRACE_FUNC_RET( lineLen );
            return KErrNotFound;
            }
        }
    
    aLine.Set( aBuffer.Mid( 0, lineLen ));
    //LOGGER_WRITE_1("aLine: %S", &aLine);
    lineLen += 2;
    //TRACE_FUNC_RET( lineLen );
    return lineLen;
    }
