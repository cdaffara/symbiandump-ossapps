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
* Description: Music Player now playing widget.
*
*/

#ifndef MPNOWPLAYINGWIDGET_H
#define MPNOWPLAYINGWIDGET_H

//includes
#include <QtGlobal>
#include <hbwidget.h>

#ifdef BUILD_MPNOWPLAYINGBANNER
#define MPNOWPLAYINGBANNER_EXPORT Q_DECL_EXPORT
#else
#define MPNOWPLAYINGBANNER_EXPORT Q_DECL_IMPORT
#endif

//forward declartions
class QGraphicsSceneMouseEvent;
class MpNowPlayingWidgetPrivate;
class HbFrameItem;


class MPNOWPLAYINGBANNER_EXPORT MpNowPlayingWidget : public HbWidget
{
    Q_OBJECT
    friend class MpNowPlayingWidgetPrivate;

public:
    explicit MpNowPlayingWidget( QGraphicsItem *parent=0 );
    virtual ~MpNowPlayingWidget();
    void setEnabled( bool enabled );
    void resizeEvent(QGraphicsSceneResizeEvent *event);
    bool isBannerAttached();

signals:
    void clicked();
    void playbackAttachmentChanged(bool attached);

private:
    void mousePressEvent( QGraphicsSceneMouseEvent *event );
    void mouseReleaseEvent( QGraphicsSceneMouseEvent *event );
    void mouseMoveEvent( QGraphicsSceneMouseEvent *event );

private:
    Q_DISABLE_COPY(MpNowPlayingWidget)
    MpNowPlayingWidgetPrivate * const d_ptr; //owned
    HbFrameItem     *mFrameItem;
};

#endif    // MPNOWPLAYINGWIDGET_H
