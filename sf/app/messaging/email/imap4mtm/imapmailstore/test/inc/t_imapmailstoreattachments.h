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

#ifndef __T_IMAPMAILSTORATTACHMENTS_H
#define __T_IMAPMAILSTORATTACHMENTS_H

#include "t_imapmailstore.h"

class CTestImapMailStoreAttachments : public  CTestImapMailStore
	{
public:
	CTestImapMailStoreAttachments();
	// Tests
	void TestAttachment1L();
	virtual void SetupL();
	static CTestSuite* CreateSuiteL(const TDesC& aName);
	};
	
#endif //__T_IMAPMAILSTORATTACHMENTS_H
