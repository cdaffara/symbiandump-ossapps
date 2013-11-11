
/**
* Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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



#if (!defined __TESTCALINTERIMAPIINSTANCEDATA_H__)
#define __TESTCALINTERIMAPIINSTANCEDATA_H__

#include <calprogresscallback.h>

class CTestCalInterimApiInstanceData : public CActive, public MCalProgressCallBack
	{
	public:	
		
		static CTestCalInterimApiInstanceData* NewL();
		CTestCalInterimApiInstanceData();
		CTestCalInterimApiInstanceData(TRequestStatus* aStatus);
		~CTestCalInterimApiInstanceData();
		void Start();
		// from MCalProgressCallBack
		void Progress(TInt aPercentageCompleted);
		void Completed(TInt Completed);
		TBool NotifyProgress();
		
	private:

		// from CActive
		void Close();
		void DoCancel();
		void RunL();
	    
	private:
		
		TRequestStatus* iExtStatus;
		TRequestStatus* status;
	};
	
_LIT(KTestCalInterimApiInstanceData,"TestCalInterimApiInstanceData");


# endif

