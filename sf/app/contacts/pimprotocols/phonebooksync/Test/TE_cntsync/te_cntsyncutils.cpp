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
//

#include "te_cntsyncutils.h"
#include "te_cntsyncbase.h"

/** 
 * Factory construction method.
 * @return Pointer to CPhbkDeleteIntegrationTest object
 */
CPhbkIntegrationTestUtility* CPhbkIntegrationTestUtility::NewL()
	{
	CPhbkIntegrationTestUtility* self = new(ELeave) CPhbkIntegrationTestUtility();
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop();
	return self;
	}

/** Destructor. */
CPhbkIntegrationTestUtility::~CPhbkIntegrationTestUtility()
	{
	iSession.Close();
	delete iDb;
	}

/** 
 * Second-phase constructor.
 * This is required because the SIM test configuration must be set before
 * the CSimPhone is constructed.
 */
void CPhbkIntegrationTestUtility::ConstructL()
	{
	User::LeaveIfError(iSession.Connect());
	User::After(3000000); // wait for session to startup

	//
	// Attempt to open the Contacts DB up to three times.
	//
	TInt  err(KErrNone);

	for (TInt count = 0;  count < 3;  count++)
		{
		TRAP(err, iDb = CContactDatabase::OpenL());
		if (err != KErrLocked)
			{
			break;
			}

		if (count + 1 < 3)
			{
			User::After(1000000);
			}
		}
	User::LeaveIfError(err);
	User::After(3000000); // wait for session to startup
	}

/** Default constructor */
CPhbkIntegrationTestUtility::CPhbkIntegrationTestUtility()
	{
	}


/** 
 * Check group membership.
 * All ICC entries synchronised should belong to a single group. This group 
 * should contain all the ADN phonebook entries
 *
 * @param aItem             ICC entry 
 * @param aExpectedMembers  Expected number of members of the ADN phonebook
 */
void CPhbkIntegrationTestUtility::CheckGroupMembershipL(CContactICCEntry& aItem, TInt aExpectedMembers)
	{
	const CContactIdArray* owned = aItem.GroupsJoined();	
	__ASSERT_ALWAYS(owned != NULL, User::Leave(EFail));
	__ASSERT_ALWAYS(owned->Count() == 1, User::Leave(EFail));
	TContactItemId groupId = (*owned)[0];

	CContactGroup* group = static_cast<CContactGroup*>(iDb->ReadContactLC(groupId));
	const CContactIdArray* itemsContained = group->ItemsContained();
	__ASSERT_ALWAYS(itemsContained->Count() == aExpectedMembers, User::Leave(EFail));
	CleanupStack::PopAndDestroy(group);	
	}

/** 
 * Check that the field of type aFieldType has the same content as 
 * aExpectedContent
 * @param aFieldset Contact item fieldset
 * @param aFieldType Field type to test
 * @param aExpectedContent Expected data
 */
void CPhbkIntegrationTestUtility::CheckFieldContentL(CContactItemFieldSet& aFieldset, TFieldType aFieldType, const TDesC& aExpectedContent)
	{
	TInt pos = aFieldset.Find(aFieldType);
	CContactItemField& field = aFieldset[pos];
	CContactTextField* textField = field.TextStorage();
	__ASSERT_ALWAYS(textField != NULL, User::Leave(EFail));
	__ASSERT_ALWAYS(textField->Text() == aExpectedContent, User::Leave(EFail));
	}

/** 
 * Check that aEntry contains correct number of fields.
 *
 * @param aPhonebook  ICC Phonebook to check.
 * @param aEntry      Contact ICC Entry
 */
void CPhbkIntegrationTestUtility::CheckFieldCountL(const TUid aPhonebook, CContactICCEntry* aEntry)
	{
	CContactItemFieldSet& fieldset = aEntry->CardFields();
	const TInt fieldsetCount = fieldset.Count(); //for debugging
	if ((aPhonebook == KUidUsimAppAdnPhonebook) || (aPhonebook == KUidIccGlobalAdnPhonebook))
		{
		__ASSERT_ALWAYS(fieldsetCount >= 6, User::Leave(EFail)); //Name,Number,email,ICC slot, phonebook
		}
	else
		{
		__ASSERT_ALWAYS(fieldsetCount == 5, User::Leave(EFail)); //Name,Number,ICC slot, phonebook
		}
	}

