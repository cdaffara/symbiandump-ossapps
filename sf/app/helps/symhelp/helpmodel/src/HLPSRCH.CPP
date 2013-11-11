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

#include "HLPSRCH.H"

#include <d32dbms.h>

#include "Hlpsqlconsts.h"
#include "hlppanic.h"


//
// CHlpSQLSearch class
//
CHlpSQLSearch::CHlpSQLSearch(MHlpDbObserver& aObserver)
:	iObserver(&aObserver)
	{
	}

CHlpSQLSearch::~CHlpSQLSearch()
	{
	delete iSQLStatement;
	delete iSQLEvaluator;
	}

void CHlpSQLSearch::ConstructL()
	{
	iSQLStatement = new (ELeave) CHlpSQLBuffer();
	iSQLStatement->ConstructL(256);
	iSQLEvaluator = CHlpSQLEvaluator::NewL(CActive::EPriorityLow);
	}

CHlpSQLSearch* CHlpSQLSearch::NewL(MHlpDbObserver& aObserver)
	{
	CHlpSQLSearch* self = new(ELeave) CHlpSQLSearch(aObserver);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(); // self
	return self;
	}

void CHlpSQLSearch::SetDatabase(CHlpDatabase& aDatabase)
	{
	iDatabase = &aDatabase;
	iView = &aDatabase.View();
	}

void CHlpSQLSearch::CancelEvaluator()
	{
	Reset();
	}

void CHlpSQLSearch::Reset()
	{
	iSQLEvaluator->Cancel();
	iSQLStatement->Reset();
	iFlags &= (~EFlagsAsynchronousSearch|EFlagsPerformSearch);
	}

void CHlpSQLSearch::SearchL(TInt aType, const TDesC& aCriterion)
	{
	__ASSERT_ALWAYS(iDatabase, Panic(EHlpDatabaseNotInTransaction));

	Reset();
	switch(aType)
		{
	case EIndexList:
		IndexListL();
		break;
	case ECategoryList:
		CategoryListL();
		break;
	case ETopicListForCategory:
		TopicListForCategoryL(aCriterion);	
		break;
	case ETopicListForCategoryUID:
		TopicListForCategoryUIDL(aCriterion);	
		break;
	case EContextSearch:
		ContextSearchL(aCriterion);
		break;
	case EIndexSearch:
		IndexIdSearchL(aCriterion);
		break;
	case EQuickSearch:
		TextSearchL(aCriterion, EFalse);
		break;
	case EFullTextSearch:
		TextSearchL(aCriterion, ETrue);
		break;
	case ETopicIdSearch:
		TopicIdSearchL(aCriterion);
		break;
	default:
		Panic(EHlpInvalidQuery);
		break;
		}	

	BuildViewL();
	}

void CHlpSQLSearch::ContextSearchL(const TDesC& aCriterion)
	{
	iFlags |= EFlagsPerformSearch;
	iSQLStatement->AppendSQLL(KSQLContextSearch());
	iSQLStatement->AppendL(aCriterion);
	iSQLStatement->AppendSQLL(KSQLSingleInvComma());
	}

			
void CHlpSQLSearch::IndexIdSearchL(const TDesC& aCriterion)
	{
	iFlags |= EFlagsPerformSearch;
	iSQLStatement->AppendSQLL(KSQLIndexIdSearch());
	iSQLStatement->AppendL(aCriterion);
	}

void CHlpSQLSearch::TopicIdSearchL(const TDesC& aCriterion)
	{
	iSQLStatement->AppendSQLL(KSQLTopicSearchProlog());
	iSQLStatement->AppendSQLL(KSQLTopicIdColumn());
	iSQLStatement->AppendSQLL(KSQLEqualOperator());
	iSQLStatement->AppendL(aCriterion);
	}

void CHlpSQLSearch::TextSearchL(const TDesC& aCriterion, TBool aFullText)
	{
	// Text searching is asynchronous
	iFlags |= EFlagsAsynchronousSearch;
	iSQLStatement->AppendSQLL(KSQLTopicIdSearchProlog());
	iSQLStatement->AppendSQLL(KSQLTxtSrchTopicTitle());
	iSQLStatement->AppendL(aCriterion);
	if	(aFullText)
		{
		iSQLStatement->AppendSQLL(KSQLTxtSrchTopicText());
		iSQLStatement->AppendL(aCriterion);
		}
	iSQLStatement->AppendSQLL(KSQLTxtSrchCat());
	iSQLStatement->AppendL(aCriterion);
	iSQLStatement->AppendSQLL(KSQLTxtSrchSynonym());
	iSQLStatement->AppendL(aCriterion);
	iSQLStatement->AppendSQLL(KSQLLikeClosingInvComma());
	}





