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
* Description:  MDE media monitor utility
 *
*/


#ifndef MDSAUDIOMONITOR_H_
#define MDSAUDIOMONITOR_H_

#include <e32def.h>
#include <mdesession.h>

//Forward declaration
class CMdeObjectQueueManager; //Queue all Audio objects for Indexing
class CMdESession;

class CMdsMediaMonitor : public CBase,
                         public MMdEObjectObserver,
                         public MMdEObjectPresentObserver
    {
public:
    /*
     * NewL construction
     * @param CMdESession* valid mde session donot own
     * @param CMdeObjectQueueManager* object queue manager do not own
     * @return instance of media monitor
     */
    static CMdsMediaMonitor* NewL(CMdESession* ,CMdeObjectQueueManager* );
    /*
     * NewLC construction
     * @param CMdESession* valid mde session do not own
     * @param CMdeObjectQueueManager* object queue manager do not own
     * @return instance of media monitor
     */
    static CMdsMediaMonitor* NewLC(CMdESession* ,CMdeObjectQueueManager* );
    /*
     * Destructor
     */
    virtual ~CMdsMediaMonitor();
private:
    /*
     * parameterized constructor
     * @param CMdESession* valid mde session do not own
     * @param CMdeObjectQueueManager* mde object queue manager donot own
     */
    CMdsMediaMonitor(CMdESession* ,CMdeObjectQueueManager* ); //default constructor
    /*
     * Second phase constructor
     */
    void ConstructL();  //Second phase constructor
public:
    /*
     * StartMonitoring
     * @param TDesC& aNameSpaceDef name space
     */
    void StartMonitoringL(const TDesC& aNameSpaceDef); //Controlled by owner
    /*
     * StopMonitoring stops sending events
     */
    void StopMonitoringL();
    //From MMdEObjectObserver
    void HandleObjectNotification(CMdESession& aSession, 
                            TObserverNotificationType aType,
                            const RArray<TItemId>& aObjectIdArray);
    void HandleObjectPresentNotification(CMdESession& aSession, 
                TBool aPresent, const RArray<TItemId>& aObjectIdArray);
private:
    CMdESession* iMdsSession; //do not own
    //Maintain queue of object array with actions for indexing
    CMdeObjectQueueManager* iMdeObjectQueueManager; 
    };

#endif /* MDSAUDIOMONITOR_H_ */
