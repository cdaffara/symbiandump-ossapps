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
// Implementation of the Background Sync Engine server. The engine provides
// the ability to perform Phonebook Sync's long running operations (e.g. Sync,
// write contact and delete contact).
// 
//

/**
 @file
 @internalComponent
*/

#include <connect/sbdefs.h>
#include <commsdattypesv1_1.h>
#include <cdblen.h>

#include "common.h"
#include "PhonebookManager.h"
#include "SyncContactICCEntry.h"
#include "phbksynclog.h"
#include "SyncEngineServer.h"
#include "SyncEngineSession.h"
#include "SyncContactsWithICC.h"
#include "WriteContactToICC.h"
#include "DeleteContactFromICC.h"


using namespace CommsDat;


/**
 * Number of retries for connecting to Etel.
 */
const TInt KMaxConnectToEtelRetries = 3;


//
// Teleplate names.  Should be in a resource file for localisation really!
//
_LIT(KPhbkTemplateADN, "SIM Card Contacts ADN");
_LIT(KPhbkTemplateSDN, "SIM Card Contacts SDN");
_LIT(KPhbkTemplateLND, "SIM Card Contacts LND");
_LIT(KPhbkTemplateUSimApp, "SIM Card Contacts USIMAPP");
_LIT(KPhbkTemplateFDN, "SIM Card Contacts FDN");
_LIT(KPhbkTemplateNotSpecified, "SIM Card Contacts Name Not Specified");


//
// Constants for encoding contacts...
//
_LIT(KEngineInternationalPrefix,"+");


//
//  Definition of iPolicy dictating the capability checking for phbksyncsvr...
//
const TUint iEngineRangeCount = 10;
	
const TInt CSyncEngineServer::iRanges[iEngineRangeCount] = 
	{
	0,		//range is 0
	1,		//range is 1-3 inclusive
	4,		//range is 4-5 inclusive
	6,		//range is 6
	7,		//range is 7-13 inclusive
	14,		//range is 14
	15,		//range is 15-16 inclusive
	17,		//range is 17-24 inclusive
	25,		//range is 25 inclusive
	26,		//range is 26-KMaxTInt inclusive
	};

const TUint8 CSyncEngineServer::iElementsIndex[iEngineRangeCount] = 
	{
	0,
	1,
	2,
	3,
	4,
	5,
	6,
	7,
	8,
	CPolicyServer::ENotSupported,
	};

const CPolicyServer::TPolicyElement CSyncEngineServer::iElements[] = 
	{
	{ _INIT_SECURITY_POLICY_C2( ECapabilityReadUserData, ECapabilityWriteUserData), CPolicyServer::EFailClient},	// policy 0:  range 0 - 0
	{ _INIT_SECURITY_POLICY_C1( ECapability_None), CPolicyServer::EFailClient},	// policy 1:  range 1 - 3
	{ _INIT_SECURITY_POLICY_C1( ECapabilityWriteUserData), CPolicyServer::EFailClient},	// policy 2:  range 4 - 5
	{ _INIT_SECURITY_POLICY_C1( ECapabilityReadUserData), CPolicyServer::EFailClient},	// policy 3:  range 6 - 6
	{ _INIT_SECURITY_POLICY_C1( ECapability_None), CPolicyServer::EFailClient},	// policy 4:  range 7 - 13
	{ _INIT_SECURITY_POLICY_C2( ECapabilityReadUserData, ECapabilityWriteUserData), CPolicyServer::EFailClient},	// policy 5:  range 14 - 14
	{ _INIT_SECURITY_POLICY_C1( ECapabilityWriteUserData), CPolicyServer::EFailClient},	// policy 6:  range 15 - 16
	{ _INIT_SECURITY_POLICY_C1( ECapability_None), CPolicyServer::EFailClient},	// policy 7:  range 17 - 24
#ifdef _DEBUG
	{ _INIT_SECURITY_POLICY_C1( ECapability_None), CPolicyServer::EFailClient}, // policy 8: range 25
#else
	{ _INIT_SECURITY_POLICY_FAIL}, // policy 8: range 25
#endif
	};

const CPolicyServer::TPolicy CSyncEngineServer::iPolicy = 
	{
	CPolicyServer::EAlwaysPass ,
	iEngineRangeCount,
	iRanges,
	iElementsIndex,
	iElements
	};


/**
 *  Static factory method used to create a CSyncEngineServer object.
 *
 *  @param aPhoneBookManager  Reference to the front-end servers's phonebook
 *                            manager.
 *
 *  @return  Pointer to the created CSyncEngineServer object, or NULL.
 */
CSyncEngineServer* CSyncEngineServer::NewL(CPhoneBookManager& aPhoneBookManager)
	{
	LOGENGINE1(_L8("NewL()"));

	CSyncEngineServer*  self = new (ELeave) CSyncEngineServer(aPhoneBookManager);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);

	return self;
	} // CSyncEngineServer::NewL


/**
 *  Standard constructor.
 *
 *  @param aPhoneBookManager  Reference to the front-end servers's phonebook
 *                            manager.
 */
CSyncEngineServer::CSyncEngineServer(CPhoneBookManager& aPhoneBookManager)
	: CPolicyServer(EPriorityNormal, iPolicy, ESharableSessions), 
	iICCCaps(RMobilePhone::KCapsSimAccessSupported), 
	iPhoneBookSyncEngineStarter(NULL),
	iConnectedSession(NULL),
	iPhonebookManager(aPhoneBookManager),
	iSyncContactsWithICC(NULL),
	iWriteContactToICC(NULL),
	iDeleteContactFromICC(NULL)
	{
	__DECLARE_NAME(_L("CSyncEngineServer"));
	} // CSyncEngineServer::CSyncEngineServer


/**
 *  Second phase constructor. Ensures the server is created and ready to run.
 */
void CSyncEngineServer::ConstructL()
	{
	StartL(PHBKSYNC_ENGINE_NAME);
	} // CSyncEngineServer::ConstructL


/**
 *  Destructor.
 */
CSyncEngineServer::~CSyncEngineServer()
	{
	//
	// Stop the engine from starting if it has initiated a startup.
	//
	delete iPhoneBookSyncEngineStarter;
	iPhoneBookSyncEngineStarter = NULL;
	
	//
	// Unconfigure the engine if needed...
	//
	TRAP_IGNORE(UnconfigureEngineL());

	delete iSyncContactsWithICC;
	delete iWriteContactToICC;
	delete iDeleteContactFromICC;
	} // CSyncEngineServer::~CSyncEngineServer


/**
 *  Configures the engine for use. This includes opening the ETel handles,
 *  connecting to the contacts database and opening the phonebook stores.
 */
void CSyncEngineServer::ConfigureEngineL()
	{
	LOGENGINE1(_L8("CSyncEngineServer::ConfigureEngineL()"));

	//
	// Connect to ETel...
	//
	TInt  etelErr(KErrNone), retryCount;

	for (retryCount = 0;  retryCount < KMaxConnectToEtelRetries;  retryCount++)
		{
		TRAP(etelErr, ConnectToEtelL());	//This can leave due to a denied access 										
											//to CommDb or Etel
		if (etelErr == KErrNone)
			{
			break;
			}

		if (retryCount < KMaxConnectToEtelRetries - 1)
			{
			User::After(1000000);
			}
		}
	User::LeaveIfError(etelErr);

	//
	// Open the Contacts DB...
	//
	TInt  dbErr(KErrNone);

	for (retryCount = 0;  retryCount < KMaxDbAccessRetryCount;  retryCount++)
		{
		TRAP(dbErr, iDb = CContactDatabase::OpenL());	// First try to open existing database
		
		if (dbErr == KErrNotFound)
			{
			TRAP(dbErr, iDb = CContactDatabase::CreateL()); // Database does not exist so create and open new one
			}
		
		if (dbErr == KErrNone)
			{
			break;
			}

		if (retryCount < KMaxDbAccessRetryCount - 1)
			{
			User::After(1000000);
			}
		}
	User::LeaveIfError(dbErr);

	//
	// Setup the phonebook parameters, read the settings from the INI
	// file and create the phonebook group IDs...
	//
	if (iPhone.GetIccAccessCaps(iICCCaps) != KErrNone) // Get type of ICC that is being used 
		{
		iICCCaps = RMobilePhone::KCapsSimAccessSupported; // set default value
		}

	CreatePhoneBookIdsL();
	} // CSyncEngineServer::ConfigureEngineL


/**
 *  Unconfigures the engine ready to shutdown.
 */
void CSyncEngineServer::UnconfigureEngineL()
	{
	LOGENGINE1(_L8("CSyncEngineServer::UnconfigureEngineL()"));

	iPhonebookManager.ClosePhoneBookStores();

	delete iDb;
	iDb = NULL;

	iPhone.Close();
	iEtelServer.Close();
	} // CSyncEngineServer::UnconfigureEngineL


/**
 *  Create a Template ID for the specified phonebook.
 *
 *  @param aPhonebookUid  UID of the phonebook to create an ID for.
 *  @param aICCCaps       The capabilities of the phone.
 */
