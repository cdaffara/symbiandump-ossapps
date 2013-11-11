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
// Contains the Phonebook Synchroniser Plug-in implementation. 
// 
//

#include <e32base.h>
#include <cntdb.h>
#include <cntitem.h>
#include <cntdef.h>
#include <ecom/implementationproxy.h>

#include "phbksyncplugin.h"
#include "phbksynclog.h"
#include "SyncContactICCEntry.h"
#include "PhonebookManager.h"
#include "SyncEngineServer.h"



/** 
 *  Validate the contact data specified by aId can be read, edited or searched. 
 * 
 *  @param aOp  Validate operation requested (read/edit/search)
 *  @param aId  Contact item ID
 */
TInt CPhoneBookSyncPlugin::ValidateContact(TValidateOperation aOp, TContactItemId aId)
	{
	LOGPLUGIN3(_L8("ValidateContact(%d, 0x%08x)"), aOp, aId);

#ifdef SYMBIAN_CNTMODEL_V2
	return iSession.ValidateContact(aOp, aId);
#else
	if (!iInServerThread)
		{
		return iSession.ValidateContact(aOp, aId);
		}
	else
		{
		return KErrNone;
		}
#endif // SYMBIAN_CNTMODEL_V2
	} // CPhoneBookSyncPlugin::ValidateContact


/** 
 *  Write the contact data specified by aContactItem to the ICC. The slot 
 *  number used to write the data is specified by the KUidContactFieldICCSlot
 *  field. Similarly the phonebook Uid is specified by the KUidContactFieldICCPhonebook 
 *  field. 
 * 
 *  @param aContactItem  Contact item to write.
 *
 *  @return KErrNone if write to ICC was successful, otherwise return error
 */
TInt CPhoneBookSyncPlugin::ValidateWriteContact(CContactICCEntry& aContactItem)
	{
	LOGPLUGIN1(_L8("ValidateWriteContact()"));

#ifndef SYMBIAN_CNTMODEL_V2
	if (!iInServerThread)
		{
#endif // SYMBIAN_CNTMODEL_V2

	TRequestStatus status(0);
	TInt slotNum = ICCSlotNumber(aContactItem);

	TUid phonebookUid(TUid::Null()); // First check whether phonebook field is supplied
	TRAPD(error, phonebookUid = ICCPhonebookL(aContactItem)); 
	if (error != KErrNone)
		{
		return error; 
		}

	TBuf<KTemplateFieldLength> buf;

	if (phonebookUid == KUidIccPhonebookNotSpecified) // phonebook field not set within aContactItem 
		{
		iSession.WriteContact(status, aContactItem, slotNum);
		}
	else
		{
		iSession.WriteContact(status, aContactItem, slotNum, phonebookUid);
		}
	User::WaitForRequest(status);
	TInt ret(status.Int());

	if (ret == KErrNone)
		{
		// Server has added slot number, so add it to aContactItem
		buf.Zero();
		buf.AppendNum(slotNum);
		TRAP(ret, AddFieldValueL(aContactItem, KUidContactFieldICCSlot, buf));
		if(phonebookUid != KUidIccPhonebookNotSpecified) 
		// Server has added phonebook Uid, so add it to aContactItem
			{
			buf.Zero();
			buf.AppendNum(TUint(phonebookUid.iUid)); // Append integer part of UID as actual field value
			TRAP(ret, AddFieldValueL(aContactItem, KUidContactFieldICCPhonebook, buf));
			}
		}

	return ret;

#ifndef SYMBIAN_CNTMODEL_V2
		}
	else
		{
		return KErrNone;
		}
#endif // SYMBIAN_CNTMODEL_V2
	} // CPhoneBookSyncPlugin::ValidateWriteContact


/** 
 *  Delete the contact data from the ICC at the slot number specified 
 *  in the CContactICCEntry item ID specified.
 * 
 *  @param aId  Contact Item ID
 *
 *  @return KErrNone if ICC deletion was successful, otherwise return error
 */
