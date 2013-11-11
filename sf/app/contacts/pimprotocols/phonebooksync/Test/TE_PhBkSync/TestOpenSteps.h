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
// "Close" test group scenarios 
// 
//

/**
 @file
 @internalComponent
*/

#ifndef __TESTOPENSTEPS_H__
#define __TESTOPENSTEPS_H__

#include "TE_PhBkSyncBase.h" 

//
//    Test step1 -- Close() matched test step       //
//
class CPhbkSyncOpenTest_01 : public CTestStepPhbkSync
	{
public:
	CPhbkSyncOpenTest_01();
	~CPhbkSyncOpenTest_01();

	virtual enum TVerdict doTestStepL(void);
	};

#endif /* __TESTOPENSTEPS_H__ */
