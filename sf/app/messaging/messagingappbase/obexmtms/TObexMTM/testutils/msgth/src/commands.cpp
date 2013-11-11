// Copyright (c) 2003-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include "commands.h"

#ifndef _NO_IAP_PREFS
#include <iapprefs.h>
#endif

_LIT(K_T_MSG_STRUCT_FILE, "c:\\logs\\email\\Entry_Structure.txt");
_LIT(KErrSelectText, "Error! Cannot find entry '%S'.\n");

const TInt KDefaultWaitTime = 5000000;


class TLineReader : public CBase
	{
private:
	RFile& iFile;
	TBuf8<512> iBuffer;
	TInt iBufferIndex;

	TBool Peek(char& rChar)
		{
		TBool characterFound = ETrue;
		if (iBufferIndex == iBuffer.Size())
			{
			// Refresh buffer
			TInt readError = iFile.Read(iBuffer);
			iBufferIndex = 0;
			if ((readError != KErrNone) || (iBuffer.Size() == 0))
				{
				characterFound = EFalse;
				}
			}

		if (characterFound)
			{
			rChar = iBuffer[iBufferIndex];
			}

		return characterFound;
		};

	TBool GetCharacter(char& rChar)
		{
		TBool characterFound = Peek(rChar);
		if (characterFound)
			{
			iBufferIndex++;
			}

		return characterFound;
		}

public:
	TLineReader(RFile& rFile) : iFile(rFile), iBufferIndex(0)
		{};

	HBufC8* GetLineLC()
		{
		// Find next cr or lf (and ignore subsequent cr's or lf's)
		HBufC8* newLine = HBufC8::NewLC(512);

		char ch;
		
		TBool characterFound = GetCharacter(ch);
		TBool eolFound = EFalse;
		
		while ((characterFound) && (!eolFound))
			{
			if ((ch == 0x0d) || (ch == 0x0a))
				// cr or lf found, ignore subsequent cr and lf's
				{
				eolFound = ETrue;
				while ((characterFound) && ((ch == 0x0d) || (ch == 0x0a)))
					{
					characterFound = Peek(ch);
					if ((characterFound) && ((ch == 0x0d) || (ch == 0x0a)))
						{
						characterFound = GetCharacter(ch);
						}
					}
				}

			if ((characterFound) && (!eolFound))
				{
				newLine->Des().Append(ch);
				characterFound = GetCharacter(ch);
				}
			}
		
		return newLine;
		};
	};




//
//
// CMsvTestCleanMessageFolder
//

EXPORT_C void CCleanMessageFolder::StartL(TRequestStatus& aStatus)
	{
	TRAPD(err, iTestUtils.CleanMessageFolderL());
	TRequestStatus* status = &aStatus;
	User::RequestComplete(status, err);
	}

EXPORT_C CCleanMessageFolder::CCleanMessageFolder(CMsvTestUtils& aTestUtils) : iTestUtils(aTestUtils)
	{
	}


//
//
// CAddComment
//

EXPORT_C CAddComment* CAddComment::NewL(const CDesCArrayFlat& aComment, CMsvTestUtils& aTestUtils)
	{
	CAddComment* self = new (ELeave) CAddComment(aTestUtils);
	CleanupStack::PushL(self);
	self->ConstructL(aComment);
	CleanupStack::Pop(self);
	return self;
	}

void CAddComment::ConstructL(const CDesCArrayFlat& aComment)
	{
	// need to construct one big HBufC string to use as the comment
	//  first, find out size of array...
	TInt commentSize = 0;
	for(TInt i = 0; i < aComment.Count(); i++)
		commentSize += ((aComment[i]).Length() + _L(" ").Size()); // extra 1 added for padding between words.

	//  second, construct HBufC.... 
	iComment = HBufC::NewL(commentSize);

	//  create a pointer to the HBufC, so that we can add text to it.
	TPtr commentPtr = iComment->Des();

	//  now create string
	for(TInt j = 0; j < aComment.Count(); j++)
		{
		commentPtr.Append((aComment)[j]);
		commentPtr.Append(_L(" ")); // extra 1 added for padding between words.
		}

	//  hopefully everything should be added ok!
	}

EXPORT_C void CAddComment::StartL(TRequestStatus& aStatus)
	{
	// now we have to add the debugging line number which we only know at run time...
	TBuf<7> commentString;
	commentString.Format(_L("[%4d] "), iDebugInfo.LineNumber());

	// create a pointer to the HBufC, realloc size and then insert new data...
	iComment = iComment->ReAllocL(iComment->Size() + commentString.Size());
	TPtr commentPtr = iComment->Des();
	commentPtr.Insert(0, commentString);

	iTestUtils.WriteComment(*iComment);

	TRequestStatus* status = &aStatus;
	User::RequestComplete(status, KErrNone);
	}

CAddComment::CAddComment(CMsvTestUtils& aTestUtils) : iTestUtils(aTestUtils)
	{
	}

EXPORT_C CAddComment::~CAddComment()
	{
	delete iComment;
	}




//
//
// CGoClientSide
//

EXPORT_C void CGoClientSide::StartL(TRequestStatus& aStatus)
	{
	iTestUtils.GoClientSideL();
	TRequestStatus* status = &aStatus;
	User::RequestComplete(status, KErrNone);
	}

