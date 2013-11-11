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
* Description: MpMpxCollectionData stub for testing mpmpxframeworkwrapper
*
*/

#ifndef MPMPXCOLLECTIONDATA_H
#define MPMPXCOLLECTIONDATA_H

#include "mpmpxcollectionviewdefs.h"

class CMPXMedia;
class CMPXMediaArray;
 

class MpMpxCollectionData 
{

public:

    enum DataType {
        Title,
        Uri,
        Duration,
        Count,
        Artist,
        Album,
        Genre,
        Rating,
        AlbumArtUri
    };

    // Stub functions
     MpMpxCollectionData();
    ~MpMpxCollectionData();

    TCollectionContext context();
    int count() const;
    QString collectionTitle() const;
    QString itemData( int index, MpMpxCollectionData::DataType type ) const;

    int itemId(int index);
    int currentAlbumIndex() const;

    void setMpxMedia( const CMPXMedia& entries, bool reopen=false );
    void incrementalOpenUpdate();
    const CMPXMedia& containerMedia();

    void setAlbumContent( const CMPXMedia& albumContent );

public:
    CMPXMedia               *mContainerMedia; //Not owned
    CMPXMediaArray          *mMediaArray; //Not owned
    TBool                   mMediaSet;
    TCollectionContext      mContext;
    int                     mCurrentAlbumIndex;
    int                     mAlbumSongCount;

};

#endif // MPMPXCOLLECTIONDATA_H

