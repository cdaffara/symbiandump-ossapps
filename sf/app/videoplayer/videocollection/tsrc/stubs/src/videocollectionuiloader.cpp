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
* Description:
*
*/

#include <qdebug.h>
#include <qset.h>
#include <qstring.h>
#include <hblistview.h>
#include <hbmenu.h>

#include "videocollectioncommon.h"
#include "videocollectionuiloader.h"
#include "videocollectionuiloaderdef.h"
#include "videolistview.h"
#include "videolistwidget.h"
#include "videolistselectiondialog.h"
#include "videocollectionwrapper.h"

#include "videocollectionuiloaderdata.h"

bool VideoCollectionUiLoaderData::mFindFailure = false;
bool VideoCollectionUiLoaderData::mFailDialogLoad = false;
QStringList VideoCollectionUiLoaderData::mFindFailureNameList;
QMap<VideoCollectionUiLoader::ActionIds, HbAction*> VideoCollectionUiLoaderData::mMenuActions;
int VideoCollectionUiLoaderData::mLastLoadPhasedData = -1;
int VideoCollectionUiLoaderData::mAddDataCallCount = 0;

VideoCollectionUiLoader::VideoCollectionUiLoader():
    HbDocumentLoader(),
    mTimerId(0),
    mSortGroup(0),
    mIsService(0)
{
    // not stubbed
}

VideoCollectionUiLoader::~VideoCollectionUiLoader()
{
    VideoCollectionUiLoaderData::reset();
}

void VideoCollectionUiLoader::loadPhase(int loadPhase)
{
    VideoCollectionUiLoaderData::mLastLoadPhasedData = loadPhase;
}

void VideoCollectionUiLoader::addData(QList<VideoCollectionUiLoaderParam> params,
    QObject *receiver,
    const char *slot)
{
    VideoCollectionUiLoaderData::mAddDataCallCount++;
    Q_UNUSED(params);
    Q_UNUSED(receiver);
    Q_UNUSED(slot);
}

void VideoCollectionUiLoader::removeOrphanFromList(QObject *object)
{
    if (mOrphans.contains(object))
    {
        mOrphans.removeOne(object);
    }

    const QString &name = mObjects.key(object);
    if (!name.isEmpty())
    {
        // found from list, remove
        mObjects.remove(name);
    }
}

void VideoCollectionUiLoader::setIsService(bool isService)
{
    mIsService = isService;
}

void VideoCollectionUiLoader::load(const QString &fileName, bool *ok)
{
    QObjectList list = HbDocumentLoader::load(fileName, ok);
    storeOrphans(list);
}

void VideoCollectionUiLoader::load(const QString &fileName, const QString &section , bool *ok)
{
    QObjectList list = HbDocumentLoader::load(fileName, section, ok);
    storeOrphans(list);
}

QGraphicsWidget* VideoCollectionUiLoader::doFindWidget(const QString &name, bool loadIfNotFound)
{
    Q_UNUSED(loadIfNotFound);
    QGraphicsWidget *widget = 0;
    if(VideoCollectionUiLoaderData::mFindFailure)
    {
        return 0; 
    }
    if(!VideoCollectionUiLoaderData::mFindFailureNameList.contains(name))
    {
        widget = HbDocumentLoader::findWidget(name);
    }
    return widget;
}

QObject* VideoCollectionUiLoader::doFindObject(const QString &name, bool loadIfNotFound)
{
    Q_UNUSED(loadIfNotFound);
    QObject *object = 0;
    if(VideoCollectionUiLoaderData::mFindFailure)
    {
        return 0; 
    }
    if(!VideoCollectionUiLoaderData::mFindFailureNameList.contains(name))
    {
        object = HbDocumentLoader::findObject(name);
    }
    return object;
}

