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

// Version : %version: 24.1.2 %

// INCLUDE FILES
#include <qgraphicswidget.h>
#include <qaction.h>
#include <qactiongroup.h>
#include <hbmenu.h>
#include <hbaction.h>
#include <hbgroupbox.h>
#include <hbpushbutton.h>

#include "videocollectionuiloader.h"
#include "videolistview.h"
#include "videolistselectiondialog.h"
#include "videolistwidget.h"
#include "videocollectionwrapper.h"
#include "videoproxymodelgeneric.h"
#include "videocollectionviewutils.h"
#include "videocollectiontrace.h"

// ---------------------------------------------------------------------------
// VideoCollectionUiLoader
// ---------------------------------------------------------------------------
//
VideoCollectionUiLoader::VideoCollectionUiLoader()
    : HbDocumentLoader()
    , mTimerId( 0 )
    , mSortGroup( 0 )
    , mIsService( false )
{
	FUNC_LOG;
}

// ---------------------------------------------------------------------------
// VideoCollectionUiLoader
// ---------------------------------------------------------------------------
//
VideoCollectionUiLoader::~VideoCollectionUiLoader()
{
	FUNC_LOG;

    // delete objects without a parent
	while (!mOrphans.isEmpty())
	{
		delete mOrphans.takeFirst();
	}

    // kill timer if running
    if (mTimerId)
    {
        killTimer(mTimerId);
        mTimerId = 0;
    }

    // clear queue and hash tables
    mQueue.clear();
    mDocmls.clear();
    mSections.clear();
    mObjects.clear();
    mPhases.clear();
}

// ---------------------------------------------------------------------------
// load
// ---------------------------------------------------------------------------
//
void VideoCollectionUiLoader::load(const QString &fileName, bool *ok)
{
	FUNC_LOG;
	if (!mDocmls.contains(fileName))
	{
		QObjectList list = HbDocumentLoader::load(fileName, ok);
		if (ok && *ok)
		{
	        mDocmls.insert(fileName);
		}
		
		// add objects without a parent to orphan list
		storeOrphans(list);
	}
	else
	{
	    *ok = true;
	}
}

// ---------------------------------------------------------------------------
// load
// ---------------------------------------------------------------------------
//
void VideoCollectionUiLoader::load(const QString &fileName,
    const QString &section,
    bool *ok)
{
	FUNC_LOG;
    if (!mSections.contains(section))
    {
        QObjectList list = HbDocumentLoader::load(fileName, section, ok);
        if (ok && *ok)
        {
            mSections.insert(section);
        }

        // add objects without a parent to orphan list
        storeOrphans(list);
    }
    else
    {
        *ok = true;
    }
}

// ---------------------------------------------------------------------------
// addData
// ---------------------------------------------------------------------------
//
void VideoCollectionUiLoader::addData(
    QList<VideoCollectionUiLoaderParam> params,
    QObject *receiver,
    const char *slot)
{
	FUNC_LOG;
    // go through all parameters and add then in to the queue
    foreach (VideoCollectionUiLoaderParam param, params)
    {
        addToQueue(param, receiver, slot);
    }
}

// ---------------------------------------------------------------------------
// loadPhase
// ---------------------------------------------------------------------------
//
void VideoCollectionUiLoader::loadPhase(int loadPhase)
{
	FUNC_LOG;
    if (!mPhases.contains(loadPhase))
    {
        mPhases.insert(loadPhase);
        if (!mTimerId)
        {
            runNext();
        }
    }
}

// ---------------------------------------------------------------------------
// removeOrphanFromList
// ---------------------------------------------------------------------------
//
void VideoCollectionUiLoader::removeOrphanFromList(QObject *object)
{
	FUNC_LOG;
    // remove from orphan list
    if (mOrphans.contains(object))
    {
        mOrphans.removeOne(object);
    }
    
    // remove from initialized objects list
    const QString &name = mObjects.key(object);
    if (!name.isEmpty())
    {
        // found from list, remove
        mObjects.remove(name);
    }
}

