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
 * Description: This widget displays the pixmap content in viewer.
 *
 */

#ifndef UNI_VIEWER_PIXMAP_WIDGET_H
#define UNI_VIEWER_PIXMAP_WIDGET_H

#include <HbIconItem>

// FORWARD DECLARATIONS
class MsgServiceLaunchUtil;
class UniMessageInfo;
class ThumbnailManager;

/**
 * This widget displays the pixmap content in viewer.
 */
class UniViewerPixmapWidget: public HbIconItem
{
Q_OBJECT

public:

    /**
     * Constructor
     */
    UniViewerPixmapWidget(QGraphicsItem *parent = 0);

    /**
     * Destructor
     */
    ~UniViewerPixmapWidget();

    /**
     * Sets the pixmap content to be displayed.
     * @param info Object information.
     */
    void populate(UniMessageInfo *info);

signals:

    /**
     * Signal emitted when widget is clicked.
     * @param mediaPath File path of the media.
     */
    void shortTap(const QString &mediaPath);

    /**
     * Signal emitted when widget is long tapped.
     * @param position Scene coordinates of tap.
     */
    void longTap(const QPointF &position);

    /**
     * Signal emitted indicating if thumbnail was found or not
     * @param result true if thumbnail was found else false.
     * @param info UniMessageInfo of the media object.
     */
    void thumbnailFound(bool result, UniMessageInfo *info);

protected:

    /**
     * Event handler for gesture events.
     * Reimplemented from HbWidgetBase.
     * @see HbWidgetBase
     */
    virtual void gestureEvent(QGestureEvent *event);

private slots:

    /**
     * Handles opening of media.
     */
    void handleOpen();

    /**
     * Slot to regrab gesture after some delay (300 ms) to avoid multiple gesture
     * events back to back.  
     */
    void regrabGesture();

    /**
     * Slot hit when the thumbnail is ready.
     */
    void thumbnailReady(const QPixmap& pixmap, void *data, int id, int error);

private:

    /**
     * Initialization function.
     */
    void init();

    /**
     * Handles short tap event.
     */
    void handleShortTap();

    /**
     * Handles long tap event.
     * @param position Scene coordinates of tap.
     */
    void handleLongTap(const QPointF &position);

    /**
     * Calculates the thumbnail size.
     * @return Thumbnail size
     */
    QSize getThumbnailSize();

private:

    /**
     * UniMessageInfo object.
     * Own.
     */
    UniMessageInfo *mInfo;

    /**
     * MsgServiceLaunchUtil object.
     * Own
     */
    MsgServiceLaunchUtil *mViewerUtils;

    /**
     * ThumbnailManager
     * Own.
     */
    ThumbnailManager *mThumbnailManager;

    /**
     * Mime Type of pixmap.
     */
    QString mMimeType;

    /**
     * Pixmap file path being set.
     */
    QString mPixmapPath;
    
#ifdef MSGUI_UNIT_TEST
    /**
     * Unit Testing
     */
    friend class TestUniViewerPixmapWidget;
#endif
};

#endif /* UNI_VIEWER_PIXMAP_WIDGET_H */

// EOF
