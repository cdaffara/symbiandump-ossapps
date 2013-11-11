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
// SMUTHDR.H
//
/**
 * @file 
 * @publishedAll
 * @released
 */
#ifndef __SMUTHDR_H__
#define __SMUTHDR_H__

#include <e32base.h>
#include <gsmubuf.h>
#include <bif.h>
#include <gsmupdu.h>
#include <msvstd.h>
#include <smutset.h>

#ifndef SYMBIAN_ENABLE_SPLIT_HEADERS  
#include <tmsvsmsentry.h>
#endif

class RMsvReadStream;
class RMsvWriteStream;
class CMsvStore;
class CSmsEmailFields;

/** 
The SMS MTM encapsulation of an SMS message.

This class allows a messaging client to access the lower-level message encapsulation 
used by the SMS stack.

It also provides functionality to restore and store an SMS message from/to the
message store.

An SMS message is encapsulated in a Protocol Data Unit (PDU). In GSM there are 
six types of SMS message PDU. In the descriptions below, SC means Service Center 
and MS means Mobile Station (the phone).

1. SMS-DELIVER - sent from SC to MS

2. SMS-DELIVER-REPORT - sent from MS to SC

3. SMS-SUBMIT - sent from MS to SC

4. SMS-SUBMIT-REPORT - sent from SC to MS

5. SMS-STATUS-REPORT - sent from SC to MS

6. SMS-COMMAND - sent from MS to SC

@publishedAll
@released
*/
NONSHARABLE_CLASS (CSmsHeader) : public CBase
	{
public:

	IMPORT_C static CSmsHeader* NewL(CSmsPDU::TSmsPDUType aType,CEditableText& aText);
	IMPORT_C static CSmsHeader* NewL(CSmsPDU::TSmsPDUType aType,CEditableText& aText,RFs& aFs);
	IMPORT_C static CSmsHeader* NewL(CSmsMessage* aMessage);
	IMPORT_C ~CSmsHeader();

	inline CSmsPDU::TSmsPDUType Type() const;

	inline CSmsSubmit& Submit();
	inline const CSmsSubmit& Submit() const;
	inline CSmsDeliver& Deliver();
	inline const CSmsDeliver& Deliver() const;
	inline CSmsCommand& Command();
	inline const CSmsCommand& Command() const;
	inline CSmsStatusReport& StatusReport();
	inline const CSmsStatusReport& StatusReport() const;
	
	inline CSmsMessage& Message();
	inline const CSmsMessage& Message() const;

	inline TBool ReplyPathProvided() const;
	inline void SetReplyPathProvided(TBool aReplyPathProvided);

	inline TBioMsgIdType BioMsgIdType() const;
	inline void SetBioMsgIdType(TBioMsgIdType aBioMsgIdType = EBioMsgIdNbs);
	
	inline TPtrC ServiceCenterAddress() const;
	inline void SetServiceCenterAddressL(const TDesC& aAddress);

	IMPORT_C void SetSmsSettingsL(const CSmsMessageSettings& aSmsSettings);
	IMPORT_C void GetSmsSettingsL(CSmsMessageSettings& aSmsSettings) const;

	inline const CArrayPtrFlat<CSmsNumber>& Recipients() const;
	inline CArrayPtrFlat<CSmsNumber>& Recipients();

	inline void SetFromAddressL(const TDesC& aAddress);
	inline TPtrC FromAddress() const;
	
	TPtrC ReplyAddressL() const;
	
	IMPORT_C void SetEmailFieldsL(const CSmsEmailFields& aEmailFields);
	void SetReplyEmailFieldsL(const CSmsEmailFields& aEmailFields, const TDesC& aReplySubjectFormat);
	void SetForwardEmailFieldsL(const CSmsEmailFields& aEmailFields, const TDesC& aForwardSubjectFormat);
	IMPORT_C const CSmsEmailFields& EmailFields() const;

	IMPORT_C void InternalizeL( RMsvReadStream& aStream);
	IMPORT_C void ExternalizeL( RMsvWriteStream& aStream) const;

	IMPORT_C void RestoreL(CMsvStore& aStore);
	IMPORT_C void StoreL(CMsvStore& aStore) const;

	IMPORT_C  TBool ContainsSmsHeaderL(const CMsvStore& aStore); 

	
private:
	CSmsHeader(CSmsMessage* aSmsMessage = NULL);
	void ConstructL(CSmsPDU::TSmsPDUType aType, CEditableText& aText);
	void ConstructL(CSmsPDU::TSmsPDUType aType, CEditableText& aText, RFs& aFs);
	void ConstructL();

	enum TSmsHeaderFlags
		{
		ESmsHeaderNoFlags			= 0x00,
		ESmsHeaderReplyPathProvided = 0x01,
		ESmsHeaderCanConcatenate	= 0x02
		};

	TBool CanConcatenate() const;
	void SetCanConcatenate(TBool aCanConcatenate);
	
	void SetEmailReplyForwardSubjectL(CSmsEmailFields* aEmailFields, const TDesC& aSubject, const TDesC& aSubjectFormat);

#if (defined SYMBIAN_MESSAGESTORE_HEADER_BODY_USING_SQLDB)
	void StoreDbL(CMsvStore& aStore) const;
	void ReStoreDBL(CMsvStore& aStore);
	void GetRecipientL(TDesC16& aRecipientStrList);
	TInt ConvertToTInt(TDesC16& aStr);
#endif

private:
	CArrayPtrFlat<CSmsNumber> iRecipients;
	CSmsMessage* iMessage;
	TUint32 iFlags;
	RFs iFs;

	// iBioMsgIdType is only used during sending and will be ignored if the 
	// TMsvEntry.iBioType == 0. The default is EBioMsgIdNbs.
	TBioMsgIdType iBioMsgIdType;
	CSmsEmailFields*	iEmailFields;
	TBool			iCloseFs;
	};
	
#include <smuthdr.inl>

#endif	// __SMUTHDR_H__