// ---------------------------------------------------------------------------
// setIsService
// ---------------------------------------------------------------------------
//
void VideoCollectionUiLoader::setIsService(bool isService)
{
	FUNC_LOG;
    mIsService = isService;
}

// ---------------------------------------------------------------------------
// doFindWidget
// ---------------------------------------------------------------------------
//
QGraphicsWidget* VideoCollectionUiLoader::doFindWidget(const QString &name, bool loadIfNotFound)
{
	FUNC_LOG;
    QGraphicsWidget *widget = 0;
    
    // 1. check from hash
    if (mObjects.contains(name))
    {
        widget = qobject_cast<QGraphicsWidget*>(mObjects.value(name));
    }

    // 2. load from document and initialize
    else if (loadIfNotFound)
    {
        // find object from queue
        int index = indexInQueue(name);
        if (index != -1)
        {
            // object found from queue, load and initialize object 
            const VideoCollectionUiLoaderParam &param = mQueue.at(index);
            widget = qobject_cast<QGraphicsWidget*>(executeRequest(param));
            
            // object in loading queue, remove it
            removeFromQueue(name);
        }
        else
        {
            // assuming that docml and section has already been loaded
            widget = HbDocumentLoader::findWidget(name);
        }
    }

    return widget;
}

// ---------------------------------------------------------------------------
// doFindObject
// ---------------------------------------------------------------------------
//
QObject* VideoCollectionUiLoader::doFindObject(const QString &name, bool loadIfNotFound)
{
	FUNC_LOG;
    QObject *object = 0;
    
    // 1. check from hash
    if (mObjects.contains(name))
    {
        object = mObjects.value(name);
    }

    // 2. load from document and initialize
    else if (loadIfNotFound)
    {
        // find object from queue
        int index = indexInQueue(name);
        if (index != -1)
        {
            // object found from queue, load and initialize object 
            const VideoCollectionUiLoaderParam &param = mQueue.at(index);
            object = executeRequest(param);
            
            // object in loading queue, remove it
            removeFromQueue(name);
        }
        else
        {
            // assuming that docml and section has already been loaded
            object = HbDocumentLoader::findObject(name);
        }
    }

    return object;
}

// ---------------------------------------------------------------------------
// addToQueue
// ---------------------------------------------------------------------------
//
void VideoCollectionUiLoader::addToQueue(VideoCollectionUiLoaderParam &param,
    QObject *receiver,
    const char *slot)
{
	FUNC_LOG;

	INFOQSTR_1("VideoCollectionUiLoader::addToQueue() name: %S", param.mName);
	
    // set receiver if not already set
    if (!param.mReceiver)
    {
        param.mReceiver = receiver;
    }
    
    // set slot if not already set
    if (!param.mMember)
    {
        param.mMember = slot;        
    }
    
    // check validity and hit it
    if (isValid(param))
    {
        // add the params in async queue
        mQueue.append(param);
    }
}

// ---------------------------------------------------------------------------
// getObject
// ---------------------------------------------------------------------------
//
QObject* VideoCollectionUiLoader::getObject(
    const VideoCollectionUiLoaderParam &param)
{
	FUNC_LOG;
    QObject *object = 0;
    
    if (param.mIsWidget)
    {
        object = HbDocumentLoader::findWidget(param.mName);
    }
    else
    {
        object = HbDocumentLoader::findObject(param.mName);
    }
    
    return object;
}

// ---------------------------------------------------------------------------
// prepareDocmlAndSection
// ---------------------------------------------------------------------------
//
bool VideoCollectionUiLoader::prepareDocmlAndSection(const char *docml,
    const char *section)
{
	FUNC_LOG;
    bool ok = true;

    // prepare docml
    if (docml && !mDocmls.contains(docml))
    {
        load(docml, &ok);
    }
        
    // prepare section
    if (ok && section && !mSections.contains(section))
    {
        load(docml, section, &ok);
    }
    
    return ok;
}