void CSyncEngineServer::CreateTemplateIdL(TUid aPhonebookUid,
										  TUint32 aICCCaps)
	{
	LOGENGINE2(_L8("CreateTemplateIdL(0x%08x)"), aPhonebookUid);

	TContactItemId  templateId(KNullContactId);
	
	//
	// Check if there is an existing template in the contact database for
	// this phonebook...
	//
	CContactIdArray*  idList = iDb->GetCardTemplateIdListL();
	CleanupStack::PushL(idList);	
	
	if (idList != NULL  &&  idList->Count() > 0)
		{
		TInt  idCount = idList->Count();

		for (TInt index = 0;  index < idCount;  index++)
			{
			CContactCardTemplate* item = static_cast<CContactCardTemplate*> (iDb->ReadContactLC((*idList)[index]));

			if ((aPhonebookUid == KUidIccGlobalAdnPhonebook  && 
				 item->GetTemplateLabelL() == KPhbkTemplateADN)  ||
				(aPhonebookUid == KUidIccGlobalSdnPhonebook  && 
				 item->GetTemplateLabelL() == KPhbkTemplateSDN)  ||
				(aPhonebookUid == KUidIccGlobalLndPhonebook  && 
				 item->GetTemplateLabelL() == KPhbkTemplateLND)  ||
				(aPhonebookUid == KUidUsimAppAdnPhonebook  && 
				 item->GetTemplateLabelL() == KPhbkTemplateUSimApp)  ||
				(aPhonebookUid == KUidIccGlobalFdnPhonebook  && 
				 item->GetTemplateLabelL() == KPhbkTemplateFDN))
				{
				templateId = (*idList)[index];

				User::LeaveIfError(iPhonebookManager.SetTemplateId(aPhonebookUid,
																   templateId));

				index = idCount; // to terminate loop
				}
			CleanupStack::PopAndDestroy(item);
			}
		}
	CleanupStack::PopAndDestroy(idList);

	//
	// If no suitable template been found create a new one...
	//
	if (templateId == KNullContactId)
		{
		LOGENGINE2(_L8("CreateTemplateIdL(0x%08x): Creating template."),
				   aPhonebookUid);

		CContactItem*  newTemplate;

		if (aPhonebookUid == KUidIccGlobalAdnPhonebook)
			{
			newTemplate = iDb->CreateContactCardTemplateLC(KPhbkTemplateADN);
			}
		else if (aPhonebookUid == KUidIccGlobalSdnPhonebook)
			{
			newTemplate = iDb->CreateContactCardTemplateLC(KPhbkTemplateSDN);
			}
		else if (aPhonebookUid == KUidIccGlobalLndPhonebook)
			{
			newTemplate = iDb->CreateContactCardTemplateLC(KPhbkTemplateLND);
			}
		else if (aPhonebookUid == KUidUsimAppAdnPhonebook)
			{
			newTemplate = iDb->CreateContactCardTemplateLC(KPhbkTemplateUSimApp);
			}
		else if (aPhonebookUid == KUidIccGlobalFdnPhonebook)
			{
			newTemplate = iDb->CreateContactCardTemplateLC(KPhbkTemplateFDN);
			}
		else
			{
			newTemplate = iDb->CreateContactCardTemplateLC(KPhbkTemplateNotSpecified);
			}

		templateId = newTemplate->Id();
		CleanupStack::PopAndDestroy(newTemplate);

		//
		// Remove all the unnecessary fields...
		//
		newTemplate = iDb->OpenContactLX(templateId);
		CleanupStack::PushL(newTemplate);	
		const TInt fieldCount = newTemplate->CardFields().Count();
		for(TInt i=fieldCount-1;i>=0;i--)
			{
			newTemplate->RemoveField(i);
			}

		//
		// Add default name field...
		//
		CContactItemField* name = CContactItemField::NewLC(KStorageTypeText,KUidContactFieldFamilyName);
		name->SetMapping(KUidContactFieldVCardMapUnusedN);
		newTemplate->AddFieldL(*name);
		CleanupStack::Pop(name);
			
		//
		// Add second name field...
		//
		CContactItemField* secondName = CContactItemField::NewLC(KStorageTypeText,KUidContactFieldSecondName);
		secondName->SetMapping(KUidContactFieldVCardMapSECONDNAME);
		secondName->SetUserFlags(EContactCategoryHome);
		newTemplate->AddFieldL(*secondName);
		CleanupStack::Pop(secondName);
			
		//
		// Add default number field...
		//
		CContactItemField* number = CContactItemField::NewLC(KStorageTypeText,KUidContactFieldPhoneNumber);
		number->SetMapping(KUidContactFieldVCardMapTEL);
		number->AddFieldTypeL(KUidContactFieldVCardMapWORK);
		number->AddFieldTypeL(KUidContactFieldVCardMapVOICE);
		number->AddFieldTypeL(KUidContactFieldVCardMapCELL);
		newTemplate->AddFieldL(*number);
		CleanupStack::Pop(number);

		//
		// Add Slot Number field...
		//
		CContactItemField* slotnum = CContactItemField::NewLC(KStorageTypeText,KUidContactFieldICCSlot);
		slotnum->SetMapping(KUidContactFieldVCardMapNotRequired);
		newTemplate->AddFieldL(*slotnum);
		CleanupStack::Pop(slotnum);

		//
		// Add Phonebook type field...
		//
		CContactItemField* phonebook = CContactItemField::NewLC(KStorageTypeText,KUidContactFieldICCPhonebook);
		phonebook->SetMapping(KUidContactFieldVCardMapNotRequired);
		newTemplate->AddFieldL(*phonebook);
		CleanupStack::Pop(phonebook);

		//
		// 3G ICC so there are additional fields for ADN and USIM App
		// phonebooks.
		//
		if (aICCCaps & RMobilePhone::KCapsUSimAccessSupported)
			{
			if (aPhonebookUid == KUidIccGlobalAdnPhonebook  || 
			    aPhonebookUid == KUidUsimAppAdnPhonebook)
				{
				//
				// Add e-mail field...
				//
				CContactItemField* emailField = CContactItemField::NewLC(KStorageTypeText,KUidContactFieldEMail);
				emailField->SetMapping(KUidContactFieldVCardMapUnusedN);
				newTemplate->AddFieldL(*emailField);
				CleanupStack::Pop(emailField);

				//
				// Add group field - this is different from contacts group.
				// This field indicates group that this ICC entry belongs to.
				// User can add this entry to a number of groups on ICC i.e.
				// business, private, etc. 
				//
				CContactItemField* group = CContactItemField::NewLC(KStorageTypeText,KUidContactFieldICCGroup);
				group->SetMapping(KUidContactFieldVCardMapUnusedN);
				newTemplate->AddFieldL(*group);
				CleanupStack::Pop(group);
				}
			}

		//
		// Store the new Template ID...
		//
		User::LeaveIfError(iPhonebookManager.SetTemplateId(aPhonebookUid, templateId));

		iDb->CommitContactL(*newTemplate);
		CleanupStack::PopAndDestroy(2); // newTemplate plus locked record
		}
	} // CSyncEngineServer::CreateTemplateIdL

		
/**
 *  Create a Group ID for the specified phonebook.
 *
 *  @param aPhonebookUid  UID of the phonebook to create an ID for.
 */
void CSyncEngineServer::CreateGroupIdL(TUid aPhonebookUid)
	{
	LOGENGINE2(_L8("CreateGroupIdL(0x%08x)"), aPhonebookUid);

	TContactItemId  groupId(KNullContactId);

	//
	// Check if there is an existing group in the contact database for this
	// phonebook...
	//
	CContactIdArray*  idList = iDb->GetGroupIdListL();
	CleanupStack::PushL(idList);

	if (idList != NULL  &&  idList->Count() > 0)
		{
		LOGENGINE2(_L8("CreateGroupIdL(0x%08x): Creating template."),
				   aPhonebookUid);

		TInt  idCount = idList->Count();

		for (TInt index = 0;  index < idCount;  index++)
			{
			//
			// Retrieve the first entry in each group, obtain that entry's
			// template ID and check the template label to confirm if it
			// relates to this phonebook.
			//
			CContactGroup*  group = static_cast<CContactGroup*> (iDb->ReadContactLC((*idList)[index]));
			CContactIdArray*  itemList = group->ItemsContainedLC();

			if (itemList->Count() > 0)
				{
				CContactItem*  groupItem = iDb->ReadContactLC((*itemList)[0]);
				TContactItemId templateId = groupItem->TemplateRefId();
				if (templateId != KNullContactId)
					{
					CContactCardTemplate*  tempItem = static_cast<CContactCardTemplate*> (iDb->ReadContactLC(templateId));
					
					TPtrC templateLabel;
					TRAPD(errorCheck, templateLabel.Set(tempItem->GetTemplateLabelL()));
					
					// Check if the first item in the group uses a template with a label then check to see if the
					// label matches one of the Phonebook template labels
					if(errorCheck != KErrNotFound)
						{
						if(	(aPhonebookUid == KUidIccGlobalAdnPhonebook && templateLabel == KPhbkTemplateADN) ||
				   			(aPhonebookUid == KUidIccGlobalSdnPhonebook && templateLabel == KPhbkTemplateSDN) ||
						   	(aPhonebookUid == KUidIccGlobalLndPhonebook && templateLabel == KPhbkTemplateLND) ||
						   	(aPhonebookUid == KUidIccGlobalFdnPhonebook && templateLabel == KPhbkTemplateFDN) ||
							(aPhonebookUid == KUidUsimAppAdnPhonebook   && templateLabel == KPhbkTemplateUSimApp))
							{
							groupId = (*idList)[index];
							User::LeaveIfError(iPhonebookManager.SetGroupId(aPhonebookUid,groupId));
							index = idCount; // to terminate loop
							}
						}
					CleanupStack::PopAndDestroy(tempItem);	
					}
				CleanupStack::PopAndDestroy(groupItem);
				}
			CleanupStack::PopAndDestroy(2, group); // itemList, group
			}
		}

	//
	// If no suitable group has been found create a new one...
	//
	if (groupId == KNullContactId)
		{
		//
		// Store the new group ID...
		//
 		CContactItem*  group = iDb->CreateContactGroupLC(KNullDesC);
 		groupId = group->Id();

		User::LeaveIfError(iPhonebookManager.SetGroupId(aPhonebookUid, groupId));
 		CleanupStack::PopAndDestroy(group);
		}

	CleanupStack::PopAndDestroy(idList);
	} // CSyncEngineServer::CreateGroupIdL


