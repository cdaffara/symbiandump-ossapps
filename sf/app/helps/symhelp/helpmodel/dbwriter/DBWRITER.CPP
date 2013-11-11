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

#include "DBWRITER.H"
#include <fbs.h>

// User includes
#include "HLPMODEL.H"
#include "hlppanic.h"




enum TDbWriterPanic
	{
	EDbWriterNoRowAtCursor,
	EDbWriterInvalidColumnType,
	EDbWriterStoreNotOpen
	};


GLDEF_C void Panic(TDbWriterPanic aReason)
	{
	_LIT(KHlpPanicText, "DbWriter");
	User::Panic(KHlpPanicText, aReason);
	}



//
// Create the database
//

CHlpRowSet::CHlpRowSet(RDbView* aView)
		: iView(aView)
	{
	}

EXPORT_C CHlpRowSet::~CHlpRowSet()
	{
	iView->Close();
	delete iView;
	}

EXPORT_C TBool CHlpRowSet::AtRow()
	{
	return iView->AtRow();
	}

EXPORT_C TBool CHlpRowSet::AtBeginning()
	{
	return iView->AtBeginning();
	}

EXPORT_C TBool CHlpRowSet::AtEnd()
	{
	return iView->AtEnd();
	}

EXPORT_C TBool CHlpRowSet::FirstL()
	{
	return iView->FirstL();
	}

EXPORT_C TBool CHlpRowSet::LastL()
	{
	return iView->LastL();
	}

EXPORT_C TInt CHlpRowSet::CountL()
	{
	return iView->CountL();
	}

EXPORT_C void CHlpRowSet::NextL()
	{
	iView->NextL();
	}

EXPORT_C void CHlpRowSet::PreviousL()
	{
	// coverity [unchecked_value]
	iView->PreviousL();
	}

EXPORT_C RDbView* CHlpRowSet::View()
	{
	return iView;
	}


// This method could leave while getting column set for the rowset.
// @leave KErrNoMemory not enough memory to carry out the operation

EXPORT_C TDbColNo CHlpRowSet::MapColNameToColNo(const TDesC& aName)
	{
	CDbColSet* columnSet = iView->ColSetL();
	CleanupStack::PushL(columnSet);
	TInt columnNumber = columnSet->ColNo(aName);
	CleanupStack::PopAndDestroy(columnSet);
	return columnNumber;
	}


// This method could leave while getting column set for the rowset.
// @leave KErrNoMemory not enough memory to carry out the operation

EXPORT_C TDbColNo CHlpRowSet::MapColNameToColNo(const TDbCol& aCol)
	{
	CDbColSet* columnSet = iView->ColSetL();
	CleanupStack::PushL(columnSet);
	TInt columnNumber = columnSet->ColNo(aCol.iName);
	CleanupStack::PopAndDestroy(columnSet);
	return columnNumber;
	}














//
// CHlpView class - Read only access to database views
//

CHlpView::CHlpView(RDbView* aView)
:	CHlpRowSet(aView)
	{
	}

CHlpView::~CHlpView()
	{
	View()->Close();
	}

void CHlpView::ConstructL()
	{
	}

EXPORT_C CHlpView* CHlpView::NewLC(RDbView* aView)
	{
	CHlpView* self = new(ELeave)CHlpView(aView);
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
	}

EXPORT_C CHlpView* CHlpView::NewL(RDbView* aView)
	{
	CHlpView* self = CHlpView::NewLC(aView);
	CleanupStack::Pop();
	return self;
	}

EXPORT_C void CHlpView::GetL()
	{
	View()->GetL();
	}

EXPORT_C void CHlpView::GetTextL(TDbColNo aCol, TDes& aText)
	{
	__ASSERT_ALWAYS(View()->AtRow(), Panic(EDbWriterNoRowAtCursor));
	aText = View()->ColDes(aCol);
	}

EXPORT_C void CHlpView::GetRichTextL(TDbColNo aTextCol, TDbColNo aMarkupCol, CRichText& aRichText)
	{
	__ASSERT_ALWAYS(View()->AtRow(), Panic(EDbWriterNoRowAtCursor));
	TPtrC ptr=View()->ColDes(aTextCol);
	aRichText.InsertL(0, ptr);
	GetMarkupL(aRichText, aMarkupCol);
	}

EXPORT_C void CHlpView::GetValL(TDbColNo aCol, TUint32& aValue)
	{
	__ASSERT_ALWAYS(View()->AtRow(), Panic(EDbWriterNoRowAtCursor));
	aValue=View()->ColUint16(aCol);
	}