TInt CPhoneBookSyncPlugin::DeleteContact(TContactItemId aId)
	{
	LOGPLUGIN2(_L8("DeleteContact(0x%08x)"), aId);
	
#ifndef SYMBIAN_CNTMODEL_V2
	if (!iInServerThread)
		{
#endif // SYMBIAN_CNTMODEL_V2

	TRequestStatus status;
	iSession.DeleteContact(status, aId);
	User::WaitForRequest(status);
	return status.Int();

#ifndef SYMBIAN_CNTMODEL_V2
		}
	else
		{
		return KErrNone;
		}
#endif // SYMBIAN_CNTMODEL_V2
	} // CPhoneBookSyncPlugin::DeleteContact


/** 
 *  Release any resources allocated by the plug-in and delete itself.
 * 
 *  This method is called by the destructor of Contact model's 
 *  CContactDatabase class - ensuring it will be called when the plug-in
 *  is unloaded.
 */
void CPhoneBookSyncPlugin::Release()
	{
	LOGPLUGIN1(_L8("Release()"));
	
	iSession.Close();
	delete this;
	} // CPhoneBookSyncPlugin::Release


/** 
 *  Update the lookup tables in the server.
 *  This method is called by the Contacts model when a new CContactICCEntry
 *  has been added to the database.
 *
 *  This is only important when it is done by a client other than the 
 *  phonebook synchronisation server because the sync server can do the 
 *  update when it knows what the UID of the newly added contact was.
 * 
 *  The plug-in should extract the appropriate details from the item 
 *  (eg. item UID, slot number and ICC phonebook type) and call the sync server to 
 *  update the lookup tables.
 *
 *  @param aContactItem  Contact Entry to update in the look up table.
 */
void CPhoneBookSyncPlugin::UpdatePostWriteL(const CContactICCEntry& aContactItem)
	{
	LOGPLUGIN1(_L8("UpdatePostWriteL()"));

#ifndef SYMBIAN_CNTMODEL_V2
	if (iInServerThread)
		{
		return;
		}
#endif // SYMBIAN_CNTMODEL_V2

	TContactItemId id(aContactItem.Id());
	TInt slotNum(ICCSlotNumber(aContactItem));
	// Extract phonebook type from CContactICCEntry
	TUid phonebook(ICCPhonebookL(const_cast<CContactICCEntry&>(aContactItem)));

	if(phonebook == KUidIccPhonebookNotSpecified) // Phonebook not specified so assume
		{
		iSession.UpdateLookuptable(id, slotNum);  // Global/GSM ADN phonebook is used
		}
	else
		{
		iSession.UpdateLookuptable(id, slotNum, phonebook); // phonebook supplied 
		}
	} // CPhoneBookSyncPlugin::UpdatePostWriteL


/** 
 *  Return the template that should be used with CContactICCEntry items residing on the 
 *  phonebook specified by the aPhonebookUid parameter.
 *
 *  @param aPhonebookUid  UID of ICC phonebook for which the template is needed
 *
 *  @return Template item ID 
 */
TContactItemId CPhoneBookSyncPlugin::ICCTemplateIdL(TUid aPhonebookUid)
	{
	LOGPLUGIN2(_L8("ICCTemplateIdL(0x%08x)"), aPhonebookUid);

	TContactItemId templateId(KNullContactId);

#ifdef SYMBIAN_CNTMODEL_V2
	User::LeaveIfError(iSession.GetPhoneBookId(templateId, RPhoneBookSession::ESyncTemplateId, aPhonebookUid));
#else
	if (!iInServerThread)
		{
		User::LeaveIfError(iSession.GetPhoneBookId(templateId, RPhoneBookSession::ESyncTemplateId, aPhonebookUid));
		}
#endif // SYMBIAN_CNTMODEL_V2

	return templateId;
	} // CPhoneBookSyncPlugin::ICCTemplateIdL


/** 
 *  Return the group to be used for the phonebook specified by the aPhonebookUid parameter.
 * 
 *  @param aPhonebookUid  UID of ICC phonebook for which the group UID is required 
 *
 *  @return Group item ID 
 */