/** 
 * Check that aEntry contains correct number of fields for minimal contact item.
 *
 * @param aPhonebook  ICC Phonebook to check.
 * @param aEntry      Contact ICC Entry
 */
void CPhbkIntegrationTestUtility::CheckMinimalFieldCountL(const TUid aPhonebook, CContactICCEntry* aEntry)
	{
	CContactItemFieldSet& fieldset = aEntry->CardFields();
	const TInt fieldsetCount = fieldset.Count(); //for debugging
	if (aPhonebook == KUidUsimAppAdnPhonebook)
		{
		__ASSERT_ALWAYS(fieldsetCount >= 5, User::Leave(EFail)); //Name,Number,email,ICC slot, phonebook
		}
	else
		{
		__ASSERT_ALWAYS(fieldsetCount == 4, User::Leave(EFail)); //Name,Number,ICC slot, phonebook
		}
	}

/** 
 * Check that the field data stored on the SIM is identical to the
 * data stored in Contacts model.
 * @param aItem ICC item to test
 * @param aSlotNumber Slot number 
 */
void CPhbkIntegrationTestUtility::CheckFieldsL(CContactICCEntry& aItem, TInt aSlotNumber)
	{
	TBuf<20> buf;
	CContactItemFieldSet& fieldset = aItem.CardFields();
	buf.Format(KNameFormat,aSlotNumber);
	CheckFieldContentL(fieldset,KUidContactFieldFamilyName,buf);
	buf.Format(KNumberFormat,aSlotNumber);
	CheckFieldContentL(fieldset,KUidContactFieldPhoneNumber,buf);
	}

/** 
 * Read an ICC entry - normal case.
 * Check group membership is correct and correct number of fields have
 * been created.
 */
void CPhbkIntegrationTestUtility::ReadContactL(const TUid aPhonebook, TInt aExpectedCount)
	{
	iDb->SetDbViewContactType(KUidContactICCEntry);
	// get the unique groupId for the given phonebook
	TContactItemId groupId(KNullContactId);
	User::LeaveIfError(iSession.GetPhoneBookId(groupId, 
						RPhoneBookSession::ESyncGroupId,
						aPhonebook));
	__ASSERT_ALWAYS(groupId != KNullContactId, User::Leave(EFail));
  
	// based on the groupId, get items belonging to the phonebook
	CContactGroup* group = NULL;
	TRAPD(err, group = static_cast<CContactGroup*>(iDb->ReadContactL(groupId)));
	__ASSERT_ALWAYS(err == KErrNone, User::Leave(EFail));
	CleanupStack::PushL(group);
	const CContactIdArray* array = group->ItemsContained();

	// Verify correct number of entries created for this phonebook
	TInt count = array->Count(); // for debugging
	__ASSERT_ALWAYS(count == aExpectedCount, User::Leave(EFail));

	// Read all items
	for (TInt i=0; i<count; ++i)
		{
		TContactItemId id = (*array)[i];
		CContactICCEntry* entry = NULL;
		TRAP(err, entry = static_cast<CContactICCEntry*>(iDb->ReadContactL(id)));
		__ASSERT_ALWAYS(err == KErrNone, User::Leave(EFail));
		CleanupStack::PushL(entry);
		// Verify correct number of fields created for entry
		CheckFieldCountL(aPhonebook, entry);
		CleanupStack::PopAndDestroy(); // entry
		}

	CleanupStack::PopAndDestroy(); // group

	}

/**
 * Read an ICC contact - minimal read operation.
 */