void CHlpView::GetMarkupL(CRichText& aRichText, TDbColNo aMarkupCol)
	{
	__ASSERT_ALWAYS(View()->ColDef(aMarkupCol).iType == EDbColLongBinary, Panic(EDbWriterInvalidColumnType));

	RDbColReadStream blob;
	blob.OpenLC(*View(), aMarkupCol);

	CEmbeddedStore* embeddedStore = CEmbeddedStore::FromLC(blob);
	RStoreReadStream markupStream;
	markupStream.OpenLC(*embeddedStore, embeddedStore->Root());
	aRichText.InternalizeMarkupDataL(markupStream);
	iStoreResolver.iStore = embeddedStore;

	aRichText.SetPictureFactory(iPictureFactory, &iStoreResolver);
	aRichText.DetachFromStoreL(CPicture::EDetachFull);
	CleanupStack::PopAndDestroy(3);	// embeddedStore, markupStream, blob
	}











//
// CHlpTable class - write access to database views
//

CHlpTable::CHlpTable(RDbView* aView)
:	CHlpRowSet(aView)
	{
	}

CHlpTable::~CHlpTable()
	{
//	View()->Close();
	}

void CHlpTable::ConstructL()
	{
	}

EXPORT_C CHlpTable* CHlpTable::NewLC(RDbView* aView)
	{
	CHlpTable* self=new(ELeave)CHlpTable(aView);
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
	}

EXPORT_C CHlpTable* CHlpTable::NewL(RDbView* aView)
	{
	CHlpTable* self=CHlpTable::NewLC(aView);
	CleanupStack::Pop();
	return self;
	}

EXPORT_C void CHlpTable::InsertL()
	{
	View()->InsertL();
	}

EXPORT_C void CHlpTable::PutL()
	{
	View()->PutL();
	}

EXPORT_C void CHlpTable::SetColL(TDbColNo aCol, const TDesC& aText)
	{
	__ASSERT_ALWAYS(View()->AtRow(), Panic(EDbWriterNoRowAtCursor));
	View()->SetColL(aCol, aText);
	}

EXPORT_C void CHlpTable::SetColL(TDbColNo aCol, TUint32 aValue)
	{
	__ASSERT_ALWAYS(View()->AtRow(), Panic(EDbWriterNoRowAtCursor));
	View()->SetColL(aCol, aValue);
	}

EXPORT_C void CHlpTable::SetColL(TDbColNo aCol, const CFbsBitmap& aBitmap)
	{
	__ASSERT_ALWAYS(View()->AtRow(), Panic(EDbWriterNoRowAtCursor));

	RDbColWriteStream blob;
	blob.OpenLC(*View(), aCol);
	blob << aBitmap;
	blob.CommitL();
	CleanupStack::PopAndDestroy(); // blob
	}

EXPORT_C void CHlpTable::SetColL(TDbColNo aTextCol, TDbColNo aMarkupCol, CRichText& aRichText)
	{
	__ASSERT_ALWAYS(View()->AtRow(), Panic(EDbWriterNoRowAtCursor));
	HBufC* buf = HBufC::NewLC(aRichText.DocumentLength());
	TPtr ptr = buf->Des();
	aRichText.Extract(ptr);
	SetLongTextL(*buf, aTextCol);
	CleanupStack::PopAndDestroy();	// buf

	if (aRichText.HasMarkupData())
		SetMarkupL(aRichText, aMarkupCol);
	else
		View()->SetColNullL(aMarkupCol);
	}

EXPORT_C void CHlpTable::SetColL(TDbColNo aTextCol, CRichText& aRichText)
	{
	__ASSERT_ALWAYS(View()->AtRow(), Panic(EDbWriterNoRowAtCursor));
	HBufC* buf = HBufC::NewL(aRichText.DocumentLength());
	CleanupStack::PushL(buf);
	TPtr ptr = buf->Des();
	aRichText.Extract(ptr);
	View()->SetColL(aTextCol,ptr);
	CleanupStack::PopAndDestroy();	// buf
	}

void CHlpTable::SetLongTextL(const TDesC& aText, TDbColNo aLongTextCol)
	{
	RDbColWriteStream stream;
	stream.OpenLC(*View(), aLongTextCol);
	stream.WriteL(aText);
	stream.CommitL();
	CleanupStack::PopAndDestroy();	// stream
	}

void CHlpTable::SetMarkupL(CRichText& aRichText, TDbColNo aMarkupCol)
	{
	aRichText.DetachFromStoreL(CPicture::EDetachFull);
	RDbColWriteStream blob;
	blob.OpenL(*View(), aMarkupCol);
	CEmbeddedStore* embeddedStore=CEmbeddedStore::NewLC(blob); // takes ownership of blob
	CStoreMap* map=CStoreMap::NewLC(*embeddedStore);
	aRichText.StoreMarkupComponentsL(*embeddedStore, *map);

	RStoreWriteStream markup(*map);
	embeddedStore->SetRootL(markup.CreateLC(*embeddedStore));
	aRichText.ExternalizeMarkupDataL(markup);
	markup.CommitL();
	map->Reset();
	CleanupStack::PopAndDestroy(2); // map, markup
	embeddedStore->CommitL();
	CleanupStack::PopAndDestroy(); // embeddedStore
	}