/**
 *  Create the template ID and group ID for all supported phonebooks. These
 *  IDs are to be used with the entries that reside on a particular phonebook.
 */
void CSyncEngineServer::CreatePhoneBookIdsL()
	{
	LOGENGINE1(_L8("CreatePhoneBookIdsL()"));

	//
	// Ensure each phonebook has its templates created...
	//
	TInt  phonebookUidCount(iPhonebookManager.GetPhonebookCount());
	
	for (TInt index = 0;  index < phonebookUidCount;  index++)
		{
		TUid  phonebookUid;
		
		User::LeaveIfError(iPhonebookManager.GetPhonebookUid(index,
															 phonebookUid));

		LOGENGINE2(_L8("CreatePhoneBookIdsL: Phonebook=0x%08x"), phonebookUid);

		//
		// Get the template ID...
		//
		TContactItemId  templateId;
		
		User::LeaveIfError(iPhonebookManager.GetTemplateId(phonebookUid,
														   templateId));

		//
		// Validate any template ID already provided in INI file
		//
		if (templateId != KNullContactId)
			{
			CContactItem*  item(NULL);
			
			TRAPD(err, item = iDb->ReadContactL(templateId));
			CleanupStack::PushL(item);
			
			if (err == KErrNotFound)
				{
 				User::LeaveIfError(iPhonebookManager.SetTemplateId(phonebookUid,
 																   KNullContactId));
				templateId = KNullContactId;
				}
			else if (err != KErrNone)
				{
				User::Leave(err);
				}
			else if (item == NULL  ||  item->Type() != KUidContactCardTemplate)
				{
 				User::LeaveIfError(iPhonebookManager.SetTemplateId(phonebookUid,
 																   KNullContactId));
				templateId = KNullContactId;
				}
			else
				{
				TPtrC  label = static_cast<CContactCardTemplate*>(item)->GetTemplateLabelL();
				
				//
				// The following test confirms if a template ID relates to this
				// phonebook's template by checking the label employed.
				//
				if (!((phonebookUid == KUidIccGlobalAdnPhonebook  &&  label == KPhbkTemplateADN)  ||
					  (phonebookUid == KUidIccGlobalSdnPhonebook  &&  label == KPhbkTemplateSDN)  ||
					  (phonebookUid == KUidIccGlobalLndPhonebook  &&  label == KPhbkTemplateLND)  ||
					  (phonebookUid == KUidUsimAppAdnPhonebook  &&  label == KPhbkTemplateUSimApp)  ||
					  (phonebookUid == KUidIccGlobalFdnPhonebook  &&  label == KPhbkTemplateFDN)))
					{
	 				User::LeaveIfError(iPhonebookManager.SetTemplateId(phonebookUid,
 																	   KNullContactId));
					templateId = KNullContactId;
					}
				}

			CleanupStack::PopAndDestroy(item);
			}

		//
		// If we don't have a template ID, then create a template...
		//
		if (templateId == KNullContactId)
			{
			CreateTemplateIdL(phonebookUid, iICCCaps);
			}

		//
		// Get the group ID...
		//
		TContactItemId  groupId;
		
		User::LeaveIfError(iPhonebookManager.GetGroupId(phonebookUid, groupId));

		//
		// Validate any group ID already provided in INI file
		//
		if (groupId != KNullContactId)
			{
			CContactItem*  item = NULL;
			TRAPD(err, item = iDb->ReadContactL(groupId));
			CleanupStack::PushL(item);

			if (err == KErrNotFound)
				{
				User::LeaveIfError(iPhonebookManager.SetGroupId(phonebookUid,
																KNullContactId));
				groupId = KNullContactId;
				}
			else if (err != KErrNone)
				{
				User::Leave(err);
				}
			else if (item == NULL  ||  item->Type() != KUidContactGroup)
				{
				User::LeaveIfError(iPhonebookManager.SetGroupId(phonebookUid,
																KNullContactId));
				groupId = KNullContactId;
				}
			else
				{
				//
				// The following test is a simplistic method to determine
				// if the group ID represents a phonebook group - as
				// employed by LPD for Linnea. It does not attempt to check
				// if it relates to the correct type of phonebook.
				//
				if (static_cast<CContactGroup*>(item)->GetGroupLabelL() != KNullDesC)
					{
					User::LeaveIfError(iPhonebookManager.SetGroupId(phonebookUid,
																	KNullContactId));
					groupId = KNullContactId;
					}
				}
			CleanupStack::PopAndDestroy(item);
			}

		if (groupId == KNullContactId)
			{
			CreateGroupIdL(phonebookUid);
			}
		}
	} // CSyncEngineServer::CreatePhoneBookIdsL


/**
 *  Removes all the contact entries for a phonebook. This is called when a
 *  sync request is made on an unsupported phonebook so that it can be
 *  cleared.
 *
 *  @param aPhonebookUid  UID of the ICC phonebook to perform the operation on.
 */
void CSyncEngineServer::RemoveAllContactsForPhoneBookL(TUid aPhonebookUid)
	{
	LOGENGINE2(_L8("RemoveAllContactsForPhoneBookL(0x%08x)"), aPhonebookUid);

	//
	// Get the Group ID to search for entries. If no ID exists then the
	// database does not contain any entries.
	//
	TContactItemId  groupId;
	
	User::LeaveIfError(iPhonebookManager.GetGroupId(aPhonebookUid, groupId));

 	//
 	// Delete all entries for this group ID...
 	//
 	if (groupId != KNullContactId  &&  groupId != KGoldenTemplateId)
		{
 		CContactItem*  group = iDb->ReadContactLC(groupId);
 		const CContactIdArray*  members = static_cast<CContactGroup*>(group)->ItemsContained();
 		const TInt  count = members->Count();

		for (TInt item = 0;  item < count;  item++)
			{
			iDb->DeleteContactL((*members)[item]);
			}

		CleanupStack::PopAndDestroy(group);

		//
		// Compress the database if needed and we are finished!
		//
		if (iDb->CompressRequired())
			{
			TRAP_IGNORE(iDb->CompactL());
			}
		}
	} // CSyncEngineServer::RemoveAllContactsForPhoneBookL


/**
 *  This method synchronises the specified phonebook. The synchronisation is
 *  done by creating an instance of the CSyncContactsWithICC Active Object 
 *  class and then starting it.
 *
 *  @param aMessage       A reference to the front-end server request.
 *  @param aPhonebookUid  UID of the ICC phonebook to perform the operation on.
 */