void CPhbkIntegrationTestUtility::ReadMinimalContactL(const TUid aPhonebook, TInt aExpectedCount)
	{
	iDb->SetDbViewContactType(KUidContactICCEntry);
	// get the unique groupId for the given phonebook
	TContactItemId groupId(KNullContactId);
	User::LeaveIfError(iSession.GetPhoneBookId(groupId, 
						RPhoneBookSession::ESyncGroupId,
						aPhonebook));
	__ASSERT_ALWAYS(groupId != KNullContactId, User::Leave(EFail));
  
	// based on the groupId, get items belonging to the phonebook
	CContactGroup* group = NULL;
	TRAPD(err, group = static_cast<CContactGroup*>(iDb->ReadContactL(groupId)));
	__ASSERT_ALWAYS(err == KErrNone, User::Leave(EFail));
	CleanupStack::PushL(group);
	const CContactIdArray* array = group->ItemsContained();

	// Verify correct number of entries created for this phonebook
	const TInt count = array->Count(); // for debugging
	__ASSERT_ALWAYS(count == aExpectedCount, User::Leave(EFail));

	// Read entry
	TContactItemId id((*array)[0]);
	CContactICCEntry* entry = static_cast<CContactICCEntry*>(iDb->ReadMinimalContactL(id));
	CleanupStack::PushL(entry);
	CheckFieldCountL(aPhonebook, entry); // Entry contains correct number of fields

	CleanupStack::PopAndDestroy(2); // group & entry
	}

/** 
 * Add a contact to the ICC & database.
 * @param aName Name
 * @param aNumber Phone number
 * @param aPhonebook Phonebook UID (optional)
 * @param aEmail Email entry (optional)
 */
void CPhbkIntegrationTestUtility::AddContactL(const TDesC& aName, const TDesC& aNumber, const TUid aPhonebook, const TDesC& aEmail)
	{
	TContactItemId templateId(iDb->ICCTemplateIdL(aPhonebook));
	CContactItem* iccTemplate = iDb->ReadContactLC(templateId);
	CContactICCEntry* entry = CContactICCEntry::NewL(*iccTemplate);
	CleanupStack::PopAndDestroy(iccTemplate);
	CleanupStack::PushL(entry);
	//Set the phonebook field, if the field doesn't exist, it should be added
	CContactItemFieldSet& fieldset = entry->CardFields();
	TInt n = fieldset.Find(KUidContactFieldICCPhonebook);
	if(n==KErrNotFound)
		{
		CContactItemField* field = CContactItemField::NewLC(KStorageTypeText,KUidContactFieldICCPhonebook);
		entry->AddFieldL(*field);
		CleanupStack::Pop(field);
		CContactItemFieldSet& fieldset = entry->CardFields();
		n = fieldset.Find(KUidContactFieldICCPhonebook);
		__ASSERT_ALWAYS(n != KErrNotFound, User::Leave(EFail));
		}
	CContactItemField& field = fieldset[n];
	CContactTextField* textField = field.TextStorage();
	TBuf<128> buf;
	TInt phonebookUidNum = aPhonebook.iUid;
	buf.AppendNum(phonebookUidNum);
	textField->SetTextL(buf);

	AddTextFieldL(KUidContactFieldFamilyName,KUidContactFieldVCardMapUnusedN,aName,*entry);
	AddTextFieldL(KUidContactFieldPhoneNumber,KUidContactFieldVCardMapTEL,aNumber,*entry);
	if (aPhonebook == KUidUsimAppAdnPhonebook)
		{
		AddTextFieldL(KUidContactFieldEMail,KUidContactFieldVCardMapUnusedN,aEmail,*entry);
		}
	TContactItemId id(iDb->AddNewContactL(*entry)); 
	CleanupStack::PopAndDestroy(entry);	
	__ASSERT_ALWAYS(id != KNullContactId, User::Leave(EFail));
	}

/** 
 * Add a contact to the ICC & database.
 * @param aName Name
 * @param aNumber Phone number
 * @param aSlotNumber Slot number
 * @param aPhonebook Phonebook UID (optional)
 * @param aEmail Email entry (optional)
 */
