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
* Description:  CSongItem class implementation
*
*/



#include "songitem.h"

#include <metadatautility.h>
#include <metadatafieldcontainer.h>
#include <metadatafield.hrh>

#include "logger.h"
#include "sconmetadatafielddefs.h"


CSongItem::CSongItem()
    {
    }

CSongItem* CSongItem::NewLC()
    {
    CSongItem* self = new (ELeave) CSongItem();
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }

void CSongItem::ConstructL()
    {
    iUri = KNullDesC().AllocL();
    iTitle = KNullDesC().AllocL();
    iMimeType = KNullDesC().AllocL();
    iArtist = KNullDesC().AllocL();
    iAlbum = KNullDesC().AllocL();
    iGenre = KNullDesC().AllocL();
    iComment = KNullDesC().AllocL();
    iComposer = KNullDesC().AllocL();
    
    iDuration = -1;
    iYear = -1;
    iAlbumTrack = -1;
    iRating = -1;
    iSize = 0;
    }

CSongItem::~CSongItem()
    {
    delete iUri;
    delete iTitle;
    delete iMimeType;
    delete iArtist;
    delete iAlbum;
    delete iGenre;
    delete iComment;
    delete iComposer;
    }

void CSongItem::SetId( TInt aId )
    {
    iId = aId;
    }

TInt CSongItem::Id() const
    {
    return iId;
    }

void CSongItem::SetUriL( const TDesC& aUri )
    {
    delete iUri;
    iUri = NULL;
    iUri = aUri.AllocL();
    }

const TDesC& CSongItem::Uri() const
    {
    return *iUri;
    }

void CSongItem::SetTitleL( const TDesC& aTitle )
    {
    delete iTitle;
    iTitle = NULL;
    iTitle = aTitle.AllocL();
    }

const TDesC& CSongItem::Title() const
    {
    return *iTitle;
    }

void CSongItem::SetMimeTypeL( const TDesC& aMimeType )
    {
    delete iMimeType;
    iMimeType = NULL;
    iMimeType = aMimeType.AllocL();
    }

const TDesC& CSongItem::MimeType() const
    {
    return *iMimeType;
    }

void CSongItem::SetDuration( TInt aDuration )
    {
    iDuration = aDuration;
    }

TInt CSongItem::Duration() const
    {
    return iDuration;
    }

void CSongItem::SetArtistL( const TDesC& aArtist )
    {
    delete iArtist;
    iArtist = NULL;
    iArtist = aArtist.AllocL();
    }

const TDesC& CSongItem::Artist() const
    {
    return *iArtist;
    }

void CSongItem::SetAlbumL( const TDesC& aAlbum )
    {
    delete iAlbum;
    iAlbum = NULL;
    iAlbum = aAlbum.AllocL();
    }

const TDesC& CSongItem::Album() const
    {
    return *iAlbum;
    }

void CSongItem::SetYear( TInt aYear )
    {
    iYear = aYear;
    }

TInt CSongItem::Year() const
    {
    return iYear;
    }

void CSongItem::SetAlbumTrack( TInt aAlbumTrack )
    {
    iAlbumTrack = aAlbumTrack;
    }

TInt CSongItem::AlbumTrack() const
    {
    return iAlbumTrack;
    }

void CSongItem::SetGenreL( const TDesC& aGenre )
    {
    delete iGenre;
    iGenre = NULL;
    iGenre = aGenre.AllocL();
    }

const TDesC& CSongItem::Genre() const
    {
    return *iGenre;
    }

void CSongItem::SetCommentL( const TDesC& aComment )
    {
    delete iComment;
    iComment = NULL;
    iComment = aComment.AllocL();
    }

const TDesC& CSongItem::Comment() const
    {
    return *iComment;
    }
    
void CSongItem::SetComposerL( const TDesC& aComposer )
    {
    delete iComposer;
    iComposer = NULL;
    iComposer = aComposer.AllocL();
    }

const TDesC& CSongItem::Composer() const
    {
    return *iComposer;
    }

void CSongItem::SetRating( TInt aRating )
    {
    iRating = aRating;
    }

TInt CSongItem::Rating() const
    {
    return iRating;
    }

void CSongItem::SetModifiedTime( TTime& aTime )
    {
    iModifiedTime = aTime;
    }

const TTime& CSongItem::ModifiedTime() const
    {
    return iModifiedTime;
    }

void CSongItem::SetFileSize( TInt32 aSize )
    {
    iSize = aSize;
    }

TInt32 CSongItem::FileSize() const
    {
    return iSize;
    }

