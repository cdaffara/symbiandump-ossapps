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

#include "HLPMODEL.H"

// System includes
#include <coehelp.h>
#include <txtrich.h>
#include <bautils.h>

// User includes
#include "hlppict.h"
#include "HLPSRCH.H"
#include "hlppanic.h"
#include "Hlpsqlconsts.h"
#ifdef SYMBIAN_ENABLE_SPLIT_HEADERS
#include "hlpmodel_internal.h"
#endif
// For searching for help files
_LIT(KHlpFileSpec,			"*.h*");
_LIT(KDefaultHelpExtension, ".hlp");

_LIT(KHlpFileSearchPath,	"\\Resource\\Help\\");

#ifdef __SHOW_LOADING_INFO__
_LIT(KFormat1, "\n%S %d: %c:%S%S");
_LIT(KFormat2, "\n%S %d: %c:%S%S");
_LIT(KCompareNewListWithMasterList, "compare master list");
_LIT(KDeleteEntry, "deleting");
_LIT(KGetNearestLanguageFile, "getting nearest languuage file for");
_LIT(KAddNewEntryToMasterList, "adding to master list");
_LIT(KLoadMasterList, "Loading");
_LIT(KFoundEntry, "Found entry during dir scan");
_LIT(KDuplicateEntry, "entry discarded as duplicate");
_LIT(KAddNewEntry, "entry added to list");
#endif


// Typedefs
typedef CArrayPtrFlat<CHlpFileEntry> CHlpFileList;

// Constants
const TInt KHlpModelNumericValueSize = 32;

// Average number of pictures per Help Topic
const TInt KAverageNumberOfPicturesInHelpTopic = 3;

#define UNUSED_VAR(a) a = a
//
// ----> CHlpFileEntry (header)
//

class CHlpFileEntry : public CBase
	{

public:
	static CHlpFileEntry* NewLC(TDriveUnit aDrive, const TDesC& aFile);

	inline TDriveUnit Drive() const { return iDrive; }
	inline const TDesC& FileName() const { return iFile; }
	inline const TDesC& Name() const { return iName; }
	void GetFullNameAndPath(TDes& aName) const;

private:
	CHlpFileEntry(TDriveUnit aDrive, const TDesC& aFile);

private:
	TName iName;
	TName iFile;
	TDriveUnit iDrive;
	};



//
// ----> Static utility function (source)
//

//#define __SHOW_LOADING_INFO__

#ifdef __SHOW_LOADING_INFO__
static void PrintEntryL(const TDesC& aPrompt, const CHlpFileEntry& aEntry, TInt aNumber = -1)
	{
	TFileName pFileName(aEntry.FileName());
	TChar driveLetter = '?';
	RFs::DriveToChar(aEntry.Drive(), driveLetter);

	HBufC* buf = HBufC::NewLC(aPrompt.Length() + pFileName.Length() + KHlpFileSearchPath().Length() + 40);
	TDes pBuf(buf->Des());


	if	(aNumber >= KErrNone)
		pBuf.Format(KFormat1, &aPrompt, aNumber, driveLetter, &KHlpFileSearchPath(), &pFileName);
	else
		pBuf.Format(KFormat2, &aPrompt, driveLetter, &KHlpFileSearchPath(), &pFileName);

	RDebug::Print(pBuf);

	CleanupStack::PopAndDestroy();
	}
#define __PRINT_FILE(aPrompt, aEntry)				(PrintEntryL(aPrompt, aEntry))
#define __PRINT_FILE_NO(aPrompt, aEntry, aNumber)	(PrintEntryL(aPrompt, aEntry, aNumber))
#else
#define __PRINT_FILE(aPrompt, aEntry)
#define __PRINT_FILE_NO(aPrompt, aEntry, aNumber)
#endif




//
// ----> CHlpFileEntry (source)
//
CHlpFileEntry::CHlpFileEntry(TDriveUnit aDrive, const TDesC& aFile)
:iFile(aFile)
,iDrive(aDrive)
	{
	TParsePtrC parser(iFile);
	iName.Copy(parser.Name());
	}

CHlpFileEntry* CHlpFileEntry::NewLC(TDriveUnit aDrive, const TDesC& aFile)
	{
	CHlpFileEntry* self = new(ELeave) CHlpFileEntry(aDrive, aFile);
	CleanupStack::PushL(self);
	return self;
	}


//
//
//
void CHlpFileEntry::GetFullNameAndPath(TDes& aName) const
	{
	TChar driveLetter = '?';
	RFs::DriveToChar(Drive(), driveLetter);
	aName.Zero();
	aName.Append(driveLetter);
	aName.Append(':');
	aName.Append(KHlpFileSearchPath);
	aName.Append(FileName());
	}


//
// ----> CHlpModel
//

CHlpModel::CHlpModel(RFs& aFs, MHlpModelObserver& aObserver)
:	iFsSession(aFs), iObserver(&aObserver)
	{
	}

EXPORT_C CHlpModel::~CHlpModel()
/** Destructor. */
	{
	if	(iDatabases)
		iDatabases->ResetAndDestroy();

	NotifyHelpModelDestructionToPictures();
	if	(iPictures)
		iPictures->Reset();

	delete iDatabases;
	delete iPictures;
	delete iSearch;
	delete iCriterion;
	delete iZoomFactors;
	}

void CHlpModel::NotifyHelpModelDestructionToPictures()
	{
	if	(!iPictures)
		return;
	const TInt count = iPictures->Count();
	for (TInt i=0; i<count; i++)
		{
		CHlpPicture& picture = *iPictures->At(i);
		picture.HandleHelpModelDestruction();
		}
	}


void CHlpModel::ConstructL()
	{
	iDatabases = new(ELeave) CHlpDatabases(2);
	iPictures = new(ELeave) CArrayPtrFlat<CHlpPicture>(KAverageNumberOfPicturesInHelpTopic);
	iSearch = CHlpSQLSearch::NewL(*this);

	// There must always be 3 default zoom ratios present in the array...
	iZoomFactors = new(ELeave) CArrayFixFlat<TInt>(3);
	iZoomFactors->AppendL(KHlpModelZoomFactorSmall);
	iZoomFactors->AppendL(KHlpModelZoomFactorMedium);
	iZoomFactors->AppendL(KHlpModelZoomFactorLarge);
	}

EXPORT_C CHlpModel* CHlpModel::NewL(RFs& aFs, MHlpModelObserver* aObserver)
/** Allocates and creates a help model object.

@param aFs Open file server handle
@param aObserver Client callback interface to handle messages from the help
model
@return New help model object */
	{
	CHlpModel* self = CHlpModel::NewLC(aFs, aObserver);
	CleanupStack::Pop();
	return self;
	}

