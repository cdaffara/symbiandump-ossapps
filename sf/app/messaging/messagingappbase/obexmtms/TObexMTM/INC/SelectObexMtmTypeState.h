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
// $Workfile: SelectObexMtmTypeState.h $
// $Author: Simonk $
// $Revision: 1 $
// $Date: 23/01/02 14:53 $
// 
//


#ifndef __SELECTOBEXMTMTYPESTATE_H__
#define __SELECTOBEXMTMTYPESTATE_H__

#include "harness.h"        //MBaseTestState

const TInt KMaxObexMtmTypeLength = 2;

class CObexClientTest;

class CSelectObexMtmTypeState : public CBase, public MBaseTestState
/*
 * A state encapsulating the selection of Obex transport type - bluetooth
 * or IR
 */
	{
public:
	CSelectObexMtmTypeState(const TDesC& aType, CObexClientTest& aClientTest);
	void StartL(TRequestStatus& aStatus);

private:
	TBufC<KMaxObexMtmTypeLength> iType;
	CObexClientTest& iClientTest;
	};


#endif // __SELECTOBEXMTMTYPESTATE_H__
