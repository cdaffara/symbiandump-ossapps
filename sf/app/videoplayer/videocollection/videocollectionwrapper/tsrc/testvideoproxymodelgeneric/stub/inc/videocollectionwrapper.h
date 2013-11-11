
/**
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
* Description:   Stub videocollectionwrapper class for VideoProxyModelGeneric unit tests
*
*/

#ifndef __VIDEOCOLLECTIONWRAPPER_H__
#define __VIDEOCOLLECTIONWRAPPER_H__

#include <QObject>
//#include "videoproxymodelgeneric.h"
class VideoProxyModelGeneric;

class VideoCollectionWrapper : public QObject
{
    Q_OBJECT
    
public: 
    
    enum TModelType
    {
        EAllVideos,
        ECollections,
        ECollectionContent,
        EGeneric     
    };
    
    static VideoCollectionWrapper &instance()
    {
        static VideoCollectionWrapper _staticWrapper;
        return _staticWrapper;
    }
    
    /**
     * constructor
     */
    VideoCollectionWrapper() :
    mProxyModel(0)
    {        
    }
    
    /**
     * returns mProxyModel
     */
    VideoProxyModelGeneric* getGenericModel()
    {
        return mProxyModel;
    }
    
    /**
     * returns mProxyModel
     */
    VideoProxyModelGeneric* getCollectionsModel()
    {
        return mProxyModel;
    }    
    
    VideoProxyModelGeneric* mProxyModel;
};
#endif // __VIDEOCOLLECTIONWRAPPER_H__
