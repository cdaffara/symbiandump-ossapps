// Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include "cimapsyncdownloadrules.h"
#include "MIUT_ERR.H"

#ifdef SYMBIAN_ENABLE_SPLIT_HEADERS  
#include "miut_errconsts.h"
#endif

/**
Factory constructor.
*/
EXPORT_C CImapSyncDownloadRules* CImapSyncDownloadRules::NewL()
	{
	CImapSyncDownloadRules* self = NewLC();
	CleanupStack::Pop();
	return self;
	}

/**
Factory constructor.
*/
EXPORT_C CImapSyncDownloadRules* CImapSyncDownloadRules::NewLC()
	{
	CImapSyncDownloadRules* self = new(ELeave) CImapSyncDownloadRules();
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
	}

/**
Class constructor
*/
CImapSyncDownloadRules::CImapSyncDownloadRules()
	{
	}

/**
Second phase constructor
*/
void CImapSyncDownloadRules::ConstructL()
	{
	}

/**
Class destructor
*/
CImapSyncDownloadRules::~CImapSyncDownloadRules()
	{
	Reset();
	}

/**
Make copy of an instance of the class.

@return Pointer to new class on the heap. Ownership is transferred to the caller.
*/
EXPORT_C CImapSyncDownloadRules* CImapSyncDownloadRules::CopyL()
	{
	CImapSyncDownloadRules* dest = CopyLC();
	CleanupStack::Pop();
	return dest;
	}

/**
Make new copy of an instance of the class.
On return the copy will be on the cleanup stack.

@return Pointer to new class on the heap. Ownership is transferred to the caller.
*/
EXPORT_C CImapSyncDownloadRules* CImapSyncDownloadRules::CopyLC()
	{
	CImapSyncDownloadRules* dest = NewLC();

	TImapSyncDownloadRules newRule;
	TInt numRules;

	for (TInt type = EInboxRulesType; type <= EFolderRulesType; ++type)
		{
		numRules = iRulesList[type].Count();

		for (TInt rule = 0; rule < numRules; ++rule)
			{
			newRule.iBearerTypes = iRulesList[type][rule].iBearerTypes;
			newRule.iMailInfo = iRulesList[type][rule].iMailInfo;

			dest->iRulesList[type].AppendL(newRule);
			}
		}

	return dest;
	}

/**
Adds a new synchronisation download rule.

The position for the new rule is specified in the parameter list. If the
position is greater than the number of rules already in the list then the
new rule is added at the end of the list. If the position is less than the
number of rules already in the list, then the new rule is inserted at that
position and all subsequent rules moved down by one position. The actual
position where the rule was added is given by the return value.

Invalid arguments passed to the routine will cause a panic on debug builds
but a leave with KErrArgument on a release build.

@param aType Type of folder the rule is for (inbox or other folder)
@param aPos Position in the list to add the rule.
@param aBearerTypes Bearer types that will use the rule.
@param aMailInfo Partial mail info options to use during synchronisation

@return The position in the list where the rule was added, or KErrOverflow
        if the list is full up, or KErrAlreadyExists if there is already a
        rule for one of the bearer types.

@leave KErrArgument Argument passed in is invalid
*/
EXPORT_C TInt CImapSyncDownloadRules::AddRuleL(TRulesType aType, TInt aPos, TUint32 aBearerTypes, const TImImap4GetPartialMailInfo& aMailInfo)
	{
	__ASSERT_DEBUG(aPos >= 0 && aPos < KMaxImapSyncDownloadRules, gPanic(EImcmSyncDownloadRulesInvalidPos1));
	__ASSERT_DEBUG(aBearerTypes != 0, gPanic(EImcmSyncDownloadRulesInvalidBearerTypes1));

	if (aPos < 0 || aPos >= KMaxImapSyncDownloadRules || aBearerTypes == 0)
		{
		User::Leave(KErrArgument);
		}

	TInt numRules = iRulesList[aType].Count();

	if (numRules >= KMaxImapSyncDownloadRules)
		{
		return KErrOverflow;
		}

	for (TInt rule = 0; rule < numRules; ++rule)
		{
		if ((iRulesList[aType][rule].iBearerTypes & aBearerTypes) != 0)
			{
			return KErrAlreadyExists;
			}
		}

	TImapSyncDownloadRules newRule;
	newRule.iBearerTypes = aBearerTypes;
	newRule.iMailInfo = aMailInfo;

	TInt insertedPos = aPos;

	if (aPos < numRules)
		{
		iRulesList[aType].InsertL(newRule, aPos);
		}
	else
		{
		iRulesList[aType].AppendL(newRule);
		insertedPos = numRules;
		}

	return insertedPos;
	}

/**
Deletes a synchronisation download rule from a given position

Invalid arguments passed to the routine will cause a panic on debug builds
but a leave with KErrArgument on a release build.

@param aType Type of folder the rule is for (inbox or other folder)
@param aPos Position in the list to add the rule.

@return ETrue if the rule was deleted, EFalse otherwise

@leave KErrArgument Argument passed in is invalid
*/
EXPORT_C TBool CImapSyncDownloadRules::DeleteRuleL(TRulesType aType, TInt aPos)
	{
	__ASSERT_DEBUG(aPos >= 0 && aPos < KMaxImapSyncDownloadRules, gPanic(EImcmSyncDownloadRulesInvalidPos2));

	if (aPos < 0 || aPos >= KMaxImapSyncDownloadRules)
		{
		User::Leave(KErrArgument);
		}

	if (aPos < iRulesList[aType].Count())
		{
		iRulesList[aType].Remove(aPos);
		return ETrue;
		}
	
	return EFalse;
	}

