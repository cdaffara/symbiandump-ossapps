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
// $Workfile: ObexFileDebugReceiveState.h $
// $Author: Simonk $
// $Revision: 2 $
// $Date: 24/01/02 16:37 $
// 
//

#ifndef __OBEXFILEDEBUGRECEIVESTATE_H__
#define __OBEXFILEDEBUGRECEIVESTATE_H__

#include "harness.h"        //MBaseTestState

// forward declarations
class CObexClientTest;


//
// CObexFileDebugReceiveState
//
class CObexFileDebugReceiveState : public CBase, public MBaseTestState
/*
 * A state class for use in the receipt of obex objects.  Present functionality actually
 * just consists of copying a file from the source path to the destination directory
 */
	{
public:
	CObexFileDebugReceiveState(const TDesC& aSource, const TDesC& aDestination, CObexClientTest& aClientTest);
	void StartL(TRequestStatus& aStatus);

private:
	CObexClientTest& iClientTest;
    TFileName iSource;
    TFileName iDestination;
	};

#endif      //__OBEXFILEDEBUGRECEIVESTATE_H__
