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
* Description: stub videolistwidgetdata
* 
*/

#ifndef VIDEOLISTWIDGETDATA_H
#define VIDEOLISTWIDGETDATA_H

#include <QModelIndex>

class VideoListWidgetData
{
public: // methods
    static void reset()
    {
        mInitializeCount = 0;
        mInitializeReturnValue = 0;
        mActivateCount = 0;
        mActivateReturnValue = 0;
        mActive = false;
        mDeactivateCount = 0;
        mEmitActivatedIndex = QModelIndex();
    }
    
public: // data
    static int mInitializeCount;
    static int mInitializeReturnValue;
    static int mActivateCount;
    static int mActivateReturnValue;
    static bool mActive;
    static int mDeactivateCount;
    static QModelIndex mEmitActivatedIndex;
};

#endif /* VIDEOLISTWIDGETDATA_H */