// -----------------------------------------------------------------------------
// CSongItem::ExportL
// Exports item as specified in "Metadata format description v3.doc" document.
// -----------------------------------------------------------------------------
void CSongItem::ExportL( RWriteStream& aStream )
    {
    TRACE_FUNC_ENTRY;
    
    // write object header

    // header id
    aStream.WriteUint8L( KSconMetadataHeaderAudio );
    
    // header version
    aStream.WriteUint8L( KSconMetadataHeaderVersion );
    
    WriteUnicodeL( aStream, iTitle->Des(), KSconAudioTitle );
    WriteUnicodeL( aStream, iUri->Des(), KSconAudioFilename );
    
    if ( iDuration > -1 )
        {
        WriteUInt32FieldL( aStream, iDuration, KSconAudioDuration );
        }
    
    WriteUnicodeL( aStream, iArtist->Des(), KSconAudioArtist );
    if ( iYear > -1 )
        {
        WriteUInt16FieldL( aStream, iYear, KSconAudioYear);
        }
    if ( iAlbumTrack > -1 )
        {
        WriteUInt16FieldL( aStream, iAlbumTrack, KSconAudioAlbumTrack );
        }
    WriteUnicodeL( aStream, iAlbum->Des(), KSconAudioAlbum );
    WriteUnicodeL( aStream, iGenre->Des(), KSconAudioGenre );
    WriteUnicodeL( aStream, iComposer->Des(), KSconAudioComposer );
    WriteUnicodeL( aStream, iComment->Des(), KSconAudioComment );

    if ( iRating > -1 )
        {
        WriteUInt16FieldL( aStream, iRating, KSconAudioRating );
        }
    WriteUnicodeL( aStream, iMimeType->Des(), KSconAudioMimeType );
    
    aStream.WriteUint8L( KSconAudioDate );
    const TUint KDateTimeSize = 7; // Int16 + 5*Int8 = 2 + 5*1 = 7 bytes
    aStream.WriteUint32L( KDateTimeSize ); //size
    
    TDateTime time = iModifiedTime.DateTime();
    aStream.WriteInt16L( time.Year() );
    aStream.WriteInt8L( time.Month()+1 ); // range 1...12
    aStream.WriteInt8L( time.Day()+1 );
    aStream.WriteInt8L( time.Hour() );
    aStream.WriteInt8L( time.Minute() );
    aStream.WriteInt8L( time.Second() );
    
    // filesize
    WriteUInt32FieldL( aStream, iSize, KSconAudioFileSize );
    
    // read album art
    CMetaDataUtility* metaDataUtil = CMetaDataUtility::NewL();
    CleanupStack::PushL( metaDataUtil );
    
    RArray<TMetaDataFieldId> wantedFields;
    CleanupClosePushL(wantedFields);
    wantedFields.AppendL(EMetaDataJpeg);
    
    metaDataUtil->OpenFileL(iUri->Des(), wantedFields);
    
    const CMetaDataFieldContainer& metaCont = 
                                    metaDataUtil->MetaDataFieldsL();
    TPtrC8 data = metaCont.Field8( EMetaDataJpeg );
    if ( data.Length() > 0 )
        {
        WriteDataFieldL( aStream, data, KSconAudioJpeg );
        }
    
    CleanupStack::PopAndDestroy(&wantedFields);
    CleanupStack::PopAndDestroy(metaDataUtil);
    
    aStream.CommitL();
    TRACE_FUNC_EXIT;
    }


void CSongItem::WriteUnicodeL( RWriteStream& aStream, const TDesC& aData, const TUint8 aFieldId )
    {
    aStream.WriteUint8L( aFieldId );
    aStream.WriteUint32L( aData.Size() );
    aStream.WriteL( aData, aData.Length() );
    }

void CSongItem::WriteDataFieldL( RWriteStream& aStream, const TDesC8& aData, const TUint8 aFieldId )
    {
    aStream.WriteUint8L( aFieldId );
    aStream.WriteUint32L( aData.Size() );
    aStream.WriteL( aData, aData.Length() );
    }

void CSongItem::WriteUInt16FieldL( RWriteStream& aStream, TUint16 aData, const TUint8 aFieldId )
    {
    aStream.WriteUint8L( aFieldId );
    const TUint KSizeOfUint16 = 2; // =sizeof(TUint16), hardcoded for performance reasons.
    aStream.WriteUint32L( KSizeOfUint16 );
    aStream.WriteUint16L( aData );
    }

void CSongItem::WriteUInt32FieldL( RWriteStream& aStream, TUint32 aData, const TUint8 aFieldId )
    {
    aStream.WriteUint8L( aFieldId );
    const TUint KSizeOfUint32 = 4; // =sizeof(TUint32), hardcoded for performance reasons.
    aStream.WriteUint32L( KSizeOfUint32 );
    aStream.WriteUint32L( aData );
    }
