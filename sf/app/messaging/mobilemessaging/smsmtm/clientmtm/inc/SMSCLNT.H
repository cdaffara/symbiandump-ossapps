// Copyright (c) 1999-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// SMSCLNT.H
//
/**
 * @file 
 * @publishedAll
 * @released
 */
#ifndef __SMSCLNT_H__
#define __SMSCLNT_H__

#include <mtclbase.h>
#include <smuthdr.h>

class CSmsHeader;
class CSmsMessage;
class CSmsSettings;
class TSmsUtilities;
class CSmsSimParamOperation;
class CMobilePhoneSmspList;
class RResourceFile;

/** 
The SMS client MTM interface.

Most of the functions in this class are implementations of the virtual functions 
defined by the base class CBaseMtm.

An object of this class can be obtained from the Client MTM registry. The message
server provides the necessary information required by the registry to provide
the SMS client MTM.

All client MTM objects have a current context. This is an entry in the message 
store that will be manipulated by certain APIs supplied by the MTM. If the 
current context is not set then certain APIs will panic. 

The CBaseMtm::HasContext API can be used to check that the current context has 
been set. It can be set using either the CBaseMtm::SwitchCurrenEntryL API or the
CBaseMtm::SetCurrentEntryL API.

@see	CClientMtmRegistry
@see	CBaseMtm::HasContext
@see	CBaseMtm::SetCurrentEntryL
@see	CBaseMtm::SwitchCurrenEntryL

@publishedAll
@released
*/
NONSHARABLE_CLASS (CSmsClientMtm) : public CBaseMtm
	{
public:

	IMPORT_C static CSmsClientMtm* NewL(CRegisteredMtmDll& aRegisteredMtmDll,CMsvSession& aSession);
	virtual ~CSmsClientMtm();

	void HandleEntryEvent(enum MMsvEntryObserver::TMsvEntryEvent,TAny*,TAny*,TAny*);

	inline CSmsHeader& SmsHeader();
	inline const CSmsHeader& SmsHeader() const;
	inline CSmsSettings& ServiceSettings();
	inline const CSmsSettings& ServiceSettings() const;
	inline TInt ServiceId() const;

	IMPORT_C void RestoreServiceAndSettingsL();
	IMPORT_C CSmsSimParamOperation* ReadSimParamsL(TRequestStatus& aObserverRequestStatus);
	IMPORT_C CSmsSimParamOperation* WriteSimParamsL(const CMobilePhoneSmspList& aList, TRequestStatus& aObserverRequestStatus);
	IMPORT_C void SaveMessageL(CMsvStore& aEditStore, TMsvEntry& aEntry);
	IMPORT_C void CreateMessageL(TMsvEntry& aEntry);
	/** 
	Sets the character encoding value. The character encoding value options are 7-bit,
	8-bit and 16-Bit Unicode. By default the character set encoding is 7 bit encoding.
	*/
	void SetMessageCharacterSetL(TSmsDataCodingScheme::TSmsAlphabet aTsmsAlphabet);

public:	// methods from CBaseMtm

	virtual void SaveMessageL(); 
	virtual void LoadMessageL();
	virtual TMsvPartList ValidateMessage(TMsvPartList aPartList);
	virtual TMsvPartList Find(const TDesC& aTextToFind, TMsvPartList aPartList);
	virtual CMsvOperation* ReplyL(TMsvId aDestination, TMsvPartList aPartList, TRequestStatus& aCompletionStatus);
	virtual CMsvOperation* ForwardL(TMsvId aDestination, TMsvPartList aPartList, TRequestStatus& aCompletionStatus);
	virtual void AddAddresseeL(const TDesC& aRealAddress);
	virtual void AddAddresseeL(const TDesC& aRealAddress, const TDesC& aAlias);
	virtual void RemoveAddressee(TInt aIndex);
	virtual TInt QueryCapability(TUid aCapability, TInt& aResponse);
	virtual void InvokeSyncFunctionL(TInt aFunctionId,const CMsvEntrySelection& aSelection, TDes8& aParameter);
	virtual CMsvOperation* InvokeAsyncFunctionL(TInt aFunctionId,const CMsvEntrySelection& aSelection, TDes8& aParameter, TRequestStatus& aCompletionStatus);

	IMPORT_C void CreateMessageL(TMsvId aServiceId);
	IMPORT_C void BioTypeChangedL(TUid aBioTypeUid);
	
protected: 
	/**
	Methods from CBaseMtm, The implementation of this function assumes that
    the new service for setting the charset encoding value for a SMS message is supported.
    */
	IMPORT_C virtual TInt Extension_(TUint aExtensionId, TAny*& a0, TAny* a1);
 
private:

	CSmsClientMtm(CRegisteredMtmDll& aRegisteredMtmDll,CMsvSession& aSession);
	void ConstructL();

	void BasicReplyForwardEntry(TMsvEntry& aNewEntry) const;
	CMsvOperation* CreateNewEntryL(TMsvEntry& aNewEntry, TMsvId aDestination,CSmsHeader& aSmsHeader,const CRichText& aBody, TRequestStatus& aCompletionStatus);
	TBool ValidRecipients() const;
	TBool ValidNumber(const TDesC& aNumber) const;

private:	// methods from CBaseMtm

	virtual void ContextEntrySwitched(); 

private:

	void DoAddAddresseeL(const TDesC& aRealAddress, const TDesC& aAlias);
	void AddRecipientL(const TDesC& aRealAddress, const TDesC& aAlias);
	void DoAddRecipientL(CSmsHeader* aSmsHeader, const TDesC& aRealAddress, const TDesC& aAlias);
	void ResetHeader();
	TMsvPartList DoFindL(const TDesC& aTextToFind,TMsvPartList aPartList);
	void FindL(const TDesC& aTextToFind, const TMsvPartList aPartList,TMsvPartList& aFoundList);
	void FindInBodyL(const TDesC& aTextToFind, const TMsvPartList& aFoundList, TMsvPartList& aResult);
	HBufC* ReadEmailSubjectFormatL(RResourceFile& aResourceFile, TInt aResourceId, const TDesC& aDefaultFormat);

private:

	CSmsSettings*	iServiceSettings;
	TMsvId			iServiceId;
	CSmsHeader*		iSmsHeader;
	TChar			iRealAddressOpen;
	TChar			iRealAddressClose;
	HBufC*			iEmailForwardSubjectFormat;
	HBufC*			iEmailReplySubjectFormat;
	};

#include <smsclnt.inl>

#endif	// __SMSCLNT_H__
