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
* Description: Music Player now playing widget - private implementation.
*
*/

#ifndef MPNOWPLAYINGWIDGET_P_H
#define MPNOWPLAYINGWIDGET_P_H

//includes
#include <QObject>
#include <hbwidget.h>

#include "mpnowplayingwidget.h"
#include "mpplaybackdata.h"

//forward declartions
class HbIconItem;
class HbLabel;
class MpEngine;
class QGraphicsSceneMouseEvent;
class HbIcon;
class HbDocumentLoader;
class QGraphicsSceneMouseEvent;

class MpNowPlayingWidgetPrivate : public QObject
{
    Q_OBJECT

private: 
    enum BannerPressedState {
        BannerNone,                // No button initially pressed
        BannerIcon,                // Icon pressed first
        BannerLabels               // Labels pressed first
    };
    
public:
    explicit MpNowPlayingWidgetPrivate( MpNowPlayingWidget *qq );
    virtual ~MpNowPlayingWidgetPrivate();

    void setEnabled( bool enabled );
    bool isBannerAttached();
    bool handleClickEvent( QGraphicsSceneMouseEvent *event );
    void handleMousePressEvent( QGraphicsSceneMouseEvent *event, bool pressed );
    void handleMouseMoveEvent( QGraphicsSceneMouseEvent *event );

public slots:

    void setState();
    void updateBannerInfo();
    
private:

    MpNowPlayingWidget                      *q_ptr;
    HbLabel                                 *mPrimaryText;
    HbLabel                                 *mSecondaryText;
    MpPlaybackData::SimplifiedState         mState;
    MpPlaybackData                          *mPlaybackData;
    MpEngine                                *mMpEngine;

    HbIcon                                  *mPlayIcon;
    HbIcon                                  *mPauseIcon;
    HbLabel                                 *mIcon;

    HbDocumentLoader                        *mDocumentLoader;

    BannerPressedState                      mCurrentPressedState;

};

#endif    // MPNOWPLAYINGWIDGET_P_H
