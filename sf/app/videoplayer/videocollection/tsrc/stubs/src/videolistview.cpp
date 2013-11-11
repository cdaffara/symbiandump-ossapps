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
* Description:   Videolist view class source code
*
*/

#include "videolistview.h"
#include "videolistwidget.h"
#include "videocollectionviewutils.h"
#include "videocollectionwrapper.h"

#include "videolistviewdata.h"

int VideoListViewData::mActivateViewCount = 0;
int VideoListViewData::mBackCount = 0;
int VideoListViewData::mDeactivateViewCount = 0;
int VideoListViewData::mInitializeViewCount = 0;
TMPXItemId VideoListViewData::mActivatedItemId = TMPXItemId::InvalidId();
VideoListWidget* VideoListViewData::mCurrentListWidget = 0;

VideoListView::VideoListView(VideoCollectionUiLoader *uiLoader, QGraphicsItem *parent) :
HbView(parent),
mUiUtils(VideoCollectionViewUtils::instance()),
mWrapper(VideoCollectionWrapper::instance()),
mUiLoader(uiLoader),
mModelReady(false),
mVideoServices(0),
mCurrentList(0)
{
    // not stubbed
}

VideoListView::~VideoListView()
{
    // disconnect signals
    disconnect();
}

int VideoListView::initializeView()
{
    // not stubbed
    VideoListViewData::mInitializeViewCount++;
    return 0;
}

void VideoListView::titleReadySlot(const QString& title)
{
    Q_UNUSED(title);
    // not stubbed
}

int VideoListView::activateView(TMPXItemId &itemId)
{
    Q_UNUSED(itemId);
    
    VideoListViewData::mActivatedItemId = itemId;
    VideoListViewData::mActivateViewCount++;
    return 0;
}

void VideoListView::modelReadySlot()
{
    // not stubbed
}

void VideoListView::albumListReadySlot()
{
    // not stubbed
}

void VideoListView::layoutChangedSlot()
{
    // not stubbed
}

void VideoListView::deactivateView()
{
    VideoListViewData::mDeactivateViewCount++;
}

void VideoListView::back()
{
    VideoListViewData::mBackCount++;
}

void VideoListView::modelReady()
{
    // not stubbed
}

void VideoListView::cleanup()
{
    // not stubbed
}

int VideoListView::createToolbar()
{
    // not stubbed
    return 0;
}

VideoListWidget *VideoListView::getCurrentList()
{
    if(VideoListViewData::mCurrentListWidget)
    {
        return VideoListViewData::mCurrentListWidget;
    }
    return mCurrentList;
}

void VideoListView::showHint(bool show)
{
    Q_UNUSED(show);
    // not stubbed
}

void VideoListView::updateSubLabel()
{
    // not stubbed
}

void VideoListView::openAllVideosViewSlot()
{
    // not stubbed
}

void VideoListView::openCollectionViewSlot()
{
    // not stubbed
}

void VideoListView::aboutToChangeOrientationSlot()
{
    // not stubbed
}

void VideoListView::orientationChangedSlot(Qt::Orientation orientation)
{
    Q_UNUSED(orientation);
    // not stubbed
}

void VideoListView::handleAsyncStatusSlot(int statusCode, QVariant &additional)
{
    Q_UNUSED(statusCode);
    Q_UNUSED(additional);
    // not stubbed
}

void VideoListView::collectionOpenedSlot(bool collectionOpened,
    const QString& collection,
    const TMPXItemId &id)
{
    Q_UNUSED(collectionOpened);
    Q_UNUSED(collection);
    Q_UNUSED(id);
    // not stubbed
}

void VideoListView::objectReadySlot(QObject *object, const QString &name)
{
    Q_UNUSED(object);
    Q_UNUSED(name);
    // not stubbed
}

void VideoListView::doDelayedsSlot()
{
    // not stubbed    
}


void VideoListView::toolbarActionsChanged(QList<QAction*> newActions)
{
    Q_UNUSED(newActions);    
}

void VideoListView::toolbarExtensionChanged(HbToolBarExtension* newExtension)
{
    Q_UNUSED(newExtension);     
}

// end of file
