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
* Description: Music Player collection view.
*
*/

#ifndef MPCOLLECTIONVIEW_H
#define MPCOLLECTIONVIEW_H

#include <QObject>

#include <hbeffect.h>

#include "mpviewbase.h"
#include "mpmpxcollectionviewdefs.h"

class QActionGroup;
class HbMainWindow;
class HbAction;
class HbMenu;
class HbToolBar;
class MpCollectionDocumentLoader;
class MpCollectionContainerFactory;
class MpCollectionContainer;
class MpCollectionDataModel;
class MpEngine;
class MpMpxCollectionData;
class MpNowPlayingWidget;
class MpSnapshotWidget;
class MpCollectionPopupHandler;

class MpCollectionView : public MpViewBase
{
    Q_OBJECT

public:

    MpCollectionView();
    virtual ~MpCollectionView();

    void initializeView();
    void activateView();
    void deactivateView();
    bool isActivated();    
    void openItem( const QModelIndex &index );
    void showItemDetails( int index );

signals:

    void command( int command );

public slots:

    void setContext( TCollectionContext context );

    void openSongs();
    void openArtists();
    void openAlbums();
    void openPlaylists();
    void find();
    void openMusicStore();

    void openIndex( int index );
    void findAlbumSongs( int index );
    void playAlbumSongs( int albumIndex, int songIndex );

    void back();
    void exit();

    void startPlaybackView();
    void attachNowPlayingBanner( bool active );
    void containerTransitionComplete( const HbEffect::EffectStatus &status );

    void shufflePlayAll();
    void addToPlaylist();
    void createNewPlaylist();
    void deleteSongs();
    void renameCurrentPlaylistContainer();

    void playlistSaved( bool success );
    void songsDeleted( bool success );
    void playlistsRenamed( bool success );

    void prepareToAddToPlaylist();
    void handleIsolatedCollectionOpened( MpMpxCollectionData* collectionData );
    void arrangeSongs();
    void openContextMenu( const QModelIndex &index, const QPointF &coords );

    void handleUsbBlocked( bool blocked );
    void handleLibraryAboutToUpdate();
    void handleLibraryUpdated();

    void setShuffleAction( bool enabled );
    
    void containerDataChanged();

private:

    void setMainToolBar();
    void setPlaylistToolBar();

    HbAction *createToolBarAction( QActionGroup *actionsGroup,
        const QString& icon, const QString& objectName );

    void updateMenu();
    void addDefaultMenuOptions( HbMenu *menu, bool optRefresh, bool optExit );
    void updateToolBar();
    void setSoftkey();

    void setBannerVisibility( bool visible );
    int generateShuffleIndex();
    void startContainerTransition( TCollectionContext contextFrom, TCollectionContext contextTo );
    void closeActiveDialog( bool onlyContextMenu = false );
    void showCorruptedNote();

private:

    TCollectionContext              mCollectionContext;

    MpEngine                        *mMpEngine;             // Not own
    MpMpxCollectionData             *mCollectionData;       // Not own

    MpCollectionContainerFactory    *mContainerFactory;     // Own
    MpCollectionContainer           *mCollectionContainer;  // Not own
    MpCollectionDataModel           *mCollectionDataModel;  // Own

    bool                            mActivated;
    MpNowPlayingWidget              *mNowPlayingBanner;     // Own
    bool                            mBannerAttached;

    HbMainWindow                    *mWindow;               // Not own
    HbAction                        *mSoftKeyQuit;          // Not own
    HbAction                        *mSoftKeyBack;          // Not own
    HbAction                        *mShuffleAction;        // Not own
    bool                            mShuffleEnabled;

    MpCollectionDocumentLoader      *mDocumentLoader;       // Own
    HbWidget                        *mMainContainer;        // Own

    HbToolBar                       *mMainToolBar;
    HbToolBar                       *mPlaylistToolBar;

    MpSnapshotWidget                *mSnapshot;

    bool                            mActivationWaiting;
    MpCollectionPopupHandler        *mMpPopupHandler;       // Own

    bool                            mUsbBlocked;

};

#endif  // MPCOLLECTIONVIEW_H

