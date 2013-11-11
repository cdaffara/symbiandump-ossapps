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

#ifndef __IMSMSEND_H__
#define __IMSMSEND_H__

#ifndef __IMSK_H__
#include <imsk.h>				// for KImMailMaxBufferSize
#endif

#include <cdbcols.h>
#include "smtpauthhelpers.h"

// forward declarations for CImSmtpFile
class CImHeader;
class CImSendMessage;
class TImImailFileProgress;
class CMsgImOutboxSend;
class CImIAPPreferences;
class CImConnect;
class CImLogMessage;

class CImSmtpFile : public CMsgActive
	{
public: 
	static	CImSmtpFile* NewL(CImTextServerSession& aSession,
	                          CMsvServerEntry& aServerEntry,
	                          TTime& aTime,
	                          TBuf8<KImMailMaxBufferSize>& aSmtpBuffer,
	                          CSmtpSettings& aSmtpSettings,
	                          TBool aIsBccRcpt);
	~CImSmtpFile();

	void StartL(TRequestStatus& aClientStatus);
	void GetProgress(TImImailFileProgress& aFileProgress);		// return progress of sending current file
    void DoCancel();

	TInt TotalMsgSizeL();
	void SetBytesToSend(TInt aNumBytes);
	TInt BytesToSend() const;

protected:
    void DoRunL();

private: 
	CImSmtpFile(CImTextServerSession& aSocket,TBuf8<KImMailMaxBufferSize>& aSmtpBuffer,CMsvServerEntry &aServerEntry);  		// constructor
	void ConstructL(TTime& aTimeNow,CSmtpSettings& aSettings,TBool aIsBccRcpt);
	void GetHeaderFromStoreL(CMsvStore& aStore, CMsvServerEntry&, TTime&);
	TBool NextRecipientL();					// returns pointer to next recipient address in header field
	TInt SelectNextStateL();					// chooses next state to go to
	void SendOneLineOfData();				// send 1 line of RFC822 message to remote server
	TInt SmtpFilePositiveResponse(TInt);
	void ChangeStateL();						// initiates the next state operation
	TInt SmtpFileError(TInt);				// Decode SMTP 3 digit response code into return code
	TInt State();							// return current state of state machine
	void DoComplete(TInt&);
	void SelectBodyEncodingTypeL(TMsgOutboxBodyEncoding anSettingsEncodingType, 
								  TMsgOutboxBodyEncoding anHeaderEncodingType);
	void SendAndQueueRead(const TDesC8&);
	TBool GetCurrentTextLine();

private: 	
	CImTextServerSession&			iSocket;
	CMsvServerEntry&				iServerEntry;
	TMsvId							iEntryId;
	TInt							iSuspendCheck;
	TBuf8<KImMailMaxBufferSize>&	iSmtpBuffer;		// SMTP data will not exceed 1000 characters
	
	TInt							iBytesToSend;
	TInt							iBytesSent;

	enum TRcptType
		{
		ERcptTo,
		ERcptCc,
		ERcptBcc
		};

	enum TSmtpFileStates		// legal states that the CImSmtpFile object's state machine may be set to...
		{
		EResettingSmtp,
		EMailFromSmtp,
		ERcptToSmtp,
		EBeginData,
		ESendData,
		EEndData
		};

    CImHeader*				iHeader;
	CImSendMessage*			iSendMessage;

    TInt					iState;
	TInt					iCompleted;		// termination status of server class, 
											// e.g. KErrNone

	CDesCArray* 			iRecipientArray;  // points to member array owned by iHeader
	TInt					iRecipientIndex;
	TInt					iRecipientType;
	TPtrC   				iRecipient;		// local copy of 1 recipient Email address

	TBool					iSmtpMultiLineResponse;
	TBool					iSmtpLastMultiLineResponse;
	TBool					iMoreRfc822Data;

	TImSendMethod			iBodyType;
	TImSMTPSendCopyToSelf	iSendCopyToSelf;
    };


