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
* Description:   stub VideoProxyModelAllVideos for VideoCollectionWrapperPrivate unit tests  
* 
*/

#ifndef VIDEOPROXYMODELALLVIDEOS_H
#define VIDEOPROXYMODELALLVIDEOS_H

#include <videoproxymodelgeneric.h>

class VideoProxyModelAllVideos : public VideoProxyModelGeneric
{
    Q_OBJECT
    
signals:
    
    void shortDetailsReady(TMPXItemId);
    
public: 
    
    /**
     * Contructor.
     *
     * @param parent parent of this widget
     */
    VideoProxyModelAllVideos(QObject *parent=0);
    
    /**
     * Destructor.
     *
     */
    virtual ~VideoProxyModelAllVideos();
    
public:
    
    static int mAllVideosModelConstructCallCount;
};

#endif // VIDEOPROXYMODELALLVIDEOS_H