void CSyncEngineServer::DoSynchronisationL(const RMessage2& aMessage,
										   TUid aPhonebookUid)
	{
	LOGENGINE2(_L8("DoSynchronisationL(0x%08x)"), aPhonebookUid);

	//
	// Set the cache state to unsynchronised to start with.
	//
	TInt  result;

	result = iPhonebookManager.SetSyncState(aPhonebookUid,
											RPhoneBookSession::EUnsynchronised);
	if (result != KErrNone)
		{
		iConnectedSession->CompleteRequest(aMessage, result);
		return;
		}

	//
	// Has the PhBkInfo structure been received yet? If not then this
	// phonebook is not usable...
	//
	TInt  phBkInfoRetrievedResult;
	
	result = iPhonebookManager.GetPhBkInfoRetrievedResult(aPhonebookUid,
														  phBkInfoRetrievedResult);
	if (result != KErrNone)
		{
		iPhonebookManager.SetLastSyncError(aPhonebookUid, result);
   		iPhonebookManager.SetSyncState(aPhonebookUid, RPhoneBookSession::EErrorDuringSync);

		iConnectedSession->CompleteRequest(aMessage, result);
		return;
		}
	
	if (phBkInfoRetrievedResult != KErrNone)
		{
		//		
		// The phonebook has been found to not exist, and a sync has been
		// requested, so remove all entries from the database.
		//
		RemoveAllContactsForPhoneBookL(aPhonebookUid);

		iPhonebookManager.SetLastSyncError(aPhonebookUid, phBkInfoRetrievedResult);
   		iPhonebookManager.SetSyncState(aPhonebookUid, RPhoneBookSession::EErrorDuringSync);

		iConnectedSession->CompleteRequest(aMessage, phBkInfoRetrievedResult);
		return;
		}

	//
	// Check that the phone store was opened...
	//
	RMobilePhoneBookStore  phonebookStore;

	result = iPhonebookManager.GetPhoneBookStore(aPhonebookUid, iPhone,
												 phonebookStore);
	if (result != KErrNone)
		{
		iPhonebookManager.SetLastSyncError(aPhonebookUid, result);
   		iPhonebookManager.SetSyncState(aPhonebookUid, RPhoneBookSession::EErrorDuringSync);

		iConnectedSession->CompleteRequest(aMessage, result);
		return;
		}

	if (phonebookStore.SubSessionHandle() == 0)
		{
		//		
		// The phonebook is not supported and a sync has been
		// requested, so remove all entries from the database.
		//
		RemoveAllContactsForPhoneBookL(aPhonebookUid);

		iPhonebookManager.SetLastSyncError(aPhonebookUid, KErrNotSupported);
   		iPhonebookManager.SetSyncState(aPhonebookUid, RPhoneBookSession::EErrorDuringSync);

		iConnectedSession->CompleteRequest(aMessage, KErrNotSupported);
		return;
		}

	//
	// Ensure there is no left sync in progress (there should not be!)...
	//
	if (iSyncContactsWithICC != NULL)
		{
		iPhonebookManager.SetLastSyncError(aPhonebookUid, KErrServerBusy);
   		iPhonebookManager.SetSyncState(aPhonebookUid, RPhoneBookSession::EErrorDuringSync);

		iConnectedSession->CompleteRequest(aMessage, KErrServerBusy);
		return;
		}

	//
	// Create the Active Object that will perform the sync...
	//
	iSyncContactsWithICC = CSyncContactsWithICC::NewL(*iConnectedSession,
													  iPhonebookManager,
													  *iDb,
													  iPhone,
													  aPhonebookUid,
													  aMessage);

	//
	// Start reading phonebook entries and populating Contacts DB...
	//	
	TRAPD(error, iSyncContactsWithICC->SyncContactsWithICCL());
	if (error != KErrNone)
		{
		CompleteDoSync(error);
		}
	} // CSyncEngineServer::DoSynchronisationL


/**
 *  Delete an entry specified by aContactId from the ICC phonebook store
 *  specified. It performs the opperation by creating a CDeleteContactFromICC
 *  Active Object and starting it.
 *
 *  @param aMessage       A reference to the front-end server request.
 *  @param aPhonebookUid  UID of the phonebook containing the entry.
 *  @param aContactId     ID of the contact to delete.
 */
void CSyncEngineServer::DeleteCntFromICCL(const RMessage2& aMessage,
										  TUid aPhonebookUid,
										  TContactItemId  aContactId)
	{
	LOGENGINE2(_L8("DeleteCntFromICCL(0%d)"), aContactId);

	//
	// This request can only proceed if the cache is valid...
	//
	RPhoneBookSession::TSyncState  syncState;
	TInt  result;
	
	result = iPhonebookManager.GetSyncState(aPhonebookUid, syncState);
	if (result != KErrNone)
		{
		iConnectedSession->CompleteRequest(aMessage, result);
		return;
		}
	
	if (syncState != RPhoneBookSession::ECacheValid)
		{
		iConnectedSession->CompleteRequest(aMessage, KErrNotReady);
		return;
		}

	//
	// Check that ICC is not locked, blocked OR pin protected...
	//
	if (iPhonebookManager.IsPin1Valid() == EFalse)
		{
		iConnectedSession->CompleteRequest(aMessage, KErrAccessDenied);
		return;
		}

	//
	// If this is a USIM App ADN phonebook then check that USIM PIN is valid...
	//
	if (aPhonebookUid == KUidUsimAppAdnPhonebook  && 
		iPhonebookManager.IsUsimAppPinValid() == EFalse)
		{
		iConnectedSession->CompleteRequest(aMessage, KErrAccessDenied);
		return;
		}
				
	//
	// If this is the FDN phonebook then check that PIN2 is valid...
	//
	if (aPhonebookUid == KUidIccGlobalFdnPhonebook  && 
		iPhonebookManager.IsPin2Valid() == EFalse)
		{
		iConnectedSession->CompleteRequest(aMessage, KErrAccessDenied);
		return;
		}
				
	//
	// Now create and run the Active Object which will process this
	// delete request...
	//
	if (iDeleteContactFromICC != NULL)
		{
		iConnectedSession->CompleteRequest(aMessage, KErrServerBusy);
		return;
		}

	iDeleteContactFromICC = CDeleteContactFromICC::NewL(*iConnectedSession,
														iPhonebookManager,
														*iDb,
														iPhone,
														aPhonebookUid,
														aMessage); 
	iDeleteContactFromICC->DoIccDelete(aContactId);
	} // CSyncEngineServer::DeleteCntFromICCL


/**
 *  Completes an outstanding Delete-from-ICC request. It is called by the
 *  active object CPhoneBookDoIccDelete class when it has finished its task.
 *  The functin completes the front-end server's DeleteContact request.
 * 
 *  @param aRetVal  Complete-request error code.
 */
void CSyncEngineServer::CompleteDoIccDelete(TInt aRetVal)
	{
	LOGENGINE2(_L8("CompleteDoIccDelete(%d)"), aRetVal);

	//
	// Complete the session request and delete the Active Object...
	//
	iConnectedSession->CompleteRequest(iDeleteContactFromICC->ClientMessage(),
									   aRetVal);

	delete iDeleteContactFromICC;
	iDeleteContactFromICC = NULL;
	} // CSyncEngineServer::CompleteDoIccDelete


/**
 *  Write an entry to the ICC phonebook store. This is performed by creating an
 *  Active Object to request the write via ETel. The PIN1 and PIN2 (if FDN)
 *  must be valid to perform such a request.
 *
 *  @param aMessage     A reference to the front-end server request.
 *  @param aTemplateId  ID of the template to use.
 *  @param aBufferSize  Size of the streamed ICC buffer.
 */
void CSyncEngineServer::WriteCntToICCL(const RMessage2& aMessage,
									   TContactItemId aTemplateId,
									   TInt aBufferSize)
	{
	LOGENGINE3(_L8("CSyncEngineServer::WriteCntToICCL(): %d 0x%08x %d"),
			   aTemplateId, aBufferSize);

	//
	// Is the ICC still in usable state for this phonebook?
	//
	TUid  phonebookUid = iPhonebookManager.GetPhonebookUidFromTemplateId(aTemplateId);

	if (iPhonebookManager.IsPin1Valid() == EFalse  ||
	    (phonebookUid == KUidUsimAppAdnPhonebook  &&
	     iPhonebookManager.IsUsimAppPinValid() == EFalse)  ||
	    (phonebookUid == KUidIccGlobalFdnPhonebook  &&
	     iPhonebookManager.IsPin2Valid() == EFalse))
		{
		iConnectedSession->CompleteRequest(aMessage, KErrAccessDenied);
		return;
		}

	//
	// Create the Active Object which will process this Write request...
	//
	if (iWriteContactToICC != NULL)
		{
		iConnectedSession->CompleteRequest(aMessage, KErrServerBusy);
		return;
		}

	iWriteContactToICC = CWriteContactToICC::NewL(*iConnectedSession,
											 	  aBufferSize,
											 	  iPhonebookManager,
											 	  iPhone,
											 	  phonebookUid,
											 	  aMessage);
											 	  
	//
	// Setup the Active Object and begin...
	//
	TRAPD(error, iWriteContactToICC->DoIccWriteL());
	if (error != KErrNone)
		{
		CompleteWriteContactToICC(error);
		}
	} // CSyncEngineServer::WriteCntToICCL


/**
 *  Complete an outstanding Write-to-ICC request. It is called by the Active
 *  Object CWriteContactToICC class when it has finished its task. The ICC
 *  slot number is written back to the front-end address space and the
 *  request is completed.
 *
 *  @param aRetVal  Complete-request error code.
 */
void CSyncEngineServer::CompleteWriteContactToICC(TInt aRetVal)
	{
	LOGENGINE2(_L8("CompleteWriteContactToICC(%d)"), aRetVal);

	if (aRetVal == KErrNone)
		{
		TPckg<TInt>  indexPckg(iWriteContactToICC->SlotNum());
		TPckg<TUid>  phonebookUidPckg(iWriteContactToICC->PhonebookUid());

		TRAPD(err1, iWriteContactToICC->ClientMessage().WriteL(1, indexPckg));
		TRAPD(err2, iWriteContactToICC->ClientMessage().WriteL(2, phonebookUidPckg));

		__ASSERT_ALWAYS(err1 == KErrNone  &&  err2 == KErrNone,
						PhBkSyncPanic(EPhBkSyncPanicBadDescriptor));
		}

	//
	// Complete the session request and delete the Active Object...
	//
	iConnectedSession->CompleteRequest(iWriteContactToICC->ClientMessage(),
									   aRetVal);
	
    delete iWriteContactToICC;
	iWriteContactToICC = NULL;
	} // CSyncEngineServer::CompleteWriteContactToICC


/**
 *  Cancel a previous synchronisation request.
 *
 *  @param aPhonebookUid  UID of the ICC phonebook to cancel the sync request.
 *
 *  @return KErrNone if the request was cancelled, otherwise returns an error.
 */
