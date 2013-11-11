// Copyright (c) 2004-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Client MTM for the POP3 protocol
// 
//


#if !defined (__POPCMTM_H__)
#define __POPCMTM_H__

#if !defined (__MIUTMSG_H__)
#include <miutmsg.h>
#endif
#if !defined (__MIUTPARS_H__)
#include <miutpars.h>		//TImMessageField
#endif
#if !defined (__POP3CMDS_H__)
#include <pop3cmds.h>		// KPOP3MTMConnect etc
#endif
#if !defined (__POP3SET_H__)
#include <pop3set.h>		// KPOP3MTMConnect etc
#endif

class CImPOP3GetMail;
class CImEmailAutoSend;
 
//  Pop3 Client MTM  //
class CPop3ClientMtm : public CBaseMtm
/** POP3 client MTM interface.

Most of the functions in this class are implementations of the virtual functions 
defined by the base class CBaseMtm. 
@publishedAll
@released
*/
	{
/** 
@deprecated
*/

public:
	enum TImPopcNewEmailHeaderType
		{
		EForwardedEmail,
		EReplyToEmail,
		EReceiptToEmail
		};

public:
	IMPORT_C static CPop3ClientMtm* NewL(CRegisteredMtmDll& aRegisteredMtmDll, CMsvSession& aMsvSession);
	~CPop3ClientMtm();

	void SaveMessageL();
	void LoadMessageL();

	IMPORT_C void StoreSettingsL();
	IMPORT_C void RestoreSettingsL();

	CMsvOperation* ReplyL  (TMsvId aId, TMsvPartList aPartlist, TRequestStatus& aCompletionStatus);
	CMsvOperation* ForwardL(TMsvId aId, TMsvPartList aPartList, TRequestStatus& aCompletionStatus);

	TUint ValidateMessage(TMsvPartList aPartList);
	TMsvPartList Find(const TDesC& aTextToFind, TMsvPartList aPartList);

	void AddAddresseeL(const TDesC& aRealAddress);
	void AddAddresseeL(const TDesC& aRealAddress, const TDesC& aAlias);
	void RemoveAddressee(TInt aIndex);

	IMPORT_C void SetSettingsL(const CImPop3Settings& aSettings);
	IMPORT_C const CImPop3Settings& Settings() const; 

	// --- RTTI functions ---
	TInt QueryCapability(TUid aCapability, TInt& aResponse);
	void InvokeSyncFunctionL(TInt aFunctionId, const CMsvEntrySelection& aSelection, TDes8& aParameter);
	CMsvOperation* InvokeAsyncFunctionL(TInt aFunctionId, const CMsvEntrySelection& aSelection, TDes8& aParameter, TRequestStatus& aCompletionStatus);

	// Attachment functions to support the SendAs API
	IMPORT_C virtual void AddAttachmentL(const TDesC& aFilePath, const TDesC8& aMimeType, TUint aCharset, TRequestStatus& aStatus);
	IMPORT_C virtual void AddAttachmentL(RFile& aFile, const TDesC8& aMimeType, TUint aCharset, TRequestStatus& aStatus);
	IMPORT_C virtual void AddLinkedAttachmentL(const TDesC& aFilePath, const TDesC8& aMimeType, TUint aCharset, TRequestStatus& aStatus);
	IMPORT_C virtual void AddEntryAsAttachmentL(TMsvId aAttachmentId, TRequestStatus& aStatus);	
	EXPORT_C virtual void CreateAttachmentL(const TDesC& aFileName, RFile& aAttachmentFile, const TDesC8& aMimeType, TUint aCharset, TRequestStatus& aStatus);
	IMPORT_C virtual void CreateMessageL(TMsvId aServiceId);

	IMPORT_C virtual TMsvId DefaultServiceL() const;
	IMPORT_C virtual void RemoveDefaultServiceL();
	IMPORT_C virtual void ChangeDefaultServiceL(const TMsvId& aService);

protected:
	CPop3ClientMtm(CRegisteredMtmDll& aRegisteredMtmDll, CMsvSession& aMsvSession);
	void ConstructL();
	void ContextEntrySwitched();

private:
	TBool ValidateAddress(const TPtrC& anAddress);
	void SendOnNextConnectionL();
	TMsvPartList DoFindL(const TDesC& aTextToFind, TMsvPartList aPartList);
	CMsvEntrySelection* FilteredChildrenLC(TBool aNewOnly, TInt aMaxEmailSize);
	CMsvEntrySelection* FilteredSelectionLC(TInt aMaxEmailSize, const CMsvEntrySelection& aSelection);
		


private:
	CImPop3Settings* iImPop3Settings;
	TImMessageField iTImMessageField;
	CImHeader* iHeader;

	HBufC* iEmailAddressFormatString;	// defines format of email address used by "Send as" API eg _LIT("/"%S/" <%S>")
	CImEmailOperation* iImEmailOperation;
	CImPOP3GetMail* iImPOP3GetMail;
	};


