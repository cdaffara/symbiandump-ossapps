// Copyright (c) 1998-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#if !defined(__IMSKSCR_H__)
#define __IMSKSCR_H__

#if !defined(__E32BASE_H__)
#include <e32base.h>
#endif

#if !defined(__F32FILE_H__)
#include <f32file.h>
#endif

// Enable Scripting in Debug Builds only
#ifdef _DEBUG
#define __IMSK_SCRIPTING
#endif


const TInt KFileSegLength=1000;

class CImTextServerScript : public CActive
/**
@internalComponent
@released
*/
	{
public:
	static CImTextServerScript *NewL(TInt aPortNum);
	~CImTextServerScript();
	
	void RetrieveResponse(TDes8& theBuffer, TRequestStatus& aStatus);
	void RunL();
	void DoCancel();

private:
	CImTextServerScript();
	void ConstructL(TInt aPortNum);

private:
	RFile							iFile;
	RFs								iFs;
	TBuf8<KFileSegLength>			iData;
	TBuf8<KFileSegLength>			iNextFileSeg;
	TRequestStatus*					iReportStatus;

//
//  MRG 30-11-98.
//  For SMTP bug fix SW1-656, need to return 8 bit data to
//  the client, TFileText makes a 16 bit read!
//
#ifndef _UNICODE
	TFileText	iScriptFileText;        //  Use TFileText for 8 bit read
#else
    TInt ReadLine();                         //  Read one line of ASCII in Unicode build..
#endif

	};

#endif
