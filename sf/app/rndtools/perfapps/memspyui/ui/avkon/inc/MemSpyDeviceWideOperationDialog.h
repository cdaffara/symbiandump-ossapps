/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies). 
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:
*
*/

#ifndef MEMSPYDEVICEWIDEOPERATIONDIALOG_H
#define MEMSPYDEVICEWIDEOPERATIONDIALOG_H

// System includes
#include <AknProgressDialog.h>
#include <AknWaitDialog.h>
#include <e32cmn.h>

// Engine includes
#include <memspy/engine/memspydevicewideoperations.h>
#include <memspyengineclientinterface.h>
#include <memspysession.h>

//#include "MemSpyAppUi.h"

// Classes referenced
//class RMemSpySession;

enum TDeviceWideOperation
{
	OutputPhoneInfo = 0,
	    
	OutputDetailedPhoneInfo,
	    
	OutputHeapInfo,
	    
	OutputCompactHeapInfo,
	    
	OutputHeapCellListing,
	    
	OutputHeapData,
	    
	OutputStackInfo,
	    
	OutputCompactStackInfo,
	    
	OutputUserStackData,
	    
	OutputKernelStackData
};


class MMemSpyDeviceWideOperationDialogObserver
    {
public:
    virtual void DWOperationStarted() = 0;
    virtual void DWOperationCancelled() = 0;
    virtual void DWOperationCompleted() = 0;
    };


class CMemSpyDwoProgressTracker : public CActive, public MProgressDialogCallback
{
public:
	CMemSpyDwoProgressTracker(RMemSpySession &aSession);
	
	virtual ~CMemSpyDwoProgressTracker();
	
	void Start();
	
	void Cancel();
	
	TMemSpyDeviceWideOperationProgress Progress() { return iProgress; }
	
	CAknProgressDialog* ProgressDialog() { return iProgressDialog; }
	
	void UpdateProcessDialogL( TInt aProgress, const TDesC& aProgressText );

protected: // from CActive
	
	void DialogDismissedL(TInt aButtonId);
	
	virtual void RunL();
	 
	virtual void DoCancel();
	 
	virtual TInt RunError(TInt aError);
	
private:
	TMemSpyDeviceWideOperationProgress iProgress;
	RMemSpySession iSession;	
	
	CEikProgressInfo* iProgressInfo;
	CAknProgressDialog* iProgressDialog;		
};






class CMemSpyDwoTracker : public CActive
{	
public:	
	CMemSpyDwoTracker(RMemSpySession &aSession, TDeviceWideOperation aOperation);
	
	virtual ~CMemSpyDwoTracker();
	
	void Start();
	
	void Cancel();		
	
protected: // from CActive
	
	virtual void RunL();
	 
	virtual void DoCancel();
	 
	virtual TInt RunError(TInt aError);		
	
private:
	RMemSpySession iSession;
	CMemSpyDwoProgressTracker *iProgressTracker;	
	TDeviceWideOperation iOperation;
};







class CMemSpyDeviceWideOperationDialog : public CBase, public MProgressDialogCallback, public MMemSpyDeviceWideOperationsObserver
    {
public:
    //static void ExecuteLD( CMemSpyEngine& aEngine, MMemSpyDeviceWideOperationDialogObserver& aObserver, CMemSpyDeviceWideOperations::TOperation aOperation );
	//static void ExecuteLD( RMemSpySession& aSession, MMemSpyDeviceWideOperationDialogObserver& aObserver, CMemSpyDeviceWideOperations::TOperation aOperation );
	static void ExecuteLD( RMemSpySession& aSession, TDeviceWideOperation aOp );
    ~CMemSpyDeviceWideOperationDialog();
    
    //added
    static CMemSpyDwoTracker* CreateDeviceWideOperation( RMemSpySession& aSession, TDeviceWideOperation aOp );

private:
    //CMemSpyDeviceWideOperationDialog( CMemSpyEngine& aEngine, MMemSpyDeviceWideOperationDialogObserver& aObserver );
    //CMemSpyDeviceWideOperationDialog( RMemSpySession& aSession, MMemSpyDeviceWideOperationDialogObserver& aObserver );
    CMemSpyDeviceWideOperationDialog( RMemSpySession& aSession );
    //void ExecuteL( CMemSpyDeviceWideOperations::TOperation aOperation );
    void ExecuteL( TDeviceWideOperation aOp );

public: // API
    void Cancel();

private: // From MProgressDialogCallback
    void DialogDismissedL( TInt aButtonId );

private: // From MMemSpyDeviceWideOperationsObserver
    void HandleDeviceWideOperationEvent( TEvent aEvent, TInt aParam1, const TDesC& aParam2 );

private: // Internal methods
    void SetDialogCaptionL( const TDesC& aText );

private: // Member data
    //CMemSpyEngine& iEngine;
    RMemSpySession iSession;	
    //MMemSpyDeviceWideOperationDialogObserver& iObserver;
    TBool iForcedCancel;
    CEikProgressInfo* iProgressInfo;
    CAknProgressDialog* iProgressDialog;
    CMemSpyDeviceWideOperations* iOperation;        
    };

#endif
