

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
* Description:  stubclass of videodeleteworker for testing VideoListDataModel class methods*
*/

#ifndef __VIDEODELETEWORKER_H__
#define __VIDEODELETEWORKER_H__

#include <QObject>
#include <QList>
#include <QVariant>
#include <mpxitemid.h>
#include "testvideolistdatamodel.h"

class VideoCollectionClient;

class VideoDeleteWorker : public QObject
{    
    /**
     * define to be able to use signals and slots
     */
    Q_OBJECT
    
    /**
     * disable copy-constructor and assignment operator
     */
    Q_DISABLE_COPY(VideoDeleteWorker) 
    
    /**
     * test class as frient for it to be able all members
     */
    friend class TestVideoListDataModel;
    
public: 

    /**
     * Default constructor
     */
    VideoDeleteWorker(VideoCollectionClient &collection, QObject *parent = 0);  
    
    /**
     * Destructor
     */
    ~VideoDeleteWorker();
    
    /**
     * Appneds privided list into mRequestList
     * 
     * @param itemList
     */
    void requestDelete(const QList<TMPXItemId> &itemList);
    
    /**
     * returns true in case there are items at the delete buffer 
     * 
     * @return bool
     */
    bool isDeleting();
    
    /**
     * Updates error status
     * 
     * @param status value to update
     * @data data to update
     */
    void updateStatus(int status, QVariant data);
    
    /**
     * returns latest status and data
     * 
     * @param data to return
     * 
     * @return status code
     */
    int getLastStatus(QVariant &data);
    
    /**
     * invalidates status and status data
     */
    void clearStatus();

signals:

    /**
     * dummy signal
     */
    void deleteStartupFailed(QList<TMPXItemId>&);
    
    
public slots:
    /**
     * dummy slot
     */
    void continueSlot(){};
    
public:
    /**
     * list items to be appended
     */
    QList<TMPXItemId> mRequestList;
    
    /**
     * returned from isDeleting
     */
    bool mIsDeleting;
    
    /**
     * setted by updateStatus
     */
    int mStatus;
    
    /**
     * setted by updateStatus
     */
    QVariant mStatusData;
    
    /**
     * count of constructor calls
     */
    static int mCreateCount;
    
};

#endif // __VIDEODELETEWORKER_H__
