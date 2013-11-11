// Copyright (c) 2002-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// "SetSyncMode" test group scenarios 
// 
//

/**
 @file
 @internalComponent
*/

#ifndef __TESTSETSYNCMODESTEPS_H__
#define __TESTSETSYNCMODESTEPS_H__

#include "TE_PhBkSyncBase.h" 

//
//    Test step1 -- Test#81 in Unit Test Spec. Doc.  //
//
class CPhbkSyncSetSyncModeTest_01 : public CTestStepPhbkSync
	{
public:
	CPhbkSyncSetSyncModeTest_01();
	~CPhbkSyncSetSyncModeTest_01();

	virtual enum TVerdict doTestStepL(void);
	};

//
//    Test step2 -- Test#82 in Unit Test Spec. Doc.  //
//
class CPhbkSyncSetSyncModeTest_02 : public CTestStepPhbkSync
	{
public:
	CPhbkSyncSetSyncModeTest_02();
	~CPhbkSyncSetSyncModeTest_02();

	virtual enum TVerdict doTestStepL(void);
	};

//
//    Test step3 -- Test#83 in Unit Test Spec. Doc.  //
//
class CPhbkSyncSetSyncModeTest_03 : public CTestStepPhbkSync
	{
public:
	CPhbkSyncSetSyncModeTest_03();
	~CPhbkSyncSetSyncModeTest_03();

	virtual enum TVerdict doTestStepL(void);
	};

#endif /* __TESTSETSYNCMODESTEPS_H__ */
