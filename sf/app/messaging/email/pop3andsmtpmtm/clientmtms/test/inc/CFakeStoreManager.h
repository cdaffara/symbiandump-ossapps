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
//

#ifndef __CFAKESTOREMANAGER_H__
#define __CFAKESTOREMANAGER_H__

#include <e32base.h>
#include <mmsvstoremanager.h>

class RFs;

const TInt KMaxStoreFilenameLength = 10+4+4; // maximum length for TInt32 represented as a descriptor + "c:\\" + ".new" for temp store files

class CFakeStoreManager : public CBase,
						  public MMsvStoreManager
	{
public:
	static CFakeStoreManager* NewL(RFs& aFs);
	~CFakeStoreManager();
	
	const TDesC& StoreFilename(TMsvId aEntryId) const;
	
	// from MMsvStoreManager
	virtual TBool CreateAttachmentForWriteL(TMsvId aEntryId, TDes& aFilename, RFile& aFile);
	virtual void OpenAttachmentL(TMsvId aEntryId, const TDesC& aFilename, RFile& aFile);
	virtual void OpenAttachmentForWriteL(TMsvId aEntryId, const TDesC& aFileName, RFile& aFile);	
	virtual TInt DeleteAttachment(TMsvId aEntryId, const TDesC& aFilename);
	virtual TInt OpenFileStoreForRead(TMsvId aEntryId, RFile& aFile);
	virtual void OpenTempStoreFileL(TMsvId aEntryId, RFile& aFile);
	virtual void ReplaceFileStoreL(TMsvId aEntryId);
	virtual void DeleteFileStoreL(TMsvId aEntryId);
	virtual TBool FileStoreExistsL(TMsvId aEntryId) const;
	virtual TBool FileExistsL(const TDesC& aFilePath);
	virtual TInt RenameAttachment(TMsvId aEntryId, const TDesC& aOldFilePath, const TDesC& aNewName);
 	virtual void ReplaceAttachmentForWriteL(TMsvId aEntryId, TDes& aFilename, RFile& aFile);
 	virtual void AttachmentFilePathL(TMsvId aEntryId, TDes& aFilePath);
 	TBool CreateShareProtectedAttachmentForWriteL(TMsvId aEntryId, TDes& aFilename, RFile& aFile);
	
	void BodyTextFilePathL(TMsvId aBodyTextId, TDes& aFilepath);
	void OpenBodyTextFileForReadL(RFile& aFile, TMsvId aBodyTextId, const TDesC& aFilePath);
	void CreatePlainTextFileL(RFile& aFile, TMsvId aBodyTextId);
	void DeletePlainTextFileL(TMsvId aBodyTextId);
	void ReplacePlainTextFileL(TMsvId aBodyTextId);

// New APIs from PREQ2073
#if (defined SYMBIAN_MESSAGESTORE_HEADER_BODY_USING_SQLDB)
	// from MMsvStoreManager
	virtual void CreateBodyL(TMsvId aEntryId, const TUid& aUid, HBufC* aBodyContent);
	virtual void LoadBodyL(TMsvId aEntryId, TUid& aUid, HBufC*& aBodyContent);
	virtual void UpdateBodyL(TMsvId aEntryId, const TUid& aUid, HBufC* aBodyContent);
	virtual void DeleteBodyL(TMsvId aEntryId);
	virtual void CreateHeaderEntryL(const TUid& aMtmId, TMsvId aEntryId, const RPointerArray<CHeaderFields>& aFieldPairList);
	virtual void LoadHeaderEntryL(const TUid& aMtmId, TMsvId aEntryId, RPointerArray<CHeaderFields>& aFieldPairList);
	virtual void DeleteHeaderEntryL(const TUid& aMtmId, TMsvId aEntryId);
	virtual void UpdateHeaderEntryL(const TUid& aMtmId, TMsvId aEntryId, const RPointerArray<CHeaderFields>& aFieldPairList);
	virtual TBool DoesAnyStoreExists(TMsvId aId, TUid aMtmId);
	virtual TBool DoesHeaderTableExist(const TUid& aMtmId);
	
#endif

private:
	CFakeStoreManager(RFs& aFs);
	void ConstructL();
	
private:
	RFs& iFs;
	mutable TBuf<KMaxStoreFilenameLength> iFilename;
	};
	
#endif // __CFAKESTOREMANAGER_H__
