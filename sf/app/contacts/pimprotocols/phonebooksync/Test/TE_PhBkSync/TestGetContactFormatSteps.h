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
// This is the header file for Phone book synchronisation
// "GetContactFormat" test group scenarios 
// 
//

/**
 @file
 @internalComponent
*/

#ifndef __TESTGETCONTACTFORMATSTEPS_H__
#define __TESTGETCONTACTFORMATSTEPS_H__

#include "TE_PhBkSyncBase.h" 

//
//    Test step1 -- Test#53 in Unit Test Spec. Doc.  //
//
class CPhbkSyncGetContactFormatTest_01 : public CTestStepPhbkSync
	{
public:
	CPhbkSyncGetContactFormatTest_01();
	~CPhbkSyncGetContactFormatTest_01();

	virtual enum TVerdict doTestStepL(void);

protected:
	void CheckContactFieldsFormatV1(RPhoneBookSession::TContactFieldsV1& aContactFormatV1);
	void CheckContactFieldsFormatV2(RPhoneBookSession::TContactFieldsV2& aContactFormatV2, TUid aPhonebook);
	void CheckContactFieldsFormatV3(RPhoneBookSession::TContactFieldsV3& aContactFormatV3, TUid aPhonebook);
	void PrintGetContactFieldsFormatV1(RPhoneBookSession::TContactFieldsV1& aContactFormatV1);
	void PrintGetContactFieldsFormatV2(RPhoneBookSession::TContactFieldsV2& aContactFormatV2);
	void PrintGetContactFieldsFormatV3(RPhoneBookSession::TContactFieldsV3& aContactFormatV3);
	};

#endif /* __TESTGETCONTACTFORMATSTEPS_H__ */
