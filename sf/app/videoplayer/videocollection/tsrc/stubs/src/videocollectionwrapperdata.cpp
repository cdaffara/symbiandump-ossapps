/*
* Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:   VideoCollectionWrapperData class implementation
* 
*/

#include "videocollectionwrapperdata.h"

bool VideoCollectionWrapperData::mGetGenericModelFails = false;
bool VideoCollectionWrapperData::mGetAllVideosModelFails = false;
bool VideoCollectionWrapperData::mGetCollectionsModelFails = false;
bool VideoCollectionWrapperData::mGetCollectionContentModelFails = false;
VideoListDataModel *VideoCollectionWrapperData::mSourceModel = 0;
QPointer<VideoProxyModelGeneric> VideoCollectionWrapperData::mAllVideosModel = 0;
QPointer<VideoProxyModelGeneric> VideoCollectionWrapperData::mCollectionsModel = 0;
QPointer<VideoProxyModelGeneric> VideoCollectionWrapperData::mCollectionContentModel = 0;
QPointer<VideoProxyModelGeneric> VideoCollectionWrapperData::mGenericModel = 0;

void VideoCollectionWrapperData::reset()
{
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
}

