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
// $Workfile: obexMtmUtil.h $
// $Author: Stevep $
// $Revision: 10 $
// $Date: 25/03/02 8:49 $
// 
//


#ifndef __OBEXMTMUTIL_H__
#define __OBEXMTMUTIL_H__


#include <e32std.h>

#include <BAUTILS.H>	// NearestLanguageFile
#include <basched.h>
#include <coeutils.h>	// ConeUtils::FileExists
#include <apparc.h>		// CApaApplication::GenerateFileName
#include <apgcli.h>		// RApaLsSession   <-- data recognition.

#include <txtrich.h>	// Richtext
#include <utf.h>		// CharConv
#include <baclipb.h>	// Clipboard

// Obex
#include <obex.h>

// MS
#include <msvAPI.h>
#include <msvIDs.h>
#include <msvUIDs.h>
#include <msvSTD.hrh>

extern const TInt KMaxObexPasswordLength;
const TUid KObexFileNameStreamUid = {0x1000AAC5};

// temporary IDs to indicate either a filename or a headerlist entry in the stream
const TInt32 KObexFileNameStreamFileNameUid		= 0x1000;
const TInt32 KObexFileNameStreamHeaderListUid	= 0x1001;

class CObexHeaderList;

//
// Container class for an array with descriptors and header lists associated with it
//
class CObexMtmFileNameWithHeaders : public CBase
/**
@internalTechnology
@released
*/
	{
public:
	/**
	 * Canonical NewL factory function.
	 *
	 * @return an empty CObexMtmFileNameWithHeaders class
	 * @leave KErrXXX system wide error codes
	 */
	IMPORT_C static CObexMtmFileNameWithHeaders* NewL();

	/**
	 * Destructor. Will destroy the data pointed to by the 2 member pointers.
	 */
	CObexMtmFileNameWithHeaders::~CObexMtmFileNameWithHeaders();

private:
	/**
	 * Constructor.
	 */
	CObexMtmFileNameWithHeaders();

public:
	/**
	 * iFileName points to a descriptor (owned by this class) which holds the filename
	 */
	TDesC16			*iFileName;
	/**
	 * iHeaderList points to a CObexHeaderList class containing the set of headers for this file.
	 */
	CObexHeaderList *iHeaderList;
	};

//
// Class to allow an arbitrary number of filenames to be saved to a CMsvEntry
//
// For obex msgs the obex objects to send are stored in files and not in the msg body. 
// These files may be copied into messaging and attached to the msg as "proper" 
// attachments, OR the msg itself can contain a list of filenames of files that exist
// anywhere on the filesystem. This class is used in the latter case to externalise/
// internalise filenames to/from the msg entry.
//
//
typedef RPointerArray<CObexMtmFileNameWithHeaders> RObexMtmFileNameWithHeadersArray;

class CObexMtmFileNameExternaliser : public CBase
/** Allows an arbitrary number of filenames to be saved to a CMsvEntry.

@publishedPartner
@released
*/
//this class was originally classified as internal technology but was changed to publised partner as one licensee used 
//the static function ExternaliseAndCommit1FileNameL() in its code before the API classification work was done
	{  
public: //static methods
	
	/**
	 * Static function to write one and only one filename to the message store, then finalise and commit it. This function
	 * should be called ONCE ONLY. To externalise more than one filename, instantiate this class and use ExternaliseFileNameL()
	 * followed by CommitStoreL().
	 * @param aEntry the entry whose store is to be written to
	 * @param aDes The filename to write to the store
	 * @leave KErrXXX system-wide error codes
	 */	
	
	IMPORT_C	static	void ExternaliseAndCommit1FileNameL(CMsvEntry* aEntry, const TDesC16* aDes);

	/**
	* Static function to read in one filename from the store.
	*
	* @param aEntry entry to read the filename from
	* @param aDes Descriptor to receive the new filename. Any existing data will be overwritten. MUST be long enough to contain the name.
	* @leave KErrXXX system wide error codes.
	*/
	
	IMPORT_C	static	void Internalise1FileNameL(CMsvEntry* aEntry, TDes16* aDes);

	/**
	 * Static function to get the filenames stored within the message entry. The returned array is pushed onto the cleanup
	 * stack.
	 *
	 * @param aMessageStore Store from which the names are to be read
	 * @return Array of filenames, pushed onto the cleanup stack.
	 * @leave System wide error codes
	 */
	
	IMPORT_C	static   RObexMtmFileNameWithHeadersArray* InternaliseFileNamesLC(CMsvStore& aMessageStore);

public: //non-static methods
	
	/**
	 * Canonical NewL factory function.
	 *
	 * @param aSession A messaging server session
	 * @param aId TMsvId of the entry to externalise/internalise to/from. This association is fixed for the lifetime of this instance of the class.
	 * @return a newly constucted CObexMtmFileNameExternaliser
	 * @leave KErrXXX system wide error codes
	 */
	
	IMPORT_C	static  CObexMtmFileNameExternaliser* NewL(CMsvSession* aSession, TMsvId aId);
	
	/**
	 * Destructor. Will revert the stream to its original state if the store hasn't been comitted.
	 */
	
	CObexMtmFileNameExternaliser::~CObexMtmFileNameExternaliser();

	/**
	 * Initialises the store of the given entry, ready for reading or writing.
	 *
	 * @param aId The TMsvId of the entry whose store should be initialised.
	 * @leave KErrXXX system wide error codes.
	 */
	
	IMPORT_C	void InitialiseStoreL(TMsvId aId);

	/**
	 * Externalises a single filename to the associated entry's message store.
	 *
	 * @param aFileName the filename to externalise.
	 * @leave KErrXXX system-wide error codes.
	 */

	IMPORT_C  	void ExternaliseFileNameL(const TDesC16* aFileName);

	/**
	 * Externalises a pair consisting of a single filename and an obex header list to the associated entry's message store.
	 *
	 * @param aFileName the filename to externalise.
	 * @param aHeaderList the header list to externalise.
	 * @leave KErrXXX system-wide error codes.
	 */

	IMPORT_C  	void ExternaliseFileNameAndHeadersL(const TDesC16* aFileName, const CObexHeaderList* aHeaderList);

	/**
	 * Finalises and commits the store. This function MUST be called once all of the filenames have been externalised, and not
	 * before. If it is not called, any changes will be lost and the store will revert to its former state.
	 *
	 * @leave KErrXXX system wide error codes.
	 */
	
	IMPORT_C  	void CommitStoreL();

private:
	
	/**
	 * Constructor	
	 *
	 * @param aSession A messaging server session
	 */
	
	CObexMtmFileNameExternaliser(CMsvSession* aSession);

private:
	CMsvSession*		iSession; //< Session with the message server
	CMsvEntry*			iEntry; //< Message entry
	CMsvStore*			iMessageStore; //< Direct interface to the message store associated with a particular entry
	RMsvWriteStream		iMessageWriteStream; //< Accesses streams in a message store with write access
	TBool				iHaveCommitted; //< Indicates whether the message write stream has committed changes
	};

#endif // __OBEXMTMUTIL_H__
