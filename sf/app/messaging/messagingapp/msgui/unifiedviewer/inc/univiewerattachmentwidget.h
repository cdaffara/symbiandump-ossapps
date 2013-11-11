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
 * Description: Widget for displaying attachment media objects.
 *
 */

#ifndef UNI_VIEWER_ATTACHMENT_WIDGET_H
#define UNI_VIEWER_ATTACHMENT_WIDGET_H

// SYSTEM INCLUDES
#include <HbWidget>

// FORWARD DECLARATIONS
class HbTextItem;
class HbIconItem;
class HbIcon;
class HbFrameItem;
class MsgServiceLaunchUtil;
class UniMessageInfo;

/**
 * @class UniViewerAttachmentWidget
 * @brief Widget for displaying attachment media objects.
 */
class UniViewerAttachmentWidget: public HbWidget
{
Q_OBJECT

public:

    /**
     * Constructor
     */
    explicit UniViewerAttachmentWidget(QGraphicsItem *parent = 0);

    /**
     * Destructor
     */
    virtual ~UniViewerAttachmentWidget();

    /**
     * Populates the widget contents.
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

private:

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
     * Handles pressed state.
     */
    void setPressed(bool pressed);

private:

    /**
     * Media icon item.
     * Own
     */
    HbIconItem *mMediaIcon;

    /**
     * Media name text item.
     * Own
     */
    HbTextItem *mName;

    /**
     * Media information text item.
     * Own
     */
    HbTextItem *mInfo;

    /**
     * Background item of widget.
     * Own
     */
    HbFrameItem *mFrameItem;

    /**
     * MsgServiceLaunchUtil object.
     * Own
     */
    MsgServiceLaunchUtil *mViewerUtils;

    /**
     * Media file path.
     */
    QString mMediaPath;

    /**
     * Mime type of media.
     */
    QString mMimeType;
    
#ifdef MSGUI_UNIT_TEST
    /**
     * Unit Testing
     */
    friend class TestUniViewerAttachmentWidget;
#endif
};

#endif // UNI_VIEWER_ATTACHMENT_WIDGET_H
// EOF