class CImSmtpSession : public CMsgActive
    {
	public: 
		static CImSmtpSession* NewL(CMsvServerEntry& aServerEntry, CSmtpSettings& aSettings, RSocketServ& aServ, CImConnect& aConnect, TMsvId aServiceId);
		~CImSmtpSession();
	
	public:
		void ConnectL(TRequestStatus& aStatus);
		void SendFilesL(CMsgImOutboxSend& aSendFiles, TRequestStatus& aStatus);
		void QuitL(TRequestStatus& aStatus);
		TImImailFileProgress FileProgress();		// Return snapshot of session progress to client
		inline TBool IsConnected();
		TInt GetConnectionIAP();			// The value of the connecting IAP
		inline TInt GetConnectionStage();			// The value of the connecting stage

	private:				// CActive
		void DoCancel();			// Allows client to stop the SMTP session prematurely

	private:				// CMsgActive
		virtual void DoRunL();
		virtual void DoComplete(TInt& aStatus);

	private:
		CImSmtpSession(CMsvServerEntry& aServerEntry, CSmtpSettings& aSettings, TMsvId aServiceId);
		void ConstructL(RSocketServ& aServ, CImConnect& aConnect);

	private:
		void SendFileL();						
		void DoStateL();						// the state machine engine
		TInt NextStateL();						// logic which calculates next state from result of current operation
		TInt NextStateFromWaitingForReply();
		TInt NextStateFromAuthorisingSmtpL();
		TInt NextStateFromSendStarttls();
		TInt NextStateFromSettingSecurityL();
		TInt NextStateFromAuthInProgressL();
		TInt NextStateFromResetSmtp();
		TInt SmtpSessionError(const TInt aSmtpErrorCode);		// Decode SMTP 3 digit response code into return code
		TBool GetCurrentTextLine();
		HBufC* CreateFormattedAddressLC(const TDesC& aString, const TDesC& aAlias);
		TBool SelectNextSMTPAuthProfileL();
		void GetIpAddress(TDes8& aAddress);
		CSmtpAuthMechanismHelper::TSmtpAuthProfileFlag NextSMTPAuthProfile(CSmtpAuthMechanismHelper::TSmtpAuthProfileFlag aPreviousProfile);
		TInt NextStateFromNextFile();
		TBool CommandAccepted();
		void UpdateAuthorisingInfo();

	private:
		enum TSessionType
			{
			ESmtpSession,			// SMTP   session
			EEnhancedSmtpSession,	// ESMTP  session
			ESecureSession          // ESMTP + TLS session
			};

	private:
		// input references to input arguments...
		CMsvServerEntry& iServerEntry;		// handle to the current server entry object
		
		CMsgImOutboxSend* iSendFiles;			// handle to owner object. Not owned by this class
		CSmtpSettings& iSettings;

		// SMTP service ID
		TMsvId iServiceId;

		TInt iState;
		TInt iCompleted;	// stores completion code from previous session state
		CImTextServerSession* iSocket;	// handle to the socket session (IMSK.DLL)
		CImSmtpFile* iSmtpFile;	// handle to the Smtp File object
		TBuf8<KImMailMaxBufferSize>	iSmtpBuffer;	// stores SMTP command string & response
													// from server
		TInt iLastSmtpCode;		// Stores the last STMP response code
		TBool iRetryAuthWithHostname;	// Flag to indicate whether to retry Smtp Auth with name instead of IP Address
		
		TInt iBccRcptIndex;
		TBool iToRcptHeaderUpdated;
		TBool iBccRcptFound;
		TBool iFinishedWithBccRcpts;

		TTime iTimeNow;				// stores time/date when each message file is sent

		TBool iSmtpMultiLineResponse;
		TBool iSmtpLastMultiLineResponse;
		TBool iSocketIsConnected;

		// ESMTP variables
		TSessionType iThisSession;
		TBool iEsmtpSpokenHere;
		TBool i8BitMimeAcceptedHere;
		TBool iSizeAcceptedHere;
		TBool iStartTlsAcceptedHere;
		TUint32 iInterAddress;
		CImLogMessage* iLogMessage;

		// Used to track the progress of sent messages
		TInt iTotalBytesToSend;
		TInt iNumMsgsToSend;
		TInt iNumMsgsSent;

		TInt iSupportedAuthProfiles; // for TSmtpAuthProfileFlags
		CSmtpAuthMechanismHelper::TSmtpAuthProfileFlag iCurrentAuthProfile;
		CSmtpAuthMechanismHelper* iSmtpAuthHelper;

		TBool iOperationComplete;
    };

// inline functions
TBool CImSmtpSession::IsConnected()			{   return iSocketIsConnected;	}
TInt CImSmtpSession::GetConnectionStage()	{	return iSocket->GetConnectionStage();	}

#endif
