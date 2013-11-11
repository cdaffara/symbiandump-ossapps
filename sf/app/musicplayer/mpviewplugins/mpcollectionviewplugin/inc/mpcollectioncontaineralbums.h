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

#ifndef MPCOLLECTIONCONTAINERALBUMS_H
#define MPCOLLECTIONCONTAINERALBUMS_H

#include "mpcollectionlistcontainer.h"

class HgMediawall;
class MpCollectionTBoneListDataModel;

class MpCollectionContainerAlbums : public MpCollectionListContainer
{
    Q_OBJECT

public:

    explicit MpCollectionContainerAlbums( HbDocumentLoader *loader, QGraphicsItem *parent=0 );
    virtual ~MpCollectionContainerAlbums();

    void setDataModel( MpCollectionDataModel *dataModel );

public slots:

    void itemActivated( const QModelIndex &index );
    void albumCentered( const QModelIndex &index);
    void dataReloaded();
    void albumDataChanged();
    void scrollingStarted();
    void albumDataAvailable();

private:

    void setupContainer();

private:
    
	HgMediawall                     *mTBone;            // Own
    MpCollectionTBoneListDataModel  *mTBoneListModel;   // Own
    int                             mCurrentAlbumIndex;

};

#endif  // MPCOLLECTIONCONTAINERALBUMS_H

