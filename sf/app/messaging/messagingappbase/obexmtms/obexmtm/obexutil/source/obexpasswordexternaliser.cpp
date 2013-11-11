// Copyright (c) 2001-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// $Workfile: obexpasswordexternaliser.cpp $
// $Author: Stevep $
// $Revision: 13 $
// $Date: 26/03/02 11:34 $
// 
//

//class include
#include "obexpasswordexternaliser.h"
//system includes
#include <msvstore.h>   //CMsvStore, RMsvWriteStream, RMsvReadStream
#include <msvids.h>
#include <msvuids.h>

//user includes
#include "obexinternalutils.h"

const TUid KObexPasswordStreamUid = {0x1000AAC6};


void ObexPasswordExternaliser::SetPasswordFromServiceEntryL(CMsvEntry& aServiceEntry, const TDesC& aPassword)
/**
* Static setter function to set the ObexPassword associated with the given entry. If the length of the password is
* zero, then the password is cleared. Calling this function causes the entry to be changed, too, which allows the watcher
* to detect the change to the password.
*
* @param aServiceEntry The service entry to set the password into
* @param aPassword the password to set
* @leave KErrXXX system-wide error codes
*/
	{
 	CMsvStore* serviceEntryStore = aServiceEntry.EditStoreL();
	CleanupStack::PushL(serviceEntryStore);

	if (!aPassword.Length())
		{
		//No password, so remove the password stream
		serviceEntryStore->Remove(KObexPasswordStreamUid);
		}
	else
		{
		//Get the stream
		RMsvWriteStream passwordWriteStream;
		passwordWriteStream.AssignL(*serviceEntryStore, KObexPasswordStreamUid);

		//Externalise the password 
		HBufC* passwordBuf = aPassword.AllocLC();
		ObexInternalUtils::ExternalizeL(passwordBuf, passwordWriteStream);
		CleanupStack::PopAndDestroy();		//passwordBuf
		passwordBuf = 0;

		//Commit the stream
		passwordWriteStream.CommitL();
		passwordWriteStream.Close();
		}

	//"Change" the entry, so that the watcher will pick up the change if it's installed
	aServiceEntry.ChangeL(aServiceEntry.Entry());

	//Commit the store
	serviceEntryStore->CommitL();

	//Cleanup
	CleanupStack::PopAndDestroy();		//serviceEntryStore
	}


EXPORT_C void ObexPasswordExternaliser::SetPasswordL(CMsvEntry& aEntry, const TDesC& aPassword)
/**
* Static setter function to set the ObexPassword associated with the given entry. If the length of the password is
* zero, then the password is cleared
*
* @param aEntry CMsvEntry whose owning service entry's password is to be set
* @param aPassword the password to set
* @leave KErrXXX system-wide error codes
*/
	{
	TMsvId originalEntry = aEntry.EntryId();
	TMsvId serviceEntry = aEntry.Entry().iServiceId;

	//Switch to the service entry
	aEntry.SetEntryL(serviceEntry);

	SetPasswordFromServiceEntryL(aEntry, aPassword);

	//Switch back
	aEntry.SetEntryL(originalEntry);
	}


EXPORT_C void ObexPasswordExternaliser::SetPasswordL(CMsvSession& aSession, const TUid& aMtmUid, const TDesC& aPassword)
/**
* Static setter function to set the ObexPassword associated with the given entry. If the length of the password is
* zero, then the password is cleared
*
* @param aEntry aSessionObserver A session observer
* @param aMtmUid Uid of the MTM whose password is to be set
* @param aPassword the password to set
* @leave KErrXXX system-wide error codes
*/
	{
	CMsvEntry* serviceEntry = GetServiceEntryLC(aSession, aMtmUid);

	SetPasswordFromServiceEntryL(*serviceEntry, aPassword);

	CleanupStack::PopAndDestroy();		//serviceEntry
	}


HBufC* ObexPasswordExternaliser::GetPasswordFromServiceEntryLC(CMsvEntry& aServiceEntry)
/**
* Static getter function to get the ObexPassword from the given service entry. If the password stream doesn't exist,
* a zero-length HBufC is returned. Ownership of the returned descriptor is passed to the caller, and is pushed onto the
* cleanup stack.
*
* @param aServiceEntry CMsvEntry set to the service entry containing the password
* @return HBufC containing the password (zero length if not set of cannot be read). Ownership is passed to the caller, and left on the cleanup stack
* @leave KErrXXX system-wide error codes
*/
	{
	HBufC* returnDescriptor = 0;

	CMsvStore* serviceEntryStore = 0;
	TRAPD(editStoreErr, serviceEntryStore = aServiceEntry.ReadStoreL());
	if (editStoreErr != KErrNone)
		{
		//Couldn't read the store--probably doesn't exist
		returnDescriptor = HBufC::NewLC(0);
		return returnDescriptor;
		}
	CleanupStack::PushL(serviceEntryStore);

	//Get the stream
	RMsvReadStream passwordReadStream;
	TRAPD(assignErr, passwordReadStream.OpenL(*serviceEntryStore, KObexPasswordStreamUid));
	if (assignErr == KErrNone)
		{
		//Stream opened OK
		passwordReadStream.PushL();
		ObexInternalUtils::InternalizeL(returnDescriptor, passwordReadStream);
		CleanupStack::PopAndDestroy(2);		//passwordReadStream, serviceEntryStore
		}
	else
		{
		//Couldn't read from stream--probably doesn't exist
		CleanupStack::PopAndDestroy();		//serviceEntryStore
		returnDescriptor = HBufC::NewL(0);
		}

	CleanupStack::PushL(returnDescriptor);
	return returnDescriptor;
	}


