// Copyright (c) 2002-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Implementation of the Phonebook Synchroniser internal ICC Entry
// representation class.
// 
//

/**
 @file
 @internalComponent
*/

#include "Phonebook.h"
#include "PhonebookManager.h"
#include "SyncContactICCEntry.h"
#include "phbksync.h"
#include "phbksyncsvr.h"
#include "phbksyncsess.h"
#include "phbksynclog.h"
#include "common.h"


/**
 *  Granularity of the arrays.
 */
const TInt KSyncArrayGranularity = 4;


/**
 *  Utility function to panic the server if needed.
 *
 *  @param aPanicCode  Phonebook Synchroniser Fault type 
 */
GLDEF_C void PhBkSyncPanic(TPhBkSyncPanic aPanicCode)
	{
	LOGCOMMON2(_L8("PhBkSync Fault %d"), aPanicCode);
	User::Panic(_L("PhBkSync Fault"), aPanicCode);
	} // PhBkSyncPanic


/**
 *  Factory method to create a CSyncContactICCEntry object.
 *
 *  @return Pointer to a new CSyncContactICCEntry object.
 */
CSyncContactICCEntry* CSyncContactICCEntry::NewL()
	{
	CSyncContactICCEntry*  ptr = new (ELeave) CSyncContactICCEntry();
	CleanupStack::PushL(ptr);
	ptr->ConstructL();
	CleanupStack::Pop(ptr);

	return ptr;
	} // CSyncContactICCEntry::NewL


/**
 *  Standard constructor.
 */
CSyncContactICCEntry::CSyncContactICCEntry()
  : iSlotNum(0),
    iTON(RMobilePhone::EUnknownNumber),
    iContactId(KNullContactId),
    iPhonebookUid(KUidIccGlobalAdnPhonebook),
    iIsHidden(EFalse),
    iTemplateId(KNullContactId)
	{
	// NOP
	} // CSyncContactICCEntry::CSyncContactICCEntry


/**
 *  Second phase constructor.
 */
void CSyncContactICCEntry::ConstructL()
	{
	iNumberList = new (ELeave) CArrayFixFlat<TSyncAdditionalNumber>(KSyncArrayGranularity);
	iGroupList = new (ELeave) CArrayFixFlat<TSyncEntryName>(KSyncArrayGranularity);
	iEmailList = new (ELeave) CArrayFixFlat<TSyncEntryName>(KSyncArrayGranularity);
	} //CSyncContactICCEntry::ConstructL


/**
 *  Standard destructor.
 */
CSyncContactICCEntry::~CSyncContactICCEntry()
	{
	delete iNumberList;
	delete iGroupList;
	delete iEmailList;
	} // CSyncContactICCEntry::~CSyncContactICCEntry


/**
 *  This method stores the streamed contents of the object into a CBufBase
 *  buffer.
 *
 *  @return Pointer to the allocated CBufFlat.
 */
CBufBase* CSyncContactICCEntry::StoreL()
	{
	CBufBase* buf=CBufFlat::NewL(KSyncArrayGranularity);
	CleanupStack::PushL(buf);

	RBufWriteStream  strm(*buf);
	strm << *this;
	strm.CommitL();
	CleanupStack::Pop(buf);

	return buf;
	} // CSyncContactICCEntry::StoreL


/**
 *  This method stores the streamed contents of the oject into a
 *  descriptor.
 *
 *  @param aDes  Descriptor in which to store the object.
 */
void CSyncContactICCEntry::StoreL(TDes8& aDes)
	{
	RDesWriteStream  strm(aDes);
	strm << *this;
	strm.CommitL();
	} // CSyncContactICCEntry::StoreL


/**
 *  This method retrieves the contents of a CSyncContactICCEntry from a
 *  descriptor.
 *
 * @param aBuf  Descriptor from which to read the CSyncContactICCEntry.
 */
void CSyncContactICCEntry::RestoreL(const TDesC8& aBuf)
	{
	RDesReadStream  strm(aBuf);
	strm >> *this;
	} // CSyncContactICCEntry::RestoreL