EXPORT_C CGoClientSide::CGoClientSide(CMsvTestUtils& aTestUtils) : iTestUtils(aTestUtils)
	{}




//
//
// CGoServerSide
//

EXPORT_C void CGoServerSide::StartL(TRequestStatus& aStatus)
	{
	iTestUtils.GoServerSideL();
	TRequestStatus* status = &aStatus;
	User::RequestComplete(status, KErrNone);
	}

EXPORT_C CGoServerSide::CGoServerSide(CMsvTestUtils& aTestUtils) : iTestUtils(aTestUtils)
	{}




//
//
// CResetMessageServer
//

EXPORT_C void CResetMessageServer::StartL(TRequestStatus& aStatus)
	{
	iParentTestHarness.Reset();
	TRequestStatus* status = &aStatus;
	User::RequestComplete(status, KErrNone);
	}

EXPORT_C CResetMessageServer::CResetMessageServer(CMsvClientTest& aParentTestHarness)
	: iParentTestHarness(aParentTestHarness)
	{}






//
//
// CCheckMessageFolder
//

EXPORT_C void CCheckMessageFolder::StartL(TRequestStatus& aStatus)
	{
	TBool matched = ETrue;
	iTestUtils.FindChildrenL(0x01000, ETrue);

	RFile file1;
	User::LeaveIfError(file1.Open(iTestUtils.FileSession(), K_T_MSG_STRUCT_FILE, EFileShareAny));

	RFile file2;
	User::LeaveIfError(file2.Open(iTestUtils.FileSession(), iFileName, EFileShareAny));

	TLineReader lineReader1(file1);
	TLineReader lineReader2(file2);

	HBufC8* actualLine = lineReader1.GetLineLC();
	HBufC8* expectedLine = lineReader2.GetLineLC();

	while ((actualLine->Des().Size() > 0) && (expectedLine->Des().Size() > 0) && (matched))
		{
		if (actualLine->Des() != expectedLine->Des())
			matched = EFalse;

		CleanupStack::PopAndDestroy(2); // actualLine, expectedLinw

		actualLine = lineReader1.GetLineLC();
		expectedLine = lineReader2.GetLineLC();
		}

	if ((actualLine->Des().Size() != 0)
		|| (expectedLine->Des().Size() != 0))
		{
		matched = EFalse;
		}

	CleanupStack::PopAndDestroy(2); // actualLine, expectedLinw

	if (!matched)
		{
		User::Leave(KErrNotFound);
		}

	file1.Close();
	file2.Close();

	TRequestStatus* status = &aStatus;
	User::RequestComplete(status, KErrNone);
	}

EXPORT_C CCheckMessageFolder::CCheckMessageFolder(const TDesC& aFileName, CMsvTestUtils& aTestUtils) : iTestUtils(aTestUtils)
	{
	iFileName = aFileName;
	}



//
//
// CSelectEntry
//

CSelectEntry::CSelectEntry(const TMsvId& aCurrentFolderId, TMsvId* aEntryId, CMsvTestUtils& aTestUtils) 
: iTestUtils(aTestUtils), 
  iCurrentFolderId(aCurrentFolderId), 
  iEntryId(aEntryId)
	{
	iEntrySelection = NULL;
	}

CSelectEntry::CSelectEntry(const TMsvId& aCurrentFolderId, CMsvEntrySelection* aSelection, CMsvTestUtils& aTestUtils) 
: iTestUtils(aTestUtils),
  iCurrentFolderId(aCurrentFolderId), 
  iEntrySelection(aSelection)
	{
	iEntryId = NULL;
	}

EXPORT_C CSelectEntry* CSelectEntry::NewL(const TDesC& aName, const TMsvId& aCurrentFolderId, TMsvId* aEntryId, CMsvTestUtils& aTestUtils)
	{
	CSelectEntry* self = new (ELeave) CSelectEntry(aCurrentFolderId, aEntryId, aTestUtils);
	CleanupStack::PushL(self);
	self->ConstructL(aName);
	CleanupStack::Pop(self);
	return self;
	}

EXPORT_C CSelectEntry* CSelectEntry::NewL(const TDesC& aName, const TMsvId& aCurrentFolderId, CMsvEntrySelection* aSelection, CMsvTestUtils& aTestUtils)
	{
	CSelectEntry* self = new (ELeave) CSelectEntry(aCurrentFolderId, aSelection, aTestUtils);
	CleanupStack::PushL(self);
	self->ConstructL(aName);
	CleanupStack::Pop(self);
	return self;
	}

void CSelectEntry::ConstructL(const TDesC& aName)
	{
	iEntryName = HBufC::NewL(aName.Length());
	(*iEntryName) = aName;
	}

