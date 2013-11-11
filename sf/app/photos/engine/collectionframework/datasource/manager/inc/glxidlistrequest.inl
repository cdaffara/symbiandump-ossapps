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
* Description:   Id list request
*
*/



// ----------------------------------------------------------------------------
// CGlxIdListRequest::CollectionPath
// ----------------------------------------------------------------------------
//
CMPXCollectionPath& CGlxIdListRequest::CollectionPath()
	{
	return iPath;
	}

// ----------------------------------------------------------------------------
// CGlxIdListRequest::ContainerId()
// ----------------------------------------------------------------------------
//
const TGlxMediaId& CGlxIdListRequest::ContainerId() const
	{
	return iMediaId;	
	}