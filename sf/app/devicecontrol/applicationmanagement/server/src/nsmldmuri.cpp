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
 * Description:  DM tree etc.
 *
 */

#include "nsmldmuri.h"

// ------------------------------------------------------------------------------------------------
//  NSmlDmURI
// ------------------------------------------------------------------------------------------------


// ------------------------------------------------------------------------------------------------
// TPtrC8 NSmlDmURI::ParentURI(const TDesC8& aURI)
// returns parent uri, i.e. removes last uri segment
// ------------------------------------------------------------------------------------------------
TPtrC8 NSmlDmURI::ParentURI(const TDesC8& aURI)
    {
    TBool onlyOneSeg = ETrue;
    TInt i;
    for (i=aURI.Length()-1; i>=0; i--)
        {
        if (aURI[i]==KNSmlDMUriSeparator)
            {
            onlyOneSeg = EFalse;
            break;
            }
        }
    if (onlyOneSeg)
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
    for (i=aURI.Length()-1; i>=0; i--)
        {
        if (aURI[i]==KNSmlDMUriSeparator)
            {
            break;
            }
        }
    if (i==0)
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

    if (aURI.Find(KNSmlDmUriDotSlash)==0)
        {
        offset = 2;
        }
    else
        {
        return aURI;
        }

    if (aURI.Length()>2&&aURI[aURI.Length()-1]==KNSmlDMUriSeparator)
        {
        endSlash = 1;
        }

    return aURI.Mid(offset, aURI.Length()-endSlash-offset);
    }

// ------------------------------------------------------------------------------------------------
// TPtrC8 NSmlDmURI::RemoveProp(const TDesC8& aURI)
// removes property from the uri
// ------------------------------------------------------------------------------------------------
TPtrC8 NSmlDmURI::RemoveProp(const TDesC8& aURI)
    {
    TInt offset = aURI.Find(KNSmlDmQuestionMark);
    if (offset!=KErrNotFound)
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
    for (i=aURI.Length()-1; i>=0; i--)
        {
        if (aURI[i]==KNSmlDMUriSeparator)
            {
            break;
            }
        }

    if (i>0)
        {
        return aURI.Left(i);
        }
    else
        {
        return KNullDesC8();
        }
    }

// ------------------------------------------------------------------------------------------------
// TPtrC8 NSmlDmURI::URISeg(const TDesC8& aURI,TInt aLocation,TInt aSegCount=1)
// Returns the aLocation:th URI segment
// ------------------------------------------------------------------------------------------------
/*TPtrC8 NSmlDmURI::URISeg(const TDesC8& aURI, TInt aLocation, TInt aSegCount/*=1*///)
    /*{
    TInt i, start;
    if (aLocation < 0)
        {
        return aURI.Mid(0, 0);
        }
    if (aLocation > 0)
        {
        for (start=0, i=0; (start<aURI.Length()) && (i<aLocation); start++)
            {
            if (aURI[start]=='/')
                {
                i++;
                }
            if (i==aLocation)
                {
                break;
                }
            }
        }
    else
        {
        start=-1;
        }
    // empty segment
    if (start+1 >= aURI.Length())
        {
        return aURI.Mid(0, 0);
        }
    // start points to beginning of segment
    for (i=start+1; i<aURI.Length(); i++)
        {
        if (aURI[i]=='/')
            {
            aSegCount--;
            if (aSegCount == 0)
                {
                break;
                }
            }
        }
    // i points to end of segment   
    return aURI.Mid(start+1, i-start-1);
    }*/

// ------------------------------------------------------------------------------------------------
// TInt NSmlDmURI::NumOfURISegs(const TDesC8& aURI)
// Returns the num of uri segs
// ------------------------------------------------------------------------------------------------
TInt NSmlDmURI::NumOfURISegs(const TDesC8& aURI)
    {
    TInt numOfURISegs = 1;
    for (TInt i=0; i<aURI.Length(); i++)
        {
        if (aURI[i]==KNSmlDMUriSeparator)
            {
            numOfURISegs++;
            }
        }
    return numOfURISegs;
    }

