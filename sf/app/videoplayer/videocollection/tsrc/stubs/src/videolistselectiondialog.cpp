/*
* Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:   videolist selection dialog implementation
* 
*/

#include <hbaction.h>

#include "videolistselectiondialog.h"
#include "videolistselectiondialogdata.h"
#include "videocollectionwrapper.h"
#include "videoproxymodelgeneric.h"

bool VideoListSelectionDialogData::mExecReturnPrimary = false;
int VideoListSelectionDialogData::mSelectionCount = 0;
int VideoListSelectionDialogData::mMultiSelectionLaunchCount = 0;    
int VideoListSelectionDialogData::mMultiSelectionItemSelectionCount = 0;
int VideoListSelectionDialogData::mSelectionType = -1;
TMPXItemId VideoListSelectionDialogData::mSettedMpxId = TMPXItemId::InvalidId();

VideoListSelectionDialog::VideoListSelectionDialog(
    VideoCollectionUiLoader *uiLoader,
    QGraphicsItem *parent) : 
    HbDialog(parent),
    mUiLoader(uiLoader),
    mHeading(0),
    mCheckboxContainer(0),
    mItemCount(0),
    mCheckBox(0),
    mListContainer(0),
    mForcedCheck(false),
    mModel(0),
    mModelReady(false),
    mAlbumListReady(false),
    mListWidget(0),
    mPrimaryAction(0),
    mSecondaryAction(0)
{
    setDismissPolicy(HbDialog::NoDismiss);
    setTimeout(HbDialog::NoTimeout);
    
    // create primary action
    mPrimaryAction = new HbAction();
    
    // create secondary action
    mSecondaryAction = new HbAction();
}

VideoListSelectionDialog::~VideoListSelectionDialog() 
{
    VideoListSelectionDialogData::reset();
}

void VideoListSelectionDialog::setupContent(int type, TMPXItemId activeItem)
{
    VideoListSelectionDialogData::mSelectionType = type;
    VideoListSelectionDialogData::mSettedMpxId = activeItem;
}

bool VideoListSelectionDialog::initDialog()
{
    // not stubbed
    return false;
}

void VideoListSelectionDialog::exec()
{
    // fill selection
    VideoListSelectionDialogData::mMultiSelectionLaunchCount++;
    VideoCollectionWrapper &wrapper = VideoCollectionWrapper::instance();
    VideoProxyModelGeneric *model = wrapper.getGenericModel();
    if (model)
    {
        for (int i = 0; i < VideoListSelectionDialogData::mSelectionCount; i++)
        {
            QModelIndex index = model->index(i, 0);
            if(index.isValid())
            {
                mSelection.append(QItemSelectionRange(index));
            }
        }
    }
    VideoListSelectionDialogData::mMultiSelectionItemSelectionCount = mSelection.indexes().count();
    
}

void VideoListSelectionDialog::finishedSlot(HbAction *action)
{
    Q_UNUSED(action);
}

void VideoListSelectionDialog::markAllStateChangedSlot(int state)
{
    Q_UNUSED(state);
    // not stubbed
}

void VideoListSelectionDialog::selectionChangedSlot(
    const QItemSelection &selected,
    const QItemSelection &deselected)
{  
    Q_UNUSED(selected);
    Q_UNUSED(deselected);
    // not stubbed
}

void VideoListSelectionDialog::singleItemSelectedSlot(const QModelIndex &index)
{
    Q_UNUSED(index);
    // not stubbed
}

void VideoListSelectionDialog::modelReadySlot()
{
    // not stubbed
}

void VideoListSelectionDialog::albumListReadySlot()
{
    // not stubbed
}

void VideoListSelectionDialog::updateCounterSlot()
{
    // not stubbed
}

void VideoListSelectionDialog::primaryActionTriggeredSlot()
{
    // not stubbed
}

void VideoListSelectionDialog::connectSignals()
{
    // not stubbed
}
 
void VideoListSelectionDialog::disconnectSignals()
{
    // not stubbed
}

void VideoListSelectionDialog::newAlbumNameDialogFinished(HbAction *action)
{
    Q_UNUSED(action);
    // not stubbed
}

void VideoListSelectionDialog::finalize(QString albumName)
{
    Q_UNUSED(albumName);
    // not stubbed
}

// end of file
