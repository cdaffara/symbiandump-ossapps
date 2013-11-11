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
* Description: stub videocollectionviewutilsdata
* 
*/

#ifndef VIDEOCOLLECTIONVIEWUTILSDATA_H
#define VIDEOCOLLECTIONVIEWUTILSDATA_H

#include <qnamespace.h>
#include <qvariant.h>
#include "videocollectioncommon.h"

#define CENREP_NO_STRING "nostring"
#define CENREP_NO_INT 1010100512

class VideoCollectionViewUtilsData
{
public: // methods
    static void reset()
    {
        mLastError = 0;
		mIsServiceValue = false;
        mLoadSortingValuesFails = false;
        mVideoSortRole = -1;
        mCollectionSortRole = -1;
        mVideoSortOrder = Qt::AscendingOrder;
        mCollectionSortOrder = Qt::AscendingOrder;
        mIconString = "";
        mPressedString = "";
        mLastStatusAdditional = QVariant();
        mWidgetLevel = VideoCollectionCommon::ELevelVideos;
        mActivityCollectionId = TMPXItemId::InvalidId();
        mActivityCollectionName = "";
        mCenRepStringValues.clear();
        mCenRepIntValues.clear();
    }   
    
public: // data
    static int mLastError;
    static bool mIsServiceValue;
	static bool mLoadSortingValuesFails;
    static int mVideoSortRole;
    static int mCollectionSortRole;
    static Qt::SortOrder mVideoSortOrder;
    static Qt::SortOrder mCollectionSortOrder;
    static QString mIconString;
    static QString mPressedString;
    static QVariant mLastStatusAdditional;
    static VideoCollectionCommon::TCollectionLevels mWidgetLevel;
    static TMPXItemId mActivityCollectionId;
    static QString mActivityCollectionName;
    static QList<QString> mCenRepStringValues;
    static QList<int> mCenRepIntValues;
};

#endif /* VIDEOCOLLECTIONVIEWUTILSDATA_H */