void VideoCollectionUiLoader::initObject(QObject *object,
    const QString &name)
{
    VideoCollectionWrapper &wrapper = VideoCollectionWrapper::instance();
    
    if (name == DOCML_NAME_VC_VIDEOLISTWIDGET ||
        name == DOCML_NAME_VC_COLLECTIONWIDGET ||
        name == DOCML_NAME_VC_COLLECTIONCONTENTWIDGET)
    {
        VideoProxyModelGeneric *model(0);
        
        if(name == DOCML_NAME_VC_COLLECTIONWIDGET) {
            model = wrapper.getCollectionsModel();
        } else if (name == DOCML_NAME_VC_COLLECTIONCONTENTWIDGET) {
            model = wrapper.getCollectionContentModel();
        } else
        {
            model = wrapper.getAllVideosModel();
        }
        VideoListWidget *videoList = qobject_cast<VideoListWidget*>(object);
        
        VideoCollectionCommon::TCollectionLevels level = VideoCollectionCommon::ELevelInvalid;
        if(name == DOCML_NAME_VC_VIDEOLISTWIDGET )
        {
            level = VideoCollectionCommon::ELevelVideos;
        }
        else if(name == DOCML_NAME_VC_COLLECTIONWIDGET)
        {
            level = VideoCollectionCommon::ELevelCategory;
        }
        videoList->initialize(*model, 0, level);
    }
    else if (name == DOCML_NAME_DIALOG)
    {

    }
    else if (name == DOCML_NAME_OPTIONS_MENU)
    {

    }
    else if (name == DOCML_NAME_SORT_MENU)
    {
        VideoCollectionUiLoaderData::mMenuActions[EActionSortBy] = qobject_cast<HbAction*>(object);
    }
    else if(name == DOCML_NAME_ADD_TO_COLLECTION)
    {
        VideoCollectionUiLoaderData::mMenuActions[EActionAddToCollection] = qobject_cast<HbAction*>(object);
    }
    else if(name == DOCML_NAME_CREATE_COLLECTION)
    {
        VideoCollectionUiLoaderData::mMenuActions[EActionNewCollection] = qobject_cast<HbAction*>(object);
    }
    else if(name == DOCML_NAME_DELETE_MULTIPLE)
    {
        VideoCollectionUiLoaderData::mMenuActions[EActionDelete] = qobject_cast<HbAction*>(object);
    }
    else if(name == DOCML_NAME_SORT_BY_DATE)
    {
        VideoCollectionUiLoaderData::mMenuActions[EActionSortByDate] = qobject_cast<HbAction*>(object);
        VideoCollectionUiLoaderData::mMenuActions[EActionSortByDate]->setCheckable(true);
    }
    else if(name == DOCML_NAME_SORT_BY_NAME)
    {
        VideoCollectionUiLoaderData::mMenuActions[EActionSortByName] = qobject_cast<HbAction*>(object);
        VideoCollectionUiLoaderData::mMenuActions[EActionSortByName]->setCheckable(true);
    }
    else if(name == DOCML_NAME_SORT_BY_NUMBER_OF_ITEMS)
    {
        VideoCollectionUiLoaderData::mMenuActions[EACtionSortByItemCount] = qobject_cast<HbAction*>(object);
        VideoCollectionUiLoaderData::mMenuActions[EACtionSortByItemCount]->setCheckable(true);
    }
    else if(name == DOCML_NAME_SORT_BY_SIZE)
    {
        VideoCollectionUiLoaderData::mMenuActions[EActionSortBySize] = qobject_cast<HbAction*>(object);
        VideoCollectionUiLoaderData::mMenuActions[EActionSortBySize]->setCheckable(true);
    }
}

void VideoCollectionUiLoader::timerEvent(QTimerEvent *event)
{
    Q_UNUSED(event);
    // not stubbed
}

QObject* VideoCollectionUiLoader::createObject(const QString& type,
    const QString &name)
{
    QObject* object = doFindObject(name, true);
    if (!object)
    {
        if (name == DOCML_NAME_VIEW)
        {
            object = new VideoListView(this);
        }
        else if (name == DOCML_NAME_VC_COLLECTIONWIDGET ||
                 name == DOCML_NAME_VC_COLLECTIONCONTENTWIDGET ||
                 name == DOCML_NAME_VC_VIDEOLISTWIDGET)
        {
            object = new VideoListWidget(this);
            initObject(qobject_cast<QGraphicsWidget*>(object), name);
        }
        else if (name == DOCML_NAME_DIALOG)
        {
            if(!VideoCollectionUiLoaderData::mFailDialogLoad)
            {
                object = new VideoListSelectionDialog(this);
            }
            else
            {
                return 0;
            }
        }
        if (object)
        {
            object->setObjectName(name);
            return object;
        }
        object = HbDocumentLoader::createObject(type, name);
        initObject(object, name);
    }
    
    return object;
}

void VideoCollectionUiLoader::runNext()
{
    // not stubbed
}

bool VideoCollectionUiLoader::isValid(const VideoCollectionUiLoaderParam &param)
{
    Q_UNUSED(param);
    // not stubbed
    return true;
}

// ---------------------------------------------------------------------------
// storeOrphans
// ---------------------------------------------------------------------------
//
void VideoCollectionUiLoader::storeOrphans(const QObjectList &list)
{
    foreach (QObject *object, list)
    {
        if (!mOrphans.contains(object))
        {
            // add to list
            mOrphans.append(object);
            
            // connect to "destroyed" signal
            connect(
                object, SIGNAL(destroyed(QObject*)),
                this, SLOT(removeOrphanFromList(QObject*)));
        }
    }
}

// end of file