const CStreamStore& THlpStoreResolver::StreamStoreL(TInt /*aPos*/) const
	{
	return *iStore;
	}















//
// CHlpDbWriter class
//

CHlpDbWriter::CHlpDbWriter(RFs& aFs)
:	iFs(aFs)
	{
	}

EXPORT_C CHlpDbWriter::~CHlpDbWriter()
	{
	iDatabase.Close();
	delete iTables;
	delete iDictionary;
	delete iStore;
	delete iUids;
	delete iImagesAlreadyStored;
}

void CHlpDbWriter::ConstructL()
	{
	iDictionary				= CStreamDictionary::NewL();
	iTables					= new(ELeave) CArrayPtrFlat<CHlpTable>(4);
	iUids					= new(ELeave) CArrayFixFlat<TUid>(2);
	iImagesAlreadyStored	= new(ELeave) CArrayFixFlat<TUid>(10);
	}

EXPORT_C CHlpDbWriter* CHlpDbWriter::NewL(RFs& aFs)
	{
	CHlpDbWriter* self = CHlpDbWriter::NewLC(aFs);
	CleanupStack::Pop(); //self
	return self;
	}

EXPORT_C CHlpDbWriter* CHlpDbWriter::NewLC(RFs& aFs)
	{
	CHlpDbWriter* self = new(ELeave) CHlpDbWriter(aFs);
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
	}

EXPORT_C void CHlpDbWriter::SetDatabaseUidL(TUid& aUid)
	{
	__ASSERT_DEBUG(iStore, Panic(EDbWriterStoreNotOpen));

	TUidType type(KPermanentFileStoreLayoutUid, KUidHlpApp, aUid);	
	iStore->SetTypeL(type);
	}

EXPORT_C void CHlpDbWriter::CreateFileL(const TDesC& aFileName)
	{
	CPermanentFileStore* store = NULL;
	
	if	(!BaflUtils::FileExists(iFs, aFileName))
		store = CPermanentFileStore::CreateL(iFs, aFileName, EFileRead|EFileWrite);
	else // Change this so that the program panics if the file exists.
		store = CPermanentFileStore::ReplaceL(iFs, aFileName, EFileRead|EFileWrite);

	delete iStore;
	iStore = store;
	iFileName = aFileName;
	}

EXPORT_C void CHlpDbWriter::AddBitmapL(TInt aBitmapId)
	{
	TKeyArrayFix key(_FOFF(TUid, iUid), ECmpTUint);
	iImagesAlreadyStored->InsertIsqL(TUid::Uid(aBitmapId), key);
	}

EXPORT_C TBool CHlpDbWriter::IsBitmapStored(TInt aBitmapId) const
	{
	TInt index = KErrNotFound;
	TKeyArrayFix key(_FOFF(TUid, iUid), ECmpTUint);
	return (!iImagesAlreadyStored->FindIsq(TUid::Uid(aBitmapId), key, index));
	}

EXPORT_C TInt CHlpDbWriter::BitmapCount() const
	{
	return iImagesAlreadyStored->Count();
	}

EXPORT_C TInt CHlpDbWriter::BitmapIdForIndex(TInt aIndex) const
	{
	return iImagesAlreadyStored->At(aIndex).iUid;
	}

EXPORT_C void CHlpDbWriter::CreateDatabaseL()
	{
	TStreamId dbStream=iDatabase.CreateL(iStore);
	iDictionary->AssignL(KUidHlpDbStream, dbStream);
	DoCreateDatabaseL();
	DoOpenTablesL();
	}

void CHlpDbWriter::DoCreateDatabaseL()
	{
	// create the table schemas
	User::LeaveIfError(iDatabase.Execute(KHlpDMLTopicTable));
	User::LeaveIfError(iDatabase.Execute(KHlpDMLIndexTable));
	User::LeaveIfError(iDatabase.Execute(KHlpDMLTopicIndexTable));
	User::LeaveIfError(iDatabase.Execute(KHlpDMLContextTable));
	User::LeaveIfError(iDatabase.Execute(KHlpDMLImageTable));

	// create the indices
	User::LeaveIfError(iDatabase.Execute(KHlpDMLTopicTableIdx));
	User::LeaveIfError(iDatabase.Execute(KHlpDMLIndexTableIdx));
	User::LeaveIfError(iDatabase.Execute(KHlpDMLTopicIndexTableIdx));
	User::LeaveIfError(iDatabase.Execute(KHlpDMLContextTableIdx));
	User::LeaveIfError(iDatabase.Execute(KHlpDMLImageTableIdx));
	}

