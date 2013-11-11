// Copyright (c) 1999-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include "HLPDB.H"

// System includes
#include <fbs.h>

// User includes
#include "HLPMODEL.H"
#include "hlppanic.h"
#include "Hlpsqlconsts.h"
#ifdef SYMBIAN_ENABLE_SPLIT_HEADERS
#include "hlpmodel_internal.h"
#endif

// Constants
const TInt KHlpModelNumberOfImageTableColumnsForV6Point0HelpFile = 2;
const TInt KHlpModelNumberOfImagesInV6Point0HelpFile = 1;


//
// ----> CHlpDatabase  
//

CHlpDatabase::CHlpDatabase(RFs& aFs)
:	iFs(aFs)
	{
	}

CHlpDatabase::~CHlpDatabase()
	{
	Close();
	iImageTable.Close();

	delete iView;
	delete iDatabase;
	delete iStore;
	delete iUids;
	}

void CHlpDatabase::ConstructL()
	{
	iView = new(ELeave) RDbView;
	iUids = new(ELeave) CArrayFixFlat<TUid>(2);
	iDatabase = new(ELeave) RDbStoreDatabase;
	}

CHlpDatabase* CHlpDatabase::NewLC(RFs& aFs)
	{
	CHlpDatabase* self = new(ELeave) CHlpDatabase(aFs);
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
	}

CHlpDatabase* CHlpDatabase::NewLC(RFs& aFs, const TDesC& aFileName)
	{
	CHlpDatabase* self = CHlpDatabase::NewLC(aFs);
	self->OpenL(aFileName);
	return self;
	}

//
//
//

void CHlpDatabase::OpenL(const TDesC& aFileName)
	{
	iFileName = aFileName;
	iStore = CPermanentFileStore::OpenL(iFs, aFileName, EFileRead);

	// Restore the dictionary
	CStreamDictionary* dictionary = CStreamDictionary::NewLC();
	RStoreReadStream in;
	in.OpenLC(*iStore, iStore->Root());
	dictionary->InternalizeL(in);
	CleanupStack::PopAndDestroy(); // in

	// Restore the database
	iDatabase->OpenL(iStore, dictionary->At(KUidHlpDbStream));

	// Open image table
	_LIT(KImageTable, "IMAGE");
	_LIT(KImageIndex, "IMAGEIDX");
	User::LeaveIfError(iImageTable.Open(*iDatabase, KImageTable, RDbRowSet::EReadOnly));
	User::LeaveIfError(iImageTable.SetIndex(KImageIndex));

	// Fetch the image row
	CDbColSet* columnSet = iImageTable.ColSetL();

	// We can identify v6 help files by the number of columns in the image table.
	// Since v6 help files only have one image for all zoom sizes, then we 
	// can identify this via the number of columns being 2.
	iOldHelpFileFormat = (columnSet->Count() == KHlpModelNumberOfImageTableColumnsForV6Point0HelpFile);
	delete columnSet;

	// Restore the Uid list
	TStreamId uidStream = dictionary->At(KUidHlpUidStream);
	in.OpenLC(*iStore, uidStream);

	TUid uid;
	TKeyArrayFix key(_FOFF(TUid, iUid), ECmpTUint32);
	const TInt count = in.ReadInt32L();
	for (TInt i=0; i < count; i++)
		{
		uid.iUid = in.ReadInt32L();
#ifdef _DEBUG
		iUids->InsertIsqL(uid, key);
#else
		iUids->InsertIsqAllowDuplicatesL(uid, key);
#endif
		}
	CleanupStack::PopAndDestroy(2); // in, dictionary

	TEntry entry;
	User::LeaveIfError(iFs.Entry(aFileName, entry));
	iHelpFileUid = entry.MostDerivedUid();
	}

void CHlpDatabase::Close()
	{
	if	(iView)
		iView->Close();
	if	(iDatabase)
		iDatabase->Close();
	}

//
//
//

void CHlpDatabase::AppendCategoryListL(CDesCArray& aList)
	{
	_LIT(KCategoryTable, "TOPIC");

	// Open the topic table so that we can use it to build the category list
	RDbTable table;
	User::LeaveIfError(table.Open(*iDatabase, KCategoryTable, RDbRowSet::EReadOnly));
	CleanupClosePushL(table);

	// Now build the list
	if (table.FirstL())
		{
		// Get the column of the category
		CDbColSet* columnSet = table.ColSetL();
		TDbColNo categoryColumn = columnSet->ColNo(KSQLCategoryColumn);
		delete columnSet;

		TInt posNotReferenced = KErrNotFound;
		TBuf<KMaxTitleColumn> title;
		do
			{
			// Add the stuff
			table.GetL();
			title = table.ColDes(categoryColumn);
			if	(aList.Find(title, posNotReferenced)) // Returns something other than 0 if not found
				aList.AppendL(title); // Not found, so append
			}
		while (table.NextL());
		}

	// Finally, close the table
	CleanupStack::PopAndDestroy(&table);
	}

