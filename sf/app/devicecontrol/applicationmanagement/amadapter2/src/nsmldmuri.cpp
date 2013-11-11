/*
* Copyright (c) 2005 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  DM Utilities
*
*/


#include "nsmldmuri.h"

// ===========================================================================
// NSmlDmURI
// ===========================================================================

// ------------------------------------------------------------------------------------------------
// TPtrC8 NSmlDmURI::ParentURI(const TDesC8& aURI)
// returns parent uri, i.e. removes last uri segment
// ------------------------------------------------------------------------------------------------
TPtrC8 NSmlDmURI::ParentURI(const TDesC8& aURI)
	{
	TBool onlyOneSeg = ETrue;
	TInt i;
	for(i=aURI.Length()-1;i>=0;i--)
		{
		if(aURI[i]==KNSmlDMUriSeparator)
			{
			onlyOneSeg = EFalse;
			break;
			}
		}
	if(onlyOneSeg)
		{
		return KNSmlDmRootUri();
		}
	else
		{
		return aURI.Left(i);
		}
	}

// ------------------------------------------------------------------------------------------------
// TPtrC8 NSmlDmURI::LastURISeg(const TDesC8& aURI)
// Returns only the last uri segemnt
// ------------------------------------------------------------------------------------------------
TPtrC8 NSmlDmURI::LastURISeg(const TDesC8& aURI)
	{
	TInt i;
	for(i=aURI.Length()-1;i>=0;i--)
		{
		if(aURI[i]==KNSmlDMUriSeparator)
			{
			break;
			}
		}
	if(i==0)
		{
		return aURI;
		}
	else
		{
		return aURI.Mid(i+1);
		}
	}

// ------------------------------------------------------------------------------------------------
// TPtrC8 NSmlDmURI::RemoveDotSlash(const TDesC8& aURI)
// return uri without dot and slash in start
// ------------------------------------------------------------------------------------------------
TPtrC8 NSmlDmURI::RemoveDotSlash(const TDesC8& aURI)
	{

	TInt offset = 0;
	TInt endSlash = 0;

	if(aURI.Find(KNSmlDmUriDotSlash)==0)
		{
		offset = 2;
		}
	else
		{
		return aURI;
		}

	if(aURI.Length()>2&&aURI[aURI.Length()-1]==KNSmlDMUriSeparator)
		{
		endSlash = 1;
		}

	return aURI.Mid(offset,aURI.Length()-endSlash-offset);
	}

// ------------------------------------------------------------------------------------------------
// TPtrC8 NSmlDmURI::RemoveProp(const TDesC8& aURI)
// removes property from the uri
// ------------------------------------------------------------------------------------------------
TPtrC8 NSmlDmURI::RemoveProp(const TDesC8& aURI)
	{
	TInt offset = aURI.Find(KNSmlDmQuestionMark);
	if(offset!=KErrNotFound)
		{
		return aURI.Left(offset); 
		}
	return aURI;
	}

// ------------------------------------------------------------------------------------------------
// TPtrC8 NSmlDmURI::RemoveLastSeg(const TDesC8& aURI)
// Removes last uri segment
// ------------------------------------------------------------------------------------------------
TPtrC8 NSmlDmURI::RemoveLastSeg(const TDesC8& aURI)
	{
	TInt i;
	for(i=aURI.Length()-1;i>=0;i--)
		{
		if(aURI[i]==KNSmlDMUriSeparator)
			{
			break;
			}
		}

	if(i>0)
		{
		return aURI.Left(i);
		}
	else
		{
		return KNullDesC8();
		}
	}


// ------------------------------------------------------------------------------------------------
// TPtrC8 NSmlDmURI::URISeg(const TDesC8& aURI,TInt aLocation)
// Returns the aLocation:th URI segment
// ------------------------------------------------------------------------------------------------
TPtrC8 NSmlDmURI::URISeg(const TDesC8& aURI,TInt aLocation)
	{
	TInt begin=0;
	TInt end;
	TInt num=0;

	for(end=0;end<aURI.Length();end++)
		{
		if(aURI[end]==KNSmlDMUriSeparator||end==aURI.Length()-1)
			{
			num++;
			if(num==aLocation)
				{
				break;
				}
			else
				{
				begin = end;
				}
			}
		}
	if(begin==0)
		{
		if(end==aURI.Length()-1)
			{
			return aURI;
			}
		else
			{
			return aURI.Left(end);
			}
		}
	else
		{
		if(end==aURI.Length()-1)
			{
			return aURI.Mid(begin+1,end-begin);
			}
		else
			{
			return aURI.Mid(begin+1,end-begin-1);
			}
		}
	}


// ------------------------------------------------------------------------------------------------
// TInt NSmlDmURI::NumOfURISegs(const TDesC8& aURI)
// Returns the num of uri segs
// ------------------------------------------------------------------------------------------------
TInt NSmlDmURI::NumOfURISegs(const TDesC8& aURI)
	{
	TInt numOfURISegs = 1;
	for(TInt i=0;i<aURI.Length();i++)
		{
		if(aURI[i]==KNSmlDMUriSeparator)
			{
			numOfURISegs++;
			}
		}
	return numOfURISegs;
	}