EXPORT_C CHlpModel* CHlpModel::NewLC(RFs& aFs, MHlpModelObserver* aObserver)
/** Allocates and creates a help model object, leaving the object on the cleanup
stack.

@param aFs Open file server handle
@param aObserver Client callback interface to handle messages from the help
model
@return New help model object */
	{
	CHlpModel* self = new(ELeave) CHlpModel(aFs, *aObserver);
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
	}


//
//
//

EXPORT_C void CHlpModel::OpenL()
/** Opens all the help files in \\Resource\\Help. */
	{
	CHlpFileList* masterList = new(ELeave) CHlpFileList(5);
	CleanupStack::PushL(TCleanupItem(ResetAndDestroyArrayOfCHlpFileEntry,masterList));

	TInt i = EDriveY;
	FOREVER
		{
		// Make sure we go from Y -> A, then do Z last
		if	(i < EDriveA)
			i = EDriveZ;

		if	(DiskPresent(i))
			{
			// This generates a list for a specific directory on the specified drive.
			CHlpFileList* list = BuildListForDriveLC(TDriveUnit(i), iFsSession);

			TInt newListCount = list->Count();
			if(newListCount)
				{
				// compare the new list with our master list to check for duplicates
				TInt masterCount = masterList->Count();
				for(TInt j=masterCount-1; j>=0; j--)
					{
					CHlpFileEntry* entry = masterList->At(j);
					__PRINT_FILE_NO(KCompareNewListWithMasterList, *entry, j);

					newListCount = list->Count();
					for(TInt k=newListCount-1; k>=0; k--)
						{
						CHlpFileEntry* newEntry = list->At(k);

						if	(entry->FileName().CompareF(newEntry->FileName()) == 0)
							{
							// Names same, so nearest language file already in master list
							__PRINT_FILE_NO(KDeleteEntry, *newEntry, k);
							delete newEntry;
							list->Delete(k);
							}
						}
					}
				}

			// At this point, anything that is left in the new list should
			// have it's nearest language file added to the master list...
			newListCount = list->Count();
			for(TInt k=newListCount-1; k>=0; k--)
				{
				//get nearest language file
				__PRINT_FILE_NO(KGetNearestLanguageFile, *list->At(k), k);

				TChar driveLetter = '?';
				RFs::DriveToChar(list->At(k)->Drive(), driveLetter);

				TFileName fileName;
				fileName.Append(driveLetter);
				fileName.Append(':');
				fileName.Append(KHlpFileSearchPath);
				fileName.Append(list->At(k)->Name());
				fileName.Append(KDefaultHelpExtension);

				BaflUtils::NearestLanguageFile(iFsSession, fileName);

				//get drive and filename+extension of nearest language file
				TParsePtrC parser(fileName);
				if	(!parser.ExtPresent())
					User::Leave(KErrCorrupt);
				TPtrC drive=parser.Drive();
				TPtrC name=parser.NameAndExt();

				//create new CHlpFileEntry
				CHlpFileEntry* entry = CHlpFileEntry::NewLC(drive, name);
				__PRINT_FILE(KAddNewEntryToMasterList, *entry);

				//add to master list
				masterList->AppendL(entry);
				CleanupStack::Pop(entry);
				}

			list->ResetAndDestroy();
			CleanupStack::PopAndDestroy(list);
			}

		if	(i-- == EDriveZ)
			break;
		}
	// The code below validates the masterList of help files and filters out unnecessary files based on the criteria specified below -
	// i)   In case of two files - abc.hlp and abc.h01 - present in the master list,since abc.h01 is the latest version, it
	//      will be picked. In other words abc.hlp will be removed from the list.
	// ii)  if two files for different language code exist, both should remain in masterList
	// iii) if the system language is changed, the file as per the new language should be retained and
	//      default file with .hlp extension should be removed from the masterList
	TLanguage language;
	language = User::Language(); //gets the default language
	TInt languageCode;
	languageCode = language; // integer equivalent of the language code. For example, ELangEnglish is 01
	for(TInt i=masterList->Count()-1; i >= 0; i--)
		{
		CHlpFileEntry* newEntry = masterList->At(i);
		TPtrC ptrFile (newEntry->FileName());
		TBufC<8> fileExtension(ptrFile.Right(2));
		TLex lex(fileExtension);
		TInt lexIntValue;
		lex.Val(lexIntValue);
		TParse parser;
		TBool hlpFlag = ETrue;
		User::LeaveIfError(parser.Set(ptrFile, NULL, NULL));

		if(lexIntValue != language)
			{
			// Get the filename with language code in file extension
			TBuf<256> fileName(newEntry->Name());
			TBuf<10> name;
			name.Format(_L(".h%02d"),languageCode);
			fileName.Append(name);

			// Get the filename with the .hlp extension
			TBuf<256> defaultFileName(newEntry->Name());
			TBuf<10> defaultName;
			defaultName.Format(_L(".hlp"));
			defaultFileName.Append(defaultName);

			//Check the file against all the files in the list
			for(TInt j = masterList->Count()-1; j >=0 ; j--)
				{
				CHlpFileEntry* entry = masterList->At(j);
				// If two files with same name exist, delete one from the masterList
				// and reset hlpFlag to EFalse
				if(entry->FileName().CompareF(fileName)==0)
					{
					masterList->Delete(i);
					hlpFlag = EFalse;
					break;
					}
				}
				if( (hlpFlag) && (parser.Ext().CompareF(KDefaultHelpExtension)!=0))
					{
					for(TInt k = masterList->Count()-1; k >=0 ; k--)
						{
						CHlpFileEntry* entry = masterList->At(k);
						if(entry->FileName().CompareF(defaultFileName)==0)
							{
							masterList->Delete(i);
							break;
							}
						}
					}
				}
			}

	// Load the master list
	TFileName file;
	TInt count = masterList->Count();

	for (i=0; i<count; i++)
		{
		masterList->At(i)->GetFullNameAndPath(file);
		__PRINT_FILE_NO(KLoadMasterList, *masterList->At(i), i);

		// This will leave with KErrArgument if it's a bad file
		TRAPD(error, OpenFileL(file));
		UNUSED_VAR(error); // used to suppress build warnings
		}

	CleanupStack::PopAndDestroy(masterList);
	}

TBool CHlpModel::DiskPresent(TInt aDrive) const
	{
	TDriveList list;
	if	(iFsSession.DriveList(list) < KErrNone)
		return EFalse;

	TDriveInfo info;
	TInt error = iFsSession.Drive(info, aDrive);
	if	(error < KErrNone)
		return EFalse;

	return (list[aDrive] && info.iType != EMediaNotPresent);
	}

