// Copyright (c) 1997-2009 Nokia Corporation and/or its subsidiary(-ies).
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
 
#if !defined(__TVGEN_H__)
#define __TVGEN_H__

#include <vcal.h>
#include <vcard.h>
#include <e32cons.h>
#include <s32stor.h>
#include <s32file.h>

class CVersitTest : public CBase
	{
public:
	CVersitTest();
	~CVersitTest();
	void StreamInL();
	void StreamOutL();
	virtual void LoadFileVersitL()=0;
	void SaveFileVersitL(CVersitParser::TVersitCharSet aDefaultCharSet = CVersitParser::EUSAsciiCharSet);
	virtual void GetSampleVersitL()=0;
	virtual void BuildVersitL()=0;
	void CheckIsValidLabelL();
public:
	CVersitParser* iParser;
protected:
	CDirectFileStore* iStore; // Store to/Restore from this store
	TBuf8<10000> iVersit;
	};

class CVCalTest : public CVersitTest
	{
public: // framework
	void LoadFileVersitL();
	void GetSampleVersitL();
	void BuildVersitL();
	};

class CVCardTest : public CVersitTest
	{
public: // framework
	void LoadFileVersitL();
	void GetSampleVersitL();
	void BuildVersitL();
	};

#endif
