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
* Description:  Part of SyncML Data Synchronization Plug In Adapter
*
*/



#ifndef TSONGITEM_H_
#define TSONGITEM_H_

#include <e32base.h>
#include <s32strm.H>


class CSongItem : public CBase
    {
public:
    
    static CSongItem* NewLC();
    ~CSongItem();
    
    void SetId( TInt aId );
    TInt Id() const;
    
    void SetUriL( const TDesC& aUri );
    const TDesC& Uri() const;
    
    void SetTitleL( const TDesC& aTitle );
    const TDesC& Title() const;
    
    void SetMimeTypeL( const TDesC& aMimeType );
    const TDesC& MimeType() const;
    
    void SetDuration( TInt aDuration );
    TInt Duration() const;
    
    void SetArtistL( const TDesC& aArtist );
    const TDesC& Artist() const;
    
    void SetAlbumL( const TDesC& aAlbum );
    const TDesC& Album() const;
    
    void SetYear( TInt aYear );
    TInt Year() const;
    
    void SetAlbumTrack( TInt aId );
    TInt AlbumTrack() const;
    
    void SetGenreL( const TDesC& aGenre );
    const TDesC& Genre() const;
    
    void SetCommentL( const TDesC& aComment );
    const TDesC& Comment() const;
    
    void SetComposerL( const TDesC& aComposer );
    const TDesC& Composer() const;
    
    void SetRating( TInt aRating );
    TInt Rating() const;
    
    void SetModifiedTime( TTime& aTime );
    const TTime& ModifiedTime() const;
    
    void SetFileSize( TInt32 aSize );
    TInt32 FileSize() const;
    
    /**
     * Export item.
     * @param aStream stream where to export.
     */
    void ExportL( RWriteStream& aStream );
    
private:
    CSongItem();
    void ConstructL();
    
    inline void WriteUnicodeL( RWriteStream& aStream, const TDesC& aData, const TUint8 aFieldId );
    
    inline void WriteDataFieldL( RWriteStream& aStream, const TDesC8& aData, const TUint8 aFieldId );
    
    inline void WriteUInt16FieldL( RWriteStream& aStream, TUint16 aData, const TUint8 aFieldId );
    
    inline void WriteUInt32FieldL( RWriteStream& aStream, TUint32 aData, const TUint8 aFieldId );
    
private:
    TInt      iId;
    HBufC*    iUri;
    HBufC*    iTitle;
    HBufC*    iMimeType;
    TInt      iDuration;
    HBufC*    iArtist;
    HBufC*    iAlbum;
    TInt      iYear;
    TInt      iAlbumTrack;
    HBufC*    iGenre;
    HBufC*    iComment;
    HBufC*    iComposer;
    TInt      iRating;
    TTime     iModifiedTime;
    TInt      iSize;
    };

#endif /* TSONGITEM_H_ */