EXPORT_C void CHlpDbWriter::CompressDatabaseL()
	{
	StoreUidsL();
	StoreStreamDictionaryL();

	iStore->CommitL();
	iDatabase.CompressL(*iStore, iDictionary->At(KUidHlpDbStream));
	iStore->CompactL();
	iStore->CommitL();
	}

void CHlpDbWriter::DoOpenTablesL()
	{
	iTables->AppendL(DoOpenTableLC(ETopicTable));
	CleanupStack::Pop(); // ret from DoOpenTableL
	iTables->AppendL(DoOpenTableLC(EIndexTable));
	CleanupStack::Pop(); // ret from DoOpenTableL
	iTables->AppendL(DoOpenTableLC(ETopicIndexTable));
	CleanupStack::Pop(); // ret from DoOpenTableL
	iTables->AppendL(DoOpenTableLC(EContextTable));
	CleanupStack::Pop(); // ret from DoOpenTableL
	iTables->AppendL(DoOpenTableLC(EImageTable));
	CleanupStack::Pop(); // ret from DoOpenTableL
	}

CHlpTable* CHlpDbWriter::DoOpenTableLC(TInt aTable)
	{
	TBuf<255> sql;

	switch(aTable)
		{
	case ETopicTable:
		sql=KHlpSQLTopicTable;
		break;
	case EIndexTable:
		sql=KHlpSQLIndexTable;
		break;
	case ETopicIndexTable:
		sql=KHlpSQLTopicIndexTable;
		break;
	case EContextTable:
		sql=KHlpSQLContextTable;
		break;
	case EImageTable:
		sql=KHlpSQLImageTable;
		break;
	default:
		User::Leave(KErrNotSupported);
		}

	RDbView* view = new(ELeave) RDbView;
	CleanupStack::PushL(view);
	User::LeaveIfError(view->Prepare(iDatabase, sql, RDbView::EUpdatable));
	User::LeaveIfError(view->EvaluateAll());
	CHlpTable* table = CHlpTable::NewL(view);
	CleanupStack::Pop(); // view
	CleanupStack::PushL(table);

	return table;
	}



void CHlpDbWriter::StoreUidsL()
	{
	RStoreWriteStream out;
	TStreamId uidStream = out.CreateLC(*iStore);
	iDictionary->AssignL(KUidHlpUidStream, uidStream);
	out.WriteInt32L(iUids->Count());
	if (iUids->Count() > 0)
		{
		for(TInt i=0; i < iUids->Count(); i++)
			{
			out.WriteInt32L((*iUids)[i].iUid);
			}
		}
	out.CommitL();
	out.Close();
	CleanupStack::PopAndDestroy(); // out
	}

void CHlpDbWriter::StoreStreamDictionaryL()
	{
	// Store the stream dictionary in the root
	RStoreWriteStream out;
	TStreamId root = out.CreateLC(*iStore);
	iStore->SetRootL(root);
	out << *iDictionary;
	out.CommitL();
	out.Close();
	CleanupStack::PopAndDestroy(); // out
	}

EXPORT_C void CHlpDbWriter::CloseFileL() // should this be make private and put in the destructor?
	{
	iDatabase.Close();
	iStore->CompactL();
	iStore->CommitL();
	delete iStore;
	iStore=NULL;
	iUids->Reset();
	iTables->ResetAndDestroy();
	iTables->Reset();
	
	// Have to do this because there is no reset procedure for a stream dictionary
	CStreamDictionary* newDictionary = CStreamDictionary::NewL();
	delete iDictionary;
	iDictionary = newDictionary;
	}

EXPORT_C void CHlpDbWriter::SetUidL(TUid aUid)
	{
	TKeyArrayFix key(_FOFF(TUid,iUid),ECmpTUint32);
	TInt indexNotReferenced = KErrNotFound;
	if	(iUids->Find(aUid,key, indexNotReferenced) != KErrNone)
		iUids->AppendL(aUid);
	}	

EXPORT_C void CHlpDbWriter::BeginTransaction()
	{
	iDatabase.Begin();
	}

EXPORT_C void CHlpDbWriter::CommitTransaction()
	{
	iDatabase.Commit();
	}

EXPORT_C CHlpTable* CHlpDbWriter::TopicTable()
	{
	return iTables->At(ETopicTable);
	}

EXPORT_C CHlpTable* CHlpDbWriter::IndexTable()
	{
	return iTables->At(EIndexTable);
	}

EXPORT_C CHlpTable* CHlpDbWriter::TopicIndexTable()
	{
	return iTables->At(ETopicIndexTable);
	}

EXPORT_C CHlpTable* CHlpDbWriter::ContextTable()
	{
	return iTables->At(EContextTable);
	}

EXPORT_C CHlpTable* CHlpDbWriter::ImageTable()
	{
	return iTables->At(EImageTable);
	}
