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
* Description: MpCollectionAlbumArtManager stub for testing MpCollectionDataModel
*
*/


#ifndef MPCOLLECTIONALBUMARTMANAGER_H
#define MPCOLLECTIONALBUMARTMANAGER_H

#include <QObject>
#include <QIcon>

#include "mpcommondefs.h"

class MpMpxCollectionData;

class MpCollectionAlbumArtManager : public QObject
{
    Q_OBJECT

public:

    // Test utility functions
    static int getInitCounter();
    static void resetInitCounter();

    // Stub functions
    explicit MpCollectionAlbumArtManager( MpMpxCollectionData *data, QObject *parent=0 );
    virtual ~MpCollectionAlbumArtManager();

    const QIcon* albumArt( int index );
    void cacheFirstScreen();
    void cancel();
    void setThumbnailSize( MpCommon::MpThumbType type );
    void enableDefaultArt( bool enabled );

signals:

    void albumArtReady( int index );

public:

    QIcon                   *mIcon;
    int                     mAlbumArtCount;
    int                     mCacheAlbumArtCount;
    MpCommon::MpThumbType   mThumbType;

};

#endif // MPCOLLECTIONALBUMARTMANAGER_H

