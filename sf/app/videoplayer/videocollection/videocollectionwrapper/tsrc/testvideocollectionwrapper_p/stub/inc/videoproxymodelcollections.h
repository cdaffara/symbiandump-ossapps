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
* Description:   stub VideoProxyModelCollections for VideoCollectionWrapperPrivate unit tests  
* 
*/

#ifndef VIDEOPROXYMODELCOLLETIONS_H
#define VIDEOPROXYMODELCOLLETIONS_H

#include <videoproxymodelgeneric.h>

class VideoProxyModelCollections : public VideoProxyModelGeneric
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
    VideoProxyModelCollections(QObject *parent=0);
    
    /**
     * Destructor.
     *
     */
    virtual ~VideoProxyModelCollections();
    
public:
    
    static int mCollectionsModelConstructCallCount;
};

#endif // VIDEOPROXYMODELCOLLETIONS_H
