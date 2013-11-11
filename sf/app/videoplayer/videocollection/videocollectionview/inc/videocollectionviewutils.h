/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies). 
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

#ifndef __VIDEOCOLLECTIONVIEWUTILS_H__
#define __VIDEOCOLLECTIONVIEWUTILS_H__

#include <qobject.h>
#include <qvariant.h>
#include "videocollectioncommon.h"

class HbListView;
class VideoProxyModelGeneric;

class VideoCollectionViewUtils : public QObject
{
    Q_OBJECT
    
public:
    /**
     * Returns singleton instance for this class.
     * 
     * WARNING! Not safe to call this from destructor of another function scope static object!
     * 
     * @return The singleton instance.
     */
    static VideoCollectionViewUtils& instance();
    
    /**
     * Saves the sorting role and order to cenrep.
     * 
     * @param role The sorting role.
     * @param order The sorting order (e.g. ascending or descending).
     * @return Zero if operation succeeded, less than zero in error cases.
     */
    int saveSortingValues(int role, Qt::SortOrder order, VideoCollectionCommon::TCollectionLevels target);
    
    /**
     * Loads the sorting role and order from cenrep.
     * 
     * @param role On return contains the sorting role.
     * @param order On return contains the sorting order
     * @return Zero if operation succeeded, less than zero in error cases.
     */
    int loadSortingValues(int& role, Qt::SortOrder& order, VideoCollectionCommon::TCollectionLevels target);
    
    /**
     * Get string value for a key from central repository.
     * 
     * @param key Video collection central repository key.
     * @return String stored in central repository. Empty string in error cases.
     */
    QString getCenRepStringValue(int key);
    
    /**
     * Get int value for a key from central repository.
     * 
     * @param key Video collection central repository key.
     * @param value Contains the read integer from cenrep at return.  
     * @return Application UID if operation succeeded, less than zero in error cases.
     */
    int getCenRepIntValue(int key);
    
    /**
     * Set video services in use.
     */
    void setIsService();

    /**
     * Get video services in use.
     */
    bool isService();

public:
    /**
     * Initializes list view for collection and selection dialog with common
     * properties.
     */
    static void initListView(HbListView *view);
    
    /**
     * Initilizes model sort values.
     */
    static void sortModel(VideoProxyModelGeneric *model, bool async, VideoCollectionCommon::TCollectionLevels target);
    
    /**
     * Method saves the latest videolist widget level into local activity manager 
     * data container.
     * 
     * Note that collection id will be cleared in activity data in case level is
     * all videos or collection list. 
     * 
     * @param level collection level
     */
    static void setWidgetActivityLevel(const VideoCollectionCommon::TCollectionLevels &level);
   
    /**
     * Method gets the latest videolist widget level from local activity manager 
     * data container.
     * 
     * @param level, reference to level variable where value is saved
     */
    static void getActivityWidgetLevel(VideoCollectionCommon::TCollectionLevels &level);
    
    /**
     * method saves provided id and string into local activity
     * manager data container.
     * 
     * @param id. Id to save
     */
    static void setCollectionActivityData(const TMPXItemId &id, const QString &name);
    
    /**
     * method reads id and string from from local activity manager data 
     * container and prepares MPX item id.
     * 
     * @param id. Id to return
     */
    static void getCollectionActivityData(TMPXItemId &id, QString &name);

public slots:
    
    /**
     * Shows status msgs
     *
     * @param statusCode status code
     * @param additional additional data gotten from the status 
     */
    void showStatusMsgSlot(int statusCode, QVariant &additional);
   
private:
    
    /**
     * disables copy-constructor and assingment operator
     */
    Q_DISABLE_COPY(VideoCollectionViewUtils)
    
    /**
     * constructor
     */
    VideoCollectionViewUtils();
     
    /**
     * destructor
     */
    virtual ~VideoCollectionViewUtils();

private:

    /** is service */
    bool mIsService;
    
    /** current sorting role */
    int mVideosSortRole;
    int mCollectionsSortRole;
    
    /** current sorting order */
    Qt::SortOrder mVideosSortOrder;
    Qt::SortOrder mCollectionsSortOrder;
};

#endif //__VIDEOCOLLECTIONUIUTILS_H__

