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
* Description: stub videooperatorservicedata
* 
*/

#ifndef VIDEOOPERATORSERVICEDATA_H
#define VIDEOOPERATORSERVICEDATA_H

#include <qlist.h>
#include <qstring.h>

class VideoOperatorServiceData
{
public: // methods
    static void reset()
    {
        mTitles.clear();
        mIcons.clear();
        mUris.clear();
        mUids.clear();
        
        mTitleCallCount = 0;
        mIconResourceCallCount = 0;
        mLaunchServiceCallCount = 0;
    }
    
public: // data
    static QList<QString> mTitles;
    static QList<QString> mIcons;
    static QList<QString> mUris;
    static QList<int> mUids;
    
    static bool loadReturnValue;
    
    static int mLoadCallCount;
    static int mTitleCallCount;
    static int mIconResourceCallCount;
    static int mLaunchServiceCallCount;
};

#endif /* VIDEOOPERATORSERVICEDATA_H */
