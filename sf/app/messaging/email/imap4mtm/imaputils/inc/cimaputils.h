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

#ifndef __CIMAPUTILS_H__
#define __CIMAPUTILS_H__

#include <e32base.h>
#include <f32file.h>
#include "imappaniccodes.h"
class CImapLogger;
class CImapCharconv;

/**
@internalTechnology
@prototype
*/
class CImapUtils : public CBase
	{
public:
	IMPORT_C static void CreateL();
	IMPORT_C static void Delete();

	IMPORT_C static CImapUtils& GetRef();
	IMPORT_C RFs& Fs();
	IMPORT_C CImapLogger& Logger();
	IMPORT_C CImapCharconv& Charconv();

private:
	void ConstructL();
	CImapUtils();
	~CImapUtils();

private:
	RFs iFs;
	CImapLogger* iLogger;
	CImapCharconv* iCharconv;
	TInt iRefCount;
	};

#endif // __CIMAPUTILS_H__
