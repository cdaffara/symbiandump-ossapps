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
* Description: stub videocollectionuiloaderdata
* 
*/

#ifndef VIDEOCOLLECTIONUILOADERDATA_H
#define VIDEOCOLLECTIONUILOADERDATA_H

#include <qstringlist.h>
#include <qmap.h>
#include "videocollectionuiloader.h"

class VideoCollectionUiLoaderData
{
public: // methods
    static void reset()
    {
        mFindFailure = false;
        mFailDialogLoad = false;
        mFindFailureNameList.clear();
        mMenuActions.clear();
        mLastLoadPhasedData = -1;
        mAddDataCallCount = 0;
    }
    
public: // data
    static bool mFindFailure;
    static bool mFailDialogLoad;
    static QStringList mFindFailureNameList;
    static QMap<VideoCollectionUiLoader::ActionIds, HbAction*> mMenuActions;
    static int mLastLoadPhasedData;
    static int mAddDataCallCount;
};

#endif /* VIDEOCOLLECTIONUILOADERDATA_H */
