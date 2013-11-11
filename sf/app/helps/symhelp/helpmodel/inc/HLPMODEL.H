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

#ifndef __HLPMODEL_H
#define __HLPMODEL_H

// System includes
#include <e32std.h>
#include <badesca.h>	// For MDesCArray
#include <txtmrtsr.h>	// For MRichTextStoreResolver
#include <gdi.h>		// For MPictureFactory
#include <d32dbms.h>
// Help model includes
#include "hlpconstants.h"

// Classes referenced
class CRichText;
class CHlpDatabase;
class CHlpFileEntry;
class CHlpSQLSearch;
class TCoeHelpContext;
class CParaFormatLayer;
class CCharFormatLayer;
class CHlpPicture;

// Typedefs
/**
@publishedAll
@released
*/
typedef CArrayPtrFlat<CHlpDatabase> CHlpDatabases;

/**
@publishedAll
@released
*/
typedef CArrayPtrFlat<CHlpFileEntry> CHlpFileList;
#ifdef SYMBIAN_ENABLE_SPLIT_HEADERS
/** Maximum Title column. 
@publishedAll
@released
*/
const TInt KMaxTitleColumn = 120;
#endif

#ifndef SYMBIAN_ENABLE_SPLIT_HEADERS
/**
@internalComponent
@released
*/
const TInt KHlpModelDefaultNumberOfImagesForV6Point2Files = 1;
/**
@internalComponent
@released
*/
const TInt KHlpModelMaximumNumberOfImagesForV6Point2Files = 3;
//
/** Default zoom factor for small zoom size. 
@internalComponent
@released
*/
const TInt KHlpModelZoomFactorSmall = 750;
/** Default zoom factor for medium zoom size. 
@internalComponent
@released
*/
const TInt KHlpModelZoomFactorMedium = 1000;
/** Default zoom factor for large zoom size. 
@internalComponent
@released
*/
const TInt KHlpModelZoomFactorLarge = 1250;

/** Default zoom factor for medium zoom size as real number. 
@internalComponent
@released
*/
const TReal KHlpModelDefaultZoomFactorAsRealNumber = 1000.0;

#endif //SYMBIAN_ENABLE_SPLIT_HEADERS

class MHlpModelObserver
/** Client callback inteface to receive events from the help model. 
@publishedAll
@released
*/
	{
public:
	/** Receives a help model event.
	
	@param aEvent Help model event. Events are listed in the enums that begin 
	ECategoryListAvailable, and ENoRecordsFound. */
	virtual void HandleModelEventL(TInt aEvent) = 0;
	};


class MHlpDbObserver
/** Internal API to handle events reported by the search engine
@publishedAll
@released
*/
	{
public: // TInt aEvent should be a named enum
	virtual void HandleDbEventL(TInt aEvent) = 0;
	};

//
// Search types (these should be named, but SC cannot be broken until v7.0
//
/** Defines the search types for use with CHlpModel::SearchL(). 
@publishedAll
@released
*/
enum
	{
	/** Gets a list of index entries for all help files.

	Success is indicated by an EIndexListAvailable event; failure by EIndexListNoneFound. 
	The list can be retrieved using CHlpModel::LoadListL(). */
	EIndexList,
	/** Gets a list of categories for all help files.

	Success is indicated by an ECategoryListAvailable event; failure by ECategoryListNoneFound. 
	The list can be retrieved using CHlpModel::CategoryListL(). */
	ECategoryList,
	/** Gets a list of topics for a specified category.

	Success is indicated by an ETopicListAvailable event; failure by ETopicListNoneFound. 
	The list can be retrieved using CHlpModel::LoadListL(). */
	ETopicListForCategory,
	/** Gets a list of topics for a specified category UID.

	Success is indicated by an ETopicListAvailable event; failure by ETopicListNoneFound. 
	The list can be retrieved using CHlpModel::LoadListL(). */
	ETopicListForCategoryUID,
	/** Searches for a topic with the specified help context.

	A successful search generates an ETopicAvailable event. The topic can then be retrieved 
	using CHlpModel::LoadTopicL(). An unsuccessful search generates an ETopicNotFound event. */
	EContextSearch,
	/** Searches for index entries for the specified help item.

	A successful search generates an ETopicListAvailable event. The list can then be retrieved 
	using CHlpModel::LoadListL(). An unsuccessful search generates an ETopicListNoneFound event. */
	EIndexSearch,
	/** Searches the topic titles for the specified text.

	A successful search generates an ESearchListAvailable event. The list can then be 
	retrieved using CHlpModel::LoadListL(). An unsuccessful search generates an 
	ESearchListNoneFound event. */
	EQuickSearch,
	/** Searches the full text of topics for the specified text.

	A successful search generates an ESearchListAvailable event. The list can then be 
	retrieved using CHlpModel::LoadListL(). An unsuccessful search generates an 
	ESearchListNoneFound event. */
	EFullTextSearch,
	/** Searches for a topic with the specified ID.

	A successful search generates an ETopicAvailable event. The topic can then be 
	retrieved using CHlpModel::LoadTopicL(). An unsuccessful search generates an 
	ETopicNotFound event. */
	ETopicIdSearch
	};

