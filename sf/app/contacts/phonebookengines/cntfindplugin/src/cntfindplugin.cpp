/*
* Copyright (c) 2002 Nokia Corporation and/or its subsidiary(-ies).
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
*     Contact model ECOM plugin for chinese find.
*
*/


// INCLUDE FILES
#include "cntfindplugin.h"
#include <FindUtil.h>

// ========================== MEMBER FUNCTIONS ===============================

// ---------------------------------------------------------------------------
// CAknFindUtilBase::NewL
// ---------------------------------------------------------------------------
//
CAknFindUtilBase* CAknFindUtilBase::NewL()
	{
	CAknFindUtilBase* self=new(ELeave) CAknFindUtilBase;
    CleanupStack::PushL( self );
    self->OpenL();
    CleanupStack::Pop();
	return self;
	}

// ---------------------------------------------------------------------------
// CAknFindUtilBase::~CAknFindUtilBase
// ---------------------------------------------------------------------------
//
CAknFindUtilBase::~CAknFindUtilBase()
	{
	delete iFindUtil;
	}

// ---------------------------------------------------------------------------
// CAknFindUtilBase::CAknFindUtilBase
// ---------------------------------------------------------------------------
//
CAknFindUtilBase::CAknFindUtilBase()
	{
	}

// ---------------------------------------------------------------------------
// CAknFindUtilBase::OpenL
// ---------------------------------------------------------------------------
//
void CAknFindUtilBase::OpenL()
	{
    if (!iFindUtil)
        {
        iFindUtil = CFindUtil::NewL();
        }
	}

// ---------------------------------------------------------------------------
// CAknFindUtilBase::Close
// ---------------------------------------------------------------------------
//
void CAknFindUtilBase::Close()
	{
    delete iFindUtil;
    iFindUtil = NULL;
	}

// ---------------------------------------------------------------------------
// CAknFindUtilBase::Match
// ---------------------------------------------------------------------------
//
TBool CAknFindUtilBase::Match(const TDesC& aContactsField, const TDesC& aWord)
	{
	return iFindUtil->Interface()->Match(aContactsField, aWord);
	}

// ---------------------------------------------------------------------------
// CAknFindUtilBase::IsWordValidForMatching
// ---------------------------------------------------------------------------
//
TBool CAknFindUtilBase::IsWordValidForMatching(const TDesC& aWord)
	{
    return iFindUtil->Interface()->IsWordValidForMatching(aWord);
	}

// ---------------------------------------------------------------------------
// CAknFindUtilBase::MatchRefineL
// ---------------------------------------------------------------------------
//
TBool CAknFindUtilBase::MatchRefineL
        (const TDesC& aItemString, const TDesC &aSearchText)
	{
	return iFindUtil->Interface()->MatchRefineL(aItemString, aSearchText);
	}

// End of File