TInt CSyncEngineServer::DoSynchronisationCancelL(TUid aPhonebookUid)
	{
	LOGENGINE1(_L8("CSyncEngineServer::DoSynchronisationCancelL()"));

	if (iSyncContactsWithICC != NULL  &&
	    iSyncContactsWithICC->PhonebookUid() == aPhonebookUid)
		{
		//
		// Cancel the Active Object which is used to sync the ICC.
		// We call DoCancel() rather than Cancel() for two reasons:
		//
		//  1) It allows the cancel to be asynchronous, which is important
		//     for TSY requests that may take a long time.
		//
		//  2) It ensures the RunL is called which will call the complete
		//     function and hence pass the request completion to the client.
		//
		iSyncContactsWithICC->DoCancel();
		}

	return(KErrNone);
	} // CSyncEngineServer::DoSynchronisationCancelL


/**
 *  Cancel a previous delete request.
 *
 *  @param aPhonebookUid  UID of the ICC phonebook to cancel the delete request.
 *
 *  @return KErrNone if the request was cancelled, otherwise returns an error.
 */
TInt CSyncEngineServer::DeleteCntFromICCCancelL(TUid aPhonebookUid)
	{
	LOGENGINE2(_L8("DeleteCntFromICCCancelL(0x%08x)"), aPhonebookUid);

	if (iDeleteContactFromICC != NULL  &&
		iDeleteContactFromICC->PhonebookUid() == aPhonebookUid)
		{
		//
		// Cancel the Active Object which is used to delete the ICC entry.
		// We call DoCancel() rather than Cancel() for two reasons:
		//
		//  1) It allows the cancel to be asynchronous, which is important
		//     for TSY requests that may take a long time.
		//
		//  2) It ensures the RunL is called which will call the complete
		//     function and hence pass the request completion to the client.
		//
		iDeleteContactFromICC->DoCancel();
		}

	return(KErrNone);
	} // CSyncEngineServer::DeleteCntFromICCCancelL


/**
 *  Cancel a previous write request.
 *
 *  @param aPhonebookUid  UID of the ICC phonebook to cancel the write request.
 *
 *  @return KErrNone if the request was cancelled, otherwise returns an error.
 */
TInt CSyncEngineServer::WriteCntToICCCancelL(TUid aPhonebookUid)
	{
	LOGENGINE2(_L8("WriteCntToICCCancelL(0x%08x)"), aPhonebookUid);

	if (iWriteContactToICC != NULL  &&
		iWriteContactToICC->PhonebookUid() == aPhonebookUid)
		{
		//
		// Cancel the Active Object which is used to write the ICC entry.
		// We call DoCancel() rather than Cancel() for two reasons:
		//
		//  1) It allows the cancel to be asynchronous, which is important
		//     for TSY requests that may take a long time.
		//
		//  2) It ensures the RunL is called which will call the complete
		//     function and hence pass the request completion to the client.
		//
		iWriteContactToICC->DoCancel();
		}

	return(KErrNone);
	} // CSyncEngineServer::WriteCntToICCCancelL


/**
 *  Connect to the ETel Sever, obtains the name of the currently selected TSY
 *  and then load the TSY.
 *
 *  @return KErrNone if connected successfully, otherwise return error.
 */
void CSyncEngineServer::ConnectToEtelL()
	{
	LOGENGINE1(_L8("ConnectToEtelL()"));
	
	//
	// Obtain the name of the currently selected TSY...
	//
#ifdef SYMBIAN_NON_SEAMLESS_NETWORK_BEARER_MOBILITY
	CMDBSession* db = CMDBSession::NewLC(KCDVersion1_2);
#else
	CMDBSession* db = CMDBSession::NewLC(KCDVersion1_1);
#endif
	
	CMDBField<TUint32>* globalSettingField = new(ELeave) CMDBField<TUint32>(KCDTIdModemPhoneServicesSMS);
	CleanupStack::PushL(globalSettingField);

	globalSettingField->SetRecordId(1);
	globalSettingField->LoadL(*db);
	TUint32 modemId = *globalSettingField;

	CMDBField<TDesC>* tsyField = new(ELeave) CMDBField<TDesC>(KCDTIdTsyName);
	CleanupStack::PushL(tsyField);

	tsyField->SetRecordId(modemId);
	TRAPD(err, tsyField->LoadL(*db));
	if (err != KErrNone)
		{
		LOGENGINE1(_L8("Unable to get default TSY"));
		}
	User::LeaveIfError(err);

	TBuf<KCommsDbSvrMaxFieldLength> tsyName;
	tsyName = *tsyField;

	CleanupStack::PopAndDestroy(3, db); // db, tsyField & globalSettingField
	
	TInt ret(iEtelServer.Connect()); // First connect to the ETel server
	if(ret==KErrNone)
		{
		LOGENGINE1(_L8("Loading TSY")); // Now load the TSY 
		ret=iEtelServer.LoadPhoneModule(tsyName);
		RTelServer::TPhoneInfo phoneInfo;
		if(ret==KErrNone)
			{
			// Determine if TSY supports V2 functionality
			// in event of a problem here assume that it does not
			ret = iEtelServer.IsSupportedByModule(tsyName, KETelExtMultimodeV2, iIsV2Tsy);
			if (ret != KErrNone)
				{
				iIsV2Tsy = EFalse;
				}

			TInt phoneIndex(0);
			iEtelServer.EnumeratePhones(phoneIndex); // Get total number of phones supported by all currently loaded TSY modules
			while(phoneIndex-->0)
				{
				TName searchTsyName;
				// Check whether this phone belongs to loaded TSY
				if((iEtelServer.GetTsyName(phoneIndex,searchTsyName)==KErrNone) && (searchTsyName.CompareF(tsyName)==KErrNone)) 
					break;
				}
			iEtelServer.GetPhoneInfo(phoneIndex,phoneInfo); // Get information associated with specified phone

			LOGENGINE1(_L8("Open the phone"));
			ret = iPhone.Open(iEtelServer,phoneInfo.iName); // Open and intialise the phone
			if (ret!=KErrNone)
				{
				LOGENGINE2(_L8("Open phone failed (ret=%d)"), ret);
				}

			ret = iEtelServer.SetExtendedErrorGranularity(RTelServer::EErrorExtended);
			if (ret!=KErrNone)
				{
				LOGENGINE2(_L8("Cannot request Extended Errors (ret=%d)"), ret);
				}
			}
		else
			{
			LOGENGINE2(_L8("Could not load the TSY (ret=%d)"), ret);
			}
		}
	else
		{
		LOGENGINE2(_L8("Could not connect to ETel (ret=%d)"), ret);
		}

	User::LeaveIfError(ret);
	} // CSyncEngineServer::ConnectToEtelL


/**
 *  Append additional field types according to textual alpha tags.
 *  The field will contain next types:
 *  TEL {HOME|WORK} {VOICE|CELL|FAX|....}
 *
 *  @param  aTextTag    String that is supposed to contain tags.
 *  @param  aCurrField  Pointer to the contact item field to which optional
 *                      types will be added.
 */
void CSyncEngineServer::AssignAlphaTagsToFieldTypeL(const TDesC16 &aTextTag,
												    CContactItemField* aCurrField)
{
	//
	// Exit immediately if there is no field pointer value...
	//
	if (aCurrField == NULL) 
		{
		return;
		}

	TFieldType alphaTagUID(KUidContactFieldVCardMapHOME); // Assume HOME by default

	CSyncContactICCEntry::TSyncEntryName textTag(aTextTag);
	textTag.UpperCase(); // convert text tag(s) to upper case, because later we will use KVersitParam* constants from "cntdef.h"

	//-- try to find out the main type of the phone (WORK/HOME). Assume HOME by default
	if(textTag.Find(KVersitParamWork)  >= 0) 
		alphaTagUID = KUidContactFieldVCardMapWORK; //-- found WORK tag, assume the work phone.

	if (!aCurrField->ContentType().ContainsFieldType(alphaTagUID))
		{
		aCurrField->AddFieldTypeL(alphaTagUID);        //-- append field type WORK/HOME
		}

	//-- now try to find more additional tags VOICE/FAX/CELL etc. Assume nothing by default.
	alphaTagUID = TUid::Null();

	if (textTag.Find(KVersitParamMsg) >= 0)
		{
		alphaTagUID = KUidContactFieldVCardMapMSG;
		}
	else if (textTag.Find(KVersitParamVoice) >= 0)
		{
		alphaTagUID = KUidContactFieldVCardMapVOICE;
		}  
	else if (textTag.Find(KVersitParamFax) >= 0)
		{
		alphaTagUID = KUidContactFieldVCardMapFAX;
		}  
	else if (textTag.Find(KVersitParamPref) >= 0)
		{
		alphaTagUID = KUidContactFieldVCardMapPREF;
		}  
	else if (textTag.Find(KVersitParamCell) >= 0)
		{
		alphaTagUID = KUidContactFieldVCardMapCELL;
		}  
	else if (textTag.Find(KVersitParamPager) >= 0)
		{
		alphaTagUID = KUidContactFieldVCardMapPAGER;
		}  
	else if (textTag.Find(KVersitParamBbs) >= 0)
		{
		alphaTagUID = KUidContactFieldVCardMapBBS;
		}  
	else if (textTag.Find(KVersitParamModem) >= 0)
		{
		alphaTagUID = KUidContactFieldVCardMapMODEM;
		}  
	else if (textTag.Find(KVersitParamCar) >= 0)
		{
		alphaTagUID = KUidContactFieldVCardMapCAR;
		}  
	else if (textTag.Find(KVersitParamIsdn)  >= 0)
		{
		alphaTagUID = KUidContactFieldVCardMapISDN;
		}  
	else if (textTag.Find(KVersitParamVideo) >= 0)
		{
		alphaTagUID = KUidContactFieldVCardMapVIDEO;
		}  

	if (alphaTagUID != TUid::Null())
		{
		//
		// Append additional field type...
		//
		if (!aCurrField->ContentType().ContainsFieldType(alphaTagUID))
			{
			aCurrField->AddFieldTypeL(alphaTagUID);
			}
		}
	} // CSyncEngineServer::AssignAlphaTagsToFieldTypeL