// ---------------------------------------------------------------------------
// initObject
// ---------------------------------------------------------------------------
//
void VideoCollectionUiLoader::initObject(QObject *object,
    const QString &name)
{
	FUNC_LOG;
    if (object)
    {
        INFOQSTR_1("VideoCollectionUiLoader::initObject() name: %S", name);
        VideoCollectionWrapper &wrapper = VideoCollectionWrapper::instance();
        if (name.compare(DOCML_NAME_VC_VIDEOLISTWIDGET) == 0)
        {
            VideoListWidget *videoList = qobject_cast<VideoListWidget*>(object);
            if (videoList)
            {
                VideoProxyModelGeneric *model = wrapper.getAllVideosModel();
                if(model)
                {
                	VideoCollectionViewUtils::sortModel(model, true, VideoCollectionCommon::ELevelVideos);
                	
                    // init widget
                    videoList->initialize(*model, mIsService, VideoCollectionCommon::ELevelVideos);
                }
            }
        }
        else if (name.compare(DOCML_NAME_VC_COLLECTIONWIDGET) == 0)
        {
            VideoProxyModelGeneric *model = wrapper.getCollectionsModel();
            if (model)
            {
                VideoCollectionViewUtils::sortModel(model, true, VideoCollectionCommon::ELevelCategory);
                
                // initialize video collection widget
                VideoListWidget *videoList =
                    qobject_cast<VideoListWidget*>(object);
                if (videoList)
                {
                    // init widget
                    videoList->initialize(*model, mIsService, VideoCollectionCommon::ELevelCategory);
                }
            }
        }
        else if (name.compare(DOCML_NAME_VC_COLLECTIONCONTENTWIDGET) == 0)
        {
            VideoProxyModelGeneric *model = wrapper.getCollectionContentModel();
            if (model)
            {
                // collection content contains always a list of videos so we use 
                // ELevelVideos as target for sorting
                VideoCollectionViewUtils::sortModel(model, false, VideoCollectionCommon::ELevelVideos);
                
                VideoListWidget *videoList = qobject_cast<VideoListWidget*>(object);
                if (videoList)
                {
                    // init widget
                    // cannot set level at this point yet: it can be either default or user defined collection
                    videoList->initialize(*model, mIsService);
                }
            }
        }
        else if (name.compare(DOCML_NAME_DIALOG) == 0)
        {
            // by default, initialize the selection dialog to delete mode
            VideoListSelectionDialog *dialog =
                qobject_cast<VideoListSelectionDialog*>(object);
            if (dialog)
            {
                dialog->setupContent(VideoListSelectionDialog::EDeleteVideos,
                    TMPXItemId::InvalidId());
            }
        }
        else if (name.compare(DOCML_NAME_SORT_MENU) == 0)
        {
            HbMenu *menu = qobject_cast<HbMenu*>(object);
            if (menu)
            {
                // create sort by menu action
                mMenuActions[EActionSortBy] = menu->menuAction();

                // ensure that all the actions related to sort menu are loaded
                // when sort menu is loaded
                findObject<HbAction>(DOCML_NAME_SORT_BY_DATE);
                findObject<HbAction>(DOCML_NAME_SORT_BY_NAME);
                findObject<HbAction>(DOCML_NAME_SORT_BY_NUMBER_OF_ITEMS);
                findObject<HbAction>(DOCML_NAME_SORT_BY_SIZE);

                // add sub menu actions
                if (!mSortGroup)
                {
                    mSortGroup = new QActionGroup(menu);
                    mSortGroup->addAction(mMenuActions[EActionSortByDate]);
                    mSortGroup->addAction(mMenuActions[EActionSortByName]);
                    mSortGroup->addAction(mMenuActions[EACtionSortByItemCount]);
                    mSortGroup->addAction(mMenuActions[EActionSortBySize]);
                    
                    // set all sub menu items checkable
                    foreach (QAction *action, menu->actions()) 
                    {
                        action->setCheckable(true);
                    }
                }
            }
        }
        else if (name.compare(DOCML_NAME_OPTIONS_MENU) == 0)
        {
            // ensure that all the actions related to options menu are loaded
            // when options menu is loaded
            // when applicaton has been launched as a service,
            // do not load components which are not required
            if(!mIsService)
            {
                findObject<HbAction>(DOCML_NAME_ADD_TO_COLLECTION);
                findObject<HbAction>(DOCML_NAME_CREATE_COLLECTION);
                findObject<HbAction>(DOCML_NAME_DELETE_MULTIPLE);
            }
        }
        else if (name.compare(DOCML_NAME_ADD_TO_COLLECTION) == 0)
        {
            HbAction *action = qobject_cast<HbAction*>(object);
            if (action)
            {
                mMenuActions[EActionAddToCollection] = action;
            }
        }
        else if (name.compare(DOCML_NAME_CREATE_COLLECTION) == 0)
        {
            HbAction *action = qobject_cast<HbAction*>(object);
            if (action)
            {
                mMenuActions[EActionNewCollection] = action;
            }
        }
        else if (name.compare(DOCML_NAME_DELETE_MULTIPLE) == 0)
        {
            HbAction *action = qobject_cast<HbAction*>(object);
            if (action)
            {
                mMenuActions[EActionDelete] = action;
            }
        }
        else if (name.compare(DOCML_NAME_SORT_BY_DATE) == 0)
        {
            HbAction *action = qobject_cast<HbAction*>(object);
            if (action)
            {
                mMenuActions[EActionSortByDate] = action;
            }
        }
        else if (name.compare(DOCML_NAME_SORT_BY_NAME) == 0)
        {
            HbAction *action = qobject_cast<HbAction*>(object);
            if (action)
            {
                mMenuActions[EActionSortByName] = action;
            }
        }
        else if (name.compare(DOCML_NAME_SORT_BY_NUMBER_OF_ITEMS) == 0)
        {
            HbAction *action = qobject_cast<HbAction*>(object);
            if (action)
            {
                mMenuActions[EACtionSortByItemCount] = action;
            }
        }
        else if (name.compare(DOCML_NAME_SORT_BY_SIZE) == 0)
        {
            HbAction *action = qobject_cast<HbAction*>(object);
            if (action)
            {
                mMenuActions[EActionSortBySize] = action;
            }
        }
    }
}

