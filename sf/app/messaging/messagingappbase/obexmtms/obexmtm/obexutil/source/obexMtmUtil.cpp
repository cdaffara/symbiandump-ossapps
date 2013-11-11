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
// $Workfile: obexMtmUtil.cpp $
// $Author: Stevep $
// $Revision: 7 $
// $Date: 8/02/02 10:11 $
// 
//

//class include
#include "obexMtmUtil.h"
#include "obexheaderlist.h"

#ifdef _DEBUG
#define DEBPRINT(Z)  User::InfoPrint(Z);
#else
#define DEBPRINT(Z)
#endif

// Uncomment this to test data compatibility with builds older than PHAR-5SDJG9.
// (if uncommented, messages are externalised the same way it was done before the PHAR-5SDJG9 changes)
//#define TEST_INC042468 

static const TInt KObexArrayGranularity = 4;

//
//
//  Class for externalising a set of filenames to the same message entry store. Rolls back if not explicitly committed.
//

// Static functions that do the whole process in one go:
EXPORT_C  void CObexMtmFileNameExternaliser::ExternaliseAndCommit1FileNameL(CMsvEntry* aEntry, const TDesC16* aFileName)
/**
* Static function to write one and only one filename to the message store, then finalise and commit it. This function
* should be called ONCE ONLY. To externalise more than one filename, instantiate this class and use ExternaliseFileNameL()
* followed by CommitStoreL().
* @param aEntry the entry whose store is to be written to
* @param aFileName The filename to write to the store
* @leave KErrXXX system-wide error codes
*/	
	{
	CMsvStore* messageStore = aEntry->EditStoreL();
	CleanupStack::PushL(messageStore);

	RMsvWriteStream messageWriteStream;
	CleanupClosePushL(messageWriteStream);

	messageWriteStream.AssignL(*messageStore, KObexFileNameStreamUid);

	// write the identifier so we know we are internalizing a stream later
#ifndef TEST_INC042468
	messageWriteStream.WriteInt32L(KObexFileNameStreamFileNameUid);
#endif
	messageWriteStream.WriteInt32L(aFileName->Length());  
	messageWriteStream.WriteL(*aFileName);

#ifndef TEST_INC042468
	messageWriteStream.WriteInt32L(KObexFileNameStreamFileNameUid);
#endif
	messageWriteStream.WriteInt32L(0);		//Mark the end of the filename list

	messageWriteStream.CommitL();

	CleanupStack::PopAndDestroy(); // trigger a call to Close(); on messageWriteStream as well as removing from the stack

	messageStore->CommitL();
	CleanupStack::PopAndDestroy(messageStore);
	}


EXPORT_C  void CObexMtmFileNameExternaliser::Internalise1FileNameL(CMsvEntry* aEntry,  TDes16* aFileName)
/**
* Static function to read in one filename from the store.
*
* @param aEntry entry to read the filename from
* @param aFileName Descriptor to receive the new filename. Any existing data will be overwritten. MUST be long enough to contain the name.
* @leave KErrXXX system wide error codes.
*/	
	{
	CMsvStore* messageStore = aEntry->ReadStoreL();
	CleanupStack::PushL(messageStore);

	RMsvReadStream messageReadStream;
	messageReadStream.OpenLC(*messageStore, KObexFileNameStreamUid);  // If data hasn't been stored then this will provide the error.

	// verify that we're reading the right type of data from the stream


	// INC042468: "AV21: Data Compatibility break caused by "PHAR-5SDJG9""
	// To regain compatibility, if the first integer read from the stream
	// is not the expected "KObexFileNameStreamFileNameUid", it is assumed it 
	// is the filename length. An extra check is made that it is a valid filename
	// length (<= KMaxFileName).
	// "KObexFileNameStreamFileNameUid" has also been modified so that there cannot
	// be a file with a name length equal to KObexFileNameStreamFileNameUid (in which
	// case we would have been in trouble ;-) )

	TInt32 firstInt = messageReadStream.ReadInt32L();
	TInt32 fileNameLength = 0;
	if (firstInt == KObexFileNameStreamFileNameUid)
		{
		// next will be the filename length
		fileNameLength = messageReadStream.ReadInt32L();
		}
	else if (firstInt <= KMaxFileName)
		{
		// let's assume the first integer in the stream was the filename length
		// (this happens if the message was saved by an old build (previous to PHAR-5SDJG9)
		fileNameLength = firstInt;
		}
	else
		{
		// neither the expected "KObexFileNameStreamFileNameUid" nor a valid filename length
		User::Leave(KErrArgument);	
		}

	messageReadStream.ReadL(*aFileName, fileNameLength);

	CleanupStack::PopAndDestroy(2, messageStore);  // messageStore, messageReadStream (calls Close() first)
	}