/**
 *  Decide whether the entry needs to be added to the contacts database or
 *  the existing one needs to be updated by comparing it to the one already
 *  stored in the database. 
 *
 *  Work out the write action required for this entry.
 *
 *  There are 3 possibilities when searching for this item in the lookup table:
 *    1 - item is not found - needs to be added
 *    2 - item is found and identical - no action required
 *    3 - item is found and different - needs to be updated
 *
 *
 *  Create an CContactICCEntry using the data provided by aContactItem and then
 *  write the entry to the Contacts Database.
 *
 *  If it is a new entry then create an CContactICCEntry using the data provided
 *  by aContactItem and then write to the Contacts Database. Otherwise edit the
 *  existing ICC contact item and commit the changes to the database. 
 * 
 *  If the entry is hidden it will be stored in the contacts database, but the
 *  server will also set the contact item's hidden attribute.
 *
 *  This means that the item will not be displayed if the view definition
 *  excludes hidden fields. 
 *
 * @param aICCEntry  Phonebook Server internal format for ICC Entry 
 *
 * @return Contacts ID if operation was successful, otherwise a NULL ID.
 */
TContactItemId CSyncEngineServer::WriteICCContactToDBL(CSyncContactICCEntry& aICCEntry)
	{
	LOGENGINE2(_L8("WriteICCContactToDBL(): 0x%08x"), aICCEntry.iPhonebookUid);
	
	//
	// Check if the ID does not yet exist, or is identical, to see what needs
	// to be done.
	//
	TInt  result;
	
	result = iPhonebookManager.GetContactIdFromSlotNum(aICCEntry.iPhonebookUid,
													   aICCEntry.iSlotNum,
													   aICCEntry.iContactId);
	if (result != KErrNone  &&  result != KErrNotFound)
		{
		User::Leave(result);
		}

	if (result == KErrNotFound)
		{
		//
		// The contact doesn't exist, we will continue and add it...
		//
		LOGENGINE2(_L8("WriteICCContactToDBL(): Slot %d add"), aICCEntry.iSlotNum);
		aICCEntry.iContactId = KNullContactId;
		}
	else if (EntriesIdenticalL(aICCEntry, aICCEntry.iContactId))
		{
		//
		// The contact exists, and is identical so there is nothing to do. We
		// can return now...
		//
		LOGENGINE2(_L8("WriteICCContactToDBL(): Slot %d nothing to do"), aICCEntry.iSlotNum);

		return aICCEntry.iContactId;
		}
	else
		{
		//
		// The entry needs to be updated. In this case, we will delete it now and then
		// continue and add it again (which is easier than clearing out all the fields
		// and lists etc.)...
		//
		LOGENGINE2(_L8("WriteICCContactToDBL(): Slot %d delete and add"), aICCEntry.iSlotNum);

		iDb->DeleteContactL(aICCEntry.iContactId);
		aICCEntry.iContactId = KNullContactId;
		}

	//
	// Get the template and group IDs...
	//
	TContactItemId  templateId;
	TContactItemId  groupId;
			
	User::LeaveIfError(iPhonebookManager.GetTemplateId(aICCEntry.iPhonebookUid, templateId));
	User::LeaveIfError(iPhonebookManager.GetGroupId(aICCEntry.iPhonebookUid, groupId));

	//
	// Create template for ICC contacts items...
	//
	CContactItem* iccTemplate = iDb->ReadContactL(templateId);
	CleanupStack::PushL(iccTemplate); 

	CContactICCEntry*  item = CContactICCEntry::NewL(*iccTemplate);
	CleanupStack::PopAndDestroy(iccTemplate); 
	CleanupStack::PushL(item); 

	//
	// Create phone number field(s)
	//

	// Default number 
	if (aICCEntry.iNumber.Length() != 0)
		{
		TBuf<RMobilePhone::KMaxMobileTelNumberSize>  number;
		if ((aICCEntry.iTON==RMobilePhone::EInternationalNumber) && (aICCEntry.iNumber[0] != KEngineInternationalPrefix()[0]))
			{
			number.Append(KEngineInternationalPrefix); // Append '+' prefix if International Number
			}
		number.Append(aICCEntry.iNumber);
		AddTextFieldToIccContactL(KStorageTypeText, KUidContactFieldPhoneNumber, KUidContactFieldVCardMapTEL, number, item, 0);
		}

	// Check whether this is hidden entry and set its hidden attributes 
	if (aICCEntry.iIsHidden)
		{
		item->SetHidden(ETrue);
		}

	// Additional numbers 
	TInt count(aICCEntry.iNumberList->Count());
	for (TInt i=0; i<count; ++i)
		{
		CSyncContactICCEntry::TSyncAdditionalNumber  additionalNum(aICCEntry.iNumberList->At(i));

		// Actual number
		TBuf<RMobilePhone::KMaxMobileTelNumberSize> number;
		if((additionalNum.iTON==RMobilePhone::EInternationalNumber) && (additionalNum.iNumber[0] != KEngineInternationalPrefix()[0])) 
			{
			number.Append(KEngineInternationalPrefix); // Append '+' prefix if International Number
			}	
		number.Append(additionalNum.iNumber);

		//-- add or modify existing text field in CContactICCEntry* item fieldset
		CContactItemField *pCurrField = 
		  AddTextFieldToIccContactL(KStorageTypeText, 
					    KUidContactFieldPhoneNumber, 
					    KUidContactFieldVCardMapTEL, 
					    number, 
					    item, 
					    i + 1);   // Here, 1 is used to distinguish the 
		                                      // default number and additional number. 
		                                      // The reason is that they share the
		                                      // same field type (see comments in 
		                                      // method AddTextFieldToIccContactL).

		
		//-- append additional field type to the current field if any.
		//-- this field type is obtained from additional number alpha tag 
		if(pCurrField)
			{
		 	AssignAlphaTagsToFieldTypeL(additionalNum.iNumberString, pCurrField);
			}
		}

	// Create name field
	if (aICCEntry.iName.Length() > 0)
		{
		AddTextFieldToIccContactL(KStorageTypeText, KUidContactFieldFamilyName, KUidContactFieldVCardMapUnusedN, aICCEntry.iName, item, 0);
		}

	// Create second name field
	if (aICCEntry.iSecondName.Length() > 0)
		{
		AddTextFieldToIccContactL(KStorageTypeText, KUidContactFieldSecondName, KUidContactFieldVCardMapSECONDNAME, aICCEntry.iSecondName, item, 0);
		}

	// Create group field(s)
	count = aICCEntry.iGroupList->Count();
	for(TInt i=0; i<count; ++i)
		{
		CSyncContactICCEntry::TSyncEntryName groupField;
		groupField.Copy(aICCEntry.iGroupList->At(i));
		AddTextFieldToIccContactL(KStorageTypeText, KUidContactFieldICCGroup, KUidContactFieldVCardMapUnusedN, groupField, item, i);
		}

	// Create e-mail field(s)
	count = aICCEntry.iEmailList->Count();
	for(TInt i=0; i<count; ++i)
		{
		CSyncContactICCEntry::TSyncEntryName emailField;
		emailField.Copy(aICCEntry.iEmailList->At(i));
		AddTextFieldToIccContactL(KStorageTypeText, KUidContactFieldEMail, KUidContactFieldVCardMapEMAILINTERNET, emailField, item, i);
		}

	// Create slot number field
	TBuf<KTemplateFieldLength> buf;
	buf.AppendNum(aICCEntry.iSlotNum);
	AddTextFieldToIccContactL(KStorageTypeText, KUidContactFieldICCSlot, KUidContactFieldVCardMapNotRequired, buf, item, 0);

	// Create phonebook type field
	buf.FillZ();
	buf.Zero();

	buf.AppendNum(aICCEntry.iPhonebookUid.iUid);
	AddTextFieldToIccContactL(KStorageTypeText, KUidContactFieldICCPhonebook, KUidContactFieldVCardMapNotRequired, buf, item, 0);

	//
	// Write the contact to the database...
	//
	LOGENGINE1(_L8("WriteICCContactToDBL(): Adding contact to database"));

	iDb->DatabaseBeginL(EFalse); // Start the transaction
	iIsInTransaction = ETrue; 

	aICCEntry.iContactId = iDb->doAddNewContactL(*item, EFalse, ETrue); // Add that new contact to Contact DB 
	CleanupStack::PopAndDestroy(item);

	// Add the contact to the correct group now because during
	// synchronisation the contacts model can't do it since the 
	// plug-in won't have a connection to the server (and therefore know 
	// which group to add it to).
	iDb->AddContactToGroupL(aICCEntry.iContactId, groupId, ETrue);
	CommitIccTransactionL();			

	return aICCEntry.iContactId;
	} // CSyncEngineServer::WriteICCContactToDBL


