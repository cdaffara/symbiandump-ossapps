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
* Description:  MDE harvester utility class
*
*/

#ifndef MDEHARVESTER_H_
#define MDEHARVESTER_H_

#include <e32def.h>
#include <e32base.h>
#include <mdesession.h>
#include <mdequery.h>
#include <mdeconstants.h>
#include <mdeobjectquery.h>
#include <mdccommon.h>
#include <mdeitem.h>
#include <mdeobject.h>
#include "mdeobjectqueuemanager.h"

class CMdeHarvester : public CActive,
                      public MMdEQueryObserver
   {
   public:
    /*
     * Construction
     * @param CMdESession* aSession valid mde session donot own
     * @param MdeMEdiaObserver* aObserver media observer for completion donot own
     * @param CMdeObjectQueueManager* aObjJobQueue object queue manager donot own
     * @return instance of mde harvester
     */
   static CMdeHarvester* NewL(CMdESession* aSession,
                              MdeMEdiaObserver* aObserver,
                              CMdeObjectQueueManager* aObjJobQueue);
   /*
    * Construction
    * @param CMdESession* aSession valid mde session
    * @param MdeMEdiaObserver* aObserver media observer for completion
    * @param CMdeObjectQueueManager* aObjJobQueue object queue manager
    * @return instance of mde harvester
    */   
   static CMdeHarvester* NewLC(CMdESession* aSession,
                               MdeMEdiaObserver* aObserver,
                               CMdeObjectQueueManager* aObjJobQueue);
   /*
    * destruction
    */
   virtual ~CMdeHarvester();
   public:
   /*
    * Callback from MMdEQueryObserver
    * @param CMdEQuery& aQuery query instance
    * @param TInt aFirstNewItemIndex
    * @param TInt aNewItemCount
    */
   void HandleQueryNewResults(CMdEQuery& aQuery,
                                          TInt aFirstNewItemIndex,
                                          TInt aNewItemCount);
   /*
    * Callback from MMdEQueryObserver
    * @param CMdEQuery& aQuery query instance
    * @param TInt aError
    */
   void HandleQueryCompleted(CMdEQuery& aQuery, TInt aError);
   /*
    * DoHarvestL start harvesting 
    * @param TDesC& aNameSpaceDef namespace definition
    */
   void DoHarvesetL(const TDesC& aNameSpaceDef);
   //from CActive
   void RunL();
   void DoCancel();
   TInt RunError();
private:
   /*
    * Second phase constructor
    * @param CMdESession* aSession valid mde session do not own
    * @param MdeMEdiaObserver* aObserver media event observer completion do notown
    * @param CMdeObjectQueueManager* aObjJobQueue job queue manager donot own
    */
   void ConstructL(CMdESession* aSession,
                   MdeMEdiaObserver* aObserver,
                   CMdeObjectQueueManager* aObjJobQueue);
   /*
    * Default constructor
    */
   CMdeHarvester();
   /*
    * Queryall media objects
    * @param TDesC& aNameSpaceDef namespace definition
    */
   void QueryAllMediaObjectsL(const TDesC& aNameSpaceDef);
   /*
    * HandleNextRequest
    */
   void HandleNextRequestL();
   /*
    * AO completion on self
    */
   void CompleteSelf();
private:
   MdeMEdiaObserver* iObserver;//Media observer for event completion    
   // session to metadata engine
   CMdESession* iSession;
   // metadata query
   CMdEObjectQuery* iQuery;
   CActiveSchedulerWait* iWait; //Nested AO wait
   CMdeObjectQueueManager* iJobQueue;//Pass the Object for Metadata processing
   TInt         iCurrentIndex; //Current index for MDE items
   };


#endif /* MDEHARVESTER_H_ */