EXPORT_C void CSelectEntry::StartL(TRequestStatus& aStatus)
	{
	// Select current folder
	CMsvEntry& entry = *(iTestUtils.iMsvEntry);
	entry.SetEntryL(iCurrentFolderId);

	// show the invisible folders.....
	TMsvSelectionOrdering order;
	order.SetShowInvisibleEntries(ETrue);
	entry.SetSortTypeL(order);

	CMsvEntrySelection* tempSelection = entry.ChildrenL();
	CleanupStack::PushL(tempSelection);
	
	// Check each child entry for a matching string
	TBool found = EFalse;
	TInt entryIndex = tempSelection->Count();
	while ((!found) && (entryIndex-- != 0))
		{
		entry.SetEntryL((*tempSelection)[entryIndex]);
		if (iEntryName->CompareF(entry.Entry().iDescription) == 0)
			{
			found = ETrue;
			}
		else if (iEntryName->CompareF(entry.Entry().iDetails) == 0)
			{
			found = ETrue;
			}
		}

	if (found)
		{
		// decide if its a Selection thats required, or an Entry
		if(iEntryId)
			{
			*iEntryId = entry.Entry().Id();
			}
		else
			{
			(*iEntrySelection).Reset();
			(*iEntrySelection).AppendL(entry.Entry().Id());
			}

		TRequestStatus* status = &aStatus;
		User::RequestComplete(status, KErrNone);
		}
	else
		{
		TBuf<256> errorString;
		errorString.Format(KErrSelectText, iEntryName);
		HBufC* errorStringHBufC = HBufC::NewLC(errorString.Length());
		(errorStringHBufC->Des()).Copy(errorString);

		iTestUtils.WriteComment(*errorStringHBufC);
		CleanupStack::PopAndDestroy();

		TRequestStatus* status = &aStatus;
		User::RequestComplete(status, KErrNotFound);
		}

	CleanupStack::PopAndDestroy(tempSelection);
	}




EXPORT_C CSelectEntry::~CSelectEntry()
	{
	delete iEntryName;
	}
/*
CSelectEntry::CSelectEntry(const TMsvId& aCurrentFolderId, TMsvId& aEntryId, CMsvTestUtils& aTestUtils) : iCurrentFolderId(aCurrentFolderId), iTestUtils(aTestUtils), iEntryId(aEntryId)
	{
	}


CSelectEntry* CSelectEntry::NewL(const TDesC& aName, const TMsvId& aCurrentFolderId, TMsvId& aEntryId, CMsvTestUtils& aTestUtils)
	{
	CSelectEntry* self = new (ELeave) CSelectEntry(aCurrentFolderId, aEntryId, aTestUtils);
	CleanupStack::PushL(self);
	self->ConstructL(aName);
	CleanupStack::Pop(self);
	return self;
	}

void CSelectEntry::ConstructL(const TDesC& aName)
	{
	iEntryName = HBufC::NewL(aName.Length());
	(*iEntryName) = aName;
	}

void CSelectEntry::StartL(TRequestStatus& aStatus)
	{
	// Select current folder
	CMsvEntry& entry = *(iTestUtils.iMsvEntry);
	entry.SetEntryL(iCurrentFolderId);

	// show the invisible folders.....
	TMsvSelectionOrdering order;
	order.SetShowInvisibleEntries(ETrue);
	entry.SetSortTypeL(order);

	CMsvEntrySelection* tempSelection = entry.ChildrenL();
	CleanupStack::PushL(tempSelection);
	
	// Check each child entry for a matching string
	TBool found = EFalse;
	TInt entryIndex = tempSelection->Count();
	while ((!found) && (entryIndex-- != 0))
		{
		entry.SetEntryL((*tempSelection)[entryIndex]);
		if (iEntryName->CompareF(entry.Entry().iDescription) == 0)
			{
			found = ETrue;
			}
		else if (iEntryName->CompareF(entry.Entry().iDetails) == 0)
			{
			found = ETrue;
			}
		}

	if (found)
		{
		iEntryId = entry.Entry().Id();

		TRequestStatus* status = &aStatus;
		User::RequestComplete(status, KErrNone);
		}
	else
		{
		TBuf<256> errorString;
		errorString.Format(KErrSelectText, iEntryName);
		HBufC* errorStringHBufC = HBufC::NewLC(errorString.Length());
		(errorStringHBufC->Des()).Copy(errorString);

		iTestUtils.WriteComment(*errorStringHBufC);
		CleanupStack::PopAndDestroy();

		TRequestStatus* status = &aStatus;
		User::RequestComplete(status, KErrNotFound);
		}

	CleanupStack::PopAndDestroy(tempSelection);
	}




CSelectEntry::~CSelectEntry()
	{
	delete iEntryName;
	}
*/

//
//
// CDumpMessageStore
//

EXPORT_C void CDumpMessageStore::StartL(TRequestStatus& aStatus)
	{
	iTestUtils.FindChildrenL(KMsvRootIndexEntryId);
	TRequestStatus* status = &aStatus;
	User::RequestComplete(status, KErrNone);
	}

EXPORT_C CDumpMessageStore::CDumpMessageStore(CMsvTestUtils& aTestUtils) : iTestUtils(aTestUtils)
	{
	}

//
//
// CCheckSelectionCount
//
	
EXPORT_C CCheckSelectionCount::CCheckSelectionCount(TInt aCount, CMsvEntrySelection& aSelection, CMsvTestUtils& aTestUtils) 
: iTestUtils(aTestUtils),
  iSelection(aSelection),
  iTestCount(aCount) 
  	{
	}

void CCheckSelectionCount::LogCommentFormatL(TRefByValue<const TDesC> format,...)
	{
	// Build parameter list.
	VA_LIST list;
	VA_START(list, format);
	TBuf<0x100> buf;
	buf.FormatList(format, list);

	// Log the debug buffer.
	iTestUtils.WriteComment(buf);
	}

