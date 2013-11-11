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
 * Description: attachment container class
 *
 */


// INCLUDES
#include <QGraphicsLinearLayout>
#include <QFileInfo>
#include <HbFrameItem>
#include <HbFrameDrawer>

// USER INCLUDES
#include "msgunieditorattachmentcontainer.h"
#include "UniEditorGenUtils.h"
#include "msgunieditormonitor.h"
#include "mmsconformancecheck.h"

// Constants

//---------------------------------------------------------------
// MsgAttachmentContainer::MsgAttachmentContainer
// @see header file
//---------------------------------------------------------------
MsgAttachmentContainer::MsgAttachmentContainer( QGraphicsItem *parent ) :
HbWidget(parent),
mIsMMContent(false)
{
    mLayout = new QGraphicsLinearLayout(Qt::Vertical, this);
    mLayout->setContentsMargins(0,0,0,0);
    mLayout->setSpacing(0);
    mMmsConformanceCheck = new MmsConformanceCheck;
}

//---------------------------------------------------------------
// MsgAttachmentContainer::~MsgAttachmentContainer
// @see header file
//---------------------------------------------------------------
MsgAttachmentContainer::~MsgAttachmentContainer()
{
    delete mMmsConformanceCheck;
}

//---------------------------------------------------------------
// MsgAttachmentContainer::addAttachment
// @see header file
//---------------------------------------------------------------
MsgAttachmentContainer::AddAttachmentStatus 
    MsgAttachmentContainer::addAttachment(const QString& filepath)
{
    //check for insert conformance
    if(EInsertSuccess != mMmsConformanceCheck->checkModeForInsert(filepath))
        return EAddNotSupported;

    int msgSize = messageSize();
    QFileInfo fileinfo(filepath);
    int fileSize = fileinfo.size() + KEstimatedMimeHeaderSize;
    
    if( (fileSize + msgSize) <= MsgUnifiedEditorMonitor::maxMmsSize())
    {
        MsgUnifiedEditorAttachment* att = new MsgUnifiedEditorAttachment(
            filepath, fileSize, this);
        if( ((mAttachmentList.count() == 0) && att->isMultimediaContent()) ||
                ((mAttachmentList.count() == 1) && !mIsMMContent) )
        {
            mIsMMContent = true;
        }
        mAttachmentList << att;
        int index = mLayout->count();
        mLayout->insertItem(index,att);
        connect(att, SIGNAL(deleteMe(MsgUnifiedEditorAttachment*)),
            this, SLOT(deleteAttachment(MsgUnifiedEditorAttachment*)));

        // emit to signal that container content & size changed
        emit contentChanged();
    }
    else
    {
        return EAddSizeExceed;
    }
    return EAddSuccess;
}

//---------------------------------------------------------------
// MsgAttachmentContainer::deleteAttachment
// @see header file
//---------------------------------------------------------------
void MsgAttachmentContainer::deleteAttachment(MsgUnifiedEditorAttachment* attachment)
{
    mAttachmentList.removeOne(attachment);
    mLayout->removeItem(attachment);
    attachment->setParent(NULL);
    delete attachment;

    if( ((mAttachmentList.count() == 1) && !mAttachmentList.first()->isMultimediaContent()) ||
        ((mAttachmentList.count() == 0) && mIsMMContent) )
    {
        mIsMMContent = false;
    }

    // emit to indicate change in container content & size
    emit contentChanged();
    if(mAttachmentList.count() == 0)
    {
        emit emptyAttachmentContainer();
    }
}

//---------------------------------------------------------------
// MsgAttachmentContainer::count
// @see header file
//---------------------------------------------------------------
int MsgAttachmentContainer::count()
{
    return mAttachmentList.count();
}

//---------------------------------------------------------------
// MsgAttachmentContainer::attachmentList
// @see header file
//---------------------------------------------------------------
MsgUnifiedEditorAttachmentList MsgAttachmentContainer::attachmentList()
{
    return mAttachmentList;
}

//---------------------------------------------------------------
// MsgAttachmentContainer::containerSize
// @see header file
//---------------------------------------------------------------
int MsgAttachmentContainer::containerSize()
{
    int attCount = count();
    int containerSize = 0;
    
    for(int i=0; i<attCount; i++)
    {
        containerSize += mAttachmentList.at(i)->size();
    }
    return containerSize;
}

//---------------------------------------------------------------
// MsgAttachmentContainer::messageSize
// @see header file
//---------------------------------------------------------------
int MsgAttachmentContainer::messageSize()
{
    return containerSize() + MsgUnifiedEditorMonitor::bodySize() + MsgUnifiedEditorMonitor::subjectSize();
}

//---------------------------------------------------------------
// MsgAttachmentContainer::hasMMContent
// @see header file
//---------------------------------------------------------------
bool MsgAttachmentContainer::hasMMContent()
{
    return mIsMMContent;
}

//EOF

