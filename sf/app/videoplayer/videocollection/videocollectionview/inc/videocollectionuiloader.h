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

#ifndef _VIDEOCOLLECTIONUILOADER_H_
#define _VIDEOCOLLECTIONUILOADER_H_

// System includes
#include <hbdocumentloader.h>
#include <qhash.h>
#include <qmap.h>
#include <qset.h>

#include "videocollectionuiloaderdef.h"

// Forward declarations
class QActionGroup;
class HbAction;

// Class declaration
class VideoCollectionUiLoader:
    public QObject,
    public HbDocumentLoader
{
    Q_OBJECT
    
public:
    /** actions used in menus and toolbars */
    enum ActionIds
    {
        EActionSortBy          =  1,
        EActionSortByDate,
        EActionSortByName,
        EACtionSortByItemCount,
        EActionSortBySize,
        EActionNewCollection,
        EActionAddToCollection,
        EActionDelete,
        ETBActionAllVideos,
        ETBActionCollections,
        ETBActionServices,
        ETBActionAddVideos,
        ETBActionRemoveVideos
    };
    
public:
    /**
     * C++ constructor.
     */
    VideoCollectionUiLoader();
    
    /**
     * C++ destructor.
     */
    virtual ~VideoCollectionUiLoader();
    
    /**
     * Adds data to ui loader loading queue.
     */
    void addData(QList<VideoCollectionUiLoaderParam> params,
        QObject *receiver,
        const char *slot);
    
    /**
     * Returns the requested widget casted to correct type
     *
     * @param name Name of the widget
     * @param loadIfNotFound Should the widget be loaded if not found in cache.
     * @return Pointer to the widget
     */
    template<class T>
    T* findWidget(const QString &name, bool loadIfNotFound = true)
    {
        return qobject_cast<T*>(doFindWidget(name, loadIfNotFound));
    }

    /**
     * Returns the requested object casted to correct type
     *
     * @param name Name of the object
     * @param loadIfNotFound Should the widget be loaded if not found in cache.
     * @return Pointer to the object
     */
    template<class T>
    T* findObject(const QString &name, bool loadIfNotFound = true)
    {
        return qobject_cast<T*>(doFindObject(name, loadIfNotFound));
    }
    
    /**
     * Set video services in use.
     */
    void setIsService(bool isService);
    
    /**
     * load
     */
    void load(const QString &fileName, bool *ok = 0);

    /**
     * load
     */
    void load(const QString &fileName, const QString &section , bool *ok = 0);
    
public slots:
    /**
     * Starts loading ui components belonging to the defined phase.
     */
    void loadPhase(int loadPhase);
    
private slots:
    /**
     * Remove object from list since it has been already deleted.
     */
    void removeOrphanFromList(QObject *object);
    
signals:
    /**
     * Signals that object has been loaded asynchonously.
     * 
     * @param object, Object which was loaded.
     * @param name, Name of the object in document.
     * @return None.
     */
    void objectReady(QObject *object, const QString &name);
    
protected:

    /**
     * Loads widget from document.
     * 
     * @param name, Widget name.
     * @param loadIfNotFound Should the widget be loaded if not found in cache.
     * @return QGraphicsWidget*.
     */
    QGraphicsWidget* doFindWidget(const QString &name, bool loadIfNotFound);
    
    /**
     * Loads object from document.
     * 
     * @param name, Object name.
     * @return QObject*.
     */
    QObject* doFindObject(const QString &name, bool loadIfNotFound);
    
    /**
     * Adds a ui section to async loading queue.
     */
    void addToQueue(VideoCollectionUiLoaderParam &param,
        QObject *receiver,
        const char *slot);
    
    /**
     * Finds an object or widget from docml based on the parameter.
     */
    QObject* getObject(const VideoCollectionUiLoaderParam &param);
    
    /**
     * Loads and prepares docml and sections if found from param.
     */
    bool prepareDocmlAndSection(const char *docml, const char *section);
    
    /**
     * Init a specific object.
     */
    void initObject(QObject *object, const QString& name);
    
    /**
     * Execute ui loader request.
     */
    QObject* executeRequest(const VideoCollectionUiLoaderParam &param);
    
    /**
     * Gets index of the item in queue, if one found.
     */
    int indexInQueue(const QString &name);
    
    /**
     * Removes request from queue.
     */
    void removeFromQueue(const QString &name);
    
private:
    /** from QObject */
    void timerEvent(QTimerEvent *event); 
    
    /** from HbDocumentLoader */
    QObject *createObject(const QString& type, const QString &name);
    
private:
    /**
     * Run next async find request from queue.
     */
    void runNext();
    
    /**
     * Check that set params are valid.
     */
    bool isValid(const VideoCollectionUiLoaderParam &param);
    
    /**
     * Store object without a parent. 
     */
    void storeOrphans(const QObjectList &list);
    
private:
    /** async queue */
    QList<VideoCollectionUiLoaderParam> mQueue;
    
    /** timer id */
    int mTimerId;
    
    /** list of initialized objects */
    QHash<QString, QObject*> mObjects;
    
    /** menu actions */
    QMap<ActionIds, HbAction*> mMenuActions;
    
    /** toolbar actions */
    QMap<ActionIds, HbAction*> mToolbarActions;
    
    /** loaded docmls */
    QSet<QString> mDocmls;
    
    /** loaded sections */
    QSet<QString> mSections;
    
    /** action group for "sort by" actions */
    QActionGroup* mSortGroup;
    
    /** is service */
    bool mIsService;
    
    /** load phases currently active */
    QSet<int> mPhases;
    
    /**
     * objects without a parent - these needs to be deleted manually unless
     * a parent is set.
     */
    QObjectList mOrphans;
};

#endif // _VIDEOCOLLECTIONUILOADER_H_