/**
Deletes bearer types from the synchronisation download rules.

Every rule is examined to see if the bearer types associated with that rule
contain all of the passed in bearer types. If there is a match then the bearer
types are removed from the rule. If this results in all the bearer types for
the rule being removed then the rule is deleted and all subsequent rules
are moved up one position.

Invalid arguments passed to the routine will cause a panic on debug builds
but a leave with KErrArgument on a release build.

@param aType Type of folder the rule is for (inbox or other folder)
@param aBearerTypes Bearer types to remove.
@param aRuleRemoved If the bearer types are removed, this flag indicates if
       the removal resulted in the rule being removed as well.

@return The position of the rule in the list that contained all the bearer types,
        or KErrNotFound if no rule was found.

@leave KErrArgument Argument passed in is invalid
*/
EXPORT_C TInt CImapSyncDownloadRules::DeleteBearerTypesL(TRulesType aType, TUint32 aBearerTypes, TBool& aRuleRemoved)
	{
	__ASSERT_DEBUG(aBearerTypes != 0, gPanic(EImcmSyncDownloadRulesInvalidBearerTypes2));

	if (aBearerTypes == 0)
		{
		User::Leave(KErrArgument);
		}

	aRuleRemoved = EFalse;
	
	TInt numRules = iRulesList[aType].Count();

	for (TInt rule = 0; rule < numRules; ++rule)
		{
		if ((iRulesList[aType][rule].iBearerTypes & aBearerTypes) == aBearerTypes)
			{
			iRulesList[aType][rule].iBearerTypes &= ~aBearerTypes;

			if (iRulesList[aType][rule].iBearerTypes == 0)
				{
				iRulesList[aType].Remove(rule);
				aRuleRemoved = ETrue;
				}

			return rule;
			}
		}

	return KErrNotFound;
	}

/**
Gets the synchronisation download rule at a given position.

Invalid arguments passed to the routine will cause a panic on debug builds
but a leave with KErrArgument on a release build.

@param aType Type of folder the rule is for (inbox or other folder)
@param aPos Position of the rule in the list
@param aBearerTypes On successful completion, this holds the bearer types for the rule
@param aMailInfo On successful completion, this holds the mail info for the rule

@return ETrue if the rule was found, EFalse otherwise

@leave KErrArgument Argument passed in is invalid
*/
EXPORT_C TBool CImapSyncDownloadRules::GetRuleL(TRulesType aType, TInt aPos, TUint32& aBearerTypes, TImImap4GetPartialMailInfo& aMailInfo) const
	{
	__ASSERT_DEBUG(aPos >= 0 && aPos < KMaxImapSyncDownloadRules, gPanic(EImcmSyncDownloadRulesInvalidPos3));

	if (aPos < 0 || aPos >= KMaxImapSyncDownloadRules)
		{
		User::Leave(KErrArgument);
		}

	if (aPos < iRulesList[aType].Count())
		{
		aBearerTypes = iRulesList[aType][aPos].iBearerTypes;
		aMailInfo = iRulesList[aType][aPos].iMailInfo;
		return ETrue;
		}

	return EFalse;
	}

/**
Gets the partial mail info object from the synchronisation download rule for
a given bearer type.

The mail info is returned only if its associated bearer types contain all the
bearer types specified in the passed in value.

Invalid arguments passed to the routine will cause a panic on debug builds
but a leave with KErrArgument on a release build.

@param aType Type of folder the rule is for (inbox or other folder)
@param aBearerTypes The bearer types to search for
@param aMailInfo On successful completion, this holds the mail info for the rule

@return The position of the rule in the list that matched the bearer types, or
        KErrNotFound if no matching bearer type was found.

@leave KErrArgument Argument passed in is invalid
*/
EXPORT_C TInt CImapSyncDownloadRules::GetMailInfoL(TRulesType aType, TUint32 aBearerTypes, TImImap4GetPartialMailInfo& aMailInfo) const
	{
	__ASSERT_DEBUG(aBearerTypes != 0, gPanic(EImcmSyncDownloadRulesInvalidBearerTypes3));

	if (aBearerTypes == 0)
		{
		User::Leave(KErrArgument);
		}

	TInt numRules = iRulesList[aType].Count();

	for (TInt rule = 0; rule < numRules; ++rule)
		{
		if ((iRulesList[aType][rule].iBearerTypes & aBearerTypes) == aBearerTypes)
			{
			aMailInfo = iRulesList[aType][rule].iMailInfo;
			return rule;
			}
		}

	return KErrNotFound;
	}

/**
Reset the rules lists so that they are empty
*/
EXPORT_C void CImapSyncDownloadRules::Reset()
	{
	iRulesList[EInboxRulesType].Reset();
	iRulesList[EFolderRulesType].Reset();
	}
