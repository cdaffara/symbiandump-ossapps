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
* Description: 
*
*/

#ifndef HARVESTEROBSERVER_H_
#define HARVESTEROBSERVER_H_

#include <mindexingpluginobserver.h>
#include "rsearchserversession.h"
#include <cindexingplugin.h>
#include "delayedcallback.h"

class CHarvesterObserver: public CBase, public MIndexingService,
                          public MDelayedCallbackObserver
    {
public:
    //Does not take ownership of aPluginToObserve
    static CHarvesterObserver* NewL( CIndexingPlugin* aPluginToObserve );
    static CHarvesterObserver* NewLC( CIndexingPlugin* aPluginToObserve );
    void ConstructL();
    ~CHarvesterObserver();
    
    void SetWaitTime(TTimeIntervalMicroSeconds32 aTimetoWait);
    
    void DelayedCallbackL(TInt aCode);
    void DelayedError(TInt aErrorCode);
    
//from MIndexingService
    void AddHarvestingQueue(CIndexingPlugin* aPlugin, const TDesC& aQualifiedBaseAppClass, TBool aForceReharvest = EFalse);
    void RemoveHarvestingQueue(CIndexingPlugin* /*aPlugin*/, const TDesC& /*aQualifiedBaseAppClass*/, TBool /*aRemovePersist = EFalse*/){};
    void HarvestingCompleted(CIndexingPlugin* aPlugin, const TDesC& aQualifiedBaseAppClass, TInt aError);

private:
    CHarvesterObserver( CIndexingPlugin* aPluginToObserve );
    
private:
    CIndexingPlugin* iObservedPlugin;
    RSearchServerSession iSearchSession;

public:
    CActiveSchedulerWait* iWaitForHarvester; //Needed public for finer grained control. 
    TTimeIntervalMicroSeconds32 iWaitTime;
    CDelayedCallback* iCallback;
    };


#endif /* HARVESTEROBSERVER_H_ */