void CHlpModel::ResetAndDestroyArrayOfCHlpFileEntry(TAny* aObject)
	{
	CArrayPtr<CHlpFileEntry>* array=REINTERPRET_CAST(CArrayPtr<CHlpFileEntry>*,aObject);
	if (array)
		array->ResetAndDestroy();
	delete array;
	}

CHlpFileList* CHlpModel::BuildListForDriveLC(TDriveUnit aDrive, RFs& aFsSession) const
//
//	Generate a list of help files for the specified drive
//
	{
	CHlpFileList* list = new(ELeave) CHlpFileList(5);
	CleanupStack::PushL(TCleanupItem(ResetAndDestroyArrayOfCHlpFileEntry,list));

	// Generate the folder spec to search in
	TFileName searchSpec;
	TChar driveLetter;
	User::LeaveIfError(RFs::DriveToChar(aDrive, driveLetter));
	searchSpec.Append(driveLetter);
	searchSpec.Append(':');
	searchSpec.Append(KHlpFileSearchPath);
	searchSpec.Append(KHlpFileSpec);

	CDir* dirList;

	TFindFile finder(aFsSession);
	TInt ret = finder.FindWildByPath(searchSpec, NULL, dirList);
	if	(ret < KErrNone)
		{
		if	(ret == KErrNotFound)
			return list;
		else
			User::Leave(ret);
		}
	CleanupStack::PushL(dirList);

	// Add files to help file list
	TInt dirCount = dirList->Count();
	for(TInt i=0; i<dirCount; i++)
		{
		CHlpFileEntry* newEntry = CHlpFileEntry::NewLC(aDrive, (*dirList)[i].iName);
		__PRINT_FILE_NO(KFoundEntry, *newEntry, i);

		//check if new file already in the list
		TBool foundMatch=EFalse;
		TInt existingEntryCount=list->Count();
		for(TInt j=existingEntryCount-1; j>=0; j--)
			{
			if(list->At(j)->Name().CompareF(newEntry->Name())==0)
				{
				__PRINT_FILE_NO(KDuplicateEntry, *newEntry, i);
				foundMatch=ETrue;
				}
			}

		if(!foundMatch)
			{
			__PRINT_FILE_NO(KAddNewEntry, *newEntry, i);
			list->AppendL(newEntry);
			CleanupStack::Pop(newEntry);
			}
		else
			CleanupStack::PopAndDestroy(newEntry);
		}
	CleanupStack::PopAndDestroy(dirList);

	return list;
	}

EXPORT_C void CHlpModel::CloseL()
/** Closes all open help files. */
	{
	const TInt KDatabaseCount = DatabaseCount();
	for (TInt i=0; i<KDatabaseCount; i++)
		iDatabases->At(i)->Close();
	iDatabases->ResetAndDestroy();
	}


//
//
//

/**
Opens a specified help file.

@param aFileName Help file to open
*/
EXPORT_C void CHlpModel::OpenFileL(const TDesC& aFileName)
	{
	TEntry entry;
	User::LeaveIfError(iFsSession.Entry(aFileName, entry));
	if	(entry.IsDir())
		{
		return; // don't try and open directories as help files ;)
		}

	TParsePtrC parser(aFileName);
	if (parser.Path() != KHlpFileSearchPath)
		{ // given file is outside \\Resource\\Help\\ directory.
		User::Leave(KErrArgument);
		}

	if	(!(entry[0] == KPermanentFileStoreLayoutUid && entry[1] == KUidHlpApp))
		{
		User::Leave(KErrArgument);
		}

	CHlpDatabase* newDatabase = CHlpDatabase::NewLC(iFsSession, aFileName);
 	iDatabases->AppendL(newDatabase);
	CleanupStack::Pop(newDatabase);
	}

EXPORT_C void CHlpModel::CloseFileL(const TDesC& aFileName)
/** Closes a specified help file.

@param aFileName Help file to close */
	{
	const TInt KDatabaseCount = DatabaseCount();
	for (TInt i=0; i<KDatabaseCount; i++)
		{
		CHlpDatabase* database = iDatabases->At(i);
		if	(database->FileName() == aFileName)
			{
			delete database;
			iDatabases->Delete(i);
			return;
			}
		}
	User::Leave(KErrNotFound);
	}

//
//
//

EXPORT_C void CHlpModel::ContextSearchL(TCoeHelpContext& aContext)
/** Searches for a topic with the specified help context.

A successful search generates an ETopicAvailable event. The topic can then
be retrieved using LoadTopicL().

An unsuccessful search generates an ETopicNotFound event.

@param aContext Help context to search for */
	{
	// Set up ready for search
	ResetReadyForSearch();
	SetSearchType(EContextSearch);

	// See if we can find the specified context in any of the meta
	// data that each database contains. This reduces the time taken to
	// perform a search, as only the database which actually contains the
	// correct information actually has the SQL executed on it (major time saver).
	TInt db = MatchUidL(aContext.iMajor);
	if	(db >= KErrNone)
		{
		// Search the specified database for the context. If this context is
		// found then the view will be prepared ready for the caller to extract
		// the topic.
		iCurrentDb = db;
		TInt searchResult = CurrentDatabase()->ContextSearchL(aContext.iContext);
		ReportEventToObserverL(searchResult);
		}
	else
		{
		ReportEventToObserverL(ETopicNotFound);
		}
	}

EXPORT_C void CHlpModel::CategoryUIDSearchL(TUid aCategoryUID)
/** Searches for topics with the specified help category UID.

A successful search generates an ETopicListAvailable event. The list can then
be retrieved using LoadListL().

An unsuccessful search generates an ETopicListNoneFound event.

@param aCategoryUID Category UID to search for */
	{
	// Set up ready for search
	ResetReadyForSearch();
	SetSearchType(ETopicListForCategoryUID);

	// See if we can find the specified category Uid in any of the meta
	// data that each database contains. This reduces the time taken to
	// perform a search, as only the database which actually contains the
	// correct information actually has the SQL executed on it (major time saver).
	TInt db=MatchUidL(aCategoryUID);
	if	(db >= KErrNone)
		{
		iTransientCategoryUid = aCategoryUID;
		iCurrentDb=db;

		TBuf<32> buf;
		buf.Num(STATIC_CAST(TUint, aCategoryUID.iUid));
		DoSearchL(ETopicListForCategoryUID, buf);

		// Reset the category uid after we've done the search (used in
		// debug invariant state check)
		iTransientCategoryUid = KNullUid;
		}
	else
		{
		ReportEventToObserverL(ETopicNotFound);
		}
	}

