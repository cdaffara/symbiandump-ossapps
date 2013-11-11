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
* Description:   VideoCollectionWrapper class definition
* 
*/

#ifndef __VIDEOCOLLECTIONWRAPPER_H__
#define __VIDEOCOLLECTIONWRAPPER_H__


// INCLUDES
#include <QObject>
#include <qabstractitemmodel.h>
#include "videocollectioncommon.h"
#include "videocollectionexport.h"

// FORWARD DECLARATIONS
class VideoCollectionWrapperPrivate;
class VideoProxyModelGeneric;


/**
 * Class is used as an interface of the video collection QT wrapper.
 * 
 * Singleton instance is deleted when application ends
 * 
 * * Usage:
 *  @code
 *  #include "videocollectioncommon.h"
 *  #include "videocollectionwrapper.h"
 *  #include "videoproxymodelgeneric.h"
 *  
 *  ...
 *  ////
 *  // Getting the instances
 *  ////
 *  VideoCollectionWrapper &wrapper = VideoCollectionWrapper::instance();
 *  // getting all videos model
 *  VideoProxyModelGeneric *model = wrapper.getAllVideosModel();
 *  ...
 *  ////
 *  // Opening collection and start fetching video item data
 *  ////
 *  if(model)
 *  {
 *      mModel.open(VideoCollectionCommon::ELevelVideos);
 *  }
 *  // see model documentation for the open funtionality
 * 
 *  @endcode
 * 
 */
class VIDEOCOLLECTION_DLL_EXPORT VideoCollectionWrapper : public QObject         
{    
    /**
     * define to be able to use signals and slots
     */
    Q_OBJECT
    
public: // Constructor
    
    /**
     * Returns singleton instance for this class.
     * 
     * WARNING! Not safe to call this from destructor of another function scope static object!
     * 
     * @return The singleton instance.
     */
    static VideoCollectionWrapper &instance();  
    
	/**
     * Returns pointer to generic video model. Null if creation fails or if
     * application is closing.
     * 
     * @return address to model or NULL if fails or if application is closing.
     */    
    VideoProxyModelGeneric* getGenericModel();

    /**
     * Returns pointer to all videos model. Null if creation fails or if
     * application is closing.
     * 
     * @return address to model or NULL if fails or if application is closing.
     */    
    VideoProxyModelGeneric* getAllVideosModel();

    /**
     * Returns pointer to collections model. Null if creation fails or if
     * application is closing.
     * 
     * @return address to model or NULL if fails or if application is closing.
     */    
    VideoProxyModelGeneric* getCollectionsModel();
    
    /**
     * Returns pointer to collection content model. Null if creation fails or if
     * application is closing.
     * 
     * @return address to model or NULL if fails or if application is closing.
     */    
    VideoProxyModelGeneric* getCollectionContentModel();
    
    /**
     * Method can be used by client to emit status signal
     * containing status code from particular async status.
     * 
     * @param statusCode code of status
     * 
     * @param additional additional information for the code
     */
    void sendAsyncStatus(int statusCode, QVariant &additional);
     
signals:     

    /**
     * Signal that can be emitted by the wrapper to indicate status
     * of some async operation.
     * 
     * See videocollectioncommon.h for codes
     * 
     * @param status code of status
     * 
     * @param additional additional information for the code
     */
    void asyncStatus(int statusCode, QVariant &additional);

private:
    
    /**
     * Private contructor.
     */    
    VideoCollectionWrapper(); 
    
    /**
     * Private destructor.
     *
     */
    virtual ~VideoCollectionWrapper();    
    
    /**
     * disables copy-constructor and assingment operator
     */
    Q_DISABLE_COPY(VideoCollectionWrapper)    
    
private:

	/**
     * d -pointer for actual implementation
     */ 
    VideoCollectionWrapperPrivate *d;

    /**
     * Reference count.
     */
    int mReferenceCount;
    
};

#endif  // __VIDEOCOLLECTIONWRAPPER_H__

// End of file
