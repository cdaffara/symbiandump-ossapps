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
* Description: VideoCollectionWrapperPrivate class definition
* 
*/

#ifndef __VIDEOCOLLECTIONWRAPPERPRIVATE_H__
#define __VIDEOCOLLECTIONWRAPPERPRIVATE_H__


// INCLUDES
#include <QObject>
#include <QPointer>
#include <qabstractitemmodel.h>
#include "videocollectioncommon.h"

// FORWARD DECLARATIONS
class CMPXMediaArray;
class CMPXMedia;
class VideoListDataModel;
class VideoProxyModelGeneric;
class VideoProxyModelAllVideos;
class VideoProxyModelCollections;
class VideoProxyModelContent;

class VideoCollectionWrapperPrivate : public QObject        
{    
	Q_DISABLE_COPY(VideoCollectionWrapperPrivate) 
	
	/**
     * define to be able to use signals and slots
     */
    Q_OBJECT
	
public: // Constructor

	/**
	* Default constructor
    */
	VideoCollectionWrapperPrivate();	
	
	/**
	 * Destructor
	 */
	~VideoCollectionWrapperPrivate();	
    
    /**
     * Returns the pointer into generic video model. Creates the model if it doesn't 
     * exist yet.
     * 
     * Noter that if application has signaled aboutToQuit -signal indicating closing, all
     * previously created models have been removed and new ones will not be created 
     * anymore
     *
     * @return address of model, NULL if creation did not succeed or if application is closing.
     */    
    VideoProxyModelGeneric* getGenericModel();

    /**
     * Returns the pointer into all videos model. Creates the model if it doesn't exist yet.
     * 
     * Noter that if application has signaled aboutToQuit -signal indicating closing, all
     * previously created models have been removed and new ones will not be created 
     * anymore
     *
     * @return address of model, NULL if creation did not succeed or if application is closing.
     */    
 
    VideoProxyModelGeneric* getAllVideosModel();

    /**
     * Returns the pointer into collections model. Creates the model if it doesn't exist yet.
     * 
     * Noter that if application has signaled aboutToQuit -signal indicating closing, all
     * previously created models have been removed and new ones will not be created 
     * anymore
     *
     * @return address of model, NULL if creation did not succeed or if application is closing.
     */    
 
    VideoProxyModelGeneric* getCollectionsModel();
    
    /**
     * Returns the pointer into collection content model. Creates the model if it doesn't 
     * exist yet.
     * 
     * Noter that if application has signaled aboutToQuit -signal indicating closing, all
     * previously created models have been removed and new ones will not be created 
     * anymore
     * 
     * @return address of model, NULL if creation did not succeed or if application is closing.
     */     
    VideoProxyModelGeneric* getCollectionContentModel();

private slots:

    /**
     * Signaled when UI environment is about to be destroyed. 
     * All models needs to be cleaned up before of that.
     * 
     */
    void aboutToQuitSlot();
    
private:
    
    /**
     * Creates proxy model and returns the pointer.
     * 
     * Noter that if application has signaled aboutToQuit -signal indicating closing, all
     * previously created models have been removed and new ones will not be created 
     * anymore.
     * 
     * @return address of model, NULL if creation did not succeed or if application is closing.
     */
    template<class T>
    T *initProxyModelModel()
    {
        if(mAboutToClose)
        {
            return 0;
        }
        
        if(!initSourceModel())
        {
            return 0;
        }
        
        T *model = 0;

        model = new T();
            
        if(model->initialize(mSourceModel) || 
           !connect(model, SIGNAL(shortDetailsReady(TMPXItemId)), 
                   mSourceModel, SIGNAL(shortDetailsReady(TMPXItemId))))
        {
            delete model;
            model = 0;
        }
        
        return model;
    }
    
private:

    /**
     * Initializes source model.
     * 
     * @return true if initialization succeeds, otherwise false.
     */
    bool initSourceModel();
    
private:
    
    /**
     * data model for collection content
     */
    QPointer<VideoProxyModelGeneric> mGenericModel;
    
	/**
     * data model for all videos
     */
	QPointer<VideoProxyModelAllVideos> mAllVideosModel;
	
	/**
	 * data model for collections
	 */
	QPointer<VideoProxyModelCollections> mCollectionsModel;
	
	/**
	 * data model for collection content
	 */
    QPointer<VideoProxyModelContent> mCollectionContentModel;
	
	/**
	 * source model
	 */
	QPointer<VideoListDataModel>     mSourceModel;
	
	/**
	 * flag to indicate, that object is to be deallocated, so no
	 * models are to be returned anymore 
	 */
	bool mAboutToClose;
    
};
#endif  // __VIDEOCOLLECTIONWRAPPERPRIVATE_H__

// End of file
