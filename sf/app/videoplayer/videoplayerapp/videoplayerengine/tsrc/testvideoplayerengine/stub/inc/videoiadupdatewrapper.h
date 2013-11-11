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
* Description:  Test stub class
*
*/

// Version : %version: 

#ifndef __VIDEOIADUPDATEWRAPPER_H__
#define __VIDEOIADUPDATEWRAPPER_H__

#include <qobject.h>


class VideoIadUpdateWrapper : public QObject
{
    /**
     * Define to be able to use signals and slots.
     */    
    Q_OBJECT

    /**
     * Disable copy-constructor and assignment operator.
     */
    Q_DISABLE_COPY(VideoIadUpdateWrapper)     
    
public:

    /**
     * Constructor
     */    
    VideoIadUpdateWrapper();

    /**
     * Destructor
     */    
    virtual ~VideoIadUpdateWrapper();
    
    /**
     * Starts the update process.
     */
    int checkForUpdates();

};

#endif // __VIDEOIADUPDATEWRAPPER_H__

// End of file.