/**
 *  This method internalises the contents of a CSyncContactICCEntry from
 *  a stream.
 *
 *  @param aStream  The read stream containing a CSyncContactICCEntry.
 */
void CSyncContactICCEntry::InternalizeL(RReadStream& aStream)
	{
	aStream >> iName;
	aStream >> iSecondName;
	aStream >> iNumber;
	TInt count(aStream.ReadInt32L());	// list of additional numbers 
	TInt i(0);
	for(i=0; i<count; ++i)
		{
		TSyncAdditionalNumber addNumber;
		aStream >> addNumber;
		iNumberList->AppendL(addNumber);
		}
	count = aStream.ReadInt32L();		// group list
	for(i=0; i<count; ++i)
		{
		TSyncEntryName group;
		aStream >> group;
		iGroupList->AppendL(group);
		}
	count = aStream.ReadInt32L();		// email list
	for(i=0; i<count; ++i)
		{
		TSyncEntryName email;
		aStream >> email;
		iEmailList->AppendL(email);
		}
	iSlotNum = aStream.ReadInt32L();
	iTON = static_cast<RMobilePhone::TMobileTON>(aStream.ReadUint32L());
	iContactId = aStream.ReadInt32L();
	iPhonebookUid = TUid::Uid(aStream.ReadInt32L());
	iIsHidden = static_cast<TBool>(aStream.ReadInt32L());
	iTemplateId = aStream.ReadInt32L();
	} // CSyncContactICCEntry::InternalizeL


/**
 *  This method externalizes the contents of a CSyncContactICCEntry into
 *  a stream.
 *
 *  @param aStream  The write stream that will contain a CSyncContactICCEntry.
 */
void CSyncContactICCEntry::ExternalizeL(RWriteStream& aStream) const
	{
	aStream << iName;
	aStream << iSecondName;
	aStream << iNumber;
	TInt count(iNumberList->Count());	// list of additional numbers
	TInt i(0);
	aStream.WriteInt32L(count);
	for(i=0; i<count; ++i)
		aStream << iNumberList->At(i);
	count = iGroupList->Count();		// group list
	aStream.WriteInt32L(count);
	for(i=0; i<count; ++i)
		aStream << iGroupList->At(i);
	count = iEmailList->Count();		// email list
	aStream.WriteInt32L(count);
	for(i=0; i<count; ++i)
		aStream << iEmailList->At(i);
	aStream.WriteInt32L(iSlotNum); 
	aStream.WriteUint32L(iTON);
	aStream.WriteInt32L(iContactId);
	aStream.WriteInt32L(iPhonebookUid.iUid);
	aStream.WriteInt32L(iIsHidden);
	aStream.WriteInt32L(iTemplateId);
	} // CSyncContactICCEntry::ExternalizeL


/**
 *  Initialise the class members from data taken from a CContactICCEntry.
 *
 *  @param aContactItem  Contacts ICC entry 
 *
 *  @return TInt KErrNotSupported if a non-supported field supplied in field set, KErrOverflow for an overflow, 
 *          othewise KErrNone
 */