/**
 *  Checks if an ICC entry is the same as one in the database.
 *
 *  @param aICCEntry  CSyncContactICCEntry reference.
 *  @param aId        TContactItemId reference.
 *
 *  @return Boolean stating if entries match.
 */
TBool CSyncEngineServer::EntriesIdenticalL(CSyncContactICCEntry& aICCEntry,
										   TContactItemId& aId)
	{
	LOGENGINE3(_L8("EntriesIdenticalL(): 0x%08x %d"), aICCEntry.iPhonebookUid,
			   aId);
	
	//
	// Get the entry from the contacts database...
	//
	CContactItem* contactEntry=iDb->ReadContactLC(aId);
	CContactItemFieldSet& fieldSet = contactEntry->CardFields();

	// Note: We have already matched the slot number field.
	
	// Check if the same amount of information is provided.
	// This will reduce time for comparison when there is a lot of additional
	// information but a small mismatch in the number of additional fields.
	TInt index;
	
	// Obtain number count
	TInt iccNumCount(aICCEntry.iNumberList->Count() + 1); // first phone number is not in additional number list
	TInt contactNumCount(0);
	index = -1;
	while((index = fieldSet.FindNext(KUidContactFieldPhoneNumber, index+1)) != KErrNotFound)
		{
		contactNumCount++;
		}
	
	// Obtain group count
	TInt iccGrpCount(aICCEntry.iGroupList->Count());
	TInt contactGrpCount(0);
	index = -1;
	while((index = fieldSet.FindNext(KUidContactFieldICCGroup, index+1)) != KErrNotFound)
		{
		const CContactItemField& testField = fieldSet[index];
		TPtrC testFieldPtr(testField.TextStorage()->Text());
		if (testFieldPtr.Length() > 0) // entry for SDN & LND will have group field but it will be empty
			{
			contactGrpCount++;
			}
		}
		
	// Obtain email count
	TInt iccEmailCount(aICCEntry.iEmailList->Count());
	TInt contactEmailCount(0);
	index = -1;
	while((index = fieldSet.FindNext(KUidContactFieldEMail, index+1)) != KErrNotFound)
		{
		const CContactItemField& testField = fieldSet[index];
		TPtrC testFieldPtr(testField.TextStorage()->Text());
		if (testFieldPtr.Length() > 0)  // entry for SDN & LND will have email field but it will be empty
			{
			contactEmailCount++;
			}
		}
		
	// Test field counts and exit if these don't match
	if(	(contactNumCount != iccNumCount) ||
		(contactGrpCount != iccGrpCount) ||
		(contactEmailCount != iccEmailCount) ||
		(contactNumCount < 1))
		{
		CleanupStack::PopAndDestroy(contactEntry);
		return EFalse;
		}
		
	// Detailed test of first representation of name
	TInt loop(0);
	TBool contentMatch(ETrue);

	index = fieldSet.Find(KUidContactFieldFamilyName); // start with family name
	if (index != KErrNotFound) 
		{
		const CContactItemField& testField1 = fieldSet[index];
		TPtrC testFieldPtr1(testField1.TextStorage()->Text());
		if (testFieldPtr1.Compare(aICCEntry.iName) != 0)
			{
			contentMatch = EFalse;
			}
		}
	else if (aICCEntry.iName.Length() > 0)
		{
		contentMatch = EFalse;
		}

	// Detailed test of second representation of name
	if (contentMatch)
		{
		index = fieldSet.Find(KUidContactFieldSecondName); // get second name
		if (index != KErrNotFound)
			{
			const CContactItemField& testField1 = fieldSet[index];
			TPtrC testFieldPtr1(testField1.TextStorage()->Text());
			if (testFieldPtr1.Compare(aICCEntry.iSecondName) != 0)
				{
				contentMatch = EFalse;
				}
			}
		else if (aICCEntry.iSecondName.Length() > 0)
			{
			contentMatch = EFalse;
			}
		}

	if (contentMatch)
		{
		// Detailed test of phone number information
		// Note: the first phone number is stored separately from additional numbers
		//       and as a result this loop treats it differently.  The loop counter
		//       is adjusted to allow for this number not being in the additional number list
		index = fieldSet.Find(KUidContactFieldPhoneNumber);
		for (loop = 0;
			 (loop < iccNumCount) && (index != KErrNotFound) && contentMatch; 
			 loop++)
			{
			const CContactItemField& testField2 = fieldSet[index];
			TBuf<RMobilePhone::KMaxMobileTelNumberSize> testFieldPtr2 = testField2.TextStorage()->Text();
			TBool isInternationalCntNumber(EFalse);

			if (testFieldPtr2.Find(KEngineInternationalPrefix)!=KErrNotFound)
				{
				testFieldPtr2 = testFieldPtr2.Mid(1);
				isInternationalCntNumber = ETrue;
				}

			// retrieving the number from the ICC entry
			TBool isInternationalICCNumber;
			TBuf<RMobilePhone::KMaxMobileTelNumberSize> testFieldPtr5; 
			if (loop == 0) // first phone number is stored separately
				{
				testFieldPtr5 = aICCEntry.iNumber;
				isInternationalICCNumber = (aICCEntry.iTON==RMobilePhone::EInternationalNumber);
				}
			else 
				{
				testFieldPtr5 = aICCEntry.iNumberList->At(loop - 1).iNumber;
				isInternationalICCNumber = (aICCEntry.iNumberList->At(loop - 1).iTON==RMobilePhone::EInternationalNumber);
				}

			// Does the ICC number contain a prefix? (apparently, some TSYs are designed to keep it)
			if (testFieldPtr5.Find(KEngineInternationalPrefix)!=KErrNotFound)
				{
				// If it does, strip it
				testFieldPtr5 = testFieldPtr5.Mid(1);
				// Also implies that it is international too, regardless of the aICCEntry.iTON flag value
				isInternationalICCNumber = ETrue;
				}
			
			// comparing the numbers
			if (  	(isInternationalCntNumber!=isInternationalICCNumber) ||
					(testFieldPtr2.Compare(testFieldPtr5) != 0) ) 
				{
				contentMatch = EFalse;
				}
			index = fieldSet.FindNext(KUidContactFieldPhoneNumber, index+1);  // get next additional number
			}

		if (contentMatch)
			{
			// Detailed test of group information
			index = fieldSet.Find(KUidContactFieldICCGroup);
			for (	loop = 0;
					(loop < iccGrpCount) && (index != KErrNotFound) && contentMatch; 
					loop++)
				{
				const CContactItemField& testField3 = fieldSet[index];
				TPtrC testFieldPtr3(testField3.TextStorage()->Text());

				// Ignore blank group entries...
				if (testFieldPtr3.Length() == 0)
					{
					index = fieldSet.FindNext(KUidContactFieldICCGroup, index+1);
					loop--;
					continue;
					}

				if (testFieldPtr3.Compare(aICCEntry.iGroupList->At(loop)) != 0)
					{
					contentMatch = EFalse;
					}
				index = fieldSet.FindNext(KUidContactFieldICCGroup, index+1);
				}
			
			if (contentMatch)
				{
				// Detailed test of Email information
				index = fieldSet.Find(KUidContactFieldEMail);
				for (loop = 0;
					 (loop < iccEmailCount) && (index != KErrNotFound) && contentMatch; 
					 loop++)
					{
					const CContactItemField& testField4 = fieldSet[index];
					TPtrC testFieldPtr4(testField4.TextStorage()->Text());

					// Ignore blank email entries...
					if (testFieldPtr4.Length() == 0)
						{
						index = fieldSet.FindNext(KUidContactFieldEMail, index+1);
						loop--;
						continue;
						}

					if (testFieldPtr4.Compare(aICCEntry.iEmailList->At(loop)) != 0)
						{
						contentMatch = EFalse;
						}
					index = fieldSet.FindNext(KUidContactFieldEMail, index+1);
					}
				}
			}
		}

	// Cleanup and return result		
	CleanupStack::PopAndDestroy(contactEntry);

	return contentMatch;
	} // CSyncEngineServer::EntriesIdenticalL


/**
 *  Commit the current transaction. 
 */
void CSyncEngineServer::CommitIccTransactionL()
	{
	if (iIsInTransaction)
		{
		LOGENGINE1(_L8("Commit Icc Transaction"));
		iDb->DatabaseCommitL(EFalse);
		iIsInTransaction =  EFalse;
		}
	} // CSyncEngineServer::CommitIccTransactionL


/**
 *  Rollback the current transaction. 
 */
void CSyncEngineServer::RollbackIccTransaction()
	{
	if (iIsInTransaction)
		{
		LOGENGINE1(_L8("Rollback Icc Transaction"));

		//
		// Transaction must be rolled back if a failure occurs otherwise
		// Contacts Database will become corrupt...
		//
		iDb->DatabaseRollback();

		//
		// Check whether database is damaged and needs to be recovered. This
		// should never happen...
		//
		if (iDb->IsDamaged())
			{
			TRAPD(recoverErr, iDb->RecoverL());
			if (recoverErr == KErrDiskFull)
				{
				//
				// The disk is full, compact the database and check for
				// damage again. Not much else we can do!
				//
				TRAPD(compactErr, iDb->CompactL());

				if (compactErr != KErrNone  &&  iDb->IsDamaged())
					{
					TRAPD(recoverErr2, iDb->RecoverL());
					if (recoverErr2 == KErrDiskFull)
						{
						TRAP_IGNORE(iDb->CompactL());
						}
					}
				}
			}
		
		//
		// We're no longer in a transaction...
		//
		iIsInTransaction = EFalse;
		}
	} // CSyncEngineServer::RollbackIccTransaction


