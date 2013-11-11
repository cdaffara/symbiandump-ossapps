// Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef __T_TEST_DECODE_STEP_H__
#define __T_TEST_DECODE_STEP_H__
 
#include <test\testexecutestepbase.h>
#include "emailtestutils.h"
#include "smstestutils.h"
#include <stdlib.h>
#include <msvapi.h>
#include <smuthdr.h> 
#include <iapprefs.h>
#include "t_decodeserver.h"
#include "decodeimapclient.h"

const TInt KMaxBufferSize = 5000;


enum TBufferType 
	{
	EUndefinedBuffer = -1,
	EBodyBuffer = 0,
	EAttachmentBuffer = 1
	};

class TBuffer
	{
public:
	TBuf8< KMaxBufferSize > iBuffer;
	TBufferType iBufferType;
	TBool iAllowJunkAtEnd;
	};

//
// CTestDecode
//

class CTestDecode : public CTestStep , MImapTestEventHandler
	{
public:
	CTestDecode( );
	virtual ~CTestDecode( );
	virtual TVerdict doTestStepPreambleL( );
	virtual TVerdict doTestStepPostambleL( );
	virtual TVerdict doTestStepL( );
private:
	void GetDetailsFromIniFileL( );
	void CreateImapAccountL( );
	void TraverseL( const TMsvId aTMsvId );
	TVerdict CheckBuffer( const TDesC8& aActualBuffer, 
						  const TDesC8& aExpectedBuffer, 
						  TBool aAllowJunkAtEnd );
	virtual void TestComplete( TInt aErrorCode );
	
	TBufferType ConvertToBufferType( const TDesC& aBufferTypeDesc );
	
private:
	CSpoofServer* iSpoofServer;
	CActiveImapClient* iImapClient;
	CConsoleBase* iConsole;
	CActiveScheduler* iScheduler;
	TMsvId	iImapService;
	CEmailTestUtils* iTestUtils;
	CMsvSession* iSession;
	TDummySessionObserver* iSessionObserver;
	TPtrC16 iScriptFile;
	TBool iFetchWholeMessage;

	CDesC8ArraySeg* iActualPartsBuffer;
	CArrayFixSeg< TBuffer >* iExpectedPartsBuffer;

	RFs iFs;
	};

 		
_LIT( KTestDecode, "TestDecode" );
	
#endif  //__T_TEST_DECODE_STEP_H__