TInt CHlpDatabase::TopicIdSearchL(const TUid aCategoryUid, const TUint aTopicId)
	{
	const TInt KNumLength = 20;
	const TInt KMaxParamLength = (2*KNumLength) + 20; // 20 for numbers, + 20 for ' = ' and ' AND ' etc.
	TBuf<KNumLength> num;

	HBufC* sqlStatement = HBufC::NewLC(KSQLTopicSearchProlog().Length() + KMaxParamLength);
	TPtr pSql(sqlStatement->Des());
	pSql.Append(KSQLTopicSearchProlog);

	pSql.Append(KSQLTopicIdColumn);
	pSql.Append(KSQLEqualOperator);
	num.Num(aTopicId);
	pSql.Append(num);

	pSql.Append(KSQLAndOperator);
	pSql.Append(KSQLCategoryUidColumn);
	pSql.Append(KSQLEqualOperator);
	num.Num(STATIC_CAST(TUint, aCategoryUid.iUid));
	pSql.Append(num);

	iView->Close();
	User::LeaveIfError(iView->Prepare(Database(), TDbQuery(pSql, EDbCompareFolded), TDbWindow::EUnlimited, RDbRowSet::EReadOnly));
	User::LeaveIfError(iView->EvaluateAll());
	CleanupStack::PopAndDestroy(); // sqlStatement
	if	(iView->CountL())
		{
		iView->FirstL();
		iView->GetL();
		return ETopicAvailable; // View is prepared
		}

	return ETopicNotFound;
	}

TInt CHlpDatabase::ContextSearchL(const TDesC& aContext)
	{
	_LIT(KContextSearch,	"SELECT CONTEXT, TOPICID FROM CONTEXT");
	_LIT(KSpecificContext,	" WHERE CONTEXT='%S'");

	TInt specificContextLength = aContext.Length();
	HBufC* sqlStatement = HBufC::NewLC(KContextSearch().Length() + KSpecificContext().Length() + specificContextLength);
	TPtr pStatement = sqlStatement->Des();
	pStatement.Append(KContextSearch);
	if	(specificContextLength)
		{
		HBufC* specificContext = HBufC::NewLC(KSpecificContext().Length() + specificContextLength);
		TPtr pContext = specificContext->Des();
		pContext.Format(KSpecificContext, &aContext);
		pStatement.Append(pContext);
		CleanupStack::PopAndDestroy(specificContext);
		}

	iView->Close();
	User::LeaveIfError(iView->Prepare(Database(), TDbQuery(pStatement, EDbCompareFolded), TDbWindow::EUnlimited, RDbRowSet::EReadOnly));
	User::LeaveIfError(iView->EvaluateAll());
	CleanupStack::PopAndDestroy(sqlStatement);

	if	(iView->CountL())
		{
		// Get the topic id from the context table
		CDbColSet* columnSet = iView->ColSetL();
		TDbColNo topicIdCol = columnSet->ColNo(KSQLTopicIdColumn);
		delete columnSet;

		// Get the topic id as a descriptor
		iView->FirstL();
		iView->GetL();
		TUint topicId = iView->ColUint(topicIdCol);
		
		_LIT(KTopicSearch, "SELECT TOPICTITLE, TOPICID, CATEGORY, TOPICTEXT, TOPICMARKUP FROM TOPIC WHERE TOPICID=%d");
		sqlStatement = HBufC::NewLC(KTopicSearch().Length() + 16); // 16 for the topic id
		pStatement.Set(sqlStatement->Des());
		pStatement.Format(KTopicSearch, topicId);

		// Prepare the topic table with the correct record...
		iView->Close(); // Finished with this view now
		User::LeaveIfError(iView->Prepare(Database(), TDbQuery(pStatement, EDbCompareFolded), TDbWindow::EUnlimited, RDbRowSet::EReadOnly));
		User::LeaveIfError(iView->EvaluateAll());
		CleanupStack::PopAndDestroy(); // sqlStatement

		return ETopicAvailable;
		}

	return ETopicNotFound;
	}

