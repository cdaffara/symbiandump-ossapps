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

#ifndef __TESTUTILS_H__
#define __TESTUTILS_H__

#include <e32test.h>
#include <s32mem.h>
#include <versit.h>
#include <vcard.h>

class CUtils : public CBase
	{
public:
	static CUtils* NewL(RTest &aTest);
	CUtils(RTest &aTest);
	~CUtils();
	void ConstructL();
		
public:
	//Utility functions
	void ReportFileErrorL(TInt aErr, const TDesC&aFileName);
	void CreateFileName(TFileName& aName, TChar aPrefix, const TDesC& aFile);
	void ExistingFileName(TFileName& aName, TChar aPrefix, const TDesC& aFile);
	void IntAndExtL(const TFileName& aInputFile, const TFileName& aOutputFile, Versit::TVersitCharSet aCharset, Versit::TVersitEncoding aEncoding);
	void TestFileL(const TDesC& aFile,Versit::TVersitCharSet aCharset=Versit::EUTF8CharSet
						, Versit::TVersitEncoding aEncoding=Versit::ENoEncoding);
																				
	void CreateAndExternaliseVCardL(const TDesC& aVCardText, const TDesC& aFileName, TFileName& aOutputFile
									, Versit::TVersitCharSet aCharset=Versit::EUTF8CharSet, Versit::TVersitEncoding aEncoding=Versit::ENoEncoding);
									
	void CreateAndExternaliseVCardL(const TDesC8& aVCardText, const TDesC& aFileName, TFileName& aOutputFile
									, Versit::TVersitCharSet aCharset=Versit::EUTF8CharSet, Versit::TVersitEncoding aEncoding=Versit::ENoEncoding);
	CVersitParser* CreateParserLC();
	void ExternaliseFileL(CVersitParser& aParser, const TDesC& aFileName);
	void InternaliseFileL(CVersitParser& aParser, const TDesC& aFileName, TBool aUseBaseParser=ETrue);
	void InternalizeFileL(CVersitParser& aParser,RFile& aInputFile);
	void CheckVCardL(const TDesC& aFile, const TDesC& aOutputFile);
	void CompareFilesL(const TDesC& aOutputFile, const TDesC& aCompareFile, const TDesC& aInputFile=KNullDesC);

private:
	RFs iFsSession;
	TBool iSessionConnected;
	CFileMan* iFm;
	RTest& iTest;				
	};


#endif