// ---------------------------------------------------------------------------
// executeRequest
// ---------------------------------------------------------------------------
//
QObject* VideoCollectionUiLoader::executeRequest(
    const VideoCollectionUiLoaderParam &param)
{
	FUNC_LOG;
    QObject *object = getObject(param);
    if (!object)
    {
        // object was not found, try preparing docml and section
        bool ok = prepareDocmlAndSection(param.mDocml, param.mSection);
        if (ok)
        {
            // try to get the object again
            object = getObject(param);
        }
    }
    
    // initialize object if it was found from docml
    if (object)
    {
        if (!mObjects.contains(param.mName))
        {
            // initialize object
            initObject(object, param.mName);

            // insert object in hash table
            mObjects.insert(param.mName, object);
        }
        
        bool ok = connect(
            this, SIGNAL(objectReady(QObject*, const QString&)),
            param.mReceiver, param.mMember);
        if (ok)
        {
            // signal client and disconnect
            emit objectReady(object, param.mName);        
            disconnect(
                this, SIGNAL(objectReady(QObject*, const QString&)),
                param.mReceiver, param.mMember);
        }
    }    
    
    return object;
}

// ---------------------------------------------------------------------------
// indexInQueue
// ---------------------------------------------------------------------------
//
int VideoCollectionUiLoader::indexInQueue(const QString &name)
{
	FUNC_LOG;
    int index = -1;
    
    int count = mQueue.count();
    for (int i = 0; i < count; i++)
    {
        const VideoCollectionUiLoaderParam &param = mQueue.at(i);
        if (param.mName == name)
        {
            // found from queue
            index = i;
            break;
        }
    }
    
    return index;
}

