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

/**
 @file
 @internalComponent
*/
   
#ifndef _TE_CNTSYNCVIEW_H_
#define _TE_CNTSYNCVIEW_H_

#include <phbksync.h>
#include <cntview.h>


/** 
 * Integration tests for phonebook synchronisation & Contacts Model ICC Local View
 */
class CPhbkSyncViewIntegrationTestBase : public CCntSyncTestStep
	{
public:
	virtual enum TVerdict doTestStepL() {return EFail; };

protected:	
	void DumpICCViewL();
	// required for the Contacts Model local view
	TContactViewEvent		iEvent;
	CContactLocalView*		iIccView;
    RContactViewSortOrder	iViewSortOrder;
	};
	

/**
 * Contact view event wait class.
 */ 
class CContactViewEventWait : 
        public CTimer, public MContactViewObserver
    {
    public:
        static CContactViewEventWait* NewL();
        ~CContactViewEventWait();

		TBool WaitForViewEvent
			(CContactViewBase* aView,
			TTimeIntervalSeconds aTimeOut,
			TContactViewEvent& aEvent, 
			TBool aWaitForAny);

    private:  // from CTimer
        void RunL();

    private: // From MContactViewObserver.
	    void HandleContactViewEvent
            (const CContactViewBase& aView, const TContactViewEvent& aEvent);

    private:  // Implementation
        CContactViewEventWait();
        void ConstructL();

    private:  // Data
        // Ref: the view to listen for events
        CContactViewBase*	iView;
		TBool				iTimedOut;
		TBool				iWaitForAny;
		TContactViewEvent	iEvent;
    };

class CPhbkViewICCSyncTest : public CPhbkSyncViewIntegrationTestBase
	{
public:
	static CPhbkViewICCSyncTest* NewL();
	virtual enum TVerdict doTestStepL();

	CPhbkViewICCSyncTest();
	};

class CPhbkViewICCLockedICCTest : public CPhbkSyncViewIntegrationTestBase
	{
public:
	static CPhbkViewICCLockedICCTest* NewL();
	virtual enum TVerdict doTestStepL();

	CPhbkViewICCLockedICCTest();
	};

class CPhbkViewICCLockedMixedTest : public CPhbkSyncViewIntegrationTestBase
	{
public:
	static CPhbkViewICCLockedMixedTest* NewL();
	virtual enum TVerdict doTestStepL();

	CPhbkViewICCLockedMixedTest();
	};

class CPhbkViewICCSinglePhonebookTest : public CPhbkSyncViewIntegrationTestBase
	{
public:
	virtual enum TVerdict doTestStepL();
	};

#endif