void CPhbkIntegrationTestUtility::AddContactL(const TDesC& aName, const TDesC& aNumber, const TDesC& aSlotNumber, const TUid aPhonebook, const TDesC& aEmail)
	{
	TContactItemId templateId(iDb->ICCTemplateIdL(aPhonebook));
	CContactItem* iccTemplate = iDb->ReadContactLC(templateId);
	CContactICCEntry* entry = CContactICCEntry::NewL(*iccTemplate);
	CleanupStack::PopAndDestroy(iccTemplate);
	CleanupStack::PushL(entry);
	//Set the phonebook field, if the field doesn't exist, it should be added
	CContactItemFieldSet& fieldset = entry->CardFields();
	TInt n = fieldset.Find(KUidContactFieldICCPhonebook);
	if(n==KErrNotFound)
		{
		CContactItemField* field = CContactItemField::NewLC(KStorageTypeText,KUidContactFieldICCPhonebook);
		entry->AddFieldL(*field);
		CleanupStack::Pop(field);
		CContactItemFieldSet& fieldset = entry->CardFields();
		n = fieldset.Find(KUidContactFieldICCPhonebook);
		__ASSERT_ALWAYS(n != KErrNotFound, User::Leave(EFail));
		}
	CContactItemField& field = fieldset[n];
	CContactTextField* textField = field.TextStorage();
	TBuf<128> buf;
	TInt phonebookUidNum = aPhonebook.iUid;
	buf.AppendNum(phonebookUidNum);
	textField->SetTextL(buf);

	AddTextFieldL(KUidContactFieldFamilyName,KUidContactFieldVCardMapUnusedN,aName,*entry);
	AddTextFieldL(KUidContactFieldPhoneNumber,KUidContactFieldVCardMapTEL,aNumber,*entry);
	AddTextFieldL(KUidContactFieldICCSlot,KUidContactFieldVCardMapTEL,aSlotNumber,*entry);
	if (aPhonebook == KUidUsimAppAdnPhonebook)
		{
		AddTextFieldL(KUidContactFieldEMail,KUidContactFieldVCardMapUnusedN,aEmail,*entry);
		}
	TContactItemId id(iDb->AddNewContactL(*entry)); 
	CleanupStack::PopAndDestroy(entry);	
	__ASSERT_ALWAYS(id != KNullContactId, User::Leave(EFail));
	}

/**
 * Add a new text field (aField) to the CContactICCEntry supplied by aIccEntry. 
 * @param aFieldType Field type
 * @param aMapping Mapping for the field's content type
 * @param aField Field data
 * @param aIccEntry CContactICCEntry item
 */ 
void CPhbkIntegrationTestUtility::AddTextFieldL(TFieldType aFieldType, TUid aMapping, const TDesC& aField, CContactICCEntry& aIccEntry)
	{
	CContactItemFieldSet& fieldSet = aIccEntry.CardFields();
	TInt pos = fieldSet.Find(aFieldType);
	if (pos!=KErrNotFound)
		{
		CContactItemField& field=fieldSet[pos];
		field.TextStorage()->SetTextL(aField);
		}
	else
		{
		CContactItemField* field=CContactItemField::NewLC(KStorageTypeText, aFieldType);
		field->SetMapping(aMapping);
		field->AddFieldTypeL(aFieldType); 
		field->TextStorage()->SetTextL(aField);
		aIccEntry.AddFieldL(*field);
		CleanupStack::Pop(field); 
		}
	}

