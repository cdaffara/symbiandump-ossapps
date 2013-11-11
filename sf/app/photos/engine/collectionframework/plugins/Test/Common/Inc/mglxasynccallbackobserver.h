/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:   Album collection plugin definition
*
*/



#ifndef C_GLXASYNCCALLBACKOBSERVER_H
#define C_GLXASYNCCALLBACKOBSERVER_H

// INCLUDES

#include <e32cmn.h>

// FORWARD DECLARATIONS

class CGlxAsyncNotifier;

// CLASS DECLARATION

class MGlxAsyncCallBackObserver
	{
	public:
		virtual ~MGlxAsyncCallBackObserver() {};
		
		virtual void AsyncCallBackCompleted(CGlxAsyncNotifier* aPtr) = 0;
	};

#endif