/** This method is called within the CHlpPicture::RestoreL() method and 
is responsible for finding the right bitmap to be retrieved and for reading 
in a pointer to that bitmap.

@param aImageId The ID of the bitmap to be retrieved.
@param aZoomState The current zoom state.
@return A pointer to a CFbsBitmap object.
@pre A valid zoom state is passed to the function.
@post A pointer to the referenced bitmap is returned. */
CFbsBitmap* CHlpDatabase::ImageForIdLC(TInt aImageId, THlpZoomState aZoomState)
{
	if	(!iImageTable.FirstL())
		User::Leave(KErrNotFound);

	if	(!iImageTable.SeekL(aImageId))
		User::Leave(KErrNotFound);

	// Fetch the image row
	iImageTable.GetL();

	// Work out which column the image is in
	CDbColSet* columnSet = iImageTable.ColSetL();
	CleanupStack::PushL(columnSet);

	TDbColNo imageColumn = KDbNullColNo;
	if	(iOldHelpFileFormat)
		{
		// OLD HELP FILE FORMAT
		// Retrieve bitmap from bitmap column in image table
		// and store it in the 'imageColumn variable'
		imageColumn = columnSet->ColNo(KSQLImageColumn);
		}
	else
		{
		// NEW HELP FILE FORMAT
		// Depending on whether the number of bitmaps is 1 or 3,
		// the bitmap from the appropriate column is retrieved 
		// and stored in the 'imageColumn' variable

		// Stores the number of bitmaps per image table row.
		const TInt bitmapCount = ImageCountForIdL(aImageId);
		__ASSERT_DEBUG(bitmapCount <= KHlpModelMaximumNumberOfImagesForV6Point2Files, User::Invariant());

		if	(bitmapCount == KHlpModelDefaultNumberOfImagesForV6Point2Files)
			imageColumn = columnSet->ColNo(KSQLImage0Column);
		else if (bitmapCount == KHlpModelMaximumNumberOfImagesForV6Point2Files)
			{
			switch(aZoomState) 
				{
				case EHlpZoomStateSmall:
					imageColumn = columnSet->ColNo(KSQLImage0Column);
					break;
				default:
				case EHlpZoomStateMedium:
					imageColumn = columnSet->ColNo(KSQLImage1Column);
					break;
				case EHlpZoomStateLarge:
					imageColumn = columnSet->ColNo(KSQLImage2Column);
					break;
				}
			}
		}
	CleanupStack::PopAndDestroy(columnSet);

	// Load the image from the blob
	CFbsBitmap* bitmap = new(ELeave) CFbsBitmap();
	CleanupStack::PushL(bitmap);

	// Read the column from the blob
	RDbColReadStream blob;
	blob.OpenLC(iImageTable, imageColumn);
	blob >> *bitmap;
	CleanupStack::PopAndDestroy(); // blob
	return bitmap;
	}

/** This method is called within the CHlpDatabase::ImageForIdLC() 
method and is responsible for returning the number of bitmaps stored 
in the image-table row, as referenced by the aImageId argument.

@param aImageId The ID of the bitmap, used for retrieving image-table 
column information.
@return An integer value that reflects the number of bitmaps within a 
particular image-table row.
@pre None.
@post The integer number at the image-table row referenced by 
aImageId is returned.
@see RDbTable::GetL()
@see RDbTable::ColSetL() */
TInt CHlpDatabase::ImageCountForIdL(TInt aImageId)
	{
	if	(!iImageTable.FirstL())
		User::Leave(KErrNotFound);

	if	(!iImageTable.SeekL(aImageId))
		User::Leave(KErrNotFound);

	// Fetch the image row
	iImageTable.GetL();

	TInt count = KErrNotFound;
	if	(iOldHelpFileFormat)
		count = KHlpModelNumberOfImagesInV6Point0HelpFile;
	else
		{
		// Work out which column the image count is in, 
		// and assign it to 'imageCountColumn'.
		CDbColSet* columnSet = iImageTable.ColSetL();
		TDbColNo imageCountColumn = columnSet->ColNo(KSQLImageCountColumn);
		delete columnSet;
		//
		count = iImageTable.ColUint(imageCountColumn);
		}
	return count;
	}

//
//
//

TBool CHlpDatabase::MatchUidL(TUid aUid)
	{
	__ASSERT_ALWAYS(iUids, Panic(EHlpFault));
	TInt index = KErrNotFound;
	TKeyArrayFix key(_FOFF(TUid, iUid), ECmpTUint32);
	// For some bizare reason, findisq returns 0 when the item was found.
	return (iUids->FindIsq(aUid, key, index) == KErrNone);
	}