EXPORT_C void CHlpModel::TopicSearchL(const CHlpItem& aHelpItem)
/** Searches for a topic for the specified help item.

A successful search generates an ETopicAvailable event. The topic can then
be retrieved using LoadTopicL().

An unsuccessful search generates an ETopicNotFound event.

@param aHelpItem Help item to search for */
	{
	// aHelpItem contains the information required for a speedy retrieval
	// of the specified topic from the help model. Using it's topic Id (iId)
	// category uid (CategoryUid()) and help file uid (HelpFileUid()) we
	// can run the search SQL on exactly the right database file, for exactly
	// the right category, and exactly the right topic.

	// if already searching, dont start another search
	if(iFound && CurrentSearchType()==ETopicIdSearch)
		return;

	// Set up ready for search
	ResetReadyForSearch();
	SetSearchType(ETopicIdSearch);

	TInt count = DatabaseCount();
	for(TInt i=0; i<count; i++)
		{
		CHlpDatabase* database = iDatabases->At(i);
		if	(database->HelpFileUid() == aHelpItem.HelpFileUid())
			{
			// Run the SQL on this database.
			iCurrentDb = i;
			TInt result = database->TopicIdSearchL(aHelpItem.CategoryUid(), STATIC_CAST(TUint, aHelpItem.iId));
			iFound = (result == ETopicAvailable);
			ReportEventToObserverL(result);
			return;
			}
		}
	ReportEventToObserverL(ETopicNotFound);
	}

EXPORT_C void CHlpModel::IndexSearchL(const CHlpItem& aHelpItem)
/** Searches for index entries for the specified help item.

A successful search generates an ETopicListAvailable event. The list can then
be retrieved using LoadListL().

An unsuccessful search generates an ETopicListNoneFound event.

@param aHelpItem Help item to search for */
	{
	// Set up ready for search
	ResetReadyForSearch();
	SetSearchType(ETopicIdSearch);

	TInt count = DatabaseCount();
	for(TInt i=0; i<count; i++)
		{
		CHlpDatabase* database = iDatabases->At(i);
		if	(database->HelpFileUid() == aHelpItem.HelpFileUid())
			{
			// Run the SQL on this database.
			iCurrentDb = i;

			TBuf<KHlpModelNumericValueSize> buf;
			buf.Num(STATIC_CAST(TUint, aHelpItem.iId));
			DoSearchL(EIndexSearch, buf);

			return;
			}
		}
	ReportEventToObserverL(EIndexSearchListNoneFound);
	}

//
//
//

EXPORT_C void CHlpModel::SearchL(TInt aType, TUint32 aId)
/** Searches using a specified type of search with a numeric criterion.

@param aType Type of search. This is a search type (EIndexList etc.) enum
value.
@param aId Numeric search criterion */
	{
	if	(aType == ETopicListForCategoryUID)
		iTransientCategoryUid = TUid::Uid(aId);

	TBuf<KHlpModelNumericValueSize> buf;
	buf.Num(STATIC_CAST(TUint, aId));
	SearchL(aType, buf);
	}

EXPORT_C void CHlpModel::SearchL(TInt aType, HBufC* aCriterion)
/** Searches using a specified type of search, allowing a NULL seach criterion.

@param aType Type of search. This is a search type (EIndexList etc.) enum
value.
@param aCriterion String search criterion. This can be NULL if the search type
requires no search criterion. */
	{
	// Kludge to remain source compatible in App Engines 5.2
	// This function should be removed.
	if	(aCriterion)
		SearchL(aType, *aCriterion);
	else
		SearchL(aType, KNullDesC);
	}

EXPORT_C void CHlpModel::SearchL(TInt aType, const TDesC& aCriterion)
//
//	This function is the base of all the SearchL(...) methods, i.e. it is callled
//	by SearchL(TInt, HBufC*) and also SearchL(TInt, TUint32)
//
/** Searches using a specified type of search with a string search criterion.

Note that the full text search, EFullTextSearch, is asynchronous.

@param aType Type of search. This is a search type (EIndexList etc.) enum
value.
@param aCriterion String search criterion */
	{
	// Set up ready for search
	ResetReadyForSearch();

	// Do the actual search itself.
	DoSearchL(aType, aCriterion);
	}

EXPORT_C TInt CHlpModel::CancelSearch()
/** Cancels a full text search.

@return KErrNone if successful, KErrArgument if a search is not in progress */
	{
	iSearch->CancelEvaluator();

	if	(CurrentSearchType() != EFullTextSearch)
		return KErrArgument;

	return KErrNone;
	}

//
//
//

EXPORT_C void CHlpModel::LoadTopicL(CRichText& aRichText, TDes& aTitle)
/** Gets the current help topic text and title.

The function assumes that a successful search has already been performed.

@param aRichText On return, the help topic text
@param aTitle On return, the help topic title */
	{
	// Fetch the rich text
	LoadTopicL(aRichText);

	// Next get the title
	RDbView& view		= CurrentDatabase()->View();
	__ASSERT_ALWAYS(view.AtRow(), Panic(EHlpNoRowAtCursor));
	CDbColSet* colset	= view.ColSetL();
	TDbColNo topicTitle	= colset->ColNo(KSQLTopicTitleColumn);
	delete colset;

	view.FirstL();
	view.GetL();
	aTitle = view.ColDes(topicTitle);
	}

EXPORT_C void CHlpModel::LoadTopicL(CRichText& aRichText)
/** Gets the current help topic text.

The function assumes that a successful search has already been performed.

@param aRichText On return, the help topic text */
	{
	RDbView& view		= CurrentDatabase()->View();
	__ASSERT_ALWAYS(view.CountL(), Panic(EHlpTopicNoRowsInView));

	CDbColSet* colset	= view.ColSetL();
	TDbColNo topicCol	= colset->ColNo(KSQLTopicTextColumn);
	TDbColNo markupCol	= colset->ColNo(KSQLTopicMarkupColumn);
	delete colset;
	view.FirstL();
	view.GetL();

	aRichText.Reset();
	TInt len = view.ColLength(topicCol);
	HBufC* buf = HBufC::NewLC(len);
	TPtr pBuf(buf->Des());

	RDbColReadStream stream;
	stream.OpenLC(view, topicCol);
	stream.ReadL(pBuf, len);
	aRichText.InsertL(0, pBuf);
	CleanupStack::PopAndDestroy(2); // stream & buf

	if (!view.IsColNull(markupCol))
		{
		RDbColReadStream blob;
		blob.OpenL(view, markupCol);

		aRichText.SetPictureFactory(this, this);
		CEmbeddedStore* embeddedStore = CEmbeddedStore::FromLC(blob);
		iCurrentRichTextStore = embeddedStore;

		RStoreReadStream readStream;
		readStream.OpenLC(*embeddedStore, embeddedStore->Root());
		aRichText.InternalizeMarkupDataL(readStream);
		aRichText.DetachFromStoreL(CPicture::EDetachFull);
		CleanupStack::PopAndDestroy(2); // embeddedStore, readStream
		aRichText.SetPictureFactory(NULL, NULL);
		iCurrentRichTextStore = NULL;
		}
	}