EXPORT_C void CCheckSelectionCount::StartL(TRequestStatus& aStatus)
	{
	TInt err=KErrNone;
	TInt selectionCount=iSelection.Count();

	LogCommentFormatL(_L("[%4d] Folder contains %d entries when %d expected."), iDebugInfo.LineNumber(), selectionCount, iTestCount);

	if (selectionCount != iTestCount)
		{
		LogCommentFormatL(_L("Error!"));
		err = KErrUnknown;
		}

	TRequestStatus* status = &aStatus;
	User::RequestComplete(status, err);
	}


//
//
// CCheckNewFlag
//
	
EXPORT_C CCheckNewFlag::CCheckNewFlag(TInt aSelectedIndex, TBool aTestNewStatus, CMsvEntrySelection& aSelection, CMsvTestUtils& aTestUtils) 
:iTestUtils(aTestUtils),
 iSelection(aSelection),
 iSelectedIndex(aSelectedIndex),
 iTestNewStatus(aTestNewStatus) 
	{
	}

void CCheckNewFlag::LogCommentFormatL(TRefByValue<const TDesC> format,...)
	{
	// Build parameter list.
	VA_LIST list;
	VA_START(list, format);
	TBuf<0x100> buf;
	buf.FormatList(format, list);

	// Log the debug buffer.
	iTestUtils.WriteComment(buf);
	}

EXPORT_C void CCheckNewFlag::StartL(TRequestStatus& aStatus)
// Check the status flag of the selected entry is the same as the test value
	{
	TInt err=KErrNone;
	CMsvEntry& msvEntry = *(iTestUtils.iMsvEntry);
	TMsvId entryId = iSelection.At(iSelectedIndex);
	msvEntry.SetEntryL(entryId);
	TBool entryIsNew = msvEntry.Entry().New();

	// Check if the Entry is New
	if (entryIsNew) 
		{
		// Check if the entry Should be New
		if (iTestNewStatus)
			{
			// The entry is new
			_LIT(KNewEntry, "Entry %d is New");
			LogCommentFormatL(KNewEntry, iSelectedIndex);
			}
		else
			{
			// The Entry is New, but is should be old
			_LIT(KErrNewEntry, "Error, the entry is New.  It should be Old!");
			LogCommentFormatL(KErrNewEntry);
			err = KErrUnknown;
			}
			
		}
	else
		{
		// The Entry is Old.  Check if it should be.
		if (!iTestNewStatus)
			{
			// The entry is Old
			_LIT(KOldEntry, "Entry %d is Old");
			LogCommentFormatL(KOldEntry, iSelectedIndex);
			}
		else
			{
			// The Entry is Old, but is should be New
			_LIT(KErrOldEntry, "Error, the entry is Old.  It should be New!");
			LogCommentFormatL(KErrOldEntry);
			err = KErrUnknown;
			}
		}

	TRequestStatus* status = &aStatus;
	User::RequestComplete(status, err);
	}



//
//
// CCheckPriority
//
	
EXPORT_C CCheckPriority::CCheckPriority(TInt aSelectedIndex, TInt aTestPriority, CMsvEntrySelection& aSelection, CMsvTestUtils& aTestUtils) 
:iTestUtils(aTestUtils),
 iSelection(aSelection),
 iSelectedIndex(aSelectedIndex),
 iTestPriority(aTestPriority) 
	{
	}

void CCheckPriority::LogCommentFormatL(TRefByValue<const TDesC> format,...)
	{
	// Build parameter list.
	VA_LIST list;
	VA_START(list, format);
	TBuf<0x100> buf;
	buf.FormatList(format, list);

	// Log the debug buffer.
	iTestUtils.WriteComment(buf);
	}

EXPORT_C void CCheckPriority::StartL(TRequestStatus& aStatus)
// Check the Priority flag of the selected entry is the same as the test value
	{
	TInt err=KErrNone;
	CMsvEntry& msvEntry = *(iTestUtils.iMsvEntry);
	TMsvId entryId = iSelection.At(iSelectedIndex);
	msvEntry.SetEntryL(entryId);
	TMsvPriority priority = msvEntry.Entry().Priority();

	// Check if the Priority flag is correct
	if (priority == iTestPriority)
		{
		// Correct Priority
		_LIT(KCorrectPriority, "Entry %d has Priority %d");
		LogCommentFormatL(KCorrectPriority, entryId, priority);
		}
	else
		{
		_LIT(KInCorrectPriority, "Error, Entry %d has Priority %d");
		LogCommentFormatL(KInCorrectPriority, entryId, priority);
		err = KErrUnknown;
		}

	TRequestStatus* status = &aStatus;
	User::RequestComplete(status, err);
	}



//
//
// CCheckAttachment
//
	
EXPORT_C CCheckAttachment::CCheckAttachment(TInt aSelectedIndex, TBool aTestAttachment, CMsvEntrySelection& aSelection, CMsvTestUtils& aTestUtils) 
:iTestUtils(aTestUtils),
 iSelection(aSelection),
 iSelectedIndex(aSelectedIndex),
 iTestAttachment(aTestAttachment) 
	{
	}

void CCheckAttachment::LogCommentFormatL(TRefByValue<const TDesC> format,...)
	{
	// Build parameter list.
	VA_LIST list;
	VA_START(list, format);
	TBuf<0x100> buf;
	buf.FormatList(format, list);

	// Log the debug buffer.
	iTestUtils.WriteComment(buf);
	}

