// Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// This is the header file for Phone book synchronisation
// file corruption scenarios 
// 
//

/**
 @file
 @internalComponent
*/

#ifndef __TESTREADINGINIFILE_H__
#define __TESTREADINGINIFILE_H__

#include "TE_PhBkSyncBase.h" 

//
//    Test step 12 //
//
class CPhbkSyncIniFileTest : public CTestStepPhbkSync
	{

public:
	CPhbkSyncIniFileTest();
	~CPhbkSyncIniFileTest();

	virtual enum TVerdict doTestStepL(void);
	
private:
	void DoToIniFileL(TInt aAction);
	void CheckIniFileL();
	
	
	};

#endif /* __TESTREADINGINIFILE_H__ */
