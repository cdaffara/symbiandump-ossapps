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


#include <eikenv.h>
#include <coeaui.h>  
#include <pathinfo.h> 
#include "creator_cmdscriptrun.h"
#include "engine.h"
#include "enginewrapper.h"
#include "creator.pan"


// delay before script running and application exiting in seconds
// when script is run from command line
const TInt KCommandLineDelay = 3;

//----------------------------------------------------------------------------

CCreatorCmdScriptRun* CCreatorCmdScriptRun::NewL(CCreatorEngine* aEngine)
    {
    CCreatorCmdScriptRun* self = CCreatorCmdScriptRun::NewLC(aEngine);
    CleanupStack::Pop(self);
    return self;
    }

CCreatorCmdScriptRun* CCreatorCmdScriptRun::NewLC(CCreatorEngine* aEngine)
    {
    CCreatorCmdScriptRun* self = new (ELeave) CCreatorCmdScriptRun;
    CleanupStack::PushL(self);
    self->ConstructL(aEngine);
    return self;
    }


// ConstructL 
void CCreatorCmdScriptRun::ConstructL(CCreatorEngine* aEngine)
    {
    iEngine = aEngine;
 
	iMode = ETimerModeNone;
	iTickCount = 0;
    iTimer = CHeartbeat::NewL(0);
    
    // parse script name and random data file name from command line
    iCommandLineScriptName = HBufC::NewL(User::CommandLineLength());
    iCommandLineRandomDataFileName = HBufC::NewL(User::CommandLineLength());
    HBufC* commandLine = HBufC::NewLC(User::CommandLineLength());
    TPtr16 ptr = commandLine->Des();
    User::CommandLine(ptr);
    commandLine->Des().Trim();
    
    TInt pos = commandLine->Des().Find(_L(" "));
    if (pos != KErrNotFound)
    	{
    	iCommandLineScriptName->Des().Copy(commandLine->Des().Left(pos));
    	iCommandLineRandomDataFileName->Des().Copy(commandLine->Des().Mid(pos));
    	iCommandLineRandomDataFileName->Des().Trim();
    	}
    else
    	{
    	iCommandLineScriptName->Des().Copy(commandLine->Des());
    	iCommandLineRandomDataFileName->Des().Copy(KNullDesC);
    	}

    CleanupStack::PopAndDestroy(commandLine);
    
    if (iCommandLineScriptName->Des().Length() > 0)
    	{
    	iMode = ETimerModeStartScript;
    	iTimer->Start(ETwelveOClock, this);
    	}
    }


 CCreatorCmdScriptRun::CCreatorCmdScriptRun()                              
    {
    // add any construction that cannot leave here
    }


 CCreatorCmdScriptRun::~CCreatorCmdScriptRun()
    {
    delete iCommandLineRandomDataFileName;
    delete iCommandLineScriptName;
    
    if (iTimer)
    	{
    	iTimer->Cancel();
        delete iTimer;
        iTimer = NULL;
    	}
    }

void  CCreatorCmdScriptRun::RunScriptDone()
	{
	if (iMode == ETimerModeStartScript)
		{
		iMode = ETimerModeExitAppUi;
		iTimer->Start(ETwelveOClock, this);
		}
	}

void  CCreatorCmdScriptRun::Beat()
	{
	TRAP_IGNORE( TickL() );
	}

void  CCreatorCmdScriptRun::Synchronize()
	{
    TRAP_IGNORE( TickL() );
	}

void  CCreatorCmdScriptRun::TickL()
	{
	iTickCount++;
	if (iTickCount > KCommandLineDelay)
		{
		iTimer->Cancel();
		iTickCount = 0;
		if (iMode == ETimerModeStartScript)
			{
        	iEngine->GetRandomDataFromFileL(*iCommandLineRandomDataFileName);
			TInt status = iEngine->RunScriptL(*iCommandLineScriptName);
			if (status != KErrNone)
				{
				TBuf<128> noteMsg;
				if (status == KErrNotFound)
					{
					_LIT(KMessage1, "Error in opening script file.");
					noteMsg.Copy(KMessage1);
					}
				else if (status == KErrCorrupt)
					{
					_LIT(KMessage2, "Error in parsing script file.");
					noteMsg.Copy(KMessage2);
					}
				else if (status == KErrCompletion)
					{
					_LIT(KMessage3, "Script file does not contain any elements.");
					noteMsg.Copy(KMessage3);
					}
				else
					{
					_LIT(KMessage4, "Unknown error in opening script file.");
					noteMsg.Copy(KMessage4);
					}
				iEngine->GetEngineWrapper()->ShowNote(noteMsg);
				RunScriptDone();
				}
			}
		else if (iMode == ETimerModeExitAppUi)
	    	{
	    	iMode = ETimerModeNone;
	    	iEngine->GetEngineWrapper()->CloseCreatorApp();
	    	}
		}
	}
