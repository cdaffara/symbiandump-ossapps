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
// $Workfile: UnRegisterObexMtmTypeState.h $
// $Author: Robertl $
// $Revision: 1 $
// $Date: 22/08/02 15:09 $
// 
//


#ifndef __UNREGISTEROBEXMTMTYPESTATE_H__
#define __UNREGISTEROBEXMTMTYPESTATE_H__

#include "harness.h"        //MBaseTestState

const TInt KMaxUnRegisterObexMtmTypeLength = 2;

class CObexClientTest;

class CUnRegisterObexMtmTypeState : public CBase, public MBaseTestState
/*
 * A state encapsulating the register of an Mtm - bluetooth
 * or IR
 */
	{
public:
	CUnRegisterObexMtmTypeState(const TDesC& aType, CObexClientTest& aClientTest);
	void StartL(TRequestStatus& aStatus);

private:
	TBufC<KMaxUnRegisterObexMtmTypeLength> iType;
	CObexClientTest& iClientTest;
	};


#endif // __UNREGISTEROBEXMTMTYPESTATE_H__
