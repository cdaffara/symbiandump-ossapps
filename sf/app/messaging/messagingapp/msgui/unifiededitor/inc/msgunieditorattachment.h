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
 * Description:attachment field to show attachment details.
 *
 */

#ifndef MSG_UNIFIED_EDITOR_ATTACHMENT_H
#define MSG_UNIFIED_EDITOR_ATTACHMENT_H

#include <HbWidget>
#include <QList>

class HbTextItem;
class HbIconItem;
class HbGestureSceneFilter;
class HbFrameItem;
class MsgUnifiedEditorUtils;

class MsgUnifiedEditorAttachment : public HbWidget
    {
    Q_OBJECT

public:

    /**
     * Constructor
     */
    MsgUnifiedEditorAttachment( const QString& attachmentpath,
                                const int filesize,
                                QGraphicsItem *parent = 0 );

    /**
     * Destructor
     */
    ~MsgUnifiedEditorAttachment();

    /**
     * Seeker method to get the path of the attachment
     * @return path of the attachment
     */
    const QString& path();

    /**
     * Seeker method to get the size of the attachment
     * @return size in bytes
     */
    qreal size();

    /**
     * Seeker method to get mimetype of the attachment
     * @return mimetype
     */
    const QString& mimeType();

    /**
     * Check if the attachment content is MM content
     * @return true/false
     */
    bool isMultimediaContent();
    
protected:
    /**
     * reimplemented from base class.
     */
    virtual void gestureEvent(QGestureEvent *event);
    
    
private:
   
    void setPressed(bool pressed);


signals:
    /**
	 * emit when short-tap happens on the attachment
	 */
    void clicked();

	/**
	 * emit when 'remove' option is selected from longpress menu
	 */
    void deleteMe(MsgUnifiedEditorAttachment* attachment);

private slots:
    /**
	 * show longpress menu for attachment object
	 */
    void handleLongTap(QPointF position);
    
    /**
     * Handles short tap event.
     */
    void handleShortTap();

	/**
	 * slot to remove attachment from msg editor
	 */
    void handleRemove();

	/**
	 * slot to open attachment
	 */
    void handleOpen();
    
    /**
     * Slot to regrab gesture after some delay (300 ms) to avoid multiple gesture
     * events back to back.  
     */
    void regrabGesture();

private:
    /**
	 * attachment file's path
	 */
    QString mPath;

    /**
	 * attachment's size in bytes
	 */
    qreal mSize;

    /**
	 * attachment file's mimetype
	 */
    QString mMimeType;

    /**
	 * layout icon to indicate attachment
	 */
    HbIconItem* mAttachmentIcon;

    /**
	 * layout item to hold attachment's name
	 */
    HbTextItem* mAttachmentName;

    /**
	 * layout item to hold attachment's details e.g. size
	 */
    HbTextItem* mAttachmentDetails;

    /**
	 * gesture filter for long press.
	 */    
    HbGestureSceneFilter* mGestureFilter;
	
	/**
     * Max limit on sms size. Store at class level for optimization
     */
    int mMaxSmsSize;
    
	/**
     * Background item
     * Own
     */
    HbFrameItem* mBackGround;
    
    /**
     * MsgUnifiedEditorUtils object.
     * Own
     */
    MsgUnifiedEditorUtils *mEditorUtils;

#ifdef MSGUI_UNIT_TEST
    /**
     * Unit Testing
     */
    friend class TestMsgUnifiedEditorAttachment;
#endif

    };

typedef QList<MsgUnifiedEditorAttachment*> MsgUnifiedEditorAttachmentList;

#endif //MSG_UNIFIED_EDITOR_ATTACHMENT_H
