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
* Description:  stub implementation of VideoActivityState
*
*/

// Version : %version: 1 %

#include "videoactivitystate.h"
#include "mpxvideo_debug.h"

int VideoActivityState::mAllDataGetCount = 0;

// -------------------------------------------------------------------------------------------------
// instance()
// -------------------------------------------------------------------------------------------------
//
VideoActivityState& VideoActivityState::instance()
{
    MPX_ENTER_EXIT(_L("VideoActivityState::instance()"));
    
    static VideoActivityState _staticWrapper;
    return _staticWrapper;
}

// -------------------------------------------------------------------------------------------------
// VideoActivityState()
// -------------------------------------------------------------------------------------------------
//
VideoActivityState::VideoActivityState()      
{
    // NOP
}

// -------------------------------------------------------------------------------------------------
// ~VideoActivityState()
// -------------------------------------------------------------------------------------------------
//
VideoActivityState::~VideoActivityState()
{
    MPX_ENTER_EXIT(_L("VideoActivityState::~VideoActivityState()"));
    mActivityData.clear();
    
}

// -------------------------------------------------------------------------------------------------
// clearActivityData()
// -------------------------------------------------------------------------------------------------
//
void VideoActivityState::clearActivityData()
{
    MPX_ENTER_EXIT(_L("VideoActivityState::clearActivityData()"));
    mActivityData.clear();
}

// -------------------------------------------------------------------------------------------------
// setActivityData()
// -------------------------------------------------------------------------------------------------
//
void VideoActivityState::setActivityData(const QVariant& data, const QString& dataKey)
{
    MPX_ENTER_EXIT(_L("VideoActivityState::setActivityData()"));
    if(dataKey.length())
    {
         // setting particular dataitem
         mActivityData[dataKey] = data;
    }
    else
    {
        // overwriting all data
        mActivityData.clear();    
        mActivityData = data.toHash(); 
    }
}
    
// -------------------------------------------------------------------------------------------------
// getActivityData()
// -------------------------------------------------------------------------------------------------
//
const QVariant VideoActivityState::getActivityData(const QString& dataKey)
{
    MPX_ENTER_EXIT(_L("VideoActivityState::getData()"));
    if(dataKey.length())
    {
        return mActivityData[dataKey];
    }
    else
    {
        mAllDataGetCount++;
        return QVariant::fromValue(mActivityData);
    }
}


// End of file
