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

#ifndef __TCAL_BADRRULE_H__
#define __TCAL_BADRRULE_H__

#include "caltestlib.h"

#include <e32std.h>
#include <e32test.h>
#include <asclisession.h>
#include <asshdalarm.h>
#include <tz.h>
#include <calalarm.h>
#include <calentry.h>
#include <calentryview.h>
#include <calprogresscallback.h>
#include <calsession.h>
#include <calrrule.h>
#include <calinstance.h>
#include <calinstanceview.h>


class CBadRRule : public CBase
	{
public:
    static CBadRRule* NewL(CCalTestLibrary* aTestLib);
    ~CBadRRule();

	void TestBadRRuleL() ;	

private:
	void ConstructL();
	
	void CreateParentL() ;
	void CreateChild1L() ;
	void CreateChild2L() ;
	
	void ModifyEntriesL() ;
	void CheckInstancesL(RArray<TTime> &insTimes) ;
	
	void DeleteEntriesL() ;
	
	void CreateToDoEntryL() ;	
				
private:
	CCalTestLibrary* iTestLib;	
	CCalEntryView* iCalEntryView ;
	};


#endif // __TCAL_BADRRULE_H__