void CSyncContactICCEntry::InitialiseFromContactICCEntryL(const CContactICCEntry& aContactItem)
	{
	//
	// Clear the fields to start with...
	//
	Reset();

	TInt retrievedFields(0); // Number of fields retrieved from aContactItem field set 
	iContactId = aContactItem.Id(); // get Contact UID
	CContactItemFieldSet& fieldSet = aContactItem.CardFields();

	TInt index(0), firstPhoneNumIndex(0);
	// get default phone number field from aContacItem
	firstPhoneNumIndex = fieldSet.Find(KUidContactFieldPhoneNumber);
	if(firstPhoneNumIndex != KErrNotFound)
		{
		++retrievedFields;
		const CContactItemField& field = fieldSet[firstPhoneNumIndex];
		if(field.TextStorage()->Text().Length() > RMobilePhone::KMaxMobileTelNumberSize)
			{
			User::Leave(KErrOverflow);
			}

		TPtrC number(field.TextStorage()->Text());
		iNumber.Copy(number);

		//
		// Set the TON/NPI for that number as International if the number contains
		// the '+' prefix, otherwise set it to unknown.
		//
		if (TLex(iNumber).Get() == '+')
			{
			iTON = RMobilePhone::EInternationalNumber;
			}
		else
			{
			iTON = RMobilePhone::EUnknownNumber;
			}

		// Check whether this entry is hidden 
		if(const_cast<CContactICCEntry&>(aContactItem).IsHidden())
			iIsHidden = ETrue;

		// Retrieve family name
		if((index = fieldSet.Find(KUidContactFieldFamilyName)) != KErrNotFound)
			{
			++retrievedFields;
			// get given name field from aContactItem
			if(field.TextStorage()->Text().Length() > CSyncContactICCEntry::KMaxPhBkNameSize)
				{
				User::Leave(KErrOverflow);
				}
			iName.Copy(fieldSet[index].TextStorage()->Text());
			} // name is not a compulsory field, so do not return an error if not found

		// retrieve second name
		if((index = fieldSet.Find(KUidContactFieldSecondName)) != KErrNotFound)
			{
			++retrievedFields;
			// get given name field from aContactItem
			if(field.TextStorage()->Text().Length() > CSyncContactICCEntry::KMaxPhBkNameSize)
				{
				User::Leave(KErrOverflow);
				}
			iSecondName.Copy(fieldSet[index].TextStorage()->Text());
			} // name is not a compulsory field, so do not return an error if not found

		// get slot number field from aContactItem
		index = fieldSet.Find(KUidContactFieldICCSlot);
		if(index!=KErrNotFound) // index field provided
			{
			++retrievedFields;
			TPtrC tempPtr(NULL,0);
			tempPtr.Set(fieldSet[index].TextStorage()->Text());
			TLex lex(tempPtr);
			TInt slot;
			TInt ret(lex.Val(slot));
			if (ret == KErrNone) // index field value supplied 
				{
				iSlotNum = slot;
				}
			else
				iSlotNum = KSyncIndexNotSupplied; // index field provided as template field, so no actual values supplied
			}
		else
			{
			// all fields supplied but slot field not found so it must be a new entry
			iSlotNum = KSyncIndexNotSupplied; // Set index to KSyncIndexNotSupplied
			}

		// handle the a phonebook UID field if present (it should always be as it is in the template)...
		index = fieldSet.Find(KUidContactFieldICCPhonebook);
		if(index!=KErrNotFound) // index field provided
			{
			++retrievedFields;
			TPtrC tempPtr(NULL,0);
			tempPtr.Set(fieldSet[index].TextStorage()->Text());
			TLex lex(tempPtr);
			TInt phonebook;
			TInt ret(lex.Val(phonebook));
			if (ret == KErrNone) // index field value supplied 
				{
				iPhonebookUid.iUid = phonebook;
				}
			else
				{
				iPhonebookUid = KUidIccGlobalAdnPhonebook;
				}
			}
		else
			{
			iPhonebookUid = KUidIccGlobalAdnPhonebook;
			}

		// now get list of additional phone numbers from CContactICCEntry 		
		index = firstPhoneNumIndex;  // skip the default phone number which has been read in early
		while((index = fieldSet.FindNext(KUidContactFieldPhoneNumber,index+1)) != KErrNotFound)
			{
			// get additional phone number field from aContacItem
			++retrievedFields;
			const CContactItemField& field = fieldSet[index];
			if(field.TextStorage()->Text().Length() > RMobilePhone::KMaxMobileTelNumberSize)
				{
				User::Leave(KErrOverflow);
				}

			TPtrC number(field.TextStorage()->Text());
			CSyncContactICCEntry::TSyncAdditionalNumber addNumber;
			addNumber.iNumber.Copy(number);

			//
			// Set the TON/NPI for that number as International if the number contains
			// the '+' prefix, otherwise set it to unknown.
			//
			if (TLex(addNumber.iNumber).Get() == '+')
				{
				addNumber.iTON = RMobilePhone::EInternationalNumber;
				}
			else
				{
				addNumber.iTON = RMobilePhone::EUnknownNumber;
				}

            //-- assign alpha string to the additional number according to additional field types
			const CContentType&  cntType = field.ContentType();
		    
		    //
		    // Assume HOME by default. Please note that this will be used as default 
		    // value if something other than HOME or WORK is supplied.
		    //
			if (cntType.ContainsFieldType(KUidContactFieldVCardMapWORK)) 
				{
				addNumber.iNumberString.Append(KVersitParamWork);
				}
			else
				{
				addNumber.iNumberString.Append(KVersitParamHome);
				}

		    //
		    // Try to locate other tags (FAX, CELL, etc)...
			//
			if (cntType.ContainsFieldType(KUidContactFieldVCardMapMSG))
				{
				addNumber.iNumberString.Append(KVersitParamMsg);
				}
			else if (cntType.ContainsFieldType(KUidContactFieldVCardMapVOICE))
				{
				addNumber.iNumberString.Append(KVersitParamVoice);
				}
			else if (cntType.ContainsFieldType(KUidContactFieldVCardMapFAX))
				{
				addNumber.iNumberString.Append(KVersitParamFax);
				}
			else if (cntType.ContainsFieldType(KUidContactFieldVCardMapPREF))
				{
				addNumber.iNumberString.Append(KVersitParamPref);
				}
			else if (cntType.ContainsFieldType(KUidContactFieldVCardMapCELL))
				{
				addNumber.iNumberString.Append(KVersitParamCell);
				}
			else if (cntType.ContainsFieldType(KUidContactFieldVCardMapPAGER))
				{
				addNumber.iNumberString.Append(KVersitParamPager);
				}
			else if (cntType.ContainsFieldType(KUidContactFieldVCardMapBBS))
				{
				addNumber.iNumberString.Append(KVersitParamBbs);
				}
			else if (cntType.ContainsFieldType(KUidContactFieldVCardMapMODEM))
				{
				addNumber.iNumberString.Append(KVersitParamModem);
				}
			else if (cntType.ContainsFieldType(KUidContactFieldVCardMapCAR))
				{
				addNumber.iNumberString.Append(KVersitParamCar);
				}
			else if (cntType.ContainsFieldType(KUidContactFieldVCardMapISDN))
				{
				addNumber.iNumberString.Append(KVersitParamIsdn);
				}
			else if (cntType.ContainsFieldType(KUidContactFieldVCardMapVIDEO))
				{
				addNumber.iNumberString.Append(KVersitParamVideo);
				}

			// finally append additional number to additional number list
			iNumberList->AppendL(addNumber);
			}

		// get all e-mail fields 
		index = -1;
		while((index = fieldSet.FindNext(KUidContactFieldEMail,index+1)) != KErrNotFound)
			{
			++retrievedFields;
			// get e-mail field from aContactItem
			if(field.TextStorage()->Text().Length() > CSyncContactICCEntry::KMaxPhBkNameSize)
				{
				User::Leave(KErrOverflow);
				}

			CSyncContactICCEntry::TSyncEntryName name;
			name.Copy(fieldSet[index].TextStorage()->Text());
			iEmailList->AppendL(name);
			} // e-mail is not a compulsory field, so do not return an error if not found

		// get all groups that this entry belongs to 
		index = -1; // following was originally commented out (JJ)	
		while((index = fieldSet.FindNext(KUidContactFieldICCGroup, index+1)) != KErrNotFound)
			{
			++retrievedFields;
			// get group field from aContactItem
			if(field.TextStorage()->Text().Length() > CSyncContactICCEntry::KMaxPhBkNameSize)
				{
				User::Leave(KErrOverflow);
				}

			CSyncContactICCEntry::TSyncEntryName name;
			name.Copy(fieldSet[index].TextStorage()->Text());
			iGroupList->AppendL(name);
			} // group is not a compulsory field, so do not return an error if not foun

		// For new entries phonebook type is not supplied, so always extract Template ID
		iTemplateId = aContactItem.TemplateRefId();
		}			

	//
	// Check that all expected fields were retrieved...
	//
	if (fieldSet.Count() != retrievedFields)
		{
#ifdef _DEBUG
		//
		// At least one field has was not supported. Log all the field types!
		//
		LOGCOMMON1(_L8("CSyncContactICCEntry: Unsupported field found!"));
		for (TInt field = 0;  field < fieldSet.Count();  field++)
			{
			LOGCOMMON2(_L8("CSyncContactICCEntry:   Field[%d]:"), field);
			for (TInt type = 0;  type < fieldSet[field].ContentType().FieldTypeCount();  type++)
				{
				LOGCOMMON3(_L8("CSyncContactICCEntry:     Type[%d]=0x%08x"), type,
				           fieldSet[field].ContentType().FieldType(type));
				}
			}
#endif

		User::Leave(KErrNotSupported);
		}
	} // CSyncContactICCEntry::InitialiseFromContactICCEntryL




