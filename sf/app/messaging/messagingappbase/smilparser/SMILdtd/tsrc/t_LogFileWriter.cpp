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


#include "t_LogFileWriter.h"


CTestDataWriter::CTestDataWriter()
	{
	}


CTestDataWriter::~CTestDataWriter()
	{
	iFile.Close();
	iFs.Close();
	}


CTestDataWriter* CTestDataWriter::NewL(const TDesC& aLogFileName)
	{
	CTestDataWriter* self = new (ELeave) CTestDataWriter();
	CleanupStack::PushL(self);
	self->ConstructL(aLogFileName);
	CleanupStack::Pop();
	return self;
	}


void CTestDataWriter::ConstructL(const TDesC& aLogFileName)
	{
	User::LeaveIfError(iFs.Connect());

	// Open the file
	TInt error = iFile.Open(iFs, aLogFileName, EFileShareAny | EFileStreamText | EFileWrite);

	if (error != KErrNone)
		{
		User::LeaveIfError(iFile.Create(iFs, aLogFileName, EFileShareAny | EFileStreamText | EFileWrite));
		}

	iWriter.Set(iFile);	 // Set up the TFileText
	}


void CTestDataWriter::WriteTest(const TDesC& aLine)
	{
	iWriter.Write(aLine);
	iFile.Flush();        // Commits the data to the file
	}
