/*
* Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   VideoServiceBrowse stub definition
*
*/

// Version : %version: 1 %

#ifndef VIDEOSERVICEBROWSE_H
#define VIDEOSERVICEBROWSE_H

//INCLUDES
#include <qobject.h>
#include "xqserviceproviderstub.h"

// FORWARD DECLARATIONS
class VideoServices; 
class QLatin1String;

class VideoServiceBrowse : public XQServiceProvider
{
    Q_OBJECT
    
public:
    VideoServiceBrowse( VideoServices *parent, QLatin1String service );
    ~VideoServiceBrowse();
    
public:

    /**
     *  Complete pending service request
     *
     *  @param None
     *  @return None
     */
    void complete();

    /**
     *  Get current browsing category
     *
     *  @param None
     *  @return VideoServices::TVideoCategory
     */
    int getBrowseCategory() const;

    /**
     *  Return context title of videos application
     *
     *  @param None
     *  @return QString
     */
    QString contextTitle() const;
    
    /**
     * Return sort role.
     * 
     * @param None.
     * @return int.
     */
    int sortRole() const;
    
    /**
     * Returns service active status
     *
     * @return bool true if active, false if not active
     *
     */
    bool isActive();

public slots:

    /**
     *  Browse video
     *
     *  @param title, Title of the embedded Videos application
     *  @param category, Category which type of videos are browsed
     *  @param sort, Sort type.
     *  @return None
     */
    void browseVideos(const QString &title, int category, int sortRole);

    /**
     *  Browse video
     *
     *  @param category, Category which type of videos are browsed
     *  @param sort, Sort type.
     *  @return None
     */
    void browseVideos(int category, int sortRole);

private:
    
    /** request index of the service */
    int mRequestIndex;
    
    /** reference to VideoServices instance */
    VideoServices *mServiceApp;
    
    /** current browsing category */
    int mCategory;
    
    /** title of the service client, if not set "Videos" is used */
    QString mTitle;
    
    /** sorting role */
    int mSortRole;
    
};

#endif//VIDEOSERVICEBROWSE_H
