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

#include "harvesterobserver.h"

CHarvesterObserver* CHarvesterObserver::NewL( CIndexingPlugin* aPluginToObserve )
    {
    __ASSERT_ALWAYS( aPluginToObserve, User::Leave( KErrArgument ) );
    CHarvesterObserver* observer = CHarvesterObserver::NewLC( aPluginToObserve );
    CleanupStack::Pop( observer );
    return observer;
    }

CHarvesterObserver* CHarvesterObserver::NewLC( CIndexingPlugin* aPluginToObserve )
    {
    CHarvesterObserver* observer = new ( ELeave ) CHarvesterObserver( aPluginToObserve );
    CleanupStack::PushL( observer );
    observer->ConstructL();
    return observer;
    }

void CHarvesterObserver::ConstructL()
    {
    iWaitForHarvester = new (ELeave) CActiveSchedulerWait;
    User::LeaveIfError( iSearchSession.Connect() );
    iObservedPlugin->SetSearchSession( iSearchSession );
    iObservedPlugin->SetObserver( *this );
    iWaitTime = TTimeIntervalMicroSeconds32(0); //Zero initially
    iCallback = CDelayedCallback::NewL(CActive::EPriorityIdle);
    }

void CHarvesterObserver::SetWaitTime(TTimeIntervalMicroSeconds32 aTimetoWait)
    {
    iWaitTime = aTimetoWait;
    }
CHarvesterObserver::CHarvesterObserver( CIndexingPlugin* aPluginToObserve )
    {
    iObservedPlugin = aPluginToObserve;
    };

CHarvesterObserver::~CHarvesterObserver()
    {
    delete iWaitForHarvester;
    iSearchSession.Close();
	delete iCallback;
    }

void CHarvesterObserver::HarvestingCompleted( CIndexingPlugin* /*aPlugin*/, const TDesC& /*aQualifiedBaseAppClass*/, TInt /*aError*/)
    {
    if(iWaitTime > TTimeIntervalMicroSeconds32(0)) //harvesters requested to wait so wait before completion
        {
        iCallback->Start( 0, this, iWaitTime.Int()); //Stop wait AO in callback
        }
    else
        {
        //Stop WAIT AO here
        iWaitForHarvester->AsyncStop();
        }
    }

void CHarvesterObserver::DelayedCallbackL(TInt aCode)
    {
    iWaitForHarvester->AsyncStop();
    }
    
void CHarvesterObserver::DelayedError(TInt aErrorCode)
    {
    iWaitForHarvester->AsyncStop();
    }

void CHarvesterObserver::AddHarvestingQueue(CIndexingPlugin* /*aPlugin*/, const TDesC& /*aQualifiedBaseAppClass*/,TBool aForceReharvest)
    {
    }
