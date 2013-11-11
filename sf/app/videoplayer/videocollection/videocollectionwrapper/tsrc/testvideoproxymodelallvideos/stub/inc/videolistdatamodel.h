
/**
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
* Description: Stub collection client to be used when unit testing the proxy model. 
* 
*/

#ifndef __STUB_VIDEOLISTDATAMODELFORPROXY_H
#define __STUB_VIDEOLISTDATAMODELFORPROXY_H

#include <QObject>
#include <QList>
#include <QString>
#include <mpxitemid.h>
#include "videocollectioncommon.h"

class  VideoListDataModel : public QObject
{
    Q_OBJECT
    
public: // Constructors and destructor

    /**
     * Constructor
     *
     */
    VideoListDataModel(QObject *parent = NULL);

    /**
     * destructor
     *
     */
    virtual ~VideoListDataModel();

    /**
     * Stub method.
     */
    TMPXItemId mediaIdAtIndex(int index);
    
    /**
     * Stub method.
     */
    const int rowCount() const;
    
public:
    
    static TMPXItemId mMediaIdAtIndexReturnValue;
    static int mMediaIdAtIndexCallCount;
    static int mRowCountReturnValue;
    
};

#endif // __STUB_VIDEOLISTDATAMODELFORPROXY_H