TContactItemId CPhoneBookSyncPlugin::GroupIdL(TUid aPhonebookUid)
	{
	LOGPLUGIN2(_L8("GroupIdL(0x%08x)"), aPhonebookUid);

	TContactItemId groupId(KNullContactId);

#ifdef SYMBIAN_CNTMODEL_V2
	User::LeaveIfError(iSession.GetPhoneBookId(groupId, RPhoneBookSession::ESyncGroupId, aPhonebookUid));
#else
	if (!iInServerThread)
		{
		User::LeaveIfError(iSession.GetPhoneBookId(groupId, RPhoneBookSession::ESyncGroupId, aPhonebookUid));
		}
#endif // SYMBIAN_CNTMODEL_V2

	return groupId;
	} // CPhoneBookSyncPlugin::GroupIdL


/** 
 *  Request notification of a phbksync cache state change.
 *
 *  @param aStatus Request to be completed.
 */
void CPhoneBookSyncPlugin::NotifySyncStateChange(TRequestStatus& aStatus, TUid aPhonebookUid)
	{
	LOGPLUGIN2(_L8("NotifySyncStateChange(0x%08x)"), aPhonebookUid);

#ifndef SYMBIAN_CNTMODEL_V2
	__ASSERT_ALWAYS(!iInServerThread, User::Invariant()); 
#endif // SYMBIAN_CNTMODEL_V2

	iSession.NotifyPhBkCacheStateChange(aStatus, aPhonebookUid);
	} // CPhoneBookSyncPlugin::NotifySyncStateChange


/** 
 *  Cancel previous request for notification of phbksync cache state change.
 */
void CPhoneBookSyncPlugin::CancelNotifyRequest(TUid aPhonebookUid)
	{
	LOGPLUGIN2(_L8("CancelNotifyRequest(0x%08x)"), aPhonebookUid);

#ifndef SYMBIAN_CNTMODEL_V2
	__ASSERT_ALWAYS(!iInServerThread, User::Invariant()); 
#endif // SYMBIAN_CNTMODEL_V2

	iSession.CancelRequest(ESyncNotifyCacheStateChangeCancelClient, aPhonebookUid);
	} // CPhoneBookSyncPlugin::CancelNotifyRequest


/** 
 *  Return the current synchronisation state of the synchroniser.
 *  If the state is EErrorDuringSync, this method leaves with the last sync
 *  error recorded.
 *
 *  @return ETrue if cache valid. EFalse, otherwise
 */
TBool CPhoneBookSyncPlugin::IsSynchronisedL(TUid aPhonebookUid)
	{
	LOGPLUGIN2(_L8("IsSynchronisedL(0x%08x)"), aPhonebookUid);

#ifndef SYMBIAN_CNTMODEL_V2
	__ASSERT_ALWAYS(!iInServerThread, User::Invariant()); 
#endif // SYMBIAN_CNTMODEL_V2

	RPhoneBookSession::TSyncState state;

	User::LeaveIfError(iSession.GetPhoneBookCacheState(state, aPhonebookUid));
	
	if (state==RPhoneBookSession::EErrorDuringSync)
		{
		TInt syncError(KErrNone);
		User::LeaveIfError(iSession.GetLastSyncError(syncError, aPhonebookUid));
		User::Leave(syncError);
		}
	
	return (state==RPhoneBookSession::ECacheValid);
	} // CPhoneBookSyncPlugin::IsSynchronisedL

/** 
 * Returns the list of Phonebooks that may be present on the ICC.
 *
 * @param aPhonebookList List of the phonebooks. If successful this updated with current list of phonebooks, otherwise it is reset.
 * @return KErrNone or a system wide error such as KErrNoMemory.
 */
