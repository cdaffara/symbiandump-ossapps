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
 * Description: This widget displays the pixmap content.
 *
 */

#ifndef MSG_UNIFIED_EDITOR_PIXMAP_WIDGET_H
#define MSG_UNIFIED_EDITOR_PIXMAP_WIDGET_H

#include <HbIconItem>

// FORWARD DECLARATIONS
class MsgUnifiedEditorUtils;

/**
 * This widget displays the pixmap content.
 */
class MsgUnifiedEditorPixmapWidget: public HbIconItem
{
Q_OBJECT

public:

    /**
     * Constructor
     */
    MsgUnifiedEditorPixmapWidget(QGraphicsItem *parent = 0);

    /**
     * Destructor
     */
    ~MsgUnifiedEditorPixmapWidget();

    /**
     * Sets the pixmap content to be displayed.
     * @param imagePath File path of the image.
     */
    void populate(const QString &imagePath);

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
     * Emitted when remove action is triggered.
     */
    void remove();

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
     * Handles removing of media.
     */
    void handleRemove(); 
    
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

private:

    /**
     * MsgUnifiedEditorUtils object.
     * Own
     */
    MsgUnifiedEditorUtils *mEditorUtils;

    /**
     * Pixmap file path being set.
     */
    QString mPixmapPath;

#ifdef MSGUI_UNIT_TEST
    /**
     * Unit Testing
     */
    friend class TestMsgUnifiedEditorPixmapWidget;
#endif
};

#endif /* MSG_UNIFIED_EDITOR_PIXMAP_WIDGET_H */

// EOF
