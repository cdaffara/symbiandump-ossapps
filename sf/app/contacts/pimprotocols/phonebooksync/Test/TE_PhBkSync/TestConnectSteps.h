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
// This is the header file for phone book synchronisation 
// "Connect" test group scenarios 
// 
//

/**
 @file
 @internalComponent
*/

#ifndef __TESTCONNECTIONSTEPS_H__
#define __TESTCONNECTIONSTEPS_H__

#include "TE_PhBkSyncBase.h" 

//
//    Test step1 -- Test#1 in Unit Test Spec. Doc.  //
//
class CPhbkSyncConnectTest_01 : public CTestStepPhbkSync
	{
public:
	CPhbkSyncConnectTest_01();
	~CPhbkSyncConnectTest_01();

	virtual enum TVerdict doTestStepL(void);
	};

//
//    Test step2 -- Test#2 in Unit Test Spec. Doc.  //
//
class CPhbkSyncConnectTest_02 : public CTestStepPhbkSync
	{
public:
	CPhbkSyncConnectTest_02();
	~CPhbkSyncConnectTest_02();

	virtual enum TVerdict doTestStepL(void);
	};

//
//    Test step3 -- Test#3 in Unit Test Spec. Doc.  //
//
class CPhbkSyncConnectTest_03 : public CTestStepPhbkSync
	{
public:
	CPhbkSyncConnectTest_03();
	~CPhbkSyncConnectTest_03();

	virtual enum TVerdict doTestStepL(void);
	};

//
//    Test step4 -- Test#4 in Unit Test Spec. Doc.  //
//
class CPhbkSyncConnectTest_04 : public CTestStepPhbkSync
	{
public:
	CPhbkSyncConnectTest_04();
	~CPhbkSyncConnectTest_04();

	virtual enum TVerdict doTestStepL(void);
	};

//
//    Test step5 -- Test#5 in Unit Test Spec. Doc.  //
//
class CPhbkSyncConnectTest_05 : public CTestStepPhbkSync
	{
public:
	CPhbkSyncConnectTest_05();
	~CPhbkSyncConnectTest_05();

	virtual enum TVerdict doTestStepL(void);
	};

//
//    Test step6 -- Test#6 in Unit Test Spec. Doc.  //
//
class CPhbkSyncConnectTest_06 : public CTestStepPhbkSync
	{
public:
	CPhbkSyncConnectTest_06();
	~CPhbkSyncConnectTest_06();

	virtual enum TVerdict doTestStepL(void);
	};

//
//    Test step7 -- Test#7 in Unit Test Spec. Doc.  //
//
class CPhbkSyncConnectTest_07 : public CTestStepPhbkSync
	{
public:
	CPhbkSyncConnectTest_07();
	~CPhbkSyncConnectTest_07();

	virtual enum TVerdict doTestStepL(void);
	};

//
//    Test step8 -- Test#8 in Unit Test Spec. Doc.  //
//
class CPhbkSyncConnectTest_08 : public CTestStepPhbkSync
	{
public:
	CPhbkSyncConnectTest_08();
	~CPhbkSyncConnectTest_08();

	virtual enum TVerdict doTestStepL(void);
	};

class CPhbkSyncConnectTest_09 : public CTestStepPhbkSync
	{
public:
	CPhbkSyncConnectTest_09();
	~CPhbkSyncConnectTest_09();

	virtual enum TVerdict doTestStepL(void);
	};

//=========================================
//               OOM Test Step
//=========================================
class CPhbkSyncOOMConnectTest_01 : public CTestStepPhbkSync
	{
public:
	CPhbkSyncOOMConnectTest_01();
	~CPhbkSyncOOMConnectTest_01();

	virtual enum TVerdict doTestStepL(void);
	};


#endif /* __TESTCONNECTIONSTEPS_H__ */