class CImPOP3GetMail : public CMsvOperation
/** Encapsulates an operation to copy/move POP3 emails from the remote inbox to 
any local folder.

Functionality is provided to:

1. copy/move any new emails

2. copy/move a selection of emails

3. copy/move all emails

4. in the Remote Inbox to a local folder.

There are three cases for copying/moving the emails from the Remote Inbox:

1. the user is connected to the account so just copy/move the emails

2. the user is not connected to the account so connect, copy/move the emails, 
and then disconnect

3. the user is not connected to the account so connect, copy/move the emails, 
and then stay online

Objects of this type are returned from CPop3ClientMtm::InvokeAsyncFunctionL() 
when a suitable get command is requested. 
@publishedAll
@released
*/
	{
public:
	/** Defines types of get mail operation. */
	enum TImPOP3GetMailType
	{
	/** Connect and copy new mail and stay online. */
		EConnectAndCopyNewMailAndStayOnline,
	/** Connect and copy new mail and disconnect. */
		EConnectAndCopyNewMailAndDisconnect,
	/** Connect and move new mail and stay online. */
		EConnectAndMoveNewMailAndStayOnline,
	/** Connect and move new mail and disconnect. */
		EConnectAndMoveNewMailAndDisconnect,
	/** Connect and copy mail selection and stay online. */
		EConnectAndCopyMailSelectionAndStayOnline,
	/** Connect and copy mail selection and disconnect. */
		EConnectAndCopyMailSelectionAndDisconnect,
	/** Connect and move mail selection and stay online. */
		EConnectAndMoveMailSelectionAndStayOnline,
	/** Connect and move mail selection and disconnect. */
		EConnectAndMoveMailSelectionAndDisconnect,
	/** Connect and copy all mail and stay online. */
		EConnectAndCopyAllMailAndStayOnline,
	/** Connect and copy all mail and disconnect. */
		EConnectAndCopyAllMailAndDisconnect,
	/** Connect and move all mail and stay online. */
		EConnectAndMoveAllMailAndStayOnline,
	/** Connect and move all mail and disconnect. */
		EConnectAndMoveAllMailAndDisconnect
	};

public:
	IMPORT_C CMsvOperation* GetMailL(CPop3ClientMtm& aPop3ClientMtm, TRequestStatus& aObserverRequestStatus, const CMsvEntrySelection& aMsvEntrySelection, TDes8& aPop3GetMailInfo, TImPOP3GetMailType aPOP3GetMailType);
	IMPORT_C const TDesC8& FinalProgress();
	IMPORT_C ~CImPOP3GetMail();
	void DoCancel();
	void RunL();
	const TDesC8& ProgressL();
private:
	CImPOP3GetMail(CMsvSession& aMsvSession, TRequestStatus& aObserverRequestStatus, CPop3ClientMtm& aPop3ClientMtm, TImPOP3GetMailType aPOP3GetMailType);
	void ConstructL(const CMsvEntrySelection& aMsvEntrySelection, TDes8& aPop3GetMailInfo);
	void SelectNextStateL();					// selects next state to go to
	void ChangeStateL();					// initiates the next state operation
	void SelectAndChangeToNextStateL();
	void RequestComplete(TInt aError);
	void Complete();
	void ConnectToMailboxL();
	void CopyMoveNewMessagesL(TBool aCopy);
	void CopyMoveMessageSelectionL(TBool aCopy);
	void CopyMoveAllMessagesL(TBool aCopy);
	void DisconnectFromMailboxL();
	void ResetProgress();
	void StoreProgressL();
private:
	enum TImPOP3GetMailState
		{
		EConnectToMailbox,
		ECopyNewMessages,
		EMoveNewMessages,
		ECopyMessageSelection,
		EMoveMessageSelection,
		ECopyAllMessages,
		EMoveAllMessages,
		EDisconnectFromMailbox,
		EFinished
		};
	
	CPop3ClientMtm& iPop3ClientMtm;
	CMsvEntrySelection* iMsvEntrySelection;
	CMsvOperation* iMsvOperation;

	TPop3Progress iProgress;
	TPop3Progress iErrorProgress;
	TImPOP3GetMailState iState;
	TImPOP3GetMailType iPOP3GetMailType;
	TPckgBuf<TPop3Progress> iProgressBuf;
	TPckgBuf<TImPop3GetMailInfo> iPop3GetMailInfo;
	};

#endif // __POPCMTM_H__