/**
 *  This method resets all attributes.
 */
void CSyncContactICCEntry::Reset()
	{
	// reset all variables
	iSlotNum      = 0; 
	iTON          = RMobilePhone::EUnknownNumber;
	iContactId    = KNullContactId;
	iPhonebookUid = KUidIccGlobalAdnPhonebook; 
	iIsHidden     = EFalse; 
	iTemplateId   = KNullContactId;
	iName.Zero();
	iSecondName.Zero();
	iNumber.Zero();

	// reset all lists
	iNumberList->Reset();
	iGroupList->Reset();	
	iEmailList->Reset();
	} // CSyncContactICCEntry::Reset


#ifdef _DEBUG
/**
 *  Logs the contact entry for debugging purposes.
 */
void CSyncContactICCEntry::LogSyncContactICCEntry() const
	{
	//
	// Log the basic fields...
	//
	TBuf8<KMaxPhBkNameSize>  fieldIn8bit;
	TBuf8<RMobilePhone::KMaxMobileTelNumberSize>  numberIn8bit;

	fieldIn8bit.Copy(iName);
	LOGCOMMON2(_L8("ICCEntry: Name=\"%S\""), &fieldIn8bit);

	fieldIn8bit.Copy(iSecondName);
	LOGCOMMON2(_L8("ICCEntry: Second Name=\"%S\""), &fieldIn8bit);

	numberIn8bit.Copy(iNumber);
	LOGCOMMON2(_L8("ICCEntry: Number=\"%S\""), &numberIn8bit);

	LOGCOMMON2(_L8("ICCEntry: SlotNum=%d"), iSlotNum);

	LOGCOMMON2(_L8("ICCEntry: TypeOfNum=%d"), iTON);

	LOGCOMMON2(_L8("ICCEntry: ContactID=0x%08x"), iContactId);

	LOGCOMMON2(_L8("ICCEntry: PhonebookUID=0x%08x"), iPhonebookUid.iUid);

	LOGCOMMON2(_L8("ICCEntry: IsHidden=%d"), iIsHidden);

	LOGCOMMON2(_L8("ICCEntry: TemplateId=%d"), iTemplateId);

	//
	// Log the number list...
	//
	TInt  listCount(iNumberList->Count());
	TInt  index;

	for (index = 0;  index < listCount;  index++)
		{
		numberIn8bit.Copy(iNumberList->At(index).iNumber);
		fieldIn8bit.Copy(iNumberList->At(index).iNumberString);
		LOGCOMMON4(_L8("ICCEntry: NumberList[%d] \"%S\" \"%S\""),
				   index, &numberIn8bit, &fieldIn8bit);
		}

	//
	// Log the group list...
	//
	listCount = iGroupList->Count();

	for (index = 0;  index < listCount;  index++)
		{
		fieldIn8bit.Copy(iGroupList->At(index));
		LOGCOMMON3(_L8("ICCEntry: GroupList[%d]=\"%S\""), index, &fieldIn8bit);
		}

	//
	// Log the email list...
	//
	listCount = iEmailList->Count();

	for (index = 0;  index < listCount;  index++)
		{
		fieldIn8bit.Copy(iEmailList->At(index));
		LOGCOMMON3(_L8("ICCEntry: EmailList[%d]=\"%S\""), index, &fieldIn8bit);
		}
	} // CSyncContactICCEntry::LogSyncContactICCEntry
