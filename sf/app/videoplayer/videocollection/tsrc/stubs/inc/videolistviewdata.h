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
* Description: stub hbstackedwidget
* 
*/

#ifndef VIDEOLISTVIEWDATA_H
#define VIDEOLISTVIEWDATA_H

#include <e32std.h>
#include <mpxitemid.h>

#include "videolistwidget.h"

class VideoListViewData
{
public: // methods
    static void reset()
    {
        mInitializeViewCount = 0;
        mActivateViewCount = 0;
        mDeactivateViewCount = 0;
        mBackCount = 0;
        mActivatedItemId = TMPXItemId::InvalidId();
        mCurrentListWidget = 0;
    }
    
public: // data
    static int mInitializeViewCount;
    static int mActivateViewCount;
    static int mDeactivateViewCount;
    static int mBackCount;    
    static TMPXItemId mActivatedItemId;
    static VideoListWidget *mCurrentListWidget;
};

#endif /* VIDEOLISTVIEWDATA_H */
