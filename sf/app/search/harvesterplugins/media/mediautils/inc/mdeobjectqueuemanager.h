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
* Description:  MDE object queuemanager for indexing scheduling
 *
*/


#ifndef MDEOBJECTQUEUEMANAGER_H_
#define MDEOBJECTQUEUEMANAGER_H_
#include <e32def.h>
#include <mdccommon.h>
#include "mediaobjecthandler.h"

class CMdeObjectQueueManager : public CActive
    {
public:
    /*
     * Construction
     * @param MMediaObjectHandler* callback to media object handler dont own
     * @return instance of object queue manager
     */
    static CMdeObjectQueueManager* NewL(MMediaObjectHandler* ); //For callback
    /*
     * Construction
     * @param MMediaObjectHandler* callback to media object handler dont own
     * @return instance of object queue manager
     */
    static CMdeObjectQueueManager* NewLC(MMediaObjectHandler* );
    /*
     * Destructor
     */
    virtual ~CMdeObjectQueueManager();
public:
    /*
     * AddMedItemtoQueueL
     * @param TItemId aObjId item id to be added to queue
     * @param TCPixActionType aActionType CPIX action on item
     */
    void AddMdeItemToQueueL( TItemId aObjId, TCPixActionType aActionType);
    //From CActive
    void RunL();
    void DoCancel();
    TInt RunError();
    void PauseL();
    void ResumeL();
private:
    /*
     * OverWriteOrAddToQueueL add or update item to queue
     * @param TItemId aObjId itemd id
     * @param TCPixActionType aActionType action cpix
     */
    void OverWriteOrAddToQueueL( TItemId aObjId, TCPixActionType aActionType);
    /*
     * First phase Constructor
     * @parm MMediaObjectHandler* media object handler callback
     */
    CMdeObjectQueueManager(MMediaObjectHandler* );
    /*
     * Activate the timer object to index queued media items
     */
    void ActivateAO();
    /*
     * Second phase constructor
     */
    void ConstructL();
private:
    enum TState 
    { 
        EStateNone = 0, 
        EStateWaiting 
    };
    enum THarvestingState 
    { 
        EStateResume = 0, 
        EStatePause 
    };
    struct TMdeActionRecord 
    {
    TItemId iObjectId;
    TCPixActionType iAction;
    TInt    iRetryCount;
    TMdeActionRecord()
        {
        iRetryCount = 0;
        }
    };
private:
    // Queue of documents to be indexed
    RArray<TMdeActionRecord> iJobQueue; 
    // State machine's state
    TState iState;
    THarvestingState iHState;
    // Timer which is used to delay indexing messages
    RTimer iTimer; //Timer for self activation of AO
    MMediaObjectHandler* iMdeObjectHandler; //media object handler
    };

#endif /* MDEOBJECTQUEUEMANAGER_H_ */