#endif


/**
 *  This method internalizes the contents of TSyncAdditionalNumber from
 *  a stream.
 *
 *  @param aStream  The read stream containing TSyncAdditionalNumber.
 */
void CSyncContactICCEntry::TSyncAdditionalNumber::InternalizeL(RReadStream& aStream)
	{
	aStream >> iNumber;
	aStream >> iNumberString;
	iTON = static_cast<RMobilePhone::TMobileTON>(aStream.ReadUint32L());
	} // CSyncContactICCEntry::TSyncAdditionalNumber::InternalizeL


/**
 *  This method externalizes the contents of TSyncAdditionalNumber into
 *  a stream.
 *
 *  @param aStream  The write stream that will contain TSyncAdditionalNumber.
 */
void CSyncContactICCEntry::TSyncAdditionalNumber::ExternalizeL(RWriteStream& aStream) const
	{
	aStream << iNumber;
	aStream << iNumberString;
	aStream.WriteUint32L(iTON);
	} // CSyncContactICCEntry::TSyncAdditionalNumber::ExternalizeL


/**
 *  Standard constructor.
 */
TContactFieldFormat::TContactFieldFormat(TInt aLength, TInt aCount,
										 TBool aDisplayed)
  : iLength(aLength),
    iCount(aCount),
    iDisplayed(aDisplayed)
	{
	// NOP
	} // TContactFieldFormat::TContactFieldFormat


