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
* Description: Music Player Media Wall view.
*
*/

#ifndef MPMEDIAWALLVIEW_H
#define MPMEDIAWALLVIEW_H

#include "mpviewbase.h"
#include "mpmpxcollectionviewdefs.h"

class HbMainWindow;
class MpEngine;
class HgMediawall;
class MpMpxCollectionData;
class MpCollectionDataModel;
class HbLabel;
class MpMediaWallDocumentLoader;
class HbAction;
class MpPlaybackData;
class QParallelAnimationGroup;
class QPropertyAnimation;
class MpAlbumCoverWidget;
class MpTrackListWidget;
class QModelIndex;


class MpMediaWallView : public MpViewBase
{
    Q_OBJECT

public:
    explicit MpMediaWallView();
    virtual ~MpMediaWallView();
    
    void initializeView();
    void activateView();
    void deactivateView();

signals:
    void command( int aCommand );
    
public slots:
    void contextOpened( TCollectionContext context );
    void containerContentsChanged();
    void libraryAboutToUpdate();
    void libraryUpdated();
    void hideTracksList();
    void showTrackList();
    void dismissListClosingAnimation();
    void listItemActivated( const QModelIndex &index );
    void fetchAlbumSongs(QModelIndex index);

private:
    void setUpMediaWallWidget();
    void scrollToDefault();
    void scrollToNowPlaying();
    void setUpListAnimation();

    
private:
    MpEngine                     *mEngine;                 // not own
    MpMpxCollectionData          *mCollectionData;         // not own
    MpCollectionDataModel        *mModel;                  // own - parented
    HgMediawall                  *mMediaWallWidget;        // own - parented
    HbLabel                      *mNoMusic;                // own - parented
    MpMediaWallDocumentLoader    *mDocumentLoader;         // own
    bool                         mLibraryUpdating;
    MpPlaybackData               *mPlaybackData;           // not own
    QParallelAnimationGroup      *mListShowAnimationGroup; // own - parented
    QPropertyAnimation           *mListShowListAnimation;  // own - parented
    QPropertyAnimation           *mCoverShowListAnimation; // own - parented
    bool                         mShowingSongsList;
    MpAlbumCoverWidget           *mAlbumCover; 
    MpTrackListWidget            *mTrackList;
    
    Q_DISABLE_COPY(MpMediaWallView)
};


#endif //MPMEDIAWALLVIEW_H