EXPORT_C void CHlpModel::LoadTopicL(CHlpTopic* aTopic)
/** Gets the current help topic.

The function assumes that a successful search has already been performed.

@param aTopic On return, the help topic. This is caller allocated. */
	{
	__ASSERT_ALWAYS(aTopic, Panic(EHlpNoTopic));

	LoadTopicL(*(aTopic->TopicText()));

	RDbView& view		= CurrentDatabase()->View();
	CDbColSet* colset	= view.ColSetL();
	TDbColNo topicTitle	= colset->ColNo(KSQLTopicTitleColumn);
	TDbColNo topicId	= colset->ColNo(KSQLTopicIdColumn);
	TDbColNo category	= colset->ColNo(KSQLCategoryColumn);
	delete colset;

	// Populate the topic's
	aTopic->iTopicTitle = view.ColDes(topicTitle);
	aTopic->iTopicId	= view.ColUint32(topicId);
	aTopic->iCategory	= view.ColDes(category);
	}

EXPORT_C void CHlpModel::LoadListL(CHlpList* aList)
/** Gets the current help list.

The function assumes that a successful search has already been performed.

@param aList On return, the help list. This is caller allocated. */
	{
	__ASSERT_ALWAYS(aList, Panic(EHlpNoHelpList));

	// Clear all entries in the list
	aList->Reset();

	// This loop provides the mechanism by which we iterate through every database
	// in the help model. We query each database in turn to find out a) if it has
	// been searched in the first place (not every search results in *every* help
	// file's database view being initialised) and b) if it actually contains any
	// results.
	const TInt KDatabaseCount = DatabaseCount();
	for (TInt i=0; i < KDatabaseCount; i++)
		{
		// Get a help database pointer and it's view.
		CHlpDatabase* currentDatabase = iDatabases->At(i);
		RDbView& view = currentDatabase->View();

		// Does this database meet the criteria described above (points a & b)?
		if	(!currentDatabase->ViewReady() || !view.CountL())
			continue; // Doesn't meet criteria, so skip this database

		// Criteria satisifed, so look up the column id's we're interested in.
		CDbColSet* colset		= view.ColSetL();
		TDbColNo colTitle		= colset->ColNo(KSQLTopicTitleColumn);
		TDbColNo colTopicId		= colset->ColNo(KSQLTopicIdColumn);
		TDbColNo colCategoryId	= colset->ColNo(KSQLCategoryUidColumn);
		TDbColNo colIndex		= colset->ColNo(KSQLIndexColumn);
		TDbColNo colIndexId		= colset->ColNo(KSQLIndexIdColumn);
		delete colset;

		// Each topic is associated with a particular category. This information
		// is cached with the help item so that we can ensure we restore the
		// topic relating to the category searched (topic id's are not unique, they
		// are assigned in increasing numerical order by the help compiler, on a
		// category basis. Category ids, however, are unique, hence we need to store
		// both in order to be able to restore the correct topic text).
		view.FirstL();
		view.GetL();

		// Iterate through each row in this database's view, and store each entry
		// as a help item in the list passed as a parameter to this function.
		FOREVER
			{
			CHlpItem* item = NULL;

			// This bit loads different content into the help item depending on the last performed
			// search type.
			switch (iSearchType)
				{
			case ECategoryList:
			case ETopicListForCategory:
			case ETopicListForCategoryUID:
				item = CHlpItem::NewLC(view.ColDes(colTitle), view.ColUint32(colTopicId), TUid::Uid(view.ColUint(colCategoryId)), currentDatabase->HelpFileUid());
				break;
			case EIndexList:
				item = CHlpItem::NewLC(view.ColDes(colIndex), view.ColUint32(colIndexId), currentDatabase->HelpFileUid());
				break;
			case EQuickSearch:
			case EFullTextSearch:
			case EIndexSearch:
				item = CHlpItem::NewLC(view.ColDes(colTitle), view.ColUint32(colTopicId), TUid::Uid(view.ColUint(colCategoryId)), currentDatabase->HelpFileUid());
				break;
			default:
				User::Leave(KErrNotSupported);
				}

			aList->AppendL(item);
			CleanupStack::Pop(item);
			view.NextL();
			if	(view.AtEnd())
				break;
			view.GetL();
			}
		}
	}

EXPORT_C void CHlpModel::CategoryListL(CDesCArray* aList)
/** Populates a list of available help categories.

This can be called without needing to perform a prior search.

@param aList On return, the list of available help categories. This is caller
allocated. */
	{
	__ASSERT_ALWAYS(aList, Panic(EHlpNoCategoryList));

	const TInt count = DatabaseCount();
	for(TInt i=0; i<count; i++)
		iDatabases->At(i)->AppendCategoryListL(*aList);
	aList->Sort();
	}

//
//
//

EXPORT_C void CHlpModel::SetZoomSizeL(THlpZoomState aState)
/** Sets the zoom size to use for help text and pictures.

@param aState Zoom size */
	{
	iZoomSize = aState;
	const TInt count = iPictures->Count();
	for (TInt i=0; i<count; i++)
		{
		// Fetch a picture from the array
		CHlpPicture* picture = iPictures->At(i);
		picture->HandleZoomChangedL(iZoomSize);
		}
	}

EXPORT_C THlpZoomState CHlpModel::ZoomSize() const
/** Gets the zoom size used for help text and pictures.

@return Zoom size */
	{
	return iZoomSize;
	}

/* Sets the iZoomFactors array to the appropriate
zoom factor value, depending on the current zoom state
as dictated by the aZoomState argument variable.

The hlpmodel's THlpZoomState enumerated type is related to
the AppUI's TZoomStates enumerated type. This relation is required
so that the correct pixels to twips ratio can be used in the
CHlpPicture::GetOriginalSizeInTwips() method.

Error Condition	: None.
@param aZoomState A zoom size.
@param aFactor The zoom factor that corresponds to the current zoom size.
@pre None.
@post The iZoomFactors data member has been populated with the appropriate
zoom factor values. */
EXPORT_C void CHlpModel::SetZoomFactors(THlpZoomState aZoomState, TInt aFactor)
/** Sets a zoom factor for a logical zoom size.

@param aZoomState Logical zoom size for which to set the factor.
@param aFactor Zoom factor. For example, 2 specifies double size. */
	{
	__ASSERT_ALWAYS(iZoomFactors->Count() == 3, Panic(EHlpNotEnoughZoomRatios));
	//
	switch(aZoomState)
		{
	/*
	 * If zoom state is large, assign zoom factor 750
	 * to index 0 of 'iZoomFactors'
	 */
	case EHlpZoomStateSmall:
		iZoomFactors->At(EHlpZoomStateSmall) = aFactor;
		break;

	default:
	/*
	 * If zoom state is large, assign zoom factor 1000
	 * to index 1 of 'iZoomFactors'
	 */
	case EHlpZoomStateMedium:
		iZoomFactors->At(EHlpZoomStateMedium) = aFactor;
		break;
	/*
	 * If zoom state is large, assign zoom factor 1250
	 * to index 2 of 'iZoomFactors'
	 */
	case EHlpZoomStateLarge:
		iZoomFactors->At(EHlpZoomStateLarge) = aFactor;
		break;
		}
	}

