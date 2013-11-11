// Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef PBAPCONTACTDBVIEWS_H
#define PBAPCONTACTDBVIEWS_H

#include <e32std.h>
#include <badesca.h>
#include <cntdb.h>
#include <cntdef.h>
#include <cntdbobs.h>
#include <cntviewbase.h>
#include <cntview.h>

#include "pbaprequest.h"


class MPbapContactDbViewsCallback
	{
public:
	virtual void HandleSearchAndSortComplete(TInt aError)=0;
	};


/**
Provides API to search database and sort the results into name or pronunciation
field order using contact database views 
*/	
class CPbapContactDbViews : public CBase, public MContactViewObserver, public MIdleFindObserver
	{
public:
	static CPbapContactDbViews* NewL(CContactDatabase& aDatabase);
	~CPbapContactDbViews();

public:
	HBufC* GetContactNameFromIdL(TContactItemId aContactId) const;
	void GetContactIdsMatchingCriteriaL(SymbianPBAP::TOrder aOrder, SymbianPBAP::TSearchAttribute aSearchAttribute, const TDesC& aSearchValue, CContactIdArray& aResults, MPbapContactDbViewsCallback& aCallback);
	void CancelSearchAndSortRequest();						 

	void CloseAllViews();
			
private:
	CPbapContactDbViews(CContactDatabase& aDatabase);

	void CreateNameViewL();
	void CreatePhoneticViewL();
	
	void AppendField(HBufC* aBuf, TUid aUid, const CContactItemFieldSet& aFieldSet, TBool aLast = EFalse) const;

	CContactIdArray* GetContactIdsInViewL(CContactViewBase& aView);
	TBool IsPhoneticViewRequired() const;
	
	static TInt SearchAndSortCallback(TAny* aAny);	
	void DoSearchAndSortL();
	void DoSortL();
	void DoSearchAndSortCallback();
	
	void FindInViewL(CContactViewBase& aView);
	void FindInPhoneFieldsL();

	void HandleIdleFindCompleteL();
	void HandleFindInViewCompleteL();

	void DoSortSearchResultsL(CContactIdArray& aContactIds);
	void ResortIdsInViewOrderL(CContactIdArray& aContactIds, CContactViewBase& aView);
	void CopyToResultsArrayL(const CContactIdArray& aContactIds);

	void NotifySearchAndSortComplete(TInt aError);
	void CancelAndCleanup();		
	
private: 
	//from MContactViewObserver	
	void HandleContactViewEvent(const CContactViewBase &aView, const TContactViewEvent &aEvent);

	//from MIdleFindObserver
	void IdleFindCallback();
	
private:
	enum TSearchAndSortOpState
		{
		EIdle,
		EPendingSearch,
		ESearching,
		ESorting
		};
private:
	CContactDatabase& iDatabase;	
	
	CContactLocalView* iNameView;
	CContactLocalView* iPhoneticView;
	TBool iNameViewReady;
	TBool iPhoneticViewReady;
	
	TSearchAndSortOpState iOpState;
	SymbianPBAP::TOrder	iOrder;
	SymbianPBAP::TSearchAttribute iSearchAttribute;
	HBufC* iSearchValue;

	CContactFindView* iFindView;
	CPtrCArray* iSearchValueArray;
		
	CIdleFinder* iIdleFinder;
	CContactItemFieldDef* iFindFieldDef;

	CAsyncCallBack* iAsyncCallback;

private:
	MPbapContactDbViewsCallback* iCallback;	// not owned	
	CContactIdArray* iResults; // not owned
	};

#endif //PBAPCONTACTDBVIEWS_H
