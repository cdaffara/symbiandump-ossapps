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

#if !defined (__MIUTRSLV_H__)
#define __MIUTRSLV_H__

#include <mentact.h>
#include <msvstd.h>
#include <msvapi.h>
#include "MIUTHDR.H"

class CImMhtmlUri : public CBase
/**
@internalComponent
@released
*/
	{
public:
	static CImMhtmlUri* NewL(const TDesC& aUriText);
	static CImMhtmlUri* NewLC(const TDesC& aUriText);

	void MakeAbsoluteL(const CImMhtmlUri& aBaseUri);

	HBufC* TextL(TBool aIncludeScheme);

	TBool IsAbsolute() const;

	TBool CompareScheme(const TDesC& aScheme) const;

	HBufC* OriginalUriText();

	~CImMhtmlUri();

private:
	void ConstructL(const TDesC& aUriText);
	CImMhtmlUri();
	inline void ParseL();
	inline void Normalise();

	enum TImUriDelimiterType
	{
	EImUriNotDelimiter,
	EImUriScheme,
	EImUriNetLoc,
	EImUriPath,
	EImUriParameter,
	EImUriQuery,
	EImUriFragment,
	EImUriEnd
	};
	
	inline TImUriDelimiterType Delimiter(TInt& aIndex);
	void ChangeState(TImUriDelimiterType aDelimiterType);

private:
	enum TImUriParseState
		{
		EParsingFirstString,
		EParsingScheme,
		EParsingNetLoc,
		EParsingPath,
		EParsingParameter,
		EParsingQuery,
		EParsingFragment,
		};

	HBufC* iUriText;
	HBufC* iScheme;
	HBufC* iNetLoc;
	CDesCArray* iDirectoryPath;
	TBool iAbsolutePath;
	TBool iContainsFileName;
	HBufC* iParameters;
	HBufC* iQuery;
	HBufC* iFragment;

	TImUriParseState iState;
	};

/**
This class holds URI Content_Location, Content_Id and corresponding Linked entry. These entries will be cached in an array, defined in the class CImMhtmlUriResolver.
@internalComponent
@released
*/
NONSHARABLE_CLASS(CImCacheUriEntry) : public CBase
 { 
public:
 static CImCacheUriEntry* NewL(const TDesC& aContentLocation, const TDesC& aContentId, TMsvId aEntry);
 ~CImCacheUriEntry();
 
 HBufC* GetContentLocation();
 HBufC* GetContentId();
 TMsvId GetUriEntry();

 private:
 void ConstructL(const TDesC& aContentLocation, const TDesC& aContentId, TMsvId aEntry);
 CImCacheUriEntry();
 private:
 TMsvId iUriEntry;
 HBufC* iContentLocation;
 HBufC* iContentId;
 };

class CImMhtmlChildEntrySearcher : public CMsgActive
/**
@internalComponent
@released
*/
	{
public:
	static CImMhtmlChildEntrySearcher* NewL(CMsvEntry& aEntry, RPointerArray<CImCacheUriEntry>& aUriCacheArray);

	void StartL(TMsvId aEntry, TDesC& aUri, TRequestStatus& aStatus);
	TBool SearchResult(TMsvId& aSearchResultEntry);
	TInt CheckCacheForEntryId(TMsvId aId);
	~CImMhtmlChildEntrySearcher();

private:
	void DoRunL();
	void DoComplete(TInt& );
	void DoCancel();

	void CheckCurrentEntryL();
	inline TBool CheckContentDetailsL(const TDesC& aContentLocation, const TDesC& aContentId);

	CImMhtmlChildEntrySearcher(CMsvEntry &aEntry, RPointerArray<CImCacheUriEntry>& aUriCacheArray);

private:
	CMsvEntry& iEntry;
	HBufC* iUri;
	CMsvEntrySelection* iChildEntries;
	TMsvId iChildEntryIndex;
	TBool iFound;
	RPointerArray<CImCacheUriEntry>& iUriCacheArray;
	};



