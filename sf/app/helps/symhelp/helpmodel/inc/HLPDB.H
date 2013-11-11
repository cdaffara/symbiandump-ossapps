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

#ifndef __HLPDB_H__
#define __HLPDB_H__

// System includes
#include <e32std.h>
#include <d32dbms.h>
#include <s32file.h>
#include <badesca.h>

// User includes
#include "hlpconstants.h"
#include "HLPMODEL.H"

// Classes referenced
class CFbsBitmap;


//
// ----> CHlpDatabase  (container class for the help database)
//
class CHlpDatabase : public CBase
	{
public:
	static CHlpDatabase* NewLC(RFs& aFs);
	static CHlpDatabase* NewLC(RFs& aFs, const TDesC& aFileName);
	~CHlpDatabase();

public:
	void OpenL(const TDesC& aFileName);
	void Close();

public: // new
	void AppendCategoryListL(CDesCArray& aList);
	TInt TopicIdSearchL(const TUid aCategoryUid, const TUint aTopicId);
	TInt ContextSearchL(const TDesC& aContext);
	CFbsBitmap* ImageForIdLC(TInt aImageId, THlpZoomState aZoomState);
	TInt ImageCountForIdL(TInt iImageId);

private:
	inline void CloseView();

public:
	TBool MatchUidL(TUid aUid);
	inline TInt ViewHasRecordsL() const;
	inline RDbStoreDatabase& Database() const;
	inline RDbView& View() const;
	inline CStreamStore& StreamStore() const;
	inline const TDesC& FileName() const;
	inline void SetViewReady(TBool aReady);
	inline TBool ViewReady() const;
	inline TUid HelpFileUid() const;

private:
	CHlpDatabase(RFs& aFs);
	void ConstructL();

private:
	TUid iHelpFileUid;
	TFileName iFileName;
	TBool iViewReady;

	/**
	* Identifies whether the help file being accessed
	* is of the old format or not.
	*/
	TBool iOldHelpFileFormat;

	RFs& iFs;
	RDbView* iView;
	RDbStoreDatabase*  iDatabase;
	CArrayFixFlat<TUid>* iUids;
	RDbTable iImageTable;

	CPermanentFileStore* iStore;
	};


//
// ----> CHlpDatabase (inlines)
//
inline TInt CHlpDatabase::ViewHasRecordsL() const
	{
	return iView->CountL();
	}
inline RDbStoreDatabase& CHlpDatabase::Database() const
	{
	return *iDatabase;
	}
inline RDbView& CHlpDatabase::View() const
	{
	return *iView;
	}
inline CStreamStore& CHlpDatabase::StreamStore() const
	{
	return *iStore;
	}
inline const TDesC& CHlpDatabase::FileName() const
	{
	return iFileName;
	}
inline void CHlpDatabase::SetViewReady(TBool aReady)
	{
	iViewReady = aReady;
	}
inline TBool CHlpDatabase::ViewReady() const
	{
	return iViewReady;
	}
inline void CHlpDatabase::CloseView()
	{
	View().Close();
	}
inline TUid CHlpDatabase::HelpFileUid() const
	{
	return iHelpFileUid;
	}

#endif
