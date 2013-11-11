/*
* Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef __SSASTARTUPWATCHER_H__
#define __SSASTARTUPWATCHER_H__

#include <e32base.h>
#include <startup.hrh>          // For start-up states
#include <startupdomaindefs.h>  // For start-up domains
#include <domaindefs.h>
#include <domainmember.h>       // For CDmDomain

typedef void (*TStartupCallBack)(TInt aValue);

/*!
    This class interacts with the Domain Manager to be kept aware of the current
    system startup state.
*/
class CSSAStartupWatcher : public CDmDomain
{

public:
	static CSSAStartupWatcher* NewL(TStartupCallBack aCallback);
	static CSSAStartupWatcher* New(TStartupCallBack aCallback);
	virtual ~CSSAStartupWatcher();

private:
	CSSAStartupWatcher(TDmHierarchyId aHierarchyId, 
	    TDmDomainId aDomainId,TStartupCallBack aCallback);
	void ConstructL();
	
	virtual void RunL();                   // from CActive
	
	void NotifyState(TInt aValue); 

private:
	
	TStartupCallBack iCallback; 
};

#endif // __SSASTARTUPWATCHER_H__
