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


#ifndef __CREATORCMDSCRIPTRUN_H__
#define __CREATORCMDSCRIPTRUN_H__

#include "engine.h"

class CCreatorCmdScriptRun : public CBase, public MBeating
    {
public:

    static CCreatorCmdScriptRun* NewL(CCreatorEngine* aEngine);
	static CCreatorCmdScriptRun* NewLC(CCreatorEngine* aEngine);
	~CCreatorCmdScriptRun();

    // callback from engine when the commands from the script file have been executed
    void RunScriptDone();

    // from MBeating
    void Beat();
    void Synchronize();
	
private:
    CCreatorCmdScriptRun();
    void ConstructL(CCreatorEngine* aEngine);
	void TickL();
	
private:
    CCreatorEngine* iEngine;
	HBufC* iCommandLineScriptName;
	HBufC* iCommandLineRandomDataFileName;
	CHeartbeat* iTimer;
	enum TTimerMode
		{
		ETimerModeNone,
		ETimerModeStartScript,
		ETimerModeExitAppUi
		};
	TTimerMode iMode;
	TInt iTickCount;
    };
	
#endif // __CREATORCMDSCRIPTRUN_H__	