//
// Search progress responses (this should be scoped as members of MHlpModelObserver
// and should also be named, but SC cannot be broken until v7.0
//
/** Search progress responses
@publishedAll
@released
*/
enum
	{
	ENoRecordsFound,
	ESearchInProgress,
	ESearchComplete
	};

//
// Search progress responses (ditto for naming and scoping)
//
/** Help model search result events
@publishedAll
@released
*/
enum
	{
	/** The search returned a category list.

	The list can be retrieved using CHlpModel::CategoryListL(). */
	ECategoryListAvailable,	
	/** The search did not return a category list. */
	ECategoryListNoneFound,
	/** The search returned a results list.

	The list can be retrieved using CHlpModel::LoadListL(). */
	ESearchListAvailable,
	/** The search did not return a results list. */
	ESearchListNoneFound,
	/** The search returned a topic list.

	The list can be retrieved using CHlpModel::LoadListL(). */
	ETopicListAvailable,		// Category expansion
	/** The search did not return a topic list. */
	ETopicListNoneFound,
	/** The search returned an index item list.

	The list can be retrieved using CHlpModel::LoadListL(). */
	EIndexListAvailable,
	/** The search did not return an index item list. */
	EIndexListNoneFound,
	/** The search returned a topic list from an index phrase search.

	The list can be retrieved using CHlpModel::LoadListL(). */
	EIndexSearchListAvailable,
	/** The search did not return a topic list from an index phrase search. */
	EIndexSearchListNoneFound,
	/** The search returned a topic.

	The topic can be retrieved using CHlpModel::LoadTopicL(). */
	ETopicAvailable,
	/** The search did not return a topic. */
	ETopicNotFound,
	/** The search is in progress. */
	EModelSearchInProgress,
	/** The search has been cancelled. */
	EHlpSearchCancelled
	};
	
	// Constants

/** Defines help model zoom sizes. 
@publishedAll
@released
*/
enum THlpZoomState
	{
	/** Small zoom. */
	EHlpZoomStateSmall = 0,
	/** Medium zoom. */
	EHlpZoomStateMedium = 1,
	/** Large zoom. */
	EHlpZoomStateLarge = 2
	};


class CHlpItem : public CBase
/** Encapsulates an individual item in a help file.

Note that item IDs are assigned in increasing numerical order by the help 
compiler and are not unique. Categories and help files are however specified 
by UID, and so are unique. 
@publishedAll
@released
*/
	{
public:
	static CHlpItem* NewL(const TDesC& aTitle, TUint32 aId, TUid aCategoryId, TUid aHelpFileUid);
	static CHlpItem* NewLC(const TDesC& aTitle, TUint32 aId, TUid aCategoryId, TUid aHelpFileUid);
	static CHlpItem* NewLC(const TDesC& aTitle, TUint32 aId, TUid aHelpFileUid);
	IMPORT_C ~CHlpItem();

public: // Access
	inline TUid CategoryUid() const 
	/** Gets the item's category ID.
	
	@return Category ID */
		{ return iCategoryUid; }
	inline TUid HelpFileUid() const 
	/** Gets the item's help file UID.
	
	@return Help file UID */
		{ return iHelpFileUid; }
	inline TUint32 Id() const 
	/** Gets the item's ID.
	
	@return Item's ID */
		{ return iId; }
	inline const TDesC& Title() const 
	/** Gets the item's title.
	
	@return Item's title */
		{ return *iTitle; }

public: // These should not be public, but I can't break SC (these were inherited from
		// the previous author.
	/** Item's title */
	HBufC* iTitle;
	/** Item's ID */
	TUint32 iId;

private:
	friend class CHlpList; // Needed for searching
	CHlpItem(TUint32 aId);
	CHlpItem(TUint32 aId, TUid aHelpFileUid);
	CHlpItem(TUint32 aId, TUid aCategoryId, TUid aHelpFileUid);
	void ConstructL(const TDesC& aTitle);

private: // Meta data required for correct restoration of topics
	TUid iCategoryUid;
	TUid iHelpFileUid;
	};



