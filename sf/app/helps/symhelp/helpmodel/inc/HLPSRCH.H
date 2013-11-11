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

#ifndef __HLPSRCH_H__
#define __HLPSRCH_H__

#include <e32std.h>
#include <d32dbms.h>
#include <s32file.h>
#include "HLPDB.H"
#include "HLPMODEL.H"

class CHlpDatabase;
class MHlpDbObserver;



//
// ----> MHlpPrivObserver 
//
class MHlpPrivObserver
	{
public:
	virtual void HandleSearchEventL(TInt aEvent)=0;
	};



//
// ----> CHlpSQLBuffer 
//
class CHlpSQLBuffer : public CBase
	{
public:
	void ConstructL(TInt aBufferSize);
	CHlpSQLBuffer();
	~CHlpSQLBuffer();
	void AppendL(const TDesC& aDes);
	void AppendL(TInt aNum);
	void AppendSQLL(const TDesC& aDes);
	const TDesC& SearchStatement() const;
	void Reset();

private:
	void AppendTextL(const TDesC& aDes);
	
private:
	HBufC* iSQLStatement;
	};


//
// ----> CHlpSQLEvaluator 
//
class CHlpSQLEvaluator : public CActive
	{
public:
	static CHlpSQLEvaluator* NewL(TInt aPriority);
	static CHlpSQLEvaluator* NewLC(TInt aPriority);
	~CHlpSQLEvaluator();

	void Initialize(RDbView& aView, MHlpPrivObserver& aObserver);

protected:
	virtual void RunL();
	virtual void DoCancel();

private:
	void Start();
	CHlpSQLEvaluator(TInt aPriority);

private:
	MHlpPrivObserver* iObserver;
	RDbView* iView;
	};



//
// ----> CHlpSQLSearch 
//
class CHlpSQLSearch : public CBase, public MHlpPrivObserver
	{
public:
	static CHlpSQLSearch* NewL(MHlpDbObserver& aObserver);
	~CHlpSQLSearch();

public:
	void SetDatabase(CHlpDatabase& aDatabase);
	void SearchL(TInt aType, const TDesC& aCriterion);
	void CancelEvaluator();

	inline RDbView& View() const;

private:
	void ContextSearchL(const TDesC& aCriterion);
	void IndexIdSearchL(const TDesC& aCriterion);
	void TopicIdSearchL(const TDesC& aCriterion);
	void TextSearchL(const TDesC& aCriterion, TBool aFullText=EFalse);

private:
	void IndexListL();
	void CategoryListL();
	void TopicListForCategoryL(const TDesC& aCriterion);
	void TopicListForCategoryUIDL(const TDesC& aCriterion);
	void TopicIdSearchL();
	void BuildViewL();
	void Reset();

private: // observer stuff
	void HandleSearchEventL(TInt aEvent);
	inline void ReportEventToObserverL(TInt aEvent)		{ iObserver->HandleDbEventL(aEvent); }

private:
	CHlpSQLSearch(MHlpDbObserver& aObserver);
	void ConstructL();

private:
	enum TFlags
		{
		EFlagsNull					= 0x0000,
		EFlagsPerformSearch			= 0x0001,
		EFlagsAsynchronousSearch	= 0x0002,
		EFlagsHaveValidDatabase		= 0x0004
		};

private:
	TUint iFlags;
	CHlpSQLBuffer* iSQLStatement;
	CHlpSQLEvaluator* iSQLEvaluator;
	CHlpDatabase* iDatabase;
	RDbView* iView;
	MHlpDbObserver* iObserver;
	};



inline RDbView& CHlpSQLSearch::View() const
	{
	return *iView;
	}

#endif
