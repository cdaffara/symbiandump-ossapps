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
* Description:   stub VideoProxyModelGeneric for VideoCollectionWrapperPrivate unit tests  
* 
*/

#ifndef VIDEOPROXYMODELGENERIC_H
#define VIDEOPROXYMODELGENERIC_H

#include <qsortfilterproxymodel.h>
#include <mpxitemid.h>

class QTimer;
class VideoListDataModel;
class VideoCollectionClient;

class VideoProxyModelGeneric : public QObject 
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
    VideoProxyModelGeneric(QObject *parent=0);
    
    /**
     * Destructor.
     *
     */
    virtual ~VideoProxyModelGeneric();
    
    /**
     * Initialize.
     */
    int initialize(VideoListDataModel *sourceModel);
    
public:

    /**
     * Pointer to the actual model.
     * Not owned.
     */
    VideoListDataModel *mModel;
    
    static int mGenericModelConstructCallCount;
};

#endif // VIDEOPROXYMODELGENERIC_H
