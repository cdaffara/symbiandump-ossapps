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
* Description: VideoDeleteWorker class definition*
*/

#ifndef __VIDEODELETEWORKER_H__
#define __VIDEODELETEWORKER_H__


// INCLUDES
#include <QObject>
#include <qvariant.h>
#include <qset.h>
#include <qlist.h>
#include <e32const.h>
#include <mpxitemid.h>

// FORWARD DECLARATIONS
class QTimer;
class VideoListDataModelPrivate;
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
	 * Sets items into mRemoveBuffer. And starts deleting of items in 
	 * blocks using mRequestWaitTimer interval
	 * 
	 * @param itemList items to be deleted
	 */
	void requestDelete(const QList<TMPXItemId> &itemList);
	
	/**
	 * Removes item from the delete and pending buffer
	 * 
	 * @param itemId item id to removed
	 * 
	 * @returns a coutn of remaining requests
	 */
	int removeFromRequest(TMPXItemId &itemId);
	
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
	 * @param data data to update
	 * 
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
	 * method resets status and statusdatas
	 */
	void clearStatus();
	
public slots:

	/**
     * Continues deleting in case there are items 
     * yet to be deleted
     */
    void continueSlot();
	
private:
	
	/**
	 * methods dumps all remaining deletes to collection
	 * without caring the result codes.
	 */
	void flushAll();
		
private slots:

    /**
     * creates a block of deleted items and calls collection 
     * to startup deletion
     */
    void execDeleteBlockSlot();
    
    
    
signals:
    
    /**
     * Emitted if some delete startup fails.
     * 
     * @param list of item ids whose deletion does not start
     */
    void deleteStartupFailed(QList<TMPXItemId>&);
	
private:
    
	/**
     * reference to collection client.
     * Not own.
     */
    VideoCollectionClient &mCollectionClient;
	
	/**
	 * buffer for items to be removed 
	 */
	QSet<TMPXItemId> mRemoveBuffer;
	
	/**
	 * timer whose interval is used between delete 
	 * requests to collection
	 */
	QTimer *mRequestWaitTimer;
	
	/**
	 * last error status 
	 */
	int mLastStatus;
	
	/**
	 * last error data 
	 */
	QVariant mLastStatusData;
   
};
#endif  // __VIDEODELETEWORKER_H__

// End of file
    


