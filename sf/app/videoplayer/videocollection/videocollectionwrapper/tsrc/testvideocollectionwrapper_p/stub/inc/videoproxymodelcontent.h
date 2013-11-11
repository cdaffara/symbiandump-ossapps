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
* Description:   stub VideoProxyModelContent for VideoCollectionWrapperPrivate unit tests  
* 
*/

#ifndef VIDEOPROXYMODELCONTENT_H
#define VIDEOPROXYMODELCONTENT_H

#include <videoproxymodelgeneric.h>

class VideoProxyModelContent : public VideoProxyModelGeneric
{
    Q_OBJECT
    
signals:
    
    void shortDetailsReady(TMPXItemId);
    
public: 
    
    /**
     * Contructor.
     *
     * @param parent parent of this widget
     * @param client Collection client pointer to use.
     */
    VideoProxyModelContent(QObject *parent=0);
    
    /**
     * Destructor.
     *
     */
    virtual ~VideoProxyModelContent();
    
public:
    
    static int mContentModelConstructCallCount;
};

#endif // VIDEOPROXYMODELCONTENT_H
