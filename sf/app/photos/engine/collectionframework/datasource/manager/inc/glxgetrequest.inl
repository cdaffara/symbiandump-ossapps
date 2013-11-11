/*
* Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   Get request
*
*/



const RArray<TGlxMediaId>& CGlxGetRequest::MediaIds() const
	{
	return iMediaIdArray;	
	}

TArray<TMPXAttribute> CGlxGetRequest::Attributes() const
	{
	return iAttributeArray.Array();
	}

TArray<TMPXAttribute> CGlxGetRequest::CpiAttributes() const
	{
	return iCpiAttributeArray.Array();
	}

TBool CGlxGetRequest::CpiAttributesSet() const
	{
	return (0 != iCpiAttributeArray.Count());
	}
	
void CGlxGetRequest::AppendCpiAttributeL(const TMPXAttribute& aAttribute)
	{
	if (KErrNotFound == iCpiAttributeArray.Find(aAttribute))
	    {
    	iCpiAttributeArray.AppendL(aAttribute);
	    }
	}
	