EXPORT_C void CCheckAttachment::StartL(TRequestStatus& aStatus)
// Check the attachment flag for this entry is the same as the test value
	{
	TInt err=KErrNone;
	CMsvEntry& msvEntry = *(iTestUtils.iMsvEntry);
	TMsvId entryId = iSelection.At(iSelectedIndex);
	msvEntry.SetEntryL(entryId);
	TBool entryHasAttachment = EFalse;

	// Find out if the Entry has an attachment
	// See if the Entry has a Child Folder Entry
	CMsvEntrySelection* children = msvEntry.ChildrenWithTypeL(KUidMsvFolderEntry);
	CleanupStack::PushL(children);
	if(children->Count() > 0)
		{
		// Find out if the Child Folder Entry has an attachment
		msvEntry.SetEntryL(children->At(0));
		CMsvEntrySelection* grandChildren = msvEntry.ChildrenWithTypeL(KUidMsvAttachmentEntry);
		CleanupStack::PushL(grandChildren);
		if(grandChildren->Count() > 0)
			{
			entryHasAttachment = ETrue;
			}

		CleanupStack::PopAndDestroy(); //grandChildren
		}

	CleanupStack::PopAndDestroy(); //children


	// Check if the Entry has an attachment 
	if (entryHasAttachment)
		{
		// Check if the entry should have an Attachment
		if (iTestAttachment)
			{
			// Entry has an attachment
			_LIT(KAttachment, "Entry %d has an Attachment");
			LogCommentFormatL(KAttachment, entryId);
			}
		else
			{
			// There should Not be an attachment, but there is
			_LIT(KErrAttachment, "Error, Entry should NOT have an Attachment!");
			LogCommentFormatL(KErrAttachment);
			err = KErrUnknown;
			}
		}
	else
		{
		// Check if the entry should NOT have an Attachment
		if (!iTestAttachment)
			{
			// Entry does NOT have an attachment
			_LIT(KNoAttachment, "Entry %d does not have an Attachment");
			LogCommentFormatL(KNoAttachment, entryId);
			}
		else
			{
			// There should be an attachment, but there is NOT
			_LIT(KErrNoAttachment, "Error, Entry should have an Attachment!");
			LogCommentFormatL(KErrNoAttachment);
			err = KErrUnknown;
			}
		}

	TRequestStatus* status = &aStatus;
	User::RequestComplete(status, err);
	}




//
//
// CCheckBodyText
//
	
CCheckBodyText::CCheckBodyText(TInt aSelectedIndex, CMsvEntrySelection& aSelection, CMsvTestUtils& aTestUtils) 
:iTestUtils(aTestUtils),
 iSelection(aSelection),
 iSelectedIndex(aSelectedIndex) 
	{
	}

EXPORT_C CCheckBodyText* CCheckBodyText::NewL(TInt aSelectedIndex, TDesC& aTestBodyText, CMsvEntrySelection& aSelection, CMsvTestUtils& aTestUtils)
	{
	CCheckBodyText* self = new (ELeave) CCheckBodyText(aSelectedIndex, aSelection, aTestUtils);
	CleanupStack::PushL(self);
	self->ConstructL(aTestBodyText);
	CleanupStack::Pop(self);
	return self;
	}

void CCheckBodyText::ConstructL(TDesC& aTestBodyText)
	{
	iTestBodyText = HBufC::NewL(200);
	iTestBodyText->Des().Append(aTestBodyText);
	}

EXPORT_C CCheckBodyText::~CCheckBodyText()
	{
	delete iTestBodyText;
	}

void CCheckBodyText::LogCommentFormatL(TRefByValue<const TDesC> format,...)
	{
	// Build parameter list.
	VA_LIST list;
	VA_START(list, format);
	TBuf<0x100> buf;
	buf.FormatList(format, list);

	// Log the debug buffer.
	iTestUtils.WriteComment(buf);
	}

// TODO: Commented out for the MMS Test Harness as leads to compile errors.