void CHlpSQLSearch::TopicIdSearchL()
	{
	iFlags &= (~EFlagsPerformSearch);
	if	(iView->CountL())
		{
		iSQLStatement->AppendSQLL(KSQLTopicIdSearchProlog());
		iView->FirstL();
		CDbColSet* colset = iView->ColSetL();
		TDbColNo topicCol = colset->ColNo(KSQLTopicIdColumn);
		delete colset;
		while(!iView->AtEnd())
			{
			iSQLStatement->AppendSQLL(KSQLTopicIdColumn());
			iSQLStatement->AppendSQLL(KSQLEqualOperator());
			iView->GetL();
			iSQLStatement->AppendL(iView->ColUint32(topicCol));
			iView->NextL();
			if	(!iView->AtEnd())
				iSQLStatement->AppendSQLL(KSQLOrOperator());
			}
		}
	}

void CHlpSQLSearch::CategoryListL()
	{
	iSQLStatement->AppendSQLL(KSQLCategoryList());
	iFlags &= (~EFlagsAsynchronousSearch);
	}

void CHlpSQLSearch::IndexListL()
	{
	iSQLStatement->AppendSQLL(KSQLIndexList());
	iFlags &= (~EFlagsAsynchronousSearch);
	}

void CHlpSQLSearch::TopicListForCategoryL(const TDesC& aCriterion)
	{
	iSQLStatement->AppendSQLL(KSQLTopicByCategoryProlog());
	iSQLStatement->AppendSQLL(KSQLSingleInvComma());
	iSQLStatement->AppendL(aCriterion);
	iSQLStatement->AppendSQLL(KSQLSingleInvComma());
	iFlags &= (~EFlagsAsynchronousSearch);
	}

void CHlpSQLSearch::TopicListForCategoryUIDL(const TDesC& aCriterion)
	{
	iSQLStatement->AppendSQLL(KSQLTopicByCategoryUIDProlog());
	iSQLStatement->AppendL(aCriterion);
	iFlags &= (~EFlagsAsynchronousSearch);
	}


void CHlpSQLSearch::HandleSearchEventL(TInt aEvent)
	{
	switch(aEvent)
		{
	case ESearchComplete:
		if	(iFlags & EFlagsPerformSearch)
			{
			ReportEventToObserverL(ESearchInProgress);
			Reset();
			TopicIdSearchL();
			BuildViewL();
			}
		else
			ReportEventToObserverL(ESearchComplete);
		break;
	default:
		ReportEventToObserverL(aEvent);
		break;
		}
	}


void CHlpSQLSearch::BuildViewL()
	{
	iView->Close();	
	TDbQuery query(iSQLStatement->SearchStatement(), EDbCompareFolded);
	User::LeaveIfError(iView->Prepare(iDatabase->Database(), query, TDbWindow::EUnlimited, RDbRowSet::EReadOnly));

	if	(iFlags & EFlagsAsynchronousSearch)
		iSQLEvaluator->Initialize(*iView, *this);
	else
		{
		User::LeaveIfError(iView->EvaluateAll());
		if	(iView->CountL())
			ReportEventToObserverL(ESearchComplete);
		else
			ReportEventToObserverL(ENoRecordsFound);
		}
	}


//
// CHlpSQLEvaluator class
//

CHlpSQLEvaluator::CHlpSQLEvaluator(TInt aPriority)
:	CActive(aPriority)
	{
	CActiveScheduler::Add(this);
	}

CHlpSQLEvaluator::~CHlpSQLEvaluator()
	{
	Cancel();
	}

CHlpSQLEvaluator* CHlpSQLEvaluator::NewLC(TInt aPriority)
	{
	CHlpSQLEvaluator* self = new (ELeave) CHlpSQLEvaluator(aPriority);
	CleanupStack::PushL(self);
	return self;
	}

CHlpSQLEvaluator* CHlpSQLEvaluator::NewL(TInt aPriority)
	{
	CHlpSQLEvaluator* self = CHlpSQLEvaluator::NewLC(aPriority);
	CleanupStack::Pop();
	return self;
	}

void CHlpSQLEvaluator::Initialize(RDbView& aView, MHlpPrivObserver& aObserver)
	{
	Cancel();
	iView = &aView;
	iObserver = &aObserver;
	Start();
	}


void CHlpSQLEvaluator::Start()
	{
	// Kludge an asynch request
	iStatus = KRequestPending;
	SetActive();
	TRequestStatus* pS = &iStatus;
	User::RequestComplete(pS, KErrNone);
	}

void CHlpSQLEvaluator::RunL()
	{
	if	(iStatus != KErrNone)
		{
		// Panic in debug mode
		__ASSERT_DEBUG(0, Panic(EHlpAsynchSearchError));

		// In release mode return nothing found
		iObserver->HandleSearchEventL(ENoRecordsFound);
		}
	else
		{
		TInt more = iView->Evaluate();
		TInt count = (more >= 0)? iView->CountL() : 0;
		if	(more > 0)
			{
			Start();
			iObserver->HandleSearchEventL(ESearchInProgress);
			}
		else 
			iObserver->HandleSearchEventL((count)? ESearchComplete : ENoRecordsFound);
		}
	}

void CHlpSQLEvaluator::DoCancel()
	{
	TRAP_IGNORE(iObserver->HandleSearchEventL(EHlpSearchCancelled)); // API is part of framework, could leave 
	}




//
// CHlpSQLBuffer class
//


void CHlpSQLBuffer::ConstructL(TInt aBufferSize)
	{
	iSQLStatement = HBufC::NewL(aBufferSize);
	}

CHlpSQLBuffer::CHlpSQLBuffer()
	{
	}

CHlpSQLBuffer::~CHlpSQLBuffer()
	{
	delete iSQLStatement;
	}

void CHlpSQLBuffer::AppendL(TInt aNum)
	{
	TBuf<32> buf;
	buf.AppendNum(aNum);
	AppendTextL(buf);
	}

void CHlpSQLBuffer::AppendL(const TDesC& aDes)
	{
	_LIT(KHlpModelInvertedComma, "'");

	// If the text contains inverted commas, then these must be escaped before
	// passing down to DBMS.
	TInt pos = aDes.Find(KHlpModelInvertedComma);
	if	(pos != KErrNotFound)
		{
		// Worst case scenario is that every character needs escaping...
		const TInt length = aDes.Length();
		HBufC* escapedText = HBufC::NewLC(length * 2);
		TPtr pText(escapedText->Des());
		pText.Copy(aDes);
		// Run through the text inserting extra apostrophes:
		for(TInt i=0; i<length; i++)
			{
			const TPtrC pChar(pText.Mid(i, 1));
			if	(pChar == KHlpModelInvertedComma)
				{
				pText.Insert(i, KHlpModelInvertedComma);

				// Skip over the newly inserted character
				++i;
				}
			}
		// Perform a normal append of the resulting escaped text:
		AppendTextL(*escapedText);
		CleanupStack::PopAndDestroy(escapedText);
		}
	else
		{
		// No apostrophes so no escaping needed:
		AppendTextL(aDes);
		}
	}

void CHlpSQLBuffer::AppendSQLL(const TDesC& aDes)
	{
	AppendTextL(aDes);
	}

void CHlpSQLBuffer::AppendTextL(const TDesC& aDes)
	{
	TPtr tempPtr(iSQLStatement->Des());
	const TInt KExtraBufferSize = 100;
	if ( (tempPtr.Length() + aDes.Length()) <= tempPtr.MaxLength() )
		{
		tempPtr.Append(aDes);
		}
	else
		{
		const TInt newSize = Max(KExtraBufferSize, aDes.Length()+iSQLStatement->Length());
		iSQLStatement = iSQLStatement->ReAllocL(newSize);
		iSQLStatement->Des().Append(aDes);
		}
	}

const TDesC& CHlpSQLBuffer::SearchStatement() const
	{
	return *iSQLStatement;
	}

void CHlpSQLBuffer::Reset()
	{
	iSQLStatement->Des().Zero();
	}


