/*
* Copyright (c) 2004-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:
*
*/



#ifndef __TEF_LOGGER__
#define __TEF_LOGGER__

#include <e32base.h>
#include <testexecutelog.h>

class CTEFLogger
	{
public:
	static void LogResult( const TDesC& aTestName, TInt aResult, RTestExecuteLogServ& aLogger );
	static void LogTraverse( const TDesC& aSuiteName, RTestExecuteLogServ& aLogger );
	};

#endif // __TEF_LOGGER__