EXPORT_C void CCheckBodyText::StartL(TRequestStatus& /*aStatus*/)
// Check the Body Text of this entry is the same as the test value
	{
/*	TInt err=KErrNone;
	CMsvEntry& msvEntry = *(iTestUtils.iMsvEntry);
	TMsvId entryId = iSelection.At(iSelectedIndex);
	msvEntry.SetEntryL(entryId);
	
	// Get the ID of the Body Text Entry, by looking at the Child Entries
	TMsvId bodyTextEntryId = KNoBodyText;
	CMsvEntrySelection* children = msvEntry.ChildrenL();
	CleanupStack::PushL(children);
	if(children->Count() > 0)
		{
		// Check if the Child Entry is Body Text or a Folder
		msvEntry.SetEntryL(children->At(0));
		switch (msvEntry.Entry().iType.iUid)
			{
			case KUidMsvEmailTextEntryValue:
				bodyTextEntryId = msvEntry.EntryId();
				break;

			case KUidMsvFolderEntryValue:
				{
				// Check if the Child Folder has a Child Body Text Entry
				CMsvEntrySelection* grandChild = msvEntry.ChildrenWithTypeL(KUidMsvEmailTextEntry);
				CleanupStack::PushL(grandChild);
				if(grandChild->Count() > 0)
					{
					// Get the Id of the Body Text Entry
					msvEntry.SetEntryL(grandChild->At(0));
					bodyTextEntryId = msvEntry.EntryId();
					}

				CleanupStack::PopAndDestroy(); // grandChild
				break;
				}

			default:
				break;
			}
		}

	
	// If there is a Body Text Entry, then check if the Text is correct
	if (bodyTextEntryId == KNoBodyText)
		{
		_LIT(KErrNoBodyText, "Error, there is no Body Text");
		LogCommentFormatL(KErrNoBodyText);
		err = KErrUnknown;
		}
	else
		{
		// Set the Entry to be the Body Text
		msvEntry.SetEntryL(bodyTextEntryId);
		if (msvEntry.HasStoreL())
			{
			// Get the store
			CMsvStore* store = msvEntry.ReadStoreL();
			CleanupStack::PushL(store);

			// Get the body Text if it exists
			CParaFormatLayer* paraFormat = CParaFormatLayer::NewL();
			CleanupStack::PushL(paraFormat);
			CCharFormatLayer* charFormat = CCharFormatLayer::NewL();
			CleanupStack::PushL(charFormat);
			CRichText* richText = CRichText::NewL(paraFormat, charFormat);
			CleanupStack::PushL(richText);
			if (store->HasBodyTextL())
				store->RestoreBodyTextL(*richText);

			// Get the Entry Body Text.  Make it the same length as the test Body Text, as the 
			// entry body text will also include the name of the attachmnet if there is one
			HBufC* bodyText = HBufC::NewL(200);
			CleanupStack::PushL(bodyText);
			TPtr bodyTextPtr = bodyText->Des();	
			richText->Extract(bodyTextPtr, 0);
			bodyTextPtr.SetLength(iTestBodyText->Length());

			// Check the Text is Correct
			if (bodyTextPtr.Compare(*iTestBodyText) == 0)
				{
				_LIT(KCorrectBodyText, "Correct Body Text");
				LogCommentFormatL(KCorrectBodyText);
				}
			else
				{
				_LIT(KErrWrongBodyText, "Error, Incorrect Body Text");
				LogCommentFormatL(KErrWrongBodyText);
				err = KErrUnknown;
				}

			CleanupStack::PopAndDestroy(5);// store, paraFormat, charFormat, richText, bodyText
			}
		else
			{
			_LIT(KErrNoBodyText, "Error, Body Text Entry has No Store");
			LogCommentFormatL(KErrNoBodyText);
			err = KErrUnknown;
			}
		}

	CleanupStack::PopAndDestroy(); // children


	TRequestStatus* status = &aStatus;
	User::RequestComplete(status, err);
	*/
	}




//
//
// CCheckRemoteFolderSize
//
	
EXPORT_C CCheckRemoteFolderSize::CCheckRemoteFolderSize(TInt aCount, CMsvTestUtils& aTestUtils) 
: iTestUtils(aTestUtils),
  iTestCount(aCount)
	{
	}

void CCheckRemoteFolderSize::LogCommentFormatL(TRefByValue<const TDesC> format,...)
	{
	// Build parameter list.
	VA_LIST list;
	VA_START(list, format);
	TBuf<0x100> buf;
	buf.FormatList(format, list);

	// Log the debug buffer.
	iTestUtils.WriteComment(buf);
	}

// TODO: Commented out for the MMS Test Harness as leads to compile errors.

EXPORT_C void CCheckRemoteFolderSize::StartL(TRequestStatus& /*aStatus*/)
	{
	/*TInt err=KErrNone;

	CMsvEntry& entry = *(iTestUtils.iMsvEntry);
	TMsvEmailEntry message=entry.Entry();
	TInt remotecount=message.RemoteFolderEntries();

	LogCommentFormatL(_L("[%4d] Remote folder contains %d entries when %d expected."), iDebugInfo.LineNumber(), remotecount, iTestCount);

	if (remotecount != iTestCount)
		{
		LogCommentFormatL(_L("Error!"));
		err = KErrUnknown;
		}

	TRequestStatus* status = &aStatus;
	User::RequestComplete(status, err);
	*/
	}


//
//
// CSelectPopulatedInSelection
//

EXPORT_C CSelectPopulatedInSelection::CSelectPopulatedInSelection(CMsvEntrySelection& aSelection, CMsvTestUtils& aTestUtils) 
: iTestUtils(aTestUtils),
  iSelection(aSelection)
	{
	}

// TODO: Commented out for the MMS Test Harness as leads to compile errors.

EXPORT_C void CSelectPopulatedInSelection::StartL(TRequestStatus& /*aStatus*/)
	{
	/*
	CMsvEntry& msvEntry = *(iTestUtils.iMsvEntry);

	TInt pos=0;
	while (pos<iSelection.Count())
		{
		msvEntry.SetEntryL(iSelection[pos]);
		TMsvEmailEntry entry(msvEntry.Entry());
		if (entry.BodyTextComplete())
			pos++;
		else
			iSelection.Delete(pos,1);
		}

	TRequestStatus* status = &aStatus;
	User::RequestComplete(status, KErrNone);
	*/
	}


//
//
// CSelectLastInSelection
//
	
