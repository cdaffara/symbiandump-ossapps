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
* Description:
*
*/

#include "clearlisttest.h"
#include <f32file.h>
#include <s32file.h>
#include <utf.h>

#ifdef EKA2 //RD_APPS_TO_EXES 
#include <eikstart.h>
#endif


#include <e32std.h>
#include <eikstart.h>

_LIT(KClearlisttestPanic,"Clearlisttest creation");
_LIT( KMaxTime, "99991130:235959.999999");

LOCAL_C void MainL(); 

// main function called by E32
GLDEF_C TInt E32Main() 
	{
	CTrapCleanup* cleanup=CTrapCleanup::New(); // get clean-up stack
	TRAPD(error,MainL());
	__ASSERT_ALWAYS(error == KErrNone,User::Panic(KClearlisttestPanic,error));
	delete cleanup; // destroy clean-up stack
	User::Heap().Reset();
	return 0; // and return
	}
      
LOCAL_C void MainL() 
	{RDebug::Print(_L("CClearlisttest MainL"));
	RThread().SetPriority(EPriorityAbsoluteForeground);	
	// install an active scheduler
	CActiveScheduler* scheduler = new(ELeave)CActiveScheduler;
	CActiveScheduler::Install(scheduler);
	
	CClearlisttest* main = 0;
	TRAPD (err, main = CClearlisttest::NewL());
    if (err == KErrNone) 
    	{RDebug::Print(_L("CClearlisttest CActiveScheduler::Start()"));
    	CActiveScheduler::Start();
    	}
  RDebug::Print(_L("CClearlisttest after  CActiveScheduler::Start()"));
  delete main;
  delete scheduler;
  CActiveScheduler::Install(NULL);
	}
      
CClearlisttest::~CClearlisttest()
    {
    	RDebug::Print(_L("CClearlisttest deleted"));
    	RDebug::Print(_L("CClearlisttest Cancel()"));
    	CActive::Cancel();
    	RDebug::Print(_L("CClearlisttest delete iLogEvent"));
    	delete iLogEvent;
    	RDebug::Print(_L("CClearlisttest delete iLogClient"));
    	delete iLogClient;
 			RDebug::Print(_L("CClearlisttest ~CClearlisttest<-"));
 			iFsSession.Close();
    }    
// From CActive
void CClearlisttest::RunL ()
	{
    RDebug::Print(_L("CClearlisttest::RunL->"));
    if (iStatus.Int() == KErrNone)
    	{
    	RDebug::Print(_L("CClearlisttest::RunL iStatus.Int() == KErrNone"));
    	}
    else
    	{
    	RDebug::Print(_L("CClearlisttest::RunL iStatus.Int() != KErrNone<-"));
    	}
    RDebug::Print(_L("CClearlisttest::RunL<-"));	
    CActiveScheduler::Stop();
	}

// From CActive
void CClearlisttest::DoCancel ()
	{
		RDebug::Print(_L("CClearlisttest::DoCancel"));
	}

CClearlisttest* CClearlisttest::NewL()
    {RDebug::Print(_L("CClearlisttest NewL"));
    CClearlisttest* self = new (ELeave) CClearlisttest();
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop();
    return self;
    }

CClearlisttest::CClearlisttest()
: CActive(EPriorityStandard)
{
}

void CClearlisttest::ConstructL()
	{RDebug::Print(_L("CClearlisttest ConstructL->"));
		CActiveScheduler::Add (this);
		
  	User::LeaveIfError(iFsSession.Connect());  
		iLogClient = CLogClient::NewL( iFsSession );	
		iLogEvent = CLogEvent::NewL();
  	User::LeaveIfError(ClearList()); 
   RDebug::Print(_L("CClearlisttest ConstructL<-"));
  }

TInt CClearlisttest::ClearList()
    {RDebug::Print(_L("CClearlisttest::ClearList()->"));
    TTime time( KMaxTime );
    TInt errorCode ( KErrNone );
    if ( !IsActive( ) )
        {
        	//const TLogRecentList KLogNullRecentList = -1;
        RDebug::Print(_L("CClearlisttest::ClearList()"));	
        //iLogClient->ClearLog( KLogNullRecentList, iStatus );
        iLogClient->ClearLog( time, iStatus );
        SetActive( );        
        }
    else
        {
        RDebug::Print(_L("CClearlisttest::ClearList()() Clear failed")); 	
        errorCode = KErrInUse;
        }
    RDebug::Print(_L("CClearlisttest::ClearList()<-"));    
    return errorCode;
    }