/**
 *  Add a new text field (aField) to the CContactICCEntry supplied by aIccEntry. 
 *
 * @param aType       Field Storage type 
 * @param aFieldType  Field type
 * @param aMapping    Mapping for the field's content type
 * @param aField      Field data
 * @param aIccEntry   CContactICCEntry item
 * @param aCount      Identifies which instance of field is to be used. Actually
 *					  it is an index of an object in one of vectors from
 *					  CSyncContactICCEntry class.
 *
 * @return            Pointer to the current (modified or added) field in
 *                    aIccEntry fieldset so that it is possible to get access
 *                    to this field later. Can be NULL.
 */ 
CContactItemField* CSyncEngineServer::AddTextFieldToIccContactL(TStorageType aType,
																TFieldType aFieldType,
																TUid aMapping,
																TDesC& aField,
																CContactICCEntry* aIccEntry,
																TInt aCount)
	{
	CContactItemFieldSet& fieldSet = aIccEntry->CardFields();
	TInt pos(KErrNotFound);
	for (TInt i = 0; i <= aCount; i++)
		{
		// "pos+1" below provides a new start position for the FindNext.
		pos = fieldSet.FindNext(aFieldType, pos + 1); 
		if (pos == KErrNotFound)
			break;
		}

    CContactItemField   *pCurrField = NULL;  // pointer to the current field in fieldset

    if (pos!=KErrNotFound) // Field already present. Note, Contacts model reads all fields
			   // in template and adds them as empty fields in ICC item
		{
		// If the "aCount" identified instance field already exists in the "fieldSet",
		// then update it.

		CContactItemField& field=fieldSet[pos];
		field.TextStorage()->SetTextL(aField);
		
        pCurrField = &field;
        }
	else
		{
		// If there is no field in the "fieldSet" whose type is the given "aFieldType",
		// then create a new field and add it into the "fieldSet". 
		// Or
		// If the "aCount" identified instance field does not exist in the "fieldSet",
		// then create a new field and add it into the "fieldSet".

		CContactItemField* field=CContactItemField::NewLC(aType, aFieldType);
		field->SetMapping(aMapping);
		field->AddFieldTypeL(aFieldType); // Appends a field type to the field's content type
		field->TextStorage()->SetTextL(aField);
		aIccEntry->AddFieldL(*field);
		CleanupStack::Pop(field); // No need to destroy it since contact item takes ownership of field
		
        pCurrField = field;
        }

	return pCurrField;
	} // CSyncEngineServer::AddTextFieldToIccContactL


/**
 *  Create a new client session.
 */
CSession2* CSyncEngineServer::NewSessionL(const TVersion& /*aVersion*/,
										  const RMessage2& /*aMessage*/) const
	{
	LOGENGINE1(_L8("CSyncEngineServer::NewSessionL"));

	//
	// Only one session connection is allowed!!!
	//
	if (iConnectedSession != NULL)
		{
		User::Leave(KErrPermissionDenied);
		}
		
	return new(ELeave) CSyncEngineSession();
	} // CSyncEngineServer::NewSessionL


/**
 *  Called when a new session is being created.
 *
 *  @param aSession  Server side session.
 */
void CSyncEngineServer::AddSessionL(CSyncEngineSession* aSession)
	{
	LOGENGINE1(_L8("CSyncEngineServer::AddSession"));
	
	//
	// Store the session pointer...
	//
	iConnectedSession = aSession;

	//
	// Queue an Active Object to configure the engine straight after this
	// session is created.
	//
	if (iPhoneBookSyncEngineStarter == NULL)
		{
		iPhoneBookSyncEngineStarter = new (ELeave) CPhoneBookSyncEngineStarter(*this);
		iPhoneBookSyncEngineStarter->Call();
		}
	} // CSyncEngineServer::AddSessionL


/**
 *  Called when a session is being destroyed.
 */
void CSyncEngineServer::DropSession(CSyncEngineSession* /*aSession*/)
	{
	LOGENGINE1(_L8("CSyncEngineServer::DropSession"));

	iConnectedSession = NULL;
	TRAP_IGNORE(UnconfigureEngineL());

	CActiveScheduler::Stop();
	} // CSyncEngineServer::DropSession


/**
 *  Standard Active Object RunError() method, called when the RunL() method
 *  leaves, which will be when the CPhoneBookSession::ServiceL() leaves.
 *
 *  Find the current message and complete it before restarting the server.
 *
 *  @param aError  Leave code from CPhoneBookSession::ServiceL().
 *
 *  @return KErrNone
 */
TInt CSyncEngineServer::RunError(TInt aError)
	{
	LOGENGINE2(_L8("CSyncEngineServer::RunError %d"),aError);

	//
	// Complete the request with the available error code.
	//
	if (Message().IsNull() == EFalse)
		{
		Message().Complete(aError);
		}

	//
	// The leave will result in an early return from CServer::RunL(), skipping
	// the call to request another message. So do that now in order to keep the
	// server running.
	//
	ReStart();
	
	return KErrNone;
	} // CSyncEngineServer::RunError


/**
 *  This method begins the completion of a synchronisation operation. It is
 *  called when the CSyncContactsWithICC Active Object has finished its task.
 *
 *  @param aRetVal  Complete-request error code.
 */
void CSyncEngineServer::CompleteDoSync(TInt aRetVal)
	{
	TUid  phonebookUid = iSyncContactsWithICC->PhonebookUid();
	
	LOGENGINE3(_L8("CompleteDoSync(%d): 0x%08x"), aRetVal, phonebookUid);

	//
	// Store the result of the synchronisation operation...
	//
	if (aRetVal == KErrNone)
		{
		iPhonebookManager.SetLastSyncError(phonebookUid, KErrNone);
   		iPhonebookManager.SetSyncState(phonebookUid,
   									   RPhoneBookSession::ECacheValid);
		}
	else if (aRetVal == KErrCancel)
		{
		iPhonebookManager.SetLastSyncError(phonebookUid, KErrCancel);
   		iPhonebookManager.SetSyncState(iSyncContactsWithICC->PhonebookUid(),
   									   RPhoneBookSession::EUnsynchronised);
		}
   	else
		{
		iPhonebookManager.SetLastSyncError(phonebookUid, aRetVal);
   		iPhonebookManager.SetSyncState(phonebookUid,
   									   RPhoneBookSession::EErrorDuringSync);
		}

	//
	// For debug purposes print out the new Look Up Table for this phonebook...
	//
#ifdef _DEBUG
	iPhonebookManager.LogLookUpTable(iSyncContactsWithICC->PhonebookUid());
#endif	

	//
	// Complete this request and release the Active Object's memory...
	//
	iConnectedSession->CompleteRequest(iSyncContactsWithICC->ClientMessage(),
									   aRetVal);

	delete iSyncContactsWithICC;
	iSyncContactsWithICC = NULL;
	} // CSyncEngineServer::CompleteDoSync


/**
 *  Create and install the active scheduler.
 */
CSyncEngineScheduler* CSyncEngineScheduler::New()
	{
	LOGENGINE1(_L8("CSyncEngineScheduler::New()"));

	CSyncEngineScheduler*  scheduler = new CSyncEngineScheduler;

	if (scheduler != NULL)
		{
		CSyncEngineScheduler::Install(scheduler);
		}

	return scheduler;
	} // CSyncEngineScheduler::New


/**
 *  Called if any RunL() method leaves.
 */
void CSyncEngineScheduler::Error(TInt aError) const
	{
#ifdef _DEBUG
	LOGENGINE2(_L8("CSyncEngineScheduler::Error(%d)"), aError);
#else
	(void) aError;
#endif

	PhBkSyncPanic(EPhBkSyncPanicUnexpectedLeave);
	} // CSyncEngineScheduler::Error


/**
 *  Standard constructor.
 *
 *  @param aEngine  Reference to the engine.
 */
CPhoneBookSyncEngineStarter::CPhoneBookSyncEngineStarter(CSyncEngineServer& aEngine)
  : CAsyncOneShot(EPriorityHigh),
    iEngine(aEngine)
	{
	// NOP
	} // CPhoneBookSyncEngineStarter::CPhoneBookSyncEngineStarter


/**
 *  RunL() for the starter object. This configures the engine fully or
 *  shuts it down.
 */
void CPhoneBookSyncEngineStarter::RunL()
	{
	LOGENGINE2(_L8("CPhoneBookSyncEngineStarter::RunL(): iStatus=%d."), iStatus.Int());

	//
	// Configure the engine...
	//
	TRAPD(configErr, iEngine.ConfigureEngineL());
	if (configErr != KErrNone)
		{
		LOGENGINE2(_L8("ConfigureEngineL() failed with error %d."), configErr);

		//
		// Shutdown the engine in this case, so it can be restarted later.
		//
		TRAP_IGNORE(iEngine.UnconfigureEngineL());
		CActiveScheduler::Stop();
		}
	} // CPhoneBookSyncEngineStarter::RunL

