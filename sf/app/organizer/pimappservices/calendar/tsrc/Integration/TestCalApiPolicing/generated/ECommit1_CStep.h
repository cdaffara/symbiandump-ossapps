// Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
// All rights reserved.
// This component and the accompanying materials are made available
// under the terms of "Eclipse Public License v1.0"
// which accompanies this distribution, and is available
// at the URL "http://www.eclipse.org/legal/epl-v10.html".
//
// Initial Contributors:
// Nokia Corporation - initial contribution.
//
// Contributors:
//
// Description:
//

#ifndef ECommit1_H_
#define ECommit1_H_

//Include the suite header
#include "CSuite.h"

#include "agmserv.h"


class CECommit1Step: public CCapabilityTestStep
	{

public:

	//Get the version of the server to be called
	TVersion Version()
	{
		return TVersion(KAgnServerMajorVersion, KAgnServerMinorVersion, KAgnServerBuildVersion);
	}	
	
	
	//Constructor called from the respective Suite.cpp from their "AddTestStep" function
	CECommit1Step() ;

	//Always clean your mess
	~CECommit1Step()
	{
		tChildThread.Close();
	}		
	
	//This is the Function called from "doTestStepL" by the test Suite,and it creates an 
	//child thread which internally calls the corresponding Exec_SendReceive_SERVERNAME fn.
	TVerdict MainThread();

	//Here's where the connection and testing the message takes place
	TInt Exec_SendReceive();
	
	};

#endif
