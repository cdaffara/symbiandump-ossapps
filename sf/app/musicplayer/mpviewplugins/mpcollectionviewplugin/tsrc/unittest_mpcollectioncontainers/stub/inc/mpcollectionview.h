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
* Description: Music Player collection view stub for testing mpcollectioncontainers
*
*/

#ifndef MPCOLLECTIONVIEW_H
#define MPCOLLECTIONVIEW_H

#include <QObject>

#include <hbview.h>
#include <hbeffect.h>

#include "mpmpxcollectionviewdefs.h"
#include "mpviewbase.h"
#include <QGraphicsWidget>

class QActionGroup;
class HbMainWindow;
class HbAction;
class HbMenu;
class HbToolBar;
class MpCollectionDocumentLoader;
class MpCollectionContainerFactory;
class MpCollectionContainer;
class MpCollectionDataModel;
class MpMpxFrameworkWrapper;
class MpMpxCollectionData;
class MpNowPlayingWidget;
class MpSnapshotWidget;
class MpCollectionSongScanner;

class MpCollectionView : public QGraphicsWidget
{
    Q_OBJECT

public:

    MpCollectionView();
    virtual ~MpCollectionView();

    void initializeView();


public slots:

    void openIndex( int index );
    void openContextMenu( const QModelIndex &index, const QPointF &coords );
    void findAlbumSongs( int index );
    void playAlbumSongs( int albumIndex, int songIndex );

public:

    TCollectionContext              mCollectionContext;

    MpMpxFrameworkWrapper           *mMpxWrapper;           // Own
    MpMpxCollectionData             *mCollectionData;       // Not own

    MpCollectionContainerFactory    *mContainerFactory;     // Own
    MpCollectionContainer           *mCollectionContainer;  // Not own
    MpCollectionDataModel           *mCollectionDataModel;  // Own

    bool                            mActivated;
    MpNowPlayingWidget              *mNowPlayingBanner;     // Own
    bool                            mBannerAttached;
    bool                            mEffectOnGoing;

    HbMainWindow                    *mWindow;               // Not own
    HbAction                        *mSoftKeyQuit;          // Not own
    HbAction                        *mSoftKeyBack;          // Not own

    MpCollectionDocumentLoader      *mDocumentLoader;       // Own
    HbWidget                        *mMainContainer;        // Own

    HbToolBar                       *mMainToolBar;
    HbToolBar                       *mPlaylistToolBar;

    MpSnapshotWidget                *mSnapshot;
    MpCollectionSongScanner         *mSongScanner;          // Own
    bool                            mScanning;

};

#endif  // MPCOLLECTIONVIEW_H

