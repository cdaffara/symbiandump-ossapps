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
* Description:    List of media items
*
*/




#include "glxmedialistarray.h"

#include <glxtracer.h>             //For logs

// ---------------------------------------------------------------------------
// CMediaListArray::InstanceL
// ---------------------------------------------------------------------------
//
CGlxMediaListArray* CGlxMediaListArray::InstanceL() 
	{
	TRACER("CGlxMediaListArray::InstanceL");
	
    return CGlxSingletonStore::InstanceL(&NewL);
    }

// ---------------------------------------------------------------------------
// CMediaListArray::Close
// ---------------------------------------------------------------------------
//
void CGlxMediaListArray::Close() 
	 {
	 TRACER("CGlxMediaListArray::Close");
	 
	 CGlxSingletonStore::Close(this);
	 }     

// ---------------------------------------------------------------------------
// CMediaListArray::NewL
// ---------------------------------------------------------------------------
//
CGlxMediaListArray* CGlxMediaListArray::NewL() 
	{
	TRACER("CGlxMediaListArray::NewL");
	
	return new (ELeave) CGlxMediaListArray;
	}

// ---------------------------------------------------------------------------
// CMediaListArray::~CMediaListArray
// ---------------------------------------------------------------------------
//
CGlxMediaListArray::~CGlxMediaListArray()
	{
	TRACER("CGlxMediaListArray::~CGlxMediaListArray");
	
	iArray.Close();
	}

// ---------------------------------------------------------------------------
// CMediaListArray::Array
// ---------------------------------------------------------------------------
//
RPointerArray<CGlxMediaList>& CGlxMediaListArray::Array()
	{
	TRACER("CGlxMediaListArray::Array");
	
	return iArray;
	}

