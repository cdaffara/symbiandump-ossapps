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
 * Description: This widget is used to display audio content.
 *
 */

#ifndef MSG_UNIFIED_EDITOR_AUDIO_WIDGET_H
#define MSG_UNIFIED_EDITOR_AUDIO_WIDGET_H

// FORWARD DECLARATIONS
class MsgUnifiedEditorUtils;

#include <HbPushButton>

/**
 * @class MsgUniFiedEditorAudioWidget
 * @brief This widget is used to display audio content in univiewer.
 */
class MsgUniFiedEditorAudioWidget: public HbPushButton
{
Q_OBJECT

public:

    /**
     * Constructor
     */
    explicit MsgUniFiedEditorAudioWidget(QGraphicsItem *parent = 0);

    /**
     * Destructor
     */
    ~MsgUniFiedEditorAudioWidget();

    /**
     * Populates the widget contents.
     * @param mimeType Mime type of media.
     * @param filePath File path of media.
     */
    void populate(const QString &filePath);

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
    
    /**
     * Emitted when remove action is triggered.
     */
    void remove();

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
     * MsgUnifiedEditorUtils object.
     * Own
     */
    MsgUnifiedEditorUtils *mEditorUtils;

    /**
     * Media file path.
     */
    QString mMediaPath;

    /**
     * True if valid media duration else false.
     */
    bool mValidMediaDuration;
    
#ifdef MSGUI_UNIT_TEST
    /**
     * Unit Testing
     */
    friend class TestMsgUniFiedEditorAudioWidget;
#endif
};

#endif //MSG_UNIFIED_EDITOR_AUDIO_WIDGET_H
// EOF