class CImMhtmlFileFinder : public CMsgActive
/**
@internalComponent
@released
*/
	{
public:
	static CImMhtmlFileFinder* NewL(CMsvEntry &aEntry, RPointerArray<CImCacheUriEntry>& aUriCacheArray);

	void FindL(const TDesC& aUri, TMsvId aBodyPartId,TBool aFileNameOnly, TRequestStatus &aStatus);
	TBool MatchFound() const;
	void Result(RFile& aFile, TMsvId& aEntryId) const;
	void Result(TFileName& aFileName, TMsvId& aEntryId) const;
	~CImMhtmlFileFinder();

private:
	void DoFindL();
	void DoCancel();
	void ConstructL(RPointerArray<CImCacheUriEntry>& aUriCacheArray);

	void DoRunL();
	void DoComplete(TInt& );

	CImMhtmlFileFinder(CMsvEntry &aEntry);

private:
	CMsvEntry& iCurrentEntry;
	TMsvId iCurrentBodyPartId;
	HBufC* iUri;
	TBool iMatchFound;
	CImMhtmlChildEntrySearcher* iChildEntrySearcher;
	TFileName iFileName;
	TMsvId iLinkedEntryId;
	mutable RFile	iFile;
	TBool 			iFileNameOnly;
	};





class CImMhtmlFirstPageFinder : public CMsgActive
/**
@internalComponent
@released
*/
	{
public:
	static CImMhtmlFirstPageFinder* NewL(CMsvEntry& aEntry);
	void Result(RFile& aFile, TMsvId& aEntryId) const;
	void Result(TFileName& aFileName, TMsvId& aEntryId) const;

	~CImMhtmlFirstPageFinder();
	void FindL(TMsvId aRootMessage,TBool aFileNameOnly, TRequestStatus& aStatus);

private:
	CImMhtmlFirstPageFinder(CMsvEntry& aEntry);
	void DoFindL();

	void ConstructL();

	void DoRunL();
	void DoComplete(TInt& );
	void DoCancel();
	inline void SetStartParameterL();
private:
	CMsvEntry& iCurrentEntry;
	CMsvEntrySelection* iEntryStack;
	TInt iEntryStackIndex;
	TMsvId iCurrentBodyPartId;
	
	TBool iHtmlPartFound;
	TMsvId iFirstHtmlPartId;
	TFileName iFileName;

	TBool iUnrelatedHtmlPartFound;
	TMsvId iFirstUnrelatedHtmlPartId;

	HBufC8* iStartParameter;

	TInt iRelatedDepthCounter;
	CMsvEntrySelection* iNextUnrelatedEntryStack;
	mutable RFile	iFile;
	TBool			iFileNameOnly;
	};




class CImMhtmlUriResolver : public CMsgActive
/**
@internalComponent
@released
*/
	{
public:
	static CImMhtmlUriResolver* NewL(CMsvEntry& aEntry);
	static CImMhtmlUriResolver* NewLC(CMsvEntry& aEntry);

	void ResolveL(const TDesC& aURI, const TDesC& aHtmlBase, TMsvId aCurrentPartId, TBool aFileNameOnly, TRequestStatus &aStatus);
	void FindFirstL(TMsvId aRootMessageId, TBool aFileNameOnly,TRequestStatus &aStatus);
	~CImMhtmlUriResolver();
	HBufC* FileNameL() const;

	// On return, will have the parameter is set to file handle 
	// and also returns the error code	
	TInt FileHandle(RFile& aFile) const;
	TMsvId LinkedEntryId() const;
	TBool CheckCacheForLinkedEntryL();

protected:
	void ConstructL();
	CImMhtmlUriResolver(CMsvEntry& aEntry);

private:
	void ResolveL(const TDesC& aHtmlBase, TMsvId aCurrentPartId);
	HBufC* GetContentLocationL(TMsvId aEntryId);

	void DoRunL();
	void DoComplete(TInt& );
	void DoCancel();

private:
	enum TImMhtmlUriResolveState
		{
		EImResolvingUri,
		EImTryingWithoutResolve,
		EImFindingFirstUri,
		EImFindingFile
		};

	TImMhtmlUriResolveState iState;

	CMsvEntry& iCurrentEntry;
	TMsvId iBodyPartId;
	CImMhtmlUri* iUri;

	TFileName iFileName;
	TMsvId iLinkedEntryId;
	TBool iResolved;

	TBool iTryingThisMessage;

	CImMhtmlFileFinder* iMhtmlFileFinder;
	CImMhtmlFirstPageFinder* iMhtmlFirstPageFinder;
	mutable RFile	iFile;	
	TBool			iFileNameOnly;
	RPointerArray<CImCacheUriEntry> iUriCacheArray;
	};

#endif