TInt CPhoneBookSyncPlugin::PhonebookList(RArray<TUid>& aPhonebookList)
	{
	LOGPLUGIN1(_L8("PhonebookList()"));

	//
	// Empty the list to start with...
	//
	aPhonebookList.Reset();

#ifndef SYMBIAN_CNTMODEL_V2
	if (iInServerThread)
		{
		return KErrNone;
		}
#endif // SYMBIAN_CNTMODEL_V2

	//
	// Return the list of phonebooks...
	//	
	RPhoneBookSession::TSyncState state;
	TInt  result;
		
	result = iSession.GetPhoneBookCacheState(state, KUidIccGlobalAdnPhonebook);
	if (result != KErrNotSupported  &&  result != KErrNotFound)
		{
		result = aPhonebookList.Append(KUidIccGlobalAdnPhonebook);
		if (result != KErrNone)
			{
			aPhonebookList.Reset();
			return result;
			}
		}

	result = iSession.GetPhoneBookCacheState(state, KUidIccGlobalSdnPhonebook);
	if (result != KErrNotSupported  &&  result != KErrNotFound)
		{
		result = aPhonebookList.Append(KUidIccGlobalSdnPhonebook);
		if (result != KErrNone)
			{
			aPhonebookList.Reset();
			return result;
			}
		}

	result = iSession.GetPhoneBookCacheState(state, KUidIccGlobalLndPhonebook);
	if (result != KErrNotSupported  &&  result != KErrNotFound)
		{
		result = aPhonebookList.Append(KUidIccGlobalLndPhonebook);
		if (result != KErrNone)
			{
			aPhonebookList.Reset();
			return result;
			}
		}

	result = iSession.GetPhoneBookCacheState(state, KUidUsimAppAdnPhonebook);
	if (result != KErrNotSupported  &&  result != KErrNotFound)
		{
		result = aPhonebookList.Append(KUidUsimAppAdnPhonebook);
		if (result != KErrNone)
			{
			aPhonebookList.Reset();
			return result;
			}
		}

	result = iSession.GetPhoneBookCacheState(state, KUidIccGlobalFdnPhonebook);
	if (result != KErrNotSupported  &&  result != KErrNotFound)
		{
		result = aPhonebookList.Append(KUidIccGlobalFdnPhonebook);
		if (result != KErrNone)
			{
			aPhonebookList.Reset();
			return result;
			}
		}

	return KErrNone;
	}


/**
 *  Extract the ICC slot number from the ICC slot field. 
 * 
 *  @param aContactItem CContactICCEntry item
 *
 *  @return ICC Slot number 
 */
TInt CPhoneBookSyncPlugin::ICCSlotNumber(const CContactICCEntry& aContactItem) const
	{
	LOGPLUGIN1(_L8("ICCSlotNumber()"));

	CContactItemFieldSet&  fieldset = aContactItem.CardFields();
	TInt  position = fieldset.Find(KUidContactFieldICCSlot);
	TInt  slotNum(KErrNotFound);
	if (position != KErrNotFound)
		{
		const CContactItemField&  field = fieldset[position];
		TPtrC  iccSlotField = field.TextStorage()->Text();
		TLex  input(iccSlotField);
		/*TInt ignore  = */ input.Val(slotNum);
		}

	return slotNum;
	} // CPhoneBookSyncPlugin::ICCSlotNumber


/**
 *  Extract the ICC phonebook Uid from the ICC phonebook field. It also creates the phonebook
 *  field if it is not supplied within aContactItem. 
 * 
 *  @param aContactItem CContactICCEntry item
 *
 *  @return TUid that represents the ICC Phonebook type
 */
