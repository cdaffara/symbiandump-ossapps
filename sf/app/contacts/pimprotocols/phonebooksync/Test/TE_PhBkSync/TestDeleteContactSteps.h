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
// "DeleteContact" test group scenarios 
// 
//

/**
 @file
 @internalComponent
*/

#ifndef __TESTDELETECONTACTSTEPS_H__
#define __TESTDELETECONTACTSTEPS_H__

#include "TE_PhBkSyncBase.h" 

//
//    Test step1 -- Test#73 in Unit Test Spec. Doc.  //
//
class CPhbkSyncDeleteContactTest_01 : public CTestStepPhbkSync
	{
public:
	CPhbkSyncDeleteContactTest_01();
	~CPhbkSyncDeleteContactTest_01();

	virtual enum TVerdict doTestStepL(void);
	};

//
//    Test step2 -- Test#74 in Unit Test Spec. Doc.  //
//
class CPhbkSyncDeleteContactTest_02 : public CTestStepPhbkSync
	{
public:
	CPhbkSyncDeleteContactTest_02();
	~CPhbkSyncDeleteContactTest_02();

	virtual enum TVerdict doTestStepL(void);
	};

//
//    Test step3 -- Test#75 in Unit Test Spec. Doc.  //
//
class CPhbkSyncDeleteContactTest_03 : public CTestStepPhbkSync
	{
public:
	CPhbkSyncDeleteContactTest_03();
	~CPhbkSyncDeleteContactTest_03();

	virtual enum TVerdict doTestStepL(void);
	};

//
//    Test step4 -- Test#76 in Unit Test Spec. Doc.  //
//
class CPhbkSyncDeleteContactTest_04 : public CTestStepPhbkSync
	{
public:
	CPhbkSyncDeleteContactTest_04();
	~CPhbkSyncDeleteContactTest_04();

	virtual enum TVerdict doTestStepL(void);
	};

//
//    Test step5 -- Test#77 in Unit Test Spec. Doc.  //
//
class CPhbkSyncDeleteContactTest_05 : public CTestStepPhbkSync
	{
public:
	CPhbkSyncDeleteContactTest_05();
	~CPhbkSyncDeleteContactTest_05();

	virtual enum TVerdict doTestStepL(void);
	};

//
//    Test step6 -- Test#78 in Unit Test Spec. Doc.  //
//
class CPhbkSyncDeleteContactTest_06 : public CTestStepPhbkSync
	{
public:
	CPhbkSyncDeleteContactTest_06();
	~CPhbkSyncDeleteContactTest_06();

	virtual enum TVerdict doTestStepL(void);
	};

//
//    Test step7 -- Test#79 in Unit Test Spec. Doc.  //
//
class CPhbkSyncDeleteContactTest_07 : public CTestStepPhbkSync
	{
public:
	CPhbkSyncDeleteContactTest_07();
	~CPhbkSyncDeleteContactTest_07();

	virtual enum TVerdict doTestStepL(void);
	};

#endif /* __TESTDELETECONTACTSTEPS_H__ */
