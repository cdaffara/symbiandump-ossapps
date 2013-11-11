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
* Description: Music Player Playback view.
*
*/

#ifndef MPPLAYBACKVIEW_H
#define MPPLAYBACKVIEW_H

#include "mpviewbase.h"

class HbAction;
class HbIcon;
class HbMainWindow;
class MpEngine;
class MpPlaybackData;
class MpPlaybackWidget;
class MpEqualizerWidget;


class MpPlaybackView : public MpViewBase
{
    Q_OBJECT
    Q_PROPERTY(QString PlaybackStatus READ playbackStatus)
    Q_PROPERTY(bool Shuffle READ shuffleEnabled)
    Q_PROPERTY(bool Repeat READ repeatEnabled)

public:

    explicit MpPlaybackView();
    virtual ~MpPlaybackView();

    void initializeView();
    void activateView();
    void deactivateView();

    QString playbackStatus();
    bool shuffleEnabled();
    bool repeatEnabled();
    
signals:

    void command( int aCommand );

public slots:

    void startSettingsView();
    void back();
    void exit();

    void playbackStateChanged();
    
    void flip();
    
    void toggleShuffle();
    void shuffleChanged( bool shuffle );
    void toggleRepeat();
    void repeatChanged( bool repeat );

    void handleSongSelected();
    void showEqualizerDialog();
    void closeEqualizerDialog();
    
    void startRewindTimer();
    void startForwardTimer();
    void startSeekRewind();
    void startSeekForward();
    void endRewind();
    void endForward();
    
    void connectButtons();
    void showCorruptedNote();

private:

    void setupMenu();
    void setupToolbar();
    void updatePlayPauseIcon();

private:

    MpEngine                *mMpEngine;         // Own
    MpPlaybackData          *mPlaybackData;     // Not own
    MpPlaybackWidget        *mPlaybackWidget;   // Not own
    MpEqualizerWidget       *mEqualizerWidget;  // Own

    HbMainWindow            *mWindow;           // Not own
    HbAction                *mSoftKeyBack;      // Own
    bool                    mActivated;

    HbIcon                  *mPlayIcon;         // Own
    HbIcon                  *mPauseIcon;        // Own

    HbAction                *mPlayPauseAction;  // Not own
    HbAction                *mRepeatAction;     // Not own
    HbAction                *mShuffleAction;    // Not own

    QTimer                  *mTimer;

    bool                    mShuffle;
    bool                    mRepeat;
    bool                    mSeeking;

    Q_DISABLE_COPY(MpPlaybackView)
};

#endif  // MPPLAYBACKVIEW_H
