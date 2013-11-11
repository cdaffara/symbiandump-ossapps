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
// $Workfile: RegisterObexMtmTypeState.h $
// $Author: Robertl $
// $Revision: 2 $
// $Date: 22/08/02 17:15 $
// 
//


#ifndef __REGISTEROBEXMTMTYPESTATE_H__
#define __REGISTEROBEXMTMTYPESTATE_H__

#include "harness.h"        //MBaseTestState

const TInt KMaxRegisterObexMtmTypeLength = 2;

class CObexClientTest;

class CRegisterObexMtmTypeState : public CBase, public MBaseTestState
/*
 * A state encapsulating the register of an Mtm - bluetooth
 * or IR
 */
	{
public:
	CRegisterObexMtmTypeState(const TDesC& aType, CObexClientTest& aClientTest);
	void StartL(TRequestStatus& aStatus);

private:
	TBufC<KMaxRegisterObexMtmTypeLength> iType;
	CObexClientTest& iClientTest;
	};


#endif // __REGISTEROBEXMTMTYPESTATE_H__
