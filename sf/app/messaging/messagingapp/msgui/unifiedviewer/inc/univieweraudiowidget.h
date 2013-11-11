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
 * Description: This widget is used to display audio content in univiewer.
 *
 */

#ifndef UNI_VIEWER_AUDIO_WIDGET_H
#define UNI_VIEWER_AUDIO_WIDGET_H

// FORWARD DECLARATIONS
class MsgServiceLaunchUtil;
class UniMessageInfo;

#include <HbPushButton>

/**
 * @class UniViewerAudioWidget
 * @brief This widget is used to display audio content in univiewer.
 */
class UniViewerAudioWidget: public HbPushButton
{
Q_OBJECT

public:

    /**
     * Constructor
     */
    explicit UniViewerAudioWidget(QGraphicsItem *parent = 0);

    /**
     * Destructor
     */
    ~UniViewerAudioWidget();

    /**
     * Populates the widget contents.
     * @param info Object information.
     */
    void populate(UniMessageInfo *info);

    /**
     * Overloaded from base class.
     * @see HbPushButton
     */
    void setStretched(bool stretched = true);

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

private slots:

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

    /**
     * True if valid media duration else false.
     */
    bool mValidMediaDuration;
    
#ifdef MSGUI_UNIT_TEST
    /**
     * Unit Testing
     */
    friend class TestUniViewerAudioWidget;
#endif
};

#endif //UNI_VIEWER_AUDIO_WIDGET_H
// EOF