EXPORT_C CSelectLastInSelection::CSelectLastInSelection(TInt aCount, CMsvEntrySelection& aSelection, CMsvTestUtils& aTestUtils) 
: iTestUtils(aTestUtils),
  iSelection(aSelection), 
  iTestCount(aCount)
	{
	}

EXPORT_C void CSelectLastInSelection::StartL(TRequestStatus& aStatus)
	{
	TInt selectionCount=iSelection.Count();
	iTestCount=Min(iTestCount,selectionCount);
	iSelection.Delete(iTestCount,iSelection.Count()-iTestCount);
	TRequestStatus* status = &aStatus;
	User::RequestComplete(status, KErrNone);
	}

//
//
// CSelectFirstInSelection
//
	
EXPORT_C CSelectFirstInSelection::CSelectFirstInSelection(TInt aCount, CMsvEntrySelection& aSelection, CMsvTestUtils& aTestUtils) 
: iTestUtils(aTestUtils),
  iSelection(aSelection), 
  iTestCount(aCount)
	{
	}

EXPORT_C void CSelectFirstInSelection::StartL(TRequestStatus& aStatus)
	{
	TInt selectionCount=iSelection.Count();
	iTestCount=Min(iTestCount,selectionCount);
	iSelection.Delete(0,iSelection.Count()-iTestCount);
	TRequestStatus* status = &aStatus;
	User::RequestComplete(status, KErrNone);
	}

//
//
// CMsvTestEntry
//

EXPORT_C CMsvTestEntry::CMsvTestEntry(CMsvClientTest& aParentTestHarness) : iParentTestHarness(aParentTestHarness)
	{
	}

EXPORT_C void CMsvTestEntry::StartL(TRequestStatus& aStatus)
	{
	TInt err = KErrNone;
	TBool match = ETrue;

	// Get access to the CMsvEntry owned by the test utils of the parent test harness
	CMsvEntry* cMsvEntry = iParentTestHarness.MsvTestUtils().iMsvEntry;
	
	// Check that there is only one entry currently selected
	if (iParentTestHarness.iCurrentSelection->Count() != 1)
		{
		match = EFalse;
		iParentTestHarness.LogCommentL(_L("Incorrect number of entries selected"));
		}

	// Set the CMsvEntry to point to the currently selected entry
	// We have already checked that there is exactly one selected entry so the index is safe
	cMsvEntry->SetEntryL((*iParentTestHarness.iCurrentSelection)[0]);

	// Get the TMsvEntry details that we are going to test
	TMsvEntry entry = cMsvEntry->Entry();

	if ((iTestVisible) && match)
		// Test the Visible() flag
		{
		match = (iVisibleValue == entry.Visible());
		if (!match)
			{
			iParentTestHarness.LogCommentL(_L("Visible() not as expected"));
			}
		}

	if ((iTestComplete) && match)
		// Test the Complete() flag
		{
		match = (iCompleteValue == entry.Complete());
		if (!match)
			{
			iParentTestHarness.LogCommentL(_L("Complete() not as expected"));
			}
		}

	if (!match)
		// If the entry did not match then log it and complete with an error
		{
		iParentTestHarness.LogCommentL(_L("Entry not as expected"));
		err = KErrUnknown;
		}

	TRequestStatus* status = &aStatus;
	User::RequestComplete(status, err);
	}

EXPORT_C void CMsvTestEntry::TestVisible(TBool aVisibleValue)
	{
	iTestVisible = ETrue; // Specify that the visible flag will be tested
	iVisibleValue = aVisibleValue; // Set the expected value
	}

EXPORT_C void CMsvTestEntry::TestComplete(TBool aCompleteValue)
	{
	iTestComplete = ETrue; // Specify that the complete flag will be tested
	iCompleteValue = aCompleteValue; // Set the expected value
	}


//
//
// CSelectEntryById
//

EXPORT_C CSelectEntryById::CSelectEntryById(TMsvId aId, TMsvId& aEntryId, CMsvTestUtils& aTestUtils) 
: iTestUtils(aTestUtils),
  iIdToSelect(aId), 
  iEntryId(aEntryId)
	{
	}

EXPORT_C void CSelectEntryById::StartL(TRequestStatus& aStatus)
	{
	iTestUtils.iMsvEntry->SetEntryL(iIdToSelect);
	iEntryId = iIdToSelect;
	TRequestStatus* status = &aStatus;
	User::RequestComplete(status, KErrNone);
	}




//
//
// CSelectAllMessages
//

EXPORT_C CSelectAllMessages::CSelectAllMessages(TMsvId& aParentFolder, CMsvEntrySelection& aSelection, CMsvTestUtils& aTestUtils) 
: iTestUtils(aTestUtils),
  iParentFolder(aParentFolder), 
  iSelection(aSelection)
	{
	}

EXPORT_C void CSelectAllMessages::StartL(TRequestStatus& aStatus)
	{
	iSelection.Reset();
	iTestUtils.iMsvEntry->SetEntryL(iParentFolder);

	CMsvEntrySelection* tempSelection = iTestUtils.iMsvEntry->ChildrenL();
	CleanupStack::PushL(tempSelection);
	// Copy the child entries to the given selection
	TInt index = tempSelection->Count();
	while (index--)
		{
		iSelection.AppendL((*tempSelection)[index]);
		}
	CleanupStack::PopAndDestroy(tempSelection);
	
	TRequestStatus* status = &aStatus;
	User::RequestComplete(status, KErrNone);
	}