//
// ----> MHlpTitleArray 
//
class MHlpTitleArray : public MDesCArray
/** Interface to get a topic ID from an array index. 
@publishedAll
@released
*/
	{
public:
	/** Gets a topic ID for the specified index.
	
	@param aIndex Index of item to get
	@return Topic ID */
	virtual TUint32 At(TInt aIndex) const = 0;
	};



//
// ----> CHlpList 
//
class CHlpList : public CBase, public MHlpTitleArray
/** A list of help items (CHlpItem objects). 
@publishedAll
@released
*/
	{
public: // Static construct / destruct
	IMPORT_C static CHlpList* NewL();
	IMPORT_C static CHlpList* NewLC();
	IMPORT_C ~CHlpList();

public: // From MDesCArray
	IMPORT_C TInt MdcaCount() const;
	IMPORT_C TPtrC MdcaPoint(TInt aIndex) const;

public:
	// 'At' returns topic Id, but this function is next to useless because you can't
	// uniquely identify a help topic by topic id alone. You need to know 3 things:-
	// a) topic id, 2) category id, 3) help file uid. This information is all
	// encapsulated in CHlpItem so help app authors should use 'Item' instead.
	IMPORT_C TUint32 At(TInt aIndex) const;
	IMPORT_C CHlpItem* Item(TInt aIndex) const;
	IMPORT_C TInt Find(TUint32 aId);
	IMPORT_C void Reset();
	IMPORT_C void AppendL(CHlpItem* aItem);

private: // 2nd phase constructor
	void ConstructL();

private:
	// This is the array of help items that were located as a result of
	// performing a search.
	CArrayPtr<CHlpItem>* iList;
	};



//
// ----> CHlpTopic 
//
class CHlpTopic : public CBase
/** Encapsulates a help topic.

A help topic has text, a title, a category, and paragraph and character formatting. 
@publishedAll
@released
*/
	{
public:
	IMPORT_C static CHlpTopic* NewL();
	IMPORT_C static CHlpTopic* NewLC();
	IMPORT_C ~CHlpTopic();

public: 
	// This function will not behave as expected in the case where it is used to 
	// restore rich text that includes pictures. Instead, only the text and markup
	// will be restored. Help App authors should use 'CHlpModel::LoadTopic' instead.
	// I can't remove this as it would break S&BC.
	IMPORT_C void RestoreL(RDbView* aView);
	
	IMPORT_C CRichText* TopicText();
	IMPORT_C TDesC& TopicTitle();
	IMPORT_C TDesC& Category();

	inline CParaFormatLayer* ParaFormatLayer() const 
	/** Gets the topic paragraph formatting.
	
	@return Topic paragraph formatting */
		{ return iGlobalParaFormatLayer; }
	inline CCharFormatLayer* CharFormatLayer() const 
	/** Gets the topic character formatting.
	
	@return Topic character formatting */
		{ return iGlobalCharFormatLayer; }

private:
	void ConstructL();

private:
	friend class CHlpModel;

	TUint32 iTopicId;
	TBuf<KMaxTitleColumn> iTopicTitle;
	TBuf<KMaxTitleColumn> iCategory;

	CRichText* iTopicText;
	CParaFormatLayer* iGlobalParaFormatLayer;  
	CCharFormatLayer* iGlobalCharFormatLayer; 
	};




