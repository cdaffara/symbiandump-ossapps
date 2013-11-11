

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
* Description:  stubclass of videodeleteworker for testing VideoListDataModel_p class methods*
*/

#ifndef __VIDEODELETEWORKER_H__
#define __VIDEODELETEWORKER_H__

#include <QObject>
#include <QList>
#include <QVariant>
#include <mpxitemid.h>


class VideoDeleteWorker : public QObject
{    
    /**
     * define to be able to use signals and slots
     */
    Q_OBJECT
    
    /**
     * disable copy-constructor and assignment operator
     */
    Q_DISABLE_COPY(VideoDeleteWorker) 

    
public: 

    /**
     * Default constructor
     */
    VideoDeleteWorker(){};  
    
    /**
     * Destructor
     */
    ~VideoDeleteWorker(){};

    /**
     * dummy impl
     */
    void removeFromRequest(TMPXItemId ){};


    
};

#endif // __VIDEODELETEWORKER_H__
