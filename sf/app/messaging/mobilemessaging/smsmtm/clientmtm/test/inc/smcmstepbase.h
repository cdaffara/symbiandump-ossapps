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

#ifndef __SMCMSTEPBASE_H__
#define __SMCMSTEPBASE_H__

#include "smcmserver.h"

#include <mtmdef.h>
#include <stdlib.h>
#include <smsclnt.h>
#include <systemmonitor/systemmonitor.h>
#include <test/testexecutestepbase.h>
#include <txtfmlyr.h>
#include <txtrich.h>

// User Data
_LIT( KUserDataOriginal, "Welcome to the Forward and Reply test harness" );
_LIT( KUserDataReply, "This is the reply to the original message" );
_LIT( KUserDataForward, "This is the text added to the forward message" );
_LIT( KUserDataFind, "This is a send test.\nдце\n Cheers, Mico\n" );

// Phone numbers

_LIT( KScriptItemSC1, "SC" );
_LIT( KScriptItemSC1Def, "+447785016005" );

_LIT( KScriptItemSC2, "radiolinja0" );
_LIT( KScriptItemSC2Def, "+358508771010" );

_LIT( KScriptItemSC3, "radiolinja1" );
_LIT( KScriptItemSC3Def, "+358508771010" );

const TInt KMaxBodyLength = KMsvMaxFindTextLength ;

const TInt KUTC0 = 0;
const TInt KUTCMin8 = -8;

enum TMessageOperation
	{
	EOperationNone,
	EOperationReply,
	EOperationForward
	};


class TMessageSettings
{
public:
	TMessageOperation iOperation;
	TMsvId iOriginalBoxId;
	CSmsPDU::TSmsPDUType iOriginalSmsPDUType;
	TInt iOriginalBioType;
};

class CSMCMStepBase : public CTestStep
	{
public:
	virtual ~CSMCMStepBase( );
	CSMCMStepBase( );
	virtual TVerdict doTestStepPreambleL( );

protected:
	void ChangeLocale( TInt aHours );

	TMsvId CreateEmailSmsMessageL( const TDesC& aEmailMessageData );
	TMsvId CreateMessageAndEntryL( const TDesC& aUserData, TMessageSettings aSettings );
	HBufC* CreateRandomLC( TInt aLen ) const;
	TVerdict PerformTestL( TMsvId aOriginalMessageId, 
						   const TDesC& aNewText, 
						   const TMessageSettings& aSettings );
	TVerdict CompareMessagesL( TMsvId aOriginalMessageId, 
							   TMsvId aTargetMessageId, 
							   const TMessageSettings& aSettings );
	void SetSettingsL( );
private:
	TMsvId PerformForwardOrReplyOperationL( TMessageOperation aOperation, 
							  				TMsvId aInFolder );							   
protected:
	CClientMtmRegistry*     iClientMtmRegistry;
	TMsvDummyObserver*      iDummyObserver;
	CMsvSession*            iServerSession;
	CSmsClientMtm*          iSmsClientMtm;
	CMsvEntry*              iMsvEntry;
	TMsvId                  iSmsServiceId;

	CSmsTestUtils*			iSmsTestUtils;
	CActiveScheduler*		iScheduler;
	};

#endif // __SMCMSTEPBASE_H__
