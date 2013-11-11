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
* Description: stub videothumbnailtestdata
* 
*/

#ifndef VIDEOTHUMBNAILTESTDATA_H
#define VIDEOTHUMBNAILTESTDATA_H

class VideoThumbnailTestData
{
public: // methods
    static void reset()
    {
        mInstanceCallCount = 0;
        mFreeThumbnailDataCallCount = 0;
        mBackgroundThumbnailFetchingEnabled = false;
        mStartBackgroundFetchingCallCount = 0;
        mFetchIndex = -1;
    }
    
public: // data
    static int mInstanceCallCount;
    static int mFreeThumbnailDataCallCount;
    static bool mBackgroundThumbnailFetchingEnabled;
    static int mStartBackgroundFetchingCallCount;
    static int mFetchIndex;
};

#endif /* VIDEOTHUMBNAILTESTDATA_H */
