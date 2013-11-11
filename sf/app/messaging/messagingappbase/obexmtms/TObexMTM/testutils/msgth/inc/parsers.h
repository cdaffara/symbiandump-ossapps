// Copyright (c) 2003-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef __PARSERS_H
#define __PARSERS_H

#include "testframeparse.h"
#include "harness.h"
#include <COMMDB.H>	    // CCommsDatabase

//
//
// CMainSectionParser
//

class CMainSectionParser : public CBaseSectionParser
	{
public:
	IMPORT_C static CMainSectionParser* NewL(CMainTestHarness& aMainTest, CTestScript& aScript);

protected:
	IMPORT_C void ConstructL();
	IMPORT_C CMainSectionParser(CMainTestHarness& aMainTest, CTestScript& aScript);
	IMPORT_C void LogCommentL(const TDesC& aComment);

private:
	CMainTestHarness& iMainTest;
	};



//
//
// CMsvUtilitiesParser
//

class CMsvUtilitiesParser : public CBaseCommandParser
	{
public:
	IMPORT_C void ProcessL();
	IMPORT_C static CMsvUtilitiesParser* NewL(CMsvClientTest& iMsvClientTest);
private:
	void ConstructL();
	CMsvUtilitiesParser(CMsvClientTest& aMsvClientTest);
	CMsvClientTest& iMsvClientTest;
	};




//
//
// CMsvSelectParser
//

class CMsvSelectParser : public CBaseCommandParser
	{
public:
	IMPORT_C void ProcessL();
	IMPORT_C static CMsvSelectParser* NewL(CMsvClientTest& iMsvClientTest);
private:
	void ConstructL();
	CMsvSelectParser(CMsvClientTest& aMsvClientTest);
	CMsvClientTest& iMsvClientTest;
	};



//
//
// CMsvOperationParser
//

class CMsvOperationParser : public CBaseCommandParser
	{
public:
	IMPORT_C void ProcessL();
	IMPORT_C static CMsvOperationParser* NewL(CMsvClientTest& iMsvClientTest);
private:
	void ConstructL();
	CMsvOperationParser(CMsvClientTest& aMsvClientTest);
	CMsvClientTest& iMsvClientTest;
	};



//
//
// CMsvTestEntryCommandParser
//

class CMsvTestEntry;

class CMsvTestEntryCommandParser : public CBaseCommandParser
	{
public:
	IMPORT_C void ProcessL();
	IMPORT_C static CMsvTestEntryCommandParser* NewL(CMsvTestEntry& aTestEntry);

private:
	void ConstructL();
	CMsvTestEntryCommandParser(CMsvTestEntry& aTestEntry);

	TBool GetBooleanArgumentL(); // Leaves if arguments consist of anything apart for a single boolean

private:
	CMsvTestEntry& iTestEntry;
	};

//
//
// CMsvTestEntrySectionParser
//

class CMsvTestEntrySectionParser : public CBaseSectionParser
	{
public:
	IMPORT_C static CMsvTestEntrySectionParser* NewL(const TDesC& aSectionName, CTestScript& aScript, CMsvTestEntry& aTestState);
	IMPORT_C ~CMsvTestEntrySectionParser();
	IMPORT_C void LogCommentL(const TDesC& aComment);

protected:
	IMPORT_C void ConstructL(const TDesC& aSectionName);
	IMPORT_C CMsvTestEntrySectionParser(CTestScript& aScript, CMsvTestEntry& aTestState);

protected:
	CMsvTestEntry& iTestState;
	};



#endif
