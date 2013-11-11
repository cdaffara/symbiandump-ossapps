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
* Description: video collection view plugin's ui utils class
* 
*/

#include "videocollectionviewutils.h"
#include "videocollectionviewutilsdata.h"

int VideoCollectionViewUtilsData::mLastError = 0;
bool VideoCollectionViewUtilsData::mLoadSortingValuesFails = false;
bool VideoCollectionViewUtilsData::mIsServiceValue = false;
int VideoCollectionViewUtilsData::mVideoSortRole = -1;
int VideoCollectionViewUtilsData::mCollectionSortRole = -1;
Qt::SortOrder VideoCollectionViewUtilsData::mVideoSortOrder = Qt::AscendingOrder;
Qt::SortOrder VideoCollectionViewUtilsData::mCollectionSortOrder = Qt::AscendingOrder;
QString VideoCollectionViewUtilsData::mIconString = "";
QString VideoCollectionViewUtilsData::mPressedString = "";
QVariant VideoCollectionViewUtilsData::mLastStatusAdditional = QVariant();
VideoCollectionCommon::TCollectionLevels VideoCollectionViewUtilsData::mWidgetLevel = VideoCollectionCommon::ELevelVideos;
TMPXItemId VideoCollectionViewUtilsData::mActivityCollectionId = TMPXItemId::InvalidId();
QString VideoCollectionViewUtilsData::mActivityCollectionName = "";
QList<QString> VideoCollectionViewUtilsData::mCenRepStringValues;
QList<int> VideoCollectionViewUtilsData::mCenRepIntValues;

VideoCollectionViewUtils& VideoCollectionViewUtils::instance()
{
     static VideoCollectionViewUtils _popupInstance;
     return _popupInstance;
}

VideoCollectionViewUtils::VideoCollectionViewUtils():
mIsService(false),
mVideosSortRole(-1),
mCollectionsSortRole(-1),
mVideosSortOrder(Qt::AscendingOrder),
mCollectionsSortOrder(Qt::AscendingOrder)
{
    // not stubbed
}

VideoCollectionViewUtils::~VideoCollectionViewUtils()
{
    VideoCollectionViewUtilsData::reset();
}

int VideoCollectionViewUtils::saveSortingValues(int role, Qt::SortOrder order, VideoCollectionCommon::TCollectionLevels target)
{
    int status(0);
    if(target == VideoCollectionCommon::ELevelCategory )
    {
        VideoCollectionViewUtilsData::mCollectionSortRole = role;
        mCollectionsSortRole = role;
        VideoCollectionViewUtilsData::mCollectionSortOrder = order;
        mCollectionsSortOrder = order;
    }
    else if(target > VideoCollectionCommon::ELevelCategory && 
            target <= VideoCollectionCommon::ELevelAlbum)
    {
        VideoCollectionViewUtilsData::mVideoSortRole = role;
        mVideosSortRole = role;
        VideoCollectionViewUtilsData::mVideoSortOrder = order;
        mVideosSortOrder = order;
    }
    else
    {
        status = -1;
    }
    return status;
}

int VideoCollectionViewUtils::loadSortingValues(int &role, Qt::SortOrder &order,  VideoCollectionCommon::TCollectionLevels target)
{
    if (VideoCollectionViewUtilsData::mLoadSortingValuesFails)
    {
        return -1;
    }
    if(target == VideoCollectionCommon::ELevelCategory )
    {
        role = VideoCollectionViewUtilsData::mCollectionSortRole;
        order = VideoCollectionViewUtilsData::mCollectionSortOrder;
    }
    else if(target > VideoCollectionCommon::ELevelCategory && 
            target <= VideoCollectionCommon::ELevelAlbum)
    {
        role = VideoCollectionViewUtilsData::mVideoSortRole;
        order = VideoCollectionViewUtilsData::mVideoSortOrder;
    }
    else
    {
        return -1;
    }
    return 0;
}

QString VideoCollectionViewUtils::getCenRepStringValue(int key)
{
    Q_UNUSED(key);
    if(VideoCollectionViewUtilsData::mCenRepStringValues.count() > 0)
    {
        QString str = VideoCollectionViewUtilsData::mCenRepStringValues.takeFirst();
        if(str != CENREP_NO_STRING) 
        {
            return str;
        }
    }
    return QString();
}

int VideoCollectionViewUtils::getCenRepIntValue(int key)
{
    Q_UNUSED(key);
    if(VideoCollectionViewUtilsData::mCenRepIntValues.count() > 0)
    {
        int value = VideoCollectionViewUtilsData::mCenRepIntValues.takeFirst();
        if(value != CENREP_NO_INT)
        {
            return value;
        }
    }
    return -1;
}

void VideoCollectionViewUtils::initListView(HbListView *view)
{
    Q_UNUSED(view);
    // not stubbed
}

void VideoCollectionViewUtils::setWidgetActivityLevel(const VideoCollectionCommon::TCollectionLevels &level)
{
    VideoCollectionViewUtilsData::mWidgetLevel = level;
}
  
void VideoCollectionViewUtils::getActivityWidgetLevel(VideoCollectionCommon::TCollectionLevels &level)
{
    level = VideoCollectionViewUtilsData::mWidgetLevel;
}

void VideoCollectionViewUtils::setCollectionActivityData(const TMPXItemId &id, const QString &name)
{
    VideoCollectionViewUtilsData::mActivityCollectionId = id;
    VideoCollectionViewUtilsData::mActivityCollectionName = name;
}
    
void VideoCollectionViewUtils::getCollectionActivityData(TMPXItemId &id, QString &name)
{
    id = VideoCollectionViewUtilsData::mActivityCollectionId;
    name = VideoCollectionViewUtilsData::mActivityCollectionName;
}


void VideoCollectionViewUtils::sortModel(VideoProxyModelGeneric *model,
    bool async, VideoCollectionCommon::TCollectionLevels target)
{
    Q_UNUSED(model);
    Q_UNUSED(async);
    Q_UNUSED(target);
    // not stubbed
}

void VideoCollectionViewUtils::showStatusMsgSlot(int statusCode, QVariant &additional)
{
    VideoCollectionViewUtilsData::mLastStatusAdditional = additional;
    VideoCollectionViewUtilsData::mLastError = statusCode;
}

void VideoCollectionViewUtils::setIsService()
{
	mIsService = VideoCollectionViewUtilsData::mIsServiceValue;
}

bool VideoCollectionViewUtils::isService()
{
    return mIsService;
}
