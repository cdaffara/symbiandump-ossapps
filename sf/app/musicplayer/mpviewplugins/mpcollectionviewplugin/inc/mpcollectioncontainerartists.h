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
* Description: Music Player collection container definition - Artists.
*
*/

#ifndef MPCOLLECTIONCONTAINERARTISTS_H
#define MPCOLLECTIONCONTAINERARTISTS_H

#include "mpcollectionlistcontainer.h"

class HgMediawall;
class MpCollectionTBoneListDataModel;

class MpCollectionContainerArtists : public MpCollectionListContainer
{
    Q_OBJECT

public:

    explicit MpCollectionContainerArtists( HbDocumentLoader *loader, QGraphicsItem *parent=0 );
    virtual ~MpCollectionContainerArtists();

    void setDataModel( MpCollectionDataModel *dataModel );

public slots:

    void itemActivated( const QModelIndex &index );
    void albumCentered( const QModelIndex &modelIndex);
    void dataReloaded();
    void albumDataChanged();
    void scrollingStarted();
    void albumDataAvailable();

private:

    void setupContainer();

private:
    
    HgMediawall                     *mTBone;            // Own
    MpCollectionTBoneListDataModel  *mTBoneListModel;   // Own
    int                             mCurrentArtistIndex;
    int                             mCurrentAlbumIndex;
    int                             mAlbumIndexOffset;

};

#endif  // MPCOLLECTIONCONTAINERARTISTS_H