/** Delete an ICC contact - normal case */
void CPhbkIntegrationTestUtility::DeleteContactL(const TUid aPhonebook, TInt aIndex)
	{
	iDb->SetDbViewContactType(KUidContactICCEntry);
	// get the unique groupId for the given phonebook
	TContactItemId groupId(KNullContactId);
	User::LeaveIfError(iSession.GetPhoneBookId(groupId, 
						RPhoneBookSession::ESyncGroupId,
						aPhonebook));
	__ASSERT_ALWAYS(groupId != KNullContactId, User::Leave(EFail));
  
	// based on the groupId, get items belonging to the phonebook
	CContactGroup* group = NULL;
	TRAPD(err, group = static_cast<CContactGroup*>(iDb->ReadContactL(groupId)));
	__ASSERT_ALWAYS(err == KErrNone, User::Leave(EFail));
	CleanupStack::PushL(group);
	const CContactIdArray* array = group->ItemsContained();

	// delete item
	TInt count = array->Count();
	__ASSERT_ALWAYS(count >= aIndex, User::Leave(EFail));
	TContactItemId id = (*array)[aIndex];
	iDb->DeleteContactL(id);

	//check the item has been removed from the database
	CContactItem* item=NULL; 
	TRAP(err, item = iDb->ReadContactL(id)); 
	__ASSERT_ALWAYS(err == KErrNotFound, User::Leave(EFail));
	delete item;

	CleanupStack::PopAndDestroy(); // group
	}

/** 
 * Find contact IDs that match aFindString in the field specified by aFieldToSearch.
 * @param aFindString Text to search for
 * @param aFieldToSearch Field to search
 * @param aExpectedMatches Expected number of matching contacts
 */
void CPhbkIntegrationTestUtility::FindContactsL(const TDesC& aFindString, TFieldType aFieldToSearch, TInt aExpectedMatches)
	{
	CContactItemFieldDef* fieldDef=new(ELeave) CContactItemFieldDef;
	CleanupStack::PushL(fieldDef);
	fieldDef->AppendL(aFieldToSearch);
	CContactIdArray* array = iDb->FindLC(aFindString,fieldDef);
	__ASSERT_ALWAYS(array->Count() == aExpectedMatches, User::Leave(EFail));
	CleanupStack::PopAndDestroy(2,fieldDef);
	}

/** 
 * Replace the current name field with another name
 * @param aItem Reference to CContactItem
 * @param aModifiedName New name to use
 */
void CPhbkIntegrationTestUtility::ChangeNameFieldL(CContactItem& aItem, const TDesC& aModifiedName)
	{
	CContactItemFieldSet& fieldset = aItem.CardFields();
	const TInt pos(fieldset.Find(KUidContactFieldFamilyName));	
	CContactItemField& field = fieldset[pos];
	CContactTextField* textfield = field.TextStorage();
	textfield->SetTextL(aModifiedName);
	}

/** 
 * Replace the current name field with another name
 * @param aItem Reference to CContactItem
 * @param aModifiedNumber New number to use
 */
void CPhbkIntegrationTestUtility::ChangeNumberFieldL(CContactItem& aItem, const TDesC& aModifiedNumber)
	{
	CContactItemFieldSet& fieldset = aItem.CardFields();
	const TInt pos(fieldset.Find(KUidContactFieldPhoneNumber));
	CContactItemField& field = fieldset[pos];
	CContactTextField* textfield = field.TextStorage();
	textfield->SetTextL(aModifiedNumber);
	}

/** Open an ICC contact for edit */
void CPhbkIntegrationTestUtility::OpenContactL()
	{
	iDb->SetDbViewContactType(KUidContactICCEntry);
	const CContactIdArray* sortedItems = iDb->SortedItemsL();
	TContactItemId id = (*sortedItems)[0];
	CContactICCEntry* entry = static_cast<CContactICCEntry*>(iDb->OpenContactLX(id));
	CleanupStack::PushL(entry);
	ChangeNameFieldL(*entry,KModifiedName);
	iDb->CloseContactL(id);
	CleanupStack::PopAndDestroy(2); //entry+lockrecord
	}

/** Edit an ICC contact */
void CPhbkIntegrationTestUtility::EditContactL()
	{
	iDb->SetDbViewContactType(KUidContactICCEntry);
	const CContactIdArray* sortedItems = iDb->SortedItemsL();
	TContactItemId id = (*sortedItems)[0];
	CContactICCEntry* entry = static_cast<CContactICCEntry*>(iDb->OpenContactLX(id));
	CleanupStack::PushL(entry);
	ChangeNameFieldL(*entry,KModifiedName);
	iDb->CommitContactL(*entry);
	CleanupStack::PopAndDestroy(2); //entry+lockrecord
	}

