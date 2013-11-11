/*
* Copyright (c) 2008 - 2009 Nokia Corporation and/or its subsidiary(-ies).
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



#ifndef _C_GLXDATASOURCETASK_H_
#define _C_GLXDATASOURCETASK_H_

// INCLUDES
#include <e32cmn.h>
#include <ecom/ecom.h>

#include "mglxdatasource.h"
// FORWARD DECLARATIONS
class CGlxRequest;
class CMPXMedia;
class CGlxDataSource;
// CONSTANTS

// CLASS DECLARATION

/**
 *  CGlxDataSourceTask class 
 *
 *  Data Source Request is the base class for executing requests.
 *	@ingroup collection_component_design
 *  @internal reviewed 11/07/2007 by Dave Schofield
 */
class CGlxDataSourceTask : 	public CBase
	{
public:
	IMPORT_C virtual ~CGlxDataSourceTask();
	IMPORT_C CGlxDataSourceTask(CGlxRequest* aRequest, MGlxDataSourceRequestObserver& aObserver, CGlxDataSource* aDataSource);
	
public:
	TBool ObserverMatch(MGlxDataSourceRequestObserver& aObserver);
    
public:
	virtual void ExecuteRequestL() = 0;
	virtual void CancelRequest() = 0;

	/**
	 * Called when request is completed sucessfully or due to an error.
	 * This is the default implementation that can be re-defined by 
	 * deriving classes if required.
	 * @param aError Error that occurred (if any)
	 */
    IMPORT_C virtual void HandleRequestComplete(TInt aError);
    IMPORT_C void CreateResponseL();
    TBool IsDataSourceNeeded() const;
protected:
	IMPORT_C virtual void ListToMediaL(const RArray<TGlxMediaId>& aList); /// @todo overloaded until don't need name hack

protected:
	CGlxRequest* iRequest;
	CMPXMedia* iResponse;
	CGlxDataSource* iDataSource; // not owned
	MGlxDataSourceRequestObserver* iObserver;
	TBool iCancelled;
	};

#endif //_C_GLXDATASOURCETASK_H_
