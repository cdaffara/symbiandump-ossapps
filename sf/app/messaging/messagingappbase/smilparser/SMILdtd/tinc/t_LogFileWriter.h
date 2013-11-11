// Copyright (c) 2001-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// This class contains the functionality required to print
// test data to a file.
// 
//


#ifndef __T_LOGFILEWRITER_H
#define __T_LOGFILEWRITER_H


#include <e32base.h>
#include <f32file.h>


class CTestDataWriter : public CBase
	{
public:
	static CTestDataWriter* NewL(const TDesC& aLogFileName);
	~CTestDataWriter();

	void WriteTest(const TDesC& aLine);

private:
	CTestDataWriter();
	void ConstructL(const TDesC& aLogFileName);

private:
	RFs         iFs;
	RFile       iFile;
	TFileText   iWriter;
	};


#endif
