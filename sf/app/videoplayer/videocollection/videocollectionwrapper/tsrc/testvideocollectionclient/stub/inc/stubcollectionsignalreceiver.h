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
* Description:  stub classes for testing CVideoCollectionClient
* 
*/

#ifndef __STUBCOLLECTIONSIGNALRECEIVER_H
#define __STUBCOLLECTIONSIGNALRECEIVER_H

#include <QObject>
// INCLUDES
#include "videodatasignalreceiver.h"

/**
 * implementation class for slots to be connected
 */
class  StubSignalReceiver  : public VideoDataSignalReceiver
{

    Q_OBJECT
    
    Q_DISABLE_COPY(StubSignalReceiver)
    
public:
    
    /**
     * contructor
     */
    StubSignalReceiver(){};
    
    /**
     * destructor
     */
    virtual ~StubSignalReceiver(){};
    
public slots:   
    
    /**
     * No implementation needed for these tests
     */
    void newVideoListSlot(CMPXMediaArray *array)
    {
        Q_UNUSED(array);
    }
   
    /**
     * No implementation needed for these tests
     */
    void appendVideoListSlot( CMPXMediaArray* array )
    {
        Q_UNUSED(array);
    }
          
    /**
     * No implementation needed for these tests
     */
    void newVideoAvailableSlot(CMPXMedia *media)
    {
        Q_UNUSED(media);
    }
    
    /**
     * No implementation needed for these tests
     */
    void itemDeletedSlot(TMPXItemId &id)
    {
        Q_UNUSED(id);
    }
    
    /**
     * No implementation needed for these tests
     */
    void albumRemoveFailureSlot(QList<TMPXItemId> *items)
    {
        Q_UNUSED(items);
    }
    
    /**
     * No implementation needed for these tests
     */
    void videoDeletedSlot(TMPXItemId &id)
    {
        Q_UNUSED(id);
    }
           
    /**
     * No implementation needed for these tests
     */
    void videoDeleteCompletedSlot(int count, QList<TMPXItemId> *failedIds)
    {
        Q_UNUSED(count);
        Q_UNUSED(failedIds);
    }
    
    /**
     * No implementation needed for these tests
     */
    void videoDetailsCompletedSlot(CMPXMedia* media)
    {
        Q_UNUSED(media);
    }
    
    /**
     * No implementation needed for these tests
     */
    void albumListAvailableSlot(TMPXItemId &albumId, CMPXMediaArray *albumItems)
    {
        Q_UNUSED(albumId);
        Q_UNUSED(albumItems);
    }

    /**
     * No implementation needed for these tests
     */
    void itemModifiedSlot(const TMPXItemId &itemId)
    {
        Q_UNUSED(itemId);
    }
    
    /**
     * No implementation needed for these tests
     */
    void videoListCompleteSlot()
    {
        
    }
    /**
     * No implementation needed for these tests
     */
    void albumListCompleteSlot()
    {
        
    }
};


#endif