// ---------------------------------------------------------------------------
// removeFromQueue
// ---------------------------------------------------------------------------
//
void VideoCollectionUiLoader::removeFromQueue(const QString &name)
{
	FUNC_LOG;
    int count = mQueue.count();
    for (int i = 0; i < count; i++)
    {
        const VideoCollectionUiLoaderParam &param = mQueue.at(i);
        if (param.mName == name)
        {
            mQueue.removeAt(i);
            break;
        }
    }
}

// ---------------------------------------------------------------------------
// timerEvent
// ---------------------------------------------------------------------------
//
void VideoCollectionUiLoader::timerEvent(QTimerEvent *event)
{
	FUNC_LOG;
    if (event)
    {
        if (event->timerId() == mTimerId)
        {
            // get current request from queue and execute it
            int count = mQueue.count();
            for (int i = 0; i < count; i++)
            {
                const VideoCollectionUiLoaderParam &param = mQueue.at(i);
                if (mPhases.contains(param.mPhase))
                {
                    // load and initialize
                    executeRequest(param);
                                
                    // remove the current request from queue and run next
                    removeFromQueue(param.mName);
                    break;
                }
            }
            runNext();
        }
    }
}

// ---------------------------------------------------------------------------
// createObject
// ---------------------------------------------------------------------------
//
QObject* VideoCollectionUiLoader::createObject( const QString& type, const QString &name )
{
	FUNC_LOG;
    QObject* object = 0;

    if ( type == VideoListView::staticMetaObject.className() )
    {
        object = new VideoListView(this);
    }
    else if ( type == VideoListSelectionDialog::staticMetaObject.className() )
    {
        object = new VideoListSelectionDialog(this);
    }
    else if ( type == VideoListWidget::staticMetaObject.className() )
    {
        object = new VideoListWidget(this);
    }
    if ( object )
    {
        object->setObjectName( name );
        return object;
    }

    return HbDocumentLoader::createObject( type, name );
}

// ---------------------------------------------------------------------------
// runNext
// ---------------------------------------------------------------------------
//
void VideoCollectionUiLoader::runNext()
{
	FUNC_LOG;
    bool runNext = false;
    int count = mQueue.count();
    for (int i = 0; i < count; i++)
    {
        const VideoCollectionUiLoaderParam &param = mQueue.at(i);
        if (mPhases.contains(param.mPhase))
        {
            runNext = true;
            break;
        }
    }
    
    if (runNext)
    {
        if (!mTimerId)
        {
            // timer not running, start it
            mTimerId = startTimer(ASYNC_FIND_TIMEOUT);
        }
    }
    else
    {
        // no new requests, kill timer
        if (mTimerId)
        {
            killTimer(mTimerId);
            mTimerId = 0;
        }
    }
}

// ---------------------------------------------------------------------------
// isValid
// ---------------------------------------------------------------------------
//
bool VideoCollectionUiLoader::isValid(const VideoCollectionUiLoaderParam &param)
{
    bool valid = true;
    
    if (param.mName.length() &&
        param.mDocml &&
        param.mMember &&
        param.mReceiver)
    {
        // check if the param is already in the queue
        int count = mQueue.count();
        for (int i = 0; i < count; i++)
        {
            if (mQueue.at(i) == param)
            {
                INFO("VideoCollectionUiLoader::isValid() already in queue.");
                valid = false;
                break;
            }
        }
        
        // check that the item has not already been loaded
        if (valid)
        {
            valid = !mObjects.contains(param.mName);
        }
    }
    else
    {
        INFO("VideoCollectionUiLoader::isValid() params missing.");
        valid = false;
    }
    
    return valid;
}

// ---------------------------------------------------------------------------
// storeOrphans
// ---------------------------------------------------------------------------
//
void VideoCollectionUiLoader::storeOrphans(const QObjectList &list)
{
	FUNC_LOG;
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