TUid CPhoneBookSyncPlugin::ICCPhonebookL(CContactICCEntry& aContactItem) const
	{
	LOGPLUGIN1(_L8("ICCPhonebookL()"));

	TUid  phonebookUid;
	CContactItemFieldSet&  fieldset = aContactItem.CardFields();
	TInt  position = fieldset.Find(KUidContactFieldICCPhonebook);

	if (position != KErrNotFound)
		{
		const CContactItemField& field = fieldset[position];
		TPtrC phonebookField = field.TextStorage()->Text();
		TLex input(phonebookField);
		input.Val(phonebookUid.iUid);
		}
	else // phonebook field not supplied within aContactItem, so create one 
		{
		// Create phonebook type field within aContactItem 
		TBuf<KTemplateFieldLength> buf;
		phonebookUid = KUidIccPhonebookNotSpecified; // default phonebook Uid value 
		buf.AppendNum(TUint(phonebookUid.iUid));
		TPtrC phonebookPtrC(buf.Ptr());
		CContactItemField* field=CContactItemField::NewLC(KStorageTypeText, KUidContactFieldICCPhonebook);
		field->SetMapping(KUidContactFieldVCardMapNotRequired);
		field->AddFieldTypeL(KUidContactFieldICCPhonebook); // Appends a field type to the field's content type
		field->TextStorage()->SetTextL(phonebookPtrC);
		aContactItem.AddFieldL(*field); // Add this phonebook field to ICC contact item 
		CleanupStack::Pop(field); // No need to destroy it since contact item takes ownership of field
		}

	return phonebookUid;
	} // CPhoneBookSyncPlugin::ICCPhonebookL


/**
 *  Add the field value aField supplied by the phonebook synchroniser to the 
 *  item's field specified by the aFieldType parameter. 
 * 
 *  @param aContactItem  CContactICCEntry item
 *  @param aFieldType    Field type
 *  @param aField        Field data
 */
void CPhoneBookSyncPlugin::AddFieldValueL(CContactICCEntry& aContactItem, TFieldType aFieldType,
										  TDesC& aField)
	{
	LOGPLUGIN1(_L8("AddFieldValueL()"));

	CContactItemFieldSet&  fieldset = aContactItem.CardFields();
	TInt  position = fieldset.Find(aFieldType);
	CContactItemField&  field = fieldset[position];
	CContactTextField*  textField = field.TextStorage();

	if (!textField->IsFull())
		{
		textField->SetTextL(aField);
		}
	} // CPhoneBookSyncPlugin::AddFieldValueL


/** 
 *  Create a connection with the phonebook synchroniser server.
 */
void CPhoneBookSyncPlugin::ConstructL()
	{
	LOGPLUGIN1(_L8("ConstructL()"));

#ifdef SYMBIAN_CNTMODEL_V2
	User::LeaveIfError(iSession.Connect());
#else
	TName threadName = RThread().Name();
	iInServerThread = (threadName.CompareF(PHBKSYNC_ENGINE_NAME)==0);
	
	if (!iInServerThread)
		{
		User::LeaveIfError(iSession.Connect());
		}
#endif // SYMBIAN_CNTMODEL_V2
	} // CPhoneBookSyncPlugin::ConstructL


/** 
 *  Non-leaving factory construction method.
 *
 *  @return Pointer to plug-in or NULL if plug-in construction unsuccessful
 */
CPhoneBookSyncPlugin* CPhoneBookSyncPlugin::New()
	{
	CPhoneBookSyncPlugin* self = NULL;

	TRAPD(err,self = NewL());

	if (err != KErrNone)
		{
		return NULL;
		}
	else
		{
		return self;
		}
	} // CPhoneBookSyncPlugin::New


/** 
 *  Standard factory construction method
 */
CPhoneBookSyncPlugin* CPhoneBookSyncPlugin::NewL()
	{
	CPhoneBookSyncPlugin* self = new (ELeave) CPhoneBookSyncPlugin();
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);

	return self;
	} // CPhoneBookSyncPlugin::NewL


/**
 *  Implementation collection function.
 */
const TImplementationProxy ImplementationTable[] = 
    {
    IMPLEMENTATION_PROXY_ENTRY(0x1020428D, CPhoneBookSyncPlugin::NewL)
    };


/**
 *  Return the implementation table size.
 */
EXPORT_C const TImplementationProxy* ImplementationGroupProxy(TInt& aTableCount)
    {
    aTableCount = sizeof(ImplementationTable) / sizeof(TImplementationProxy);

    return ImplementationTable;
    } // ImplementationGroupProxy
