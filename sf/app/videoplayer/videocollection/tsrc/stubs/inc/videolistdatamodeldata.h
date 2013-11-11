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
* Description: stub videolistdatamodeldata
* 
*/

#ifndef VIDEOLISTDATAMODELDATA_H
#define VIDEOLISTDATAMODELDATA_H

#include "videolistdatamodel.h"

class VideoListDataModelData
{
public: // methods
    static void reset()
    {
        mData.clear();
        mLastIndex = QModelIndex();
        mDataAccessCount = 0;
        mRowCount = 0;
        mRowCountDecrement = 0;
        mItemId = TMPXItemId::InvalidId();
    }
    
    static QModelIndex lastIndex()
    {
        return mLastIndex;
    }
    
    static int dataAccessCount()
    {
        return mDataAccessCount;
    }
    
    static void setData(int role, QVariant data)
    {
        mData.insert(role, data);
    }
    
    static void setRowCount(VideoListDataModel &model, int count)
    {
        if (count != mRowCount)
        {
            mRowCount = count;
            model.initialize();
        }
    }
    
public: // data
    static QMap<int, QVariant> mData;
    static QModelIndex mLastIndex;
    static int mDataAccessCount;
    static int mRowCount;
    static int mRowCountDecrement;
    static TMPXItemId mItemId;
};

#endif /* VIDEOLISTDATAMODELDATA_H */