EXPORT_C HBufC* ObexPasswordExternaliser::GetPasswordLC(CMsvEntry& aEntry)
/**
 * Static getter function to get the ObexPassword associated with the given entry. If the password stream doesn't exist,
 * a zero-length HBufC is returned. Ownership of the returned descriptor is passed to the caller, and is pushed onto the
 * cleanup stack.
 *
 * @param aEntry CMsvEntry whose owning service entry's password is to be set
 * @return HBufC containing the password (zero length if not set). Ownership is passed to the caller, and left on the cleanup stack
 * @leave KErrXXX system-wide error codes
 */
	{
	TMsvId originalEntry = aEntry.EntryId();
	TMsvId serviceEntry = aEntry.Entry().iServiceId;

	//Switch to the service entry, and get the store
	aEntry.SetEntryL(serviceEntry);

	HBufC* returnDescriptor = GetPasswordFromServiceEntryLC(aEntry);

	//Switch back
	aEntry.SetEntryL(originalEntry);

	return returnDescriptor;
	}


EXPORT_C HBufC* ObexPasswordExternaliser::GetPasswordLC(CMsvSession& aSession, const TUid& aMtmUid)
/**
 * Static getter function to get the ObexPassword associated with the given MTM UID. The zeroth service entry is assumed.
 * If the password stream doesn't exist,a zero-length HBufC is returned. Ownership of the returned descriptor is passed
 * to the caller, and is pushed onto the cleanup stack.
 *
 * @param aSessionObserver a session observer
 * @param aMtmUid UID of the MTM whose password is to be set
 * @return HBufC containing the password (zero length if not set). Ownership is passed to the caller, and left on the cleanup stack
 * @leave KErrXXX system-wide error codes
 */
	{
	CMsvEntry* serviceEntry = GetServiceEntryLC(aSession, aMtmUid);

	HBufC* returnDescriptor = GetPasswordFromServiceEntryLC(*serviceEntry);

	CleanupStack::Pop();			//returnDescriptor
	CleanupStack::PopAndDestroy();	//serviceEntry
	CleanupStack::PushL(returnDescriptor);

	return returnDescriptor;
	}


EXPORT_C void ObexPasswordExternaliser::ResetPasswordL(CMsvEntry& aEntry)
/**
* Resets the password associated with the given entry. Does nothing if there is no password.
*
* @param aEntry CMsvEntry whose owning service entry's password is to be reset
* @leave KErrXXX system-wide error codes
*/	
	{
	ObexPasswordExternaliser::SetPasswordL(aEntry, KNullDesC);
	}


EXPORT_C void ObexPasswordExternaliser::ResetPasswordL(CMsvSession& aSession, const TUid& aMtmUid)
/**
* Resets the password associated with the given entry. Does nothing if there is no password.
*
* @param aEntry aSessionObserver A session observer
* @param aMtmUid Uid of the MTM whose password is to be set
* @leave KErrXXX system-wide error codes
*/	
	{
	ObexPasswordExternaliser::SetPasswordL(aSession, aMtmUid, KNullDesC);
	}


CMsvEntry* ObexPasswordExternaliser::GetServiceEntryLC(CMsvSession& aSession, const TUid& aMtmUid)
/**
* Returns a CMsvEntry pointing to the zeroth service entry associated with aMtmUid. Ownership of the returned CMsvEntry
* is passed to the caller, and it is left on the cleanup stack.
*
* @param aSession a session
* @param aSessionObserver A session observer
* @param aMtmUid Uid of the MTM whose service entry is to be found
* @return A newly created CMsvEntry
*/
	{
	//Create the entry
	TMsvSelectionOrdering order;
	order.SetShowInvisibleEntries(ETrue);
	CMsvEntry* returnEntry = CMsvEntry::NewL(aSession, KMsvRootIndexEntryId, order);
	CleanupStack::PushL(returnEntry);

	//Find the service entry
	CMsvEntrySelection* serviceEntrySelection = returnEntry->ChildrenWithTypeL(KUidMsvServiceEntry);
	CleanupStack::PushL(serviceEntrySelection);

	TInt current;
	for	(current = 0; current < serviceEntrySelection->Count(); current++)
		{
		returnEntry->SetEntryL(serviceEntrySelection->At(current));
		if (returnEntry->Entry().iMtm == aMtmUid) break;
		}

	if (current == serviceEntrySelection->Count())
		{
		//No entry found--leave
		User::Leave(KErrNotFound);
		}
	CleanupStack::PopAndDestroy(serviceEntrySelection);	

	return returnEntry;
	}