/*
Accesses the iZoomFactors data member, and returns
the current zoom factor, depending on the value of
the current zoom state, which is to be used for the
pixel scaling of a picture in CHlpPicture::GetOriginalSizeInTwips().
Error Condition	: None

@return The zoom factor, corresponding to the current zoom size.
@pre None.
@post Current zoom factor is returned. */
TInt CHlpModel::CurrentZoomFactor() const
	{
	__ASSERT_ALWAYS(iZoomFactors->Count() == 3, Panic(EHlpNotEnoughZoomRatios));
	//
	switch (iZoomSize)
		{
	case EHlpZoomStateSmall:
		return iZoomFactors->At(EHlpZoomStateSmall);
	default:
	case EHlpZoomStateMedium:
		return iZoomFactors->At(EHlpZoomStateMedium);
	case EHlpZoomStateLarge:
		return iZoomFactors->At(EHlpZoomStateLarge);
		}
	}

/* Removes a CHlpPicture from the iPictures array.

Error Condition	: None

@param aHelpPicture A pointer to a CHlpPicture object.
@pre A valid CHlpPicture is passed to the function.
@post CHlpPicture's that are no longer displayed have been removed
from the iPictures data member. */
void CHlpModel::RemoveHelpPicture(CHlpPicture* aHelpPicture)
	{
	if (iPictures != 0)
	{
	const TInt count = iPictures->Count();
	__ASSERT_DEBUG(count > 0, User::Invariant());
	//
	for(TInt i=0; i<count; i++)
		{
		if	(aHelpPicture == iPictures->At(i))
			{
			iPictures->Delete(i);
			return;
			}
		}
	__ASSERT_DEBUG(EFalse, Panic(EHlpUnlocatedHelpPicture));
	}
	}


//
//
//

/* This is a mixin callback required to restore pictures
from the help model database into the topic rich text.
Because pictures are stored in their own picture table within the
help database (this means that the same picture may be used many
times within the rich text, but will only actually appear once
in the database - a major space saver) the restoration function
must have a primed view so that it knows which database to use
as the source picture table.

@param aHdr A reference to a picture header.
@param aDeferredPictureStore A stream store where the pictures are kept.
@pre A valid picture database must exist, so that the stream store can relate to.
@post A TInt pointer (index) to the picture table is inserted
in the rich text to indicate which picture needs restoring.
The CHlpPicture looks up this index in the picture table. */
void CHlpModel::NewPictureL(TPictureHeader& aHeader, const CStreamStore& aDeferredPictureStore) const
	{
	if	(aHeader.iPictureType != KUidHelpImage)
		User::Leave(KErrNotSupported);
	if	(!aHeader.iPicture.IsId())
		User::Leave(KErrBadHandle);

	TStreamId id = aHeader.iPicture.AsId();
	CHlpPicture* picture = CHlpPicture::NewLC(aDeferredPictureStore, id, *CurrentDatabase(), *const_cast<CHlpModel*>(this));
	aHeader.iPicture = picture;

	// Add picture to the picture array. We need to do this so that we can update the picture
	// when the zoom size is changed by the UI.
	iPictures->AppendL(picture);

	CleanupStack::Pop(picture);
	}

EXPORT_C const CStreamStore& CHlpModel::StreamStoreL(TInt /*aPos*/) const
/** Gets the current rich text store.

@param aPos Unused
@return Current rich text store */
	{
	__ASSERT_ALWAYS(iCurrentRichTextStore, Panic(EHlpNoPictureStore));
	return *iCurrentRichTextStore;
	}

//
//
//

EXPORT_C TInt CHlpModel::MatchUidL(TUid aUid)
/** Searches the open help databases for the specified topic UID.

@param aUid Topic UID to search for
@return Index of the database if the item was found, or KErrNotFound if not */
	{
	const TInt KDatabaseCount = DatabaseCount();
	for (TInt i=0; i<KDatabaseCount; i++)
		{
		if (iDatabases->At(i)->MatchUidL(aUid))
			return i;
		}
	return KErrNotFound;
	}

EXPORT_C void CHlpModel::SetObserver(MHlpModelObserver* aObserver)
/** Sets the client callback interface.

@param aObserver Client callback interface */
	{
	iObserver = aObserver;
	}

//
//
//

void CHlpModel::DoSearchL(TInt aType, const TDesC& aCriterion)
	{
	// Initialise the member data with the type of search we are about to perform,
	// and also setup the search criteria which is needed to peform multiple searches
	// across databases.
	SetSearchType(aType);
	SetCriterionL(aCriterion);

	if	(!DatabaseCount())
		{
		ReportEventToObserverL(ETopicNotFound);
		return;
		}

	// Prepare the searcher with the database to search
	iSearch->SetDatabase(*CurrentDatabase());

	// Indicate that this database is being searched, and therefore it is guaranteed not
	// to have a null view
	CurrentDatabase()->SetViewReady(ETrue);

	// Get the searcher to actually do the search, which includes
	// building the necessary SQL statement and then running the SQL on the
	// correct table.
	iSearch->SearchL(aType, *iCriterion);
	}

void CHlpModel::DoNextSearchL()
//
//	This function is used to perform an incremental search (mirroring an inner join which
//	EPOC DBMS doesn't support) across multiple databases.
//
	{
	if	(iCurrentDb < DatabaseCount()-1)
		{
		// If we're peforming a category Uid-based search, then
		// we only run the sql seach on the databases that actually
		// hold meta data on the specified category.
		iCurrentDb++;
		if	(CurrentSearchType() == ETopicListForCategoryUID)
			{
			__ASSERT_DEBUG(iTransientCategoryUid != KNullUid, Panic(EHlpFault));
			TInt numberOfMatches = 0;
			do
				{
				if	(CurrentDatabase()->MatchUidL(iTransientCategoryUid))
					{
					// This database does have meta data on the specified category,
					// so it's worth running the check
					DoSearchL(CurrentSearchType(), *iCriterion);
					++numberOfMatches;
					}
				}
			while (++iCurrentDb < DatabaseCount());
			if	(numberOfMatches == 0)
				ReportEventToObserverL(ESearchComplete);
			}
		else
			{
			// Doesn't matter what type of search it is, we still
			// run the SQL :(
			DoSearchL(CurrentSearchType(), *iCriterion);
			}
		}
	else
		{
		ReportEventToObserverL(ESearchComplete);
		}
	}