//
// ----> CHlpModel 
//
class CHlpModel : public CBase, public MHlpDbObserver, public MPictureFactory, public MRichTextStoreResolver
/** Help model interface.

It provides functions to search help files in various ways.

The interface implements MHlpDbObserver for help database events, and MRichTextStoreResolver 
and MPictureFactory to obtain pictures from rich text stores. 
@publishedAll
@released
*/
	{
public: // Construct / destruct
	IMPORT_C static CHlpModel* NewL(RFs& aFs, MHlpModelObserver* aObserver);
	IMPORT_C static CHlpModel* NewLC(RFs& aFs, MHlpModelObserver* aObserver);
	IMPORT_C ~CHlpModel();

public:	// Opens all the help files in \System\Help
	IMPORT_C void OpenL();
	IMPORT_C void CloseL();

public: // Opens specific help files
	IMPORT_C void OpenFileL(const TDesC& aFileName);
	IMPORT_C void CloseFileL(const TDesC& aFileName);

public: // Specialized searching
	IMPORT_C void ContextSearchL(TCoeHelpContext& aContext);
	IMPORT_C void CategoryUIDSearchL(TUid aCategoryUID);
	IMPORT_C void TopicSearchL(const CHlpItem& aHelpItem);
	IMPORT_C void IndexSearchL(const CHlpItem& aHelpItem);

		// Generic searching
	IMPORT_C void SearchL(TInt aType, TUint32 aId);
	IMPORT_C void SearchL(TInt aType, HBufC* aCriterion=NULL);
	IMPORT_C void SearchL(TInt aType, const TDesC& aCriterion);

public: // Cancel EFullTextSearch types - will return KErrArgument if not the correct type
	IMPORT_C TInt CancelSearch();

public: // Assumes that the search has already been performed and that the view is valid
	IMPORT_C void LoadTopicL(CRichText& aRichText, TDes& aTitle);
	IMPORT_C void LoadTopicL(CRichText& aRichText);
	IMPORT_C void LoadTopicL(CHlpTopic* aTopic);
	IMPORT_C void LoadListL(CHlpList* aList);
	IMPORT_C void CategoryListL(CDesCArray* aList);

public:	
	IMPORT_C void SetZoomSizeL(THlpZoomState aState = EHlpZoomStateMedium);
	IMPORT_C THlpZoomState ZoomSize() const;
	IMPORT_C void SetZoomFactors(THlpZoomState aZoomState, TInt aFactor);
	TInt CurrentZoomFactor() const;
	void RemoveHelpPicture(CHlpPicture* aHelpPicture);

public: // FROM MPictureFactory
	void NewPictureL(TPictureHeader& aHdr, const CStreamStore& aDeferredPictureStore) const;

public: // FROM MRichTextStoreResolver
	IMPORT_C const CStreamStore& StreamStoreL(TInt aPos) const;

public:
	// Check to see if any of the databases have matching meta data
	IMPORT_C TInt MatchUidL(TUid aUid);

	// Replace the current observer with another
	IMPORT_C void SetObserver(MHlpModelObserver* aObserver);

public:
	void NotifyHelpModelDestructionToPictures();

private: // Internal search API
	void DoSearchL(TInt aType, const TDesC& aCriterion);
	void DoNextSearchL();
	void ResetReadyForSearch();
	void ResetViews();
	void SetCriterionL(const TDesC& aCriterion);

private: // Event management
	void HandleDbEventL(TInt aEvent);
	void ReportEventToObserverL(TInt aEvent);

private: // Misc internal functions
	inline CHlpDatabase* CurrentDatabase() const;
	inline TInt DatabaseCount() const;
	inline void SetSearchType(TInt aSearchType);
	inline TInt CurrentSearchType() const;
	RDbView* CurrentView() const;
	TBool DiskPresent(TInt aDrive) const;
	CHlpFileList* BuildListForDriveLC(TDriveUnit aDrive, RFs& aFsSession) const;
	static void ResetAndDestroyArrayOfCHlpFileEntry(TAny* aObject);

private:
	CHlpModel(RFs& aFs, MHlpModelObserver& aObserver);
	void ConstructL();

private: // Member data
	RFs& iFsSession;
	
	// This is required in order to restore rich text pictures from the database
	CStreamStore* iCurrentRichTextStore;
	
	// Creates our SQL string based upon the search type and criteria
	CHlpSQLSearch* iSearch;

	// An array of all the help files currently available
	CHlpDatabases* iDatabases;

	// Receives help model notifications as various actions are performed
	MHlpModelObserver* iObserver;

	// Were any matching results found for this search
	TBool iFound;

	// What was being searched for
	HBufC* iCriterion;

	// What type of search was being performed
	TInt iSearchType;

	// Which database are we currently searching
	TInt iCurrentDb;

	// Transient category Uid used when creating category lists
	TUid iTransientCategoryUid;

	// What zoom size is used
	THlpZoomState iZoomSize;

	// Array to hold the bitmaps that are currently used in the help rich text
	CArrayPtr<CHlpPicture>* iPictures;

	// Array to hold the zoom factors that correspond to each zoom size
	CArrayFix<TInt>* iZoomFactors;
	};

//
// ----> CHlpModel (inlines)
//
inline TInt CHlpModel::DatabaseCount() const
	{
	return iDatabases->Count();
	}
inline void CHlpModel::SetSearchType(TInt aSearchType)
	{
	iSearchType = aSearchType;
	}
inline CHlpDatabase* CHlpModel::CurrentDatabase() const
	{
	return iDatabases->At(iCurrentDb);
	}
inline TInt CHlpModel::CurrentSearchType() const
	{
	return iSearchType;
	}



#endif