//
//
// Class for storing multiple Filenames within a CMsvEntry store.
//
EXPORT_C  CObexMtmFileNameExternaliser* CObexMtmFileNameExternaliser::NewL(CMsvSession* aSession, TMsvId aId)
/**
* Canonical NewL factory function.
*
* @param aSession A messaging server session
* @param aId TMsvId of the entry to externalise/internalise to/from. This association is fixed for the lifetime of this instance of the class.
* @return a newly constucted CObexMtmFileNameExternaliser
* @leave KErrXXX system wide error codes
*/
	{
	CObexMtmFileNameExternaliser* self = new(ELeave) CObexMtmFileNameExternaliser(aSession);
	CleanupStack::PushL(self);
	self->InitialiseStoreL(aId);
	CleanupStack::Pop();  // self
	return self;
	}
CObexMtmFileNameExternaliser::CObexMtmFileNameExternaliser(CMsvSession* aSession)   :   iSession(aSession)
/**
* Constructor
*
*  @param aSession A messaging server session
*/
	{}
CObexMtmFileNameExternaliser::~CObexMtmFileNameExternaliser()
/**
* Destructor. Will revert the stream to its original state if the store hasn't been comitted.
*/
	{
	if(! iHaveCommitted)
		{
		iMessageWriteStream.Close();  
		if(iMessageStore)
		    iMessageStore->Revert();  // Don't commit. Should have been called explicitly using CommitStoreL().
		}
	delete iMessageStore;
	delete iEntry;
	}

void CObexMtmFileNameExternaliser::InitialiseStoreL(TMsvId aId)
/**
 * Initialises the store of the given entry, ready for reading or writing.
 *
 * @param aId The TMsvId of the entry whose store should be initialised.
 * @leave KErrXXX system wide error codes.
 */
	{
	iEntry = iSession->GetEntryL(aId);

	iMessageStore = iEntry->EditStoreL();

	iMessageWriteStream.AssignL(*iMessageStore, KObexFileNameStreamUid);
	iHaveCommitted = EFalse;
	}

EXPORT_C  void CObexMtmFileNameExternaliser::ExternaliseFileNameL(const TDesC16* aFileName)
/**
 * Externalises a single filename to the associated entry's message store.
 *
 * @param aFileName the filename to externalise.
 * @leave KErrXXX system-wide error codes.
 */
	{
#ifndef TEST_INC042468
	iMessageWriteStream.WriteInt32L(KObexFileNameStreamFileNameUid);
#endif
	iMessageWriteStream.WriteInt32L(aFileName->Length());
	iMessageWriteStream.WriteL(*aFileName);
	}

EXPORT_C  void CObexMtmFileNameExternaliser::ExternaliseFileNameAndHeadersL(const TDesC16* aFileName, const CObexHeaderList* aHeaderList)
/**
 * Externalises a single filename to the associated entry's message store.
 *
 * @param aFileName the filename to externalise.
 * @leave KErrXXX system-wide error codes.
 */
	{
	ExternaliseFileNameL(aFileName);
#ifndef TEST_INC042468
	iMessageWriteStream.WriteInt32L(KObexFileNameStreamHeaderListUid);
	aHeaderList->ExternalizeL(iMessageWriteStream);
#endif
	}

EXPORT_C  void CObexMtmFileNameExternaliser::CommitStoreL()
/**
 * Finalises and commits the store. This function MUST be called once all of the filenames have been externalised, and not
 * before. If it is not called, any changes will be lost and the store will revert to its former state.
 *
 * @leave KErrXXX system wide error codes.
 */
	{
#ifndef TEST_INC042468
	iMessageWriteStream.WriteInt32L(KObexFileNameStreamFileNameUid);
#endif
	iMessageWriteStream.WriteInt32L(0);  // length == 0  --> no more names in store.
	iMessageWriteStream.CommitL();
	iHaveCommitted = ETrue;
	iMessageWriteStream.Close();

	iMessageStore->CommitL();

	delete iMessageStore;
	iMessageStore=NULL;
	}

