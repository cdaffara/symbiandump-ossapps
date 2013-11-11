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

#ifndef __T_IMAPMAILSTOREBODYDATA16_H
#define __T_IMAPMAILSTOREBODYDATA16_H

#include "t_imapmailstore.h"

class CTestImapMailStoreBodyData16 : public  CTestImapMailStore
	{
private:
	TBool iBinaryCap;
public:
	CTestImapMailStoreBodyData16();
	// Tests
	void TestInitialiseBodyDataL();
#if (defined SYMBIAN_EMAIL_CAPABILITY_SUPPORT)
	void TestBodyData2L();
#endif
	void TestBodyData1L();
	virtual void SetupL();
	static CTestSuite* CreateSuiteL(const TDesC& aName);
	};
	
#endif //__T_IMAPMAILSTOREBODYDATA16_H
