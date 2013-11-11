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
// popservermtm module header
// 
//

/**
 @file
 @internalComponent
 @released
*/

#ifndef __POPS_H__
#define __POPS_H__

#include <e32base.h>
#include <imsk.h>
#include <msvstd.h>
#include <cimcaf.h>

class CImPop3Settings;
class CPop3IdleTimer;
class CImIAPPreferences;
class CImPop3Capa;
#if (defined SYMBIAN_EMAIL_CAPABILITY_SUPPORT)
class CPopAuthMechanismHelper;
#endif


const TInt KImPopSessionPriority = 1;

//
// CImPop3Session
//
// A Pop3 Session
//
//
class CImPop3Session : public CMsgActive
    {
public:
	static CImPop3Session* NewL(RSocketServ& aServ, CImConnect& aConnect);
	~CImPop3Session();		// destructor kills socket server

	void ConnectL(CImPop3Settings* aPopSettings, const CImIAPPreferences& aPrefs,TRequestStatus& aStatus);
	void Quit(TRequestStatus&);
	void Waiting(TRequestStatus&);

	void SetMessageArray(TInt32* aIdArray, TUint aNoMessages);
	TInt32* MessageArray();
	TInt GetNumMessages();
	TInt MessageNo(TMsvId);
	CImTextServerSession* TextServerSession();

	void SetPending();
	TBool IsPending();
	void SetOpNotPending();
	TBool IsConnectedToInternet();
	CImCaf* GetCafL(RFs& aFs);
	TBool PipeliningSupport();

private:
	CImPop3Session();	// constructor
	void ConstructL(RSocketServ& aServ, CImConnect& aConnect);

	void DoRunL();
	void DoCancel();
	void DoComplete(TInt&);

    void ChangeStateL();				// the state machine engine
	void ConstructApopL();
	void SetOpPending();
	TInt GetPopError();
	void GetCapabilitiesL();
#if (defined SYMBIAN_EMAIL_CAPABILITY_SUPPORT)
	void SelectAuthExtensionProfileL();
#endif	

private:
	enum				// States of "session" state machine
		{
		EConnectingToPop,
		EPopCapabilities,
#if (defined SYMBIAN_EMAIL_CAPABILITY_SUPPORT)
		ESaslAuthInProgress,
		ESaslAuthIsDone,
#endif		
		EWaitingForReply,
		EAuthorisingUser,
		EAuthorisingPass,
		EAuthorisingApop,
		EPopConnected,
		EStopPop,
		ERequestingTLS,
		ESettingTLS,
		ELoggingEvent
		};
private:
	TInt				iState;
	TInt				iNextState;
	TInt				iCompleted;
	TBool				iOpPending;

	TBool				iSocketConnected;

	HBufC8*				iPopApop;
	HBufC8*				iConnectReply;

	CImPop3Settings*	iPopSettings;
	
	TInt				iNoMessages;
	TInt32*				iIdTab;

    CImTextServerSession*			iSocket;
	CImCaf*				iCaf;

	CImPop3Capa*		iPopCapabilities;
	TBuf8<KImMailMaxBufferSize>	iResponseBuffer;
#if (defined SYMBIAN_EMAIL_CAPABILITY_SUPPORT)
	TBool iSaslAuthLogin;
	CPopAuthMechanismHelper* iPopAuthHelper;
#endif

public:
	TInt MaxHeaders();
    };

#endif