/**
 *  The old PhBkSyncServerThread Server Thread. This thread is no longer
 *  supported.
 */
EXPORT_C TInt PhBkSyncServerThread(TAny*)
	{
	return KErrNotSupported;
	} // PhBkSyncServerThread


/**
 *  This method returns the Phonebook Synchroniser API extension number
 *  of the class.
 *
 *  @return An integer that will indicate the version of the type.
 */
EXPORT_C TInt RPhoneBookSession::TContactFieldsV1::ExtensionId() const
	{
	return iExtensionId;
	} // RPhoneBookSession::TContactFieldsV1::ExtensionId()


/**
 *  Standard constructor.
 */
EXPORT_C RPhoneBookSession::TContactFieldsV1::TContactFieldsV1()
  : iNameField(0, 0, EFalse),
    iNumberField(0,0,EFalse),
    iIccSlotField(0,0,EFalse), 
    iExtensionId(KPhBkSyncTContactFieldsV1)
	{
	// NOP
	} // RPhoneBookSession::TContactFieldsV1::TContactFieldsV1


/**
 *  Standard constructor.
 */
EXPORT_C RPhoneBookSession::TContactFieldsV2::TContactFieldsV2()
  : TContactFieldsV1(),
    iAdditionalNumString(0, 0, EFalse),
    iGroupField(0,0,EFalse), 
    iEmailField(0,0,EFalse),
    iPhonebook(KUidIccGlobalAdnPhonebook)
	{
	iExtensionId = KPhBkSyncTContactFieldsV2;
	} // RPhoneBookSession::TContactFieldsV2::TContactFieldsV2()


/**
 *  Standard constructor.
 */
EXPORT_C RPhoneBookSession::TContactFieldsV3::TContactFieldsV3()
  : TContactFieldsV2(),
    iAdditionalNumAlphaString(0, 0, EFalse),
    iSecondNameField(0,0,EFalse)
	{
	iExtensionId = KPhBkSyncTContactFieldsV3;
	} // RPhoneBookSession::TContactFieldsV3::TContactFieldsV3()