void CHlpModel::ResetReadyForSearch()
//
//	This function is called regardless of search type - it prepares all the necessary
//	variables ready for a search.
//
	{
	// Set to the first database
	iCurrentDb=0;

	// Indicate that we've currently not found any results
	iFound=EFalse;

	// Reset the database views to 'not yet ready'
	ResetViews();
	}

void CHlpModel::ResetViews()
//
//	Go through each database and reset the "view ready" flag to EFalse
//	to indicate that this particular database's view has not yet been primed.
//
	{
	const TInt count = DatabaseCount();
	for(TInt i=0; i<count; i++)
		iDatabases->At(i)->SetViewReady(EFalse); // not ready
	}

void CHlpModel::SetCriterionL(const TDesC& aCriterion)
//
//	Updates the internal iCriterion pointer to contain the new
//	criteria for searching.
//
	{
	HBufC* newCriteria = aCriterion.AllocL();
	delete iCriterion;
	iCriterion = newCriteria;
	}

RDbView* CHlpModel::CurrentView() const
	{
	return &(CurrentDatabase()->View());
	}

//
//
//

void CHlpModel::HandleDbEventL(TInt aEvent)
//
//	Called by the SQL searcher. This function routes responses from the searcher
//	to the model observer, and performs and recursive searching that is required.
//
	{
	switch(aEvent)
		{
	case ENoRecordsFound:
		// No records were found for this search. If a context search was requested,
		// we indicate that nothing was found and let the observer of the database
		// perform any action. Otherwise, we search the next database in turn.
		if	(CurrentSearchType() != ETopicIdSearch &&
			 CurrentSearchType() != EContextSearch &&
			 CurrentSearchType() != EIndexSearch
			)
			DoNextSearchL();
		else
			ReportEventToObserverL(ENoRecordsFound);
		break;
	case ESearchComplete:
		// Indicate that at least some matching critera was found and then
		// either end the search, or search the next database.
		iFound=ETrue;
		if	(CurrentSearchType() != ETopicIdSearch &&
			 CurrentSearchType() != EContextSearch &&
			 CurrentSearchType() != EIndexSearch
			)
			DoNextSearchL();
		else
			ReportEventToObserverL(ESearchComplete);
		break;
	case ESearchInProgress:
		// Indicate to the observer that a search is in progress
		ReportEventToObserverL(ESearchInProgress);
		break;
	case EHlpSearchCancelled:
		// Indicate to the observer that a search was cancelled
		ReportEventToObserverL(EHlpSearchCancelled);
		break;
	default:
		__ASSERT_DEBUG(EFalse, Panic(EHlpFault));
		break;
		}
	}

void CHlpModel::ReportEventToObserverL(TInt aEvent)
	{
	if	(!iObserver)
		return; // can't do anything without an observer

	switch(aEvent)
		{
	case ESearchInProgress:
		iObserver->HandleModelEventL(EModelSearchInProgress);
		return;
	case EHlpSearchCancelled:
		iObserver->HandleModelEventL(EHlpSearchCancelled);
		return;
	default: // Keep GCC happy
		break;
		}

	switch(CurrentSearchType())
		{
	case EIndexList:
		iObserver->HandleModelEventL((iFound?EIndexListAvailable:EIndexListNoneFound));
		break;
	case ECategoryList:
		iObserver->HandleModelEventL((iFound?ECategoryListAvailable:ECategoryListNoneFound));
		break;
	case ETopicListForCategory:
	case ETopicListForCategoryUID:
		iObserver->HandleModelEventL((iFound?ETopicListAvailable:ETopicListNoneFound));
		break;
	case EContextSearch:
		iObserver->HandleModelEventL(aEvent);
		break;
	case EIndexSearch:
		iObserver->HandleModelEventL((iFound?EIndexSearchListAvailable:EIndexSearchListNoneFound));
		break;
	case EQuickSearch:
	case EFullTextSearch:
		iObserver->HandleModelEventL((iFound?ESearchListAvailable:ESearchListNoneFound));
		break;
	case ETopicIdSearch:
		iObserver->HandleModelEventL((iFound?ETopicAvailable:ETopicNotFound));
		// event has been reported, reset now.
		iFound=EFalse;
		break;
	default:
		__ASSERT_DEBUG(EFalse, Panic(EHlpFault));
		break;
		}
	}








//
// ----> CHlpList
//

EXPORT_C CHlpList::~CHlpList()
/** Destructor. */
	{
	if(iList)
		iList->ResetAndDestroy();
	delete iList;
	}

void CHlpList::ConstructL()
	{
	iList = new(ELeave) CArrayPtrFlat<CHlpItem>(2);
	}

EXPORT_C CHlpList* CHlpList::NewL()
/** Allocates and creates a new help list object.

@return New help list object */
	{
	CHlpList* self = CHlpList::NewLC();
	CleanupStack::Pop(self);
	return self;
	}

EXPORT_C CHlpList* CHlpList::NewLC()
/** Allocates and creates a new help list object, leaving the object on the cleanup
stack.

@return New help list object */
	{
	CHlpList* self = new(ELeave) CHlpList;
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
	}

EXPORT_C TInt CHlpList::MdcaCount() const
/** Gets the number of items in the list.

@return Number of items in the list */
	{
	return iList->Count();
	}

EXPORT_C TPtrC CHlpList::MdcaPoint(TInt aIndex) const
/** Gets the title of the item at the specified index.

@param aIndex Item index
@return Title of the item */
	{
	return TPtrC(iList->At(aIndex)->iTitle->Des());
	}

EXPORT_C TUint32 CHlpList::At(TInt aIndex) const
/** Gets the topic ID of the item at the specified index.

@param aIndex Item index
@return Topic ID */
	{
	return iList->At(aIndex)->iId;
	}

EXPORT_C void CHlpList::Reset()
/** Resets the list. */
	{
	iList->ResetAndDestroy();
	}

EXPORT_C CHlpItem* CHlpList::Item(TInt aIndex) const
/** Gets the item at the specified index.

@param aIndex Item index
@return Item */
	{
	return iList->At(aIndex);
	}

EXPORT_C TInt CHlpList::Find(TUint32 aId)
/** Searches the list for a specified item ID.

@param aId Item ID
@return Item index, or KErrNotFound if not found */
	{
	CHlpItem* item = new CHlpItem(aId);
	if	(!item)
		return KErrNoMemory;

	TKeyArrayFix key(_FOFF(CHlpItem, iId), ECmpTUint32);
	TInt pos;
	TInt result = iList->Find(item, key, pos);
	delete item;

	if (!result)
		return pos;
	else
		return KErrNotFound;
	}