EXPORT_C  RObexMtmFileNameWithHeadersArray* CObexMtmFileNameExternaliser::InternaliseFileNamesLC(CMsvStore& aMessageStore)
/**
 * Static function to get the filenames stored within the message entry. The returned array is pushed onto the cleanup
 * stack.
 *
 * @param aMessageStore Store from which the names are to be read
 * @return Array of filenames, pushed onto the cleanup stack.
 * @leave System wide error codes
 */
	{
	RObexMtmFileNameWithHeadersArray* fileNameArray = new(ELeave) RObexMtmFileNameWithHeadersArray(KObexArrayGranularity);
	CleanupStack::PushL(fileNameArray);

	RMsvReadStream  newMessageReadStream;
	newMessageReadStream.OpenL(aMessageStore, KObexFileNameStreamUid);  // If data hasn't been stored then this will provide the error.
	CleanupClosePushL(newMessageReadStream);




	// INC042468: "AV21: Data Compatibility break caused by "PHAR-5SDJG9""
	// To regain compatibility, if the first integer read from the stream
	// is not the expected "KObexFileNameStreamFileNameUid", it is assumed it 
	// is the filename length. An extra check is made that it is a valid filename
	// length (<= KMaxFileName).
	// "KObexFileNameStreamFileNameUid" has also been modified so that there cannot
	// be a file with a name length equal to KObexFileNameStreamFileNameUid (in which
	// case we would have been in trouble ;-) )

	TInt32 firstInt = newMessageReadStream.ReadInt32L();
	
	TInt32 fileNameLength = 0;
	if (firstInt == KObexFileNameStreamFileNameUid)
		{
		// next will be the filename length
		fileNameLength = newMessageReadStream.ReadInt32L();
		}
	else if (firstInt <= KMaxFileName)
		{
		// let's assume the first integer in the stream was the filename length
		// (this happens if the message was saved by an old build (previous to PHAR-5SDJG9)
		fileNameLength = firstInt;
		}
	else
		{
		// neither the expected "KObexFileNameStreamFileNameUid" nor a valid filename length
		User::Leave(KErrArgument);	
		}

	TInt32 elementTypeID;
	TBool lengthAlreadyReadFlag;
	TFileName thisName;
	CObexMtmFileNameWithHeaders *thisPackage;
	while(fileNameLength)
		{

		thisPackage = CObexMtmFileNameWithHeaders::NewL();
		CleanupStack::PushL(thisPackage);
				
		// read 'bodylength' bytes into a descriptor			
		newMessageReadStream.ReadL(thisName, fileNameLength);
		
		// store this descriptor in the container
		thisPackage->iFileName = new(ELeave) TFileName(thisName);
		CleanupStack::PushL(thisPackage->iFileName);

		lengthAlreadyReadFlag = EFalse;	
		// get the type of the next element
		elementTypeID = newMessageReadStream.ReadInt32L();

		switch(elementTypeID)
			{
		case KObexFileNameStreamHeaderListUid:
			// this filename has some headers associated with it
			
			// read these headers in...
			thisPackage->iHeaderList = CObexHeaderList::NewLC();
			thisPackage->iHeaderList->InternalizeL(newMessageReadStream);

			// get the type of the next element
			elementTypeID = newMessageReadStream.ReadInt32L();
						
			// fall through to add this container to the array
		case KObexFileNameStreamFileNameUid:
			// this filename has no headers associated with it, this is OK.
			fileNameArray->Append(thisPackage);
			break;

		default:
			// builds previous to PHAR-5SDJG9: 
			// no stream Uids are externalised and elementTypeID contains already the filename length
			if (elementTypeID <= KMaxFileName)
				{
				fileNameArray->Append(thisPackage);
				fileNameLength = elementTypeID;
				lengthAlreadyReadFlag = ETrue;
				}
			else
				{
				User::Leave(KErrArgument);
				}
			// if this value is not one of the two element types, then we can't read the stream
			
			break;
			}

		// this member will be zero if there were no headers on this filename
		if(thisPackage->iHeaderList)
			{
			CleanupStack::Pop(thisPackage->iHeaderList);
			}

		// safely added filename and headers to list, pop the allocations off the cleanup stack.
		CleanupStack::Pop(thisPackage->iFileName);
		
		CleanupStack::Pop(thisPackage);

		// get the length of the next element
		if (!lengthAlreadyReadFlag)
			{
			TRAPD (err, fileNameLength = newMessageReadStream.ReadInt32L();)  // Length == 0 --> end of array
			}
		}

	newMessageReadStream.Close();
	CleanupStack::PopAndDestroy();  //newMessageReadStream

	return fileNameArray;
	}

//
// CObexMtmFileNameWithHeaders
//

EXPORT_C CObexMtmFileNameWithHeaders* CObexMtmFileNameWithHeaders::NewL()
	/**
	 * Canonical NewL factory function.
	 *
	 * @return an empty CObexMtmFileNameWithHeaders class
	 * @leave KErrXXX system wide error codes
	 */
	{
	CObexMtmFileNameWithHeaders *self = new(ELeave)CObexMtmFileNameWithHeaders;
	return self;
	}

CObexMtmFileNameWithHeaders::~CObexMtmFileNameWithHeaders()
/**
 * Destructor. Will destroy the data pointed to by the 2 member pointers.
 */
	{
	delete iFileName;
	delete iHeaderList;
	}

CObexMtmFileNameWithHeaders::CObexMtmFileNameWithHeaders()
	/**
	 * Constructor.
	 */
	{
	}


