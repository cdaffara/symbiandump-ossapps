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
* Description:   stub VideoActivityState class definition for collection view unite tests
*
*/

// Version : %version:  1 %

#ifndef __VIDEOACTIVITYSTATE_H__
#define __VIDEOACTIVITYSTATE_H__

#include <qhash.h>
#include <qvariant.h>



namespace VideoActivityData
{        
    // type of collectionview plugin's widget level(int): 
    // all videos, collections or collection video list
    static const QString KEY_WIDGET_LEVEL    = "_VideoActivity_widget_level_";
    
    // id of the collection whose videolist is to be shown (int).
    static const QString KEY_WIDGET_ID       = "_VideoActivity_view_item_id_";
}


class VideoActivityState 
{
    
public:
 
    /**
     * method returns a static instance of this class
     */
    static VideoActivityState &instance();
    
    /**
     * Sets activity data. If dataKey is not defined, all activity data
     * will be overwritten by a provided data.
     * 
     * @param data Data to save
     * @param dataKey Key indicating dataitem. If empty, all data will be overwritten by provided data 
     */
    void setActivityData(const QVariant& data, const QString& dataKey = QString(""));

    /**
     * Get activity data. If dataKey is not defined, all activity data
     * will be returned. If item corresponding provided dataKey is not found,
     * method returns empty QVariant.
     * 
     * @param dataKey Key indicating dataitem client wants. If empty, all data will be returned
     * 
     * @return QVariant 
     */
    const QVariant getActivityData(const QString& dataKey = QString(""));

private:
    
    /**
     * private default contructor
     */
    VideoActivityState();  
    
    /**
     * copy contructor definition
     */
    VideoActivityState(const VideoActivityState& other);  
    
    /**
     * Private destructor.
     *
     */
    virtual ~VideoActivityState();    

private:
    
    /**
     * locally saved activity data
     */
    QHash<QString, QVariant> mActivityData;
        
};

#endif //__VIDEOACTIVITYSTATE_H__
