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
* Description: stub videocollectionwrapperdata
* 
*/

#ifndef VIDEOCOLLECTIONWRAPPERDATA_H
#define VIDEOCOLLECTIONWRAPPERDATA_H

#include "videoproxymodelgeneric.h"
#include "videoproxymodelallvideos.h"
#include "videoproxymodelcollections.h"
#include "videoproxymodelcontent.h"
#include "videolistdatamodel.h"
#include <qpointer.h>

class VideoCollectionWrapperData : public QObject
{
    Q_OBJECT 
    
public: // methods
    static void reset();
    /*{
        mGetGenericModelFails = false;
        mGetAllVideosModelFails = false;
        mGetCollectionsModelFails = false;
        mGetCollectionContentModelFails = false;
        
        delete mAllVideosModel;
        delete mCollectionsModel;
        delete mCollectionContentModel;
        delete mGenericModel;

        delete mSourceModel;
        mSourceModel = 0;        
    }*/
    
public: // data
    static bool mGetGenericModelFails;
    static bool mGetAllVideosModelFails;
    static bool mGetCollectionsModelFails;
    static bool mGetCollectionContentModelFails;
    static VideoListDataModel *mSourceModel;
    static QPointer<VideoProxyModelGeneric> mAllVideosModel;
    static QPointer<VideoProxyModelGeneric> mCollectionsModel;
    static QPointer<VideoProxyModelGeneric> mCollectionContentModel;
    static QPointer<VideoProxyModelGeneric> mGenericModel;
};

#endif /* VIDEOCOLLECTIONWRAPPERDATA_H */
