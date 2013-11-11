// Copyright (c) 2001-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Basic test script handling.
// 
//


#ifndef __T_TESTSCRIPT_H
#define __T_TESTSCRIPT_H


#include <e32std.h>
#include <f32file.h>
#include <e32base.h>


//
//
// CTestScript
//

class CTestScript : public CBase
	{
public:
	typedef TBuf8<128> TTestScriptString;

	static CTestScript* NewL(RFs& aFs);
	~CTestScript();

	TBool LoadFileL(const TDesC& aFileName);
	TBool GetLineL(TTestScriptString& aLine);

private:
	void ConstructL();
	CTestScript(RFs& aFs);

private:
	TBuf<512> iLastError;
	RFile iFile;
	RFs& iFs;
	TBool iEndOfFile;
	};


#endif
