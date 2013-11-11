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
 * Description: Container class for msg attachments
 *
 */
#ifndef MSG_UNIFIED_EDITOR_ATTACHMENT_CONTAINER_H
#define MSG_UNIFIED_EDITOR_ATTACHMENT_CONTAINER_H

#include <HbWidget>
#include "msgunieditorattachment.h"

class QGraphicsLinearLayout;
class MmsConformanceCheck;

class MsgAttachmentContainer : public HbWidget
    {
    Q_OBJECT

public:
    /**
     * Add Attachment success/failure states
     */
    enum AddAttachmentStatus
    {
        EAddSuccess = 0, EAddNotSupported, EAddSizeExceed
    };

    /**
     * Constructor
     */
    MsgAttachmentContainer( QGraphicsItem *parent = 0);

    /**
     * Destructor
     */
    ~MsgAttachmentContainer();

    /**
     * add attachment to the container
	 * @param filepath of the attachment
	 * @return add operation status
     */
    MsgAttachmentContainer::AddAttachmentStatus addAttachment(
            const QString& filepath);

    /**
     * count of attachments in the container
	 * @return count
     */
    int count();
    
    /**
     * List of attachments in the container
     * @return attachments list
     */
    MsgUnifiedEditorAttachmentList attachmentList();
    
    /**
     * Check if the container hold multimedia content
     * @return true, if multimedia content is present
     *         false, otherwise.
     */
    bool hasMMContent();
    
    /**
     * Size of the attachment container
     * @return size
     */
    int containerSize();

signals:
    /**
     * emit when container content changes
     */
    void contentChanged();

    /**
     * emit to indicate view that container is now empty
	 * View deletes the container if it becomes empty
     */
    void emptyAttachmentContainer();
    
public slots:
    /**
     * delete attachment from the container
     */
    void deleteAttachment(MsgUnifiedEditorAttachment* attachment);
    
private:
    /**
     * size of the msg
     */
    int messageSize();
    
private:
    
    /**
     * container's layout
     */
    QGraphicsLinearLayout* mLayout;
    
    /**
     * MMS conformance check utility class
     */
    MmsConformanceCheck* mMmsConformanceCheck;

    /**
     * attachment list
     */
    MsgUnifiedEditorAttachmentList mAttachmentList;

    /**
     * flag to tell, if container is holding MM content
     */
    bool mIsMMContent;
    };

#endif // MSG_UNIFIED_EDITOR_ATTACHMENT_CONTAINER_H
