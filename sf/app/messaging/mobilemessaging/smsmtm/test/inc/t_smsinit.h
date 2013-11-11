// Copyright (c) 1999-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include "smstestbase.h"

class CSmsInit : public CSmsTestBase
	{
public:
	static CSmsInit* NewL(RTest& aTest, TUint aCreationFlags);
	~CSmsInit();

private:
	CSmsInit();
	void ConstructL(RTest& aTest, TUint aCreationFlags);

	void RunL() {};
	void DoCancel() {};

	void ShowMenuL();
	void RunAutoL() {ShowMenuL();}
	};
