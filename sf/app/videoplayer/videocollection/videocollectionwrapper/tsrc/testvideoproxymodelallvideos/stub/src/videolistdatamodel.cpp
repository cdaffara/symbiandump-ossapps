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
* Description: Stub model to be used when unit testing the proxy model. 
* 
*/


// INCLUDES
#include "videolistdatamodel.h"

TMPXItemId VideoListDataModel::mMediaIdAtIndexReturnValue;
int VideoListDataModel::mMediaIdAtIndexCallCount = 0;
int VideoListDataModel::mRowCountReturnValue = 0;

// ---------------------------------------------------------------------------
// VideoListDataModel
// ---------------------------------------------------------------------------
//
VideoListDataModel::VideoListDataModel(QObject *parent)
{
    Q_UNUSED(parent);
}

// ---------------------------------------------------------------------------
// ~VideoListDataModel
// ---------------------------------------------------------------------------
//
VideoListDataModel::~VideoListDataModel()
{

}

// ---------------------------------------------------------------------------
// mediaIdAtIndex
// ---------------------------------------------------------------------------
//
TMPXItemId VideoListDataModel::mediaIdAtIndex(int index)
{
    Q_UNUSED(index);
    mMediaIdAtIndexCallCount++;
    return mMediaIdAtIndexReturnValue;
}

// ---------------------------------------------------------------------------
// rowCount
// ---------------------------------------------------------------------------
//
const int VideoListDataModel::rowCount() const
{
    return mRowCountReturnValue;
}


// End of file
    


