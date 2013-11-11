/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies). 
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

#ifndef __APAID_H__
#define __APAID_H__

#include <e32base.h>
#include <apadef.h>

class TApaAppInfo
	{
public:
	TApaAppInfo() { mConstructCount++; iFullName.Copy(_L("")); }
public:
	/** The full path name of the application DLL. */
	TFileName iFullName;
	
	static TInt mConstructCount;
	};

#endif

// EOF
