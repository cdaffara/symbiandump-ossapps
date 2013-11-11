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
* Description:   
*
*/



#ifndef _MGLXDATASOURCE_H_
#define _MGLXDATASOURCE_H_

// INCLUDES
#include <e32cmn.h>
#include "mglxdatasourceupdateobserver.h"
#include "mglxdatasourcerequestobserver.h"

// FORWARD DECLARATIONS
class CGlxRequest;

// CONSTANTS

// CLASS DECLARATION

/**
 *  MGlxDataSource class 
 *
 *  Data Source  collates Data Source information.
 *  @lib GlxDataSource.lib
 *	@ingroup collection_component_design
 *  @internal reviewed 11/07/2007 by Dave Schofield
 */
class MGlxDataSource 
	{
public:
	/**
	 * Asynchronous call to retrieve 
	 * 
	 * @param aListId list for this query relates to
	 * @return error code
	 */
	virtual void DataSourceRequestL(CGlxRequest* aRequest, MGlxDataSourceRequestObserver& aObserver) = 0;
	/**
	 * Cancel asynchronous request call
	 * as each requestor can only have one active request the observer is sufficient to cancel the request.
	 * 
	 */
	virtual void CancelRequest(MGlxDataSourceRequestObserver& aObserver) = 0;
public:

	/**
	 * Since there should be only one data source manager for a server,
	 * clients can use this function to either create the instance or 
	 * return the pointer if the instance already exists.
	 *
	 * The pointers are reference counter, and the instance is deleted after
	 * there are no more clients. It is, therefore, necessary
	 * to release the instance by calling Close() when no longer needed.
	 *
	 * @returns a pointer to a new or existing list manager instance
	 */
	IMPORT_C static MGlxDataSource* OpenDataSourceL(const TUid& aDataSource, MGlxDataSourceUpdateObserver& aObserver);
	

	/**
	 * Decrements the reference count of the data source manager. Deletes the 
	 * data source manager if there are no more references.
	 */
	virtual void Close(MGlxDataSourceUpdateObserver& aObserver)=0;
	};



#endif //_MGLXDATASOURCE_H_