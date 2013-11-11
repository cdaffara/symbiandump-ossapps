// Copyright (c) 2004-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef __COBEXTESTFILENAMEANDHEADERS_H__
#define __COBEXTESTFILENAMEANDHEADERS_H__

#include <e32base.h>

class CObexHeaderList;

class CObexTestFilenameAndHeaders : public CBase
	{
public:
	static CObexTestFilenameAndHeaders* NewL();
	~CObexTestFilenameAndHeaders();

private:
	CObexTestFilenameAndHeaders();
	
public:
	TFileName iFileName;
	CObexHeaderList* iHeaderList;
	};

#endif // __COBEXTESTFILENAMEANDHEADERS_H__