EXPORT_C void CHlpList::AppendL(CHlpItem* aItem)
/** Appends an item to the list.

@param aItem Item to add */
	{
	__ASSERT_ALWAYS(aItem, Panic(EHlpNoItem));
	iList->AppendL(aItem);
	}





//
// ----> CHlpItem - representing an individual item in the help file
//

CHlpItem::CHlpItem(TUint32 aId)
:	iId(aId), iCategoryUid(KNullUid), iHelpFileUid(KNullUid)
	{
	}

CHlpItem::CHlpItem(TUint32 aId, TUid aHelpFileUid)
:	iId(aId), iCategoryUid(KNullUid), iHelpFileUid(aHelpFileUid)
	{
	}

CHlpItem::CHlpItem(TUint32 aId, TUid aCategoryId, TUid aHelpFileUid)
:	iId(aId), iCategoryUid(aCategoryId), iHelpFileUid(aHelpFileUid)
	{
	}

EXPORT_C CHlpItem::~CHlpItem()
/** Destructor. */
	{
	delete iTitle;
	}

void CHlpItem::ConstructL(const TDesC& aTitle)
	{
	iTitle = aTitle.AllocL();
	}

CHlpItem* CHlpItem::NewL(const TDesC& aTitle, TUint32 aId, TUid aCategoryId, TUid aHelpFileUid)
/** Allocates and creates a new help item object.

@param aTitle Item title
@param aId Item ID
@param aCategoryId Category ID
@param aHelpFileUid Help file UID
@return New help item object */
	{
	CHlpItem* self = NewLC(aTitle, aId, aCategoryId, aHelpFileUid);
	CleanupStack::Pop(self);
	return self;
	}

CHlpItem* CHlpItem::NewLC(const TDesC& aTitle, TUint32 aId, TUid aCategoryId, TUid aHelpFileUid)
/** Allocates and creates a new help item object, leaving the object on the cleanup
stack.

@param aTitle Item title
@param aId Item ID
@param aCategoryId Category ID
@param aHelpFileUid Help file UID
@return New help item object */
	{
	CHlpItem* self = new(ELeave) CHlpItem(aId, aCategoryId, aHelpFileUid);
	CleanupStack::PushL(self);
	self->ConstructL(aTitle);
	return self;
	}

CHlpItem* CHlpItem::NewLC(const TDesC& aTitle, TUint32 aId, TUid aHelpFileUid)
/** Allocates and creates a new help item object, leaving the object on the cleanup
stack.

This overload does not specify a category UID.

@param aTitle Item title
@param aId Item ID
@param aHelpFileUid Help file UID
@return New help item object */
	{
	CHlpItem* self = new(ELeave) CHlpItem(aId, aHelpFileUid);
	CleanupStack::PushL(self);
	self->ConstructL(aTitle);
	return self;
	}






//
// ----> CHlpTopic
//

EXPORT_C CHlpTopic::~CHlpTopic()
	{
	delete iTopicText;
	delete iGlobalCharFormatLayer;
	delete iGlobalParaFormatLayer;
	}

void CHlpTopic::ConstructL()
	{
	// Create the necessary formatting layers for the rich text object.
	iGlobalParaFormatLayer	= CParaFormatLayer::NewL();
	iGlobalCharFormatLayer	= CCharFormatLayer::NewL();
	iTopicText				= CRichText::NewL(iGlobalParaFormatLayer, iGlobalCharFormatLayer);
	}

EXPORT_C CHlpTopic* CHlpTopic::NewL()
/** Allocates and creates a new help topic object.

@return New help topic object */
	{
	CHlpTopic* self = CHlpTopic::NewLC();
	CleanupStack::Pop(self);
	return self;
	}

EXPORT_C CHlpTopic* CHlpTopic::NewLC()
/** Allocates and creates a new help topic object, leaving the object on the cleanup
stack.

@return New help topic object */
	{
	CHlpTopic* self = new(ELeave) CHlpTopic();
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
	}

EXPORT_C void CHlpTopic::RestoreL(RDbView* aView)
/** Restores the object from a database view.

This only restores text and markup, not pictures. Help application authors
should use CHlpModel::LoadTopicL() instead.

@param aView Database view */
	{
	// NOTE: This function does not restore pictures because it does not know
	// which help model database to use as the source for the picture table.
	// Use CHlpModel::LoadTopicL(...) instead
	__ASSERT_ALWAYS(aView, Panic(EHlpNoView));
	__ASSERT_ALWAYS(aView->AtRow(), Panic(EHlpNoRowAtCursor));

	aView->FirstL();
	CDbColSet* colset	= aView->ColSetL();
	TDbColNo topicCol	= colset->ColNo(KSQLTopicTextColumn);
	TDbColNo markupCol	= colset->ColNo(KSQLTopicMarkupColumn);
	TDbColNo titleCol	= colset->ColNo(KSQLTopicTitleColumn);
	TDbColNo catCol		= colset->ColNo(KSQLCategoryColumn);
	TDbColNo idCol		= colset->ColNo(KSQLTopicIdColumn);
	delete colset;

	aView->GetL();

	TInt len = aView->ColLength(topicCol);
	HBufC* buf=HBufC::NewLC(len);
	TPtr pBuf(buf->Des());

	RDbColReadStream stream;
	stream.OpenLC(*aView, topicCol);
	stream.ReadL(pBuf, len);
	iTopicText->Reset();
	iTopicText->InsertL(0, *buf);
	CleanupStack::PopAndDestroy(2); // stream, buf

	if	(!aView->IsColNull(markupCol))
		{
		RDbColReadStream blob;
		blob.OpenL(*aView, markupCol);
		CEmbeddedStore* embeddedStore = CEmbeddedStore::FromLC(blob);
		RStoreReadStream readStream;
		readStream.OpenLC(*embeddedStore, embeddedStore->Root());
		iTopicText->InternalizeMarkupDataL(readStream);
		CleanupStack::PopAndDestroy(2); // embeddedStore, readStream
		}

	iCategory.Append(aView->ColDes(catCol));
	iTopicTitle.Append(aView->ColDes(titleCol));
	iTopicId=aView->ColUint32(idCol);
	}

EXPORT_C CRichText* CHlpTopic::TopicText()
/** Gets the topic text.

@return Topic text */
	{
	return iTopicText;
	}

EXPORT_C TDesC& CHlpTopic::TopicTitle()
/** Gets the topic title.

@return Topic title */
	{
	return iTopicTitle;
	}

EXPORT_C TDesC& CHlpTopic::Category()
/** Gets the topic category.

@return Topic category */
	{
	return iCategory;
	}
