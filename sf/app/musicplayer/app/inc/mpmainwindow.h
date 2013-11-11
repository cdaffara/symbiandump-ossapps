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
* Description: Qt Music Player main window definition.
*
*/


#ifndef MPMAINVIEW_H
#define MPMAINVIEW_H

// System includes
#include <hbmainwindow.h>
#include <mpxviewframeworkqt.h>
#include <afactivities_global.h>

#include "mpcommondefs.h"

// Forward declarations
class MpxViewPlugin;
class MusicServices;
class MpGlobalPopupHandler;
class HbActivityManager;
class MpMediaController;
class AfActivityStorage;
class AfActivation;

// Class declaration
class MpMainWindow: public MpxViewFramework
{
    Q_OBJECT

public:

    enum ViewType {
        NoView = 0,
        CollectionView,
        PlaybackView,
        SettingsView,
        DetailsView,
        MediaWallView};
    
    enum ActivityMode{
        MusicMainView,
        MusicNowPlayingView,
        MusicNowPlayingViewShuffleAll
    };

    MpMainWindow();
    ~MpMainWindow();

    void initialize();

signals:
    // For automation testability
    void applicationReady();

public slots:
    void handleCommand( int commandCode );
    void handleLibraryUpdated();

    void switchView( Qt::Orientation orientation );
    void initializeServiceView( quint32 clientSecureId );
    void loadActivityData( Af::ActivationReason reason, const QString &name, QVariantHash parameter );
    void saveActivity();
    void handleRestorePathFailed();

private:
    void activateView(ViewType);
    void connectView();
    void disconnectView();
    void keyPressEvent(QKeyEvent *event);
    MpxViewPlugin*  loadView( ViewType type, MpCommon::MpViewMode viewMode= MpCommon::DefaultView );
    void loadActivity( QVariant data );

private:

    MpxViewPlugin         *mCollectionViewPlugin; // Own
    MpxViewPlugin         *mPlaybackViewPlugin;   // Own
    MpxViewPlugin         *mSettingsViewPlugin;   // Own
    MpxViewPlugin         *mDetailsViewPlugin;    // Own
    MpxViewPlugin         *mMediaWallViewPlugin;  // Own
    MpxViewPlugin         *mCurrentViewPlugin;    // Own
    ViewType              mVerticalViewType;
    ViewType              mPreviousVerticalViewType;
    MusicServices         *mMusicServices;         // Own
    MpGlobalPopupHandler  *mPopupHandler;          // Own
    bool                  mUserExit;
    MpMediaController     *mMpMediaController;     // Own
    AfActivityStorage     *mActivityStorage;       // Own
};

#endif // MPMAINVIEW_H