//
//
// CLongWait
//
EXPORT_C CLongWait::CLongWait() 
: CActive(EPriorityNormal),
  iWaitTime(KDefaultWaitTime)
	{
	CActiveScheduler::Add(this);
	}

EXPORT_C CLongWait::CLongWait(TInt aWaitTime) 
: CActive(EPriorityNormal)
	{
	iWaitTime = aWaitTime * 1000000;
	CActiveScheduler::Add(this);
	}

EXPORT_C CLongWait::~CLongWait()
	{
	delete iTimer;
	}

EXPORT_C void CLongWait::StartL(TRequestStatus& aStatus)
	{
	delete iTimer;
	iTimer = 0;
	iTimer = CTestTimer::NewL();
	iReportStatus = &aStatus;
	iTimer->AfterReq(iWaitTime, iStatus);
	aStatus = KRequestPending;
	SetActive();
	}



EXPORT_C void CLongWait::RunL()
	{
	User::RequestComplete(iReportStatus, iStatus.Int());
	}

EXPORT_C void CLongWait::DoCancel()
	{
	}




//
//
// CCopySelection
//

EXPORT_C CCopySelection::CCopySelection(CMsvClientTest& aParentTestHarness) : CActive(EPriorityNormal), iParentTestHarness(aParentTestHarness)
	{
	CActiveScheduler::Add(this);
	}

EXPORT_C void CCopySelection::StartL(TRequestStatus& aStatus)
	{
	iParentTestHarness.MsvTestUtils().iMsvEntry->SetEntryL(iParentTestHarness.iSelectedFolder);
	iReportStatus = &aStatus;
	iParentTestHarness.SetCurrentOperation(iParentTestHarness.MsvTestUtils().iMsvEntry->CopyL(*(iParentTestHarness.iCurrentSelection), iParentTestHarness.iDestinationFolder, iStatus));
	aStatus = KRequestPending;
	SetActive();
	}

EXPORT_C void CCopySelection::RunL()
	{
	User::RequestComplete(iReportStatus, iStatus.Int());
	}

EXPORT_C void CCopySelection::DoCancel()
	{
	iParentTestHarness.CurrentOperation().Cancel();
	}


EXPORT_C CCopySelection::~CCopySelection()
	{
	}



//
//
// CMoveSelection
//

EXPORT_C CMoveSelection::CMoveSelection(CMsvClientTest& aParentTestHarness) : CActive(EPriorityNormal), iParentTestHarness(aParentTestHarness)
	{
	CActiveScheduler::Add(this);
	}

EXPORT_C void CMoveSelection::StartL(TRequestStatus& aStatus)
	{
	iParentTestHarness.MsvTestUtils().iMsvEntry->SetEntryL(iParentTestHarness.iSelectedFolder);
	iReportStatus = &aStatus;
	iParentTestHarness.SetCurrentOperation(iParentTestHarness.MsvTestUtils().iMsvEntry->MoveL(*(iParentTestHarness.iCurrentSelection), iParentTestHarness.iDestinationFolder, iStatus));
	aStatus = KRequestPending;
	SetActive();
	}
EXPORT_C void CMoveSelection::RunL()
	{
	User::RequestComplete(iReportStatus, iStatus.Int());
	}

EXPORT_C void CMoveSelection::DoCancel()
	{
	iParentTestHarness.CurrentOperation().Cancel();
	}


EXPORT_C CMoveSelection::~CMoveSelection()
	{
	}


//
//
// CDeleteSelection
//

EXPORT_C CDeleteSelection::CDeleteSelection(CMsvClientTest& aParentTestHarness) : CActive(EPriorityNormal), iParentTestHarness(aParentTestHarness)
	{
	CActiveScheduler::Add(this);
	}

EXPORT_C void CDeleteSelection::StartL(TRequestStatus& aStatus)
	{
	if (iParentTestHarness.iCurrentSelection->Count() == 0)
		// If there are no entries currently selected then complete without an error
		{
		TRequestStatus* status = &aStatus;
		User::RequestComplete(status, KErrNone);
		}
	else
		// If there are some selected messages then delete them.
		{
		CMsvEntry& sharedCMsvEntry = *(iParentTestHarness.MsvTestUtils().iMsvEntry);

		// Set the CMsvEntry to point to the folder selected by the parent test harness
		sharedCMsvEntry.SetEntryL(iParentTestHarness.iSelectedFolder);

		iReportStatus = &aStatus;

		// Start the delete operation
		CMsvOperation* deleteOperaiton= sharedCMsvEntry.DeleteL(*(iParentTestHarness.iCurrentSelection), iStatus);

		// Set the current delete operation to be the current operation on the parent test harness
		iParentTestHarness.SetCurrentOperation(deleteOperaiton);
		aStatus = KRequestPending;
		SetActive();
		}
	}

EXPORT_C void CDeleteSelection::RunL()
	{
	User::RequestComplete(iReportStatus, iStatus.Int());
	}

EXPORT_C void CDeleteSelection::DoCancel()
	{
	iParentTestHarness.CurrentOperation().Cancel();
	}


EXPORT_C CDeleteSelection::~CDeleteSelection()
	{
	}

