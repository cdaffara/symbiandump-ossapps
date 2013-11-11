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

#include <f32file.h>

#include "CFakeStoreManager.h"

_LIT(KDriveLetter, "c:\\");

CFakeStoreManager* CFakeStoreManager::NewL(RFs& aFs)
	{
	CFakeStoreManager* self = new(ELeave) CFakeStoreManager(aFs);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}
	
CFakeStoreManager::~CFakeStoreManager()
	{
	}
	
CFakeStoreManager::CFakeStoreManager(RFs& aFs)
	: iFs(aFs)
	{
	}
	
void CFakeStoreManager::ConstructL()
	{
	}
	
TBool CFakeStoreManager::CreateAttachmentForWriteL(TMsvId /*aEntryId*/, TDes& /*aFilename*/, RFile& /*aFile*/)
	{
	User::Invariant();
	return EFalse;
	}
	
void CFakeStoreManager::OpenAttachmentL(TMsvId /*aEntryId*/, const TDesC& /*aFilename*/, RFile& /*aFile*/)
	{
	User::Invariant();
	}
	
TInt CFakeStoreManager::DeleteAttachment(TMsvId /*aEntryId*/, const TDesC& /*aFilename*/)
	{
	return KErrNotSupported;
	}
	
TBool CFakeStoreManager::FileExistsL(const TDesC& /*aFilePath*/)
	{
	return KErrNotSupported;
	}	

TInt CFakeStoreManager::RenameAttachment(TMsvId /*aEntryId*/, const TDesC& /*aOldFilePath*/, const TDesC& /*aNewName*/)
	{
	return KErrNotSupported;
	}
	
void CFakeStoreManager::ReplaceAttachmentForWriteL(TMsvId /*aEntryId*/, TDes& /*aFilename*/, RFile& /*aFile*/)
	{
	User::Invariant();
	}
	
void CFakeStoreManager::AttachmentFilePathL(TMsvId /*aEntryId*/, TDes& /*aFilePath*/)
	{
	User::Invariant();
	}
	
TInt CFakeStoreManager::OpenFileStoreForRead(TMsvId aEntryId, RFile& aFile)
	{
	return aFile.Open(iFs, StoreFilename(aEntryId), EFileRead | EFileShareReadersOnly);
	}
	
void CFakeStoreManager::OpenTempStoreFileL(TMsvId aEntryId, RFile& aFile)
	{
	TBuf<KMaxStoreFilenameLength> tempFilename;
	tempFilename.Append(StoreFilename(aEntryId));
	_LIT(KTxtTempExt, ".new");
	tempFilename.Append(KTxtTempExt());
	User::LeaveIfError(aFile.Replace(iFs, tempFilename, EFileWrite));
	}
	
void CFakeStoreManager::ReplaceFileStoreL(TMsvId aEntryId)
	{
	TBuf<KMaxStoreFilenameLength> tempFilename;
	tempFilename.Append(StoreFilename(aEntryId));
	_LIT(KTxtTempExt, ".new");
	tempFilename.Append(KTxtTempExt());
	
	User::LeaveIfError(iFs.Replace(tempFilename, StoreFilename(aEntryId)));
	}
	
void CFakeStoreManager::DeleteFileStoreL(TMsvId aEntryId)
	{
	TInt err = iFs.Delete(StoreFilename(aEntryId));
	
	if( (err!=KErrNotFound) && (err!=KErrPathNotFound) )
		{
		User::LeaveIfError(err);
		}
	}
	
TBool CFakeStoreManager::FileStoreExistsL(TMsvId aEntryId) const
	{
	TBool isOpen = EFalse;
	TInt err = iFs.IsFileOpen(StoreFilename(aEntryId), isOpen);
	if( (err==KErrNotFound) || (err==KErrPathNotFound) )
		{
		return EFalse;
		}
	
	User::LeaveIfError(err);
	
	return ETrue;
	}
	
void CFakeStoreManager::OpenAttachmentForWriteL(TMsvId /*aEntryId*/, const TDesC& /*aFileName*/, RFile& /*aFile*/)	
	{
	User::Invariant();
	}

const TDesC& CFakeStoreManager::StoreFilename(TMsvId aEntryId) const
	{
	iFilename.Zero();
	iFilename.Append(KDriveLetter);
	iFilename.AppendNum(static_cast<TInt>(aEntryId));
	
	return iFilename;
	}
	
TBool CFakeStoreManager::CreateShareProtectedAttachmentForWriteL(TMsvId /*aEntryId*/, TDes& /*aFilename*/, RFile& /*aFile*/)
	{
	 User::Leave(KErrNotSupported);
	//Avoid complier errors and warnings...
	return EFalse;
	}

void CFakeStoreManager::BodyTextFilePathL(TMsvId /*aMessageId*/, TDes& /*aFilename*/)
	{
	User::Leave(KErrNotSupported);
	}


void CFakeStoreManager::OpenBodyTextFileForReadL(RFile& /*aFile*/, TMsvId /*aBodyTextId*/, const TDesC& /*aFilePath*/)
	{

	User::Leave(KErrNotSupported);
	}

void CFakeStoreManager::CreatePlainTextFileL(RFile& /*aFile*/, TMsvId /*aBodyTextId*/)
	{
	User::Leave(KErrNotSupported);
	}
	
void CFakeStoreManager::DeletePlainTextFileL(TMsvId /*aBodyTextId*/)
	{
	 User::Leave(KErrNotSupported);
	}

void CFakeStoreManager::ReplacePlainTextFileL(TMsvId /*aBodyTextId*/)
	{
	User::Leave(KErrNotSupported);
	}


// New APIs from PREQ2073
#if (defined SYMBIAN_MESSAGESTORE_HEADER_BODY_USING_SQLDB)

void CFakeStoreManager::CreateBodyL(TMsvId /*aEntryId*/, const TUid& /*aUid*/, HBufC* /*aBodyContent*/)
	{
		
	}
	
void CFakeStoreManager::LoadBodyL(TMsvId /*aEntryId*/, TUid& /*aUid*/, HBufC*& /*aBodyContent*/)
	{
		
	}
	
void CFakeStoreManager::UpdateBodyL(TMsvId /*aEntryId*/, const TUid& /*aUid*/, HBufC* /*aBodyContent*/)
	{
	
	}

void CFakeStoreManager::DeleteBodyL(TMsvId /*aEntryId*/)
	{
	
	}
	
void CFakeStoreManager::CreateHeaderEntryL(const TUid& /*aMtmId*/, TMsvId /*aEntryId*/, const RPointerArray<CHeaderFields>& /*aFieldPairList*/)
	{
	
	}
	
void CFakeStoreManager::LoadHeaderEntryL(const TUid& /*aMtmId*/, TMsvId /*aEntryId*/, RPointerArray<CHeaderFields>& /*aFieldPairList*/)
	{
	
	}
	
void CFakeStoreManager::DeleteHeaderEntryL(const TUid& /*aMtmId*/, TMsvId /*aEntryId*/)
	{
	
	}
	
void CFakeStoreManager::UpdateHeaderEntryL(const TUid& /*aMtmId*/, TMsvId /*aEntryId*/, const RPointerArray<CHeaderFields>& /*aFieldPairList*/)
	{
	
	}
TBool CFakeStoreManager::DoesAnyStoreExists(TMsvId /*aId*/ , TUid /*aMtmId*/)
	{
	return ETrue;
	}

TBool CFakeStoreManager::DoesHeaderTableExist(const TUid& /*aMtmId*/)
	{
	return ETrue;
	}
	
#endif	
