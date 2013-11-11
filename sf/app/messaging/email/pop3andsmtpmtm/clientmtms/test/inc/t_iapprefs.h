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

#include <iapprefs.h>

class CT_iapprefs : public CBase
	{
	public:
		static CT_iapprefs* NewLC();
		~CT_iapprefs();
		void TestL();
		void HeapTestL();
	private:
		CImIAPPreferences* iPref;
	private:
		void TestAllButStoreL();

		void TestVersionL();
		void TestNoIAPsL();
		void TestAddIAPL();
		void TestRemoveIAPL();

		CT_iapprefs();
		CT_iapprefs(CT_iapprefs&); // no implementation, not to be used
		void ConstructL();

		void TestL(TBool aCondition);
		void PopulateChoicesL(int n);

		enum { KErrTestFailed = 7801 };

		struct PointlessObserver : MMsvSessionObserver
			{
			void HandleSessionEventL(MMsvSessionObserver::TMsvSessionEvent, void*, void*, void*){};
			} iObserver;
	};

