/*
* Copyright (c) 2009-2009 Nokia Corporation and/or its subsidiary(-ies).
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



#ifndef _MGLXDATASOURCEREQUESTOBSERVER_H_
#define _MGLXDATASOURCEREQUESTOBSERVER_H_

// INCLUDES
#include <e32cmn.h>
#include "glxmediaid.h"
#include "glxrequest.h"

// FORWARD DECLARATIONS
class TGlxThumbnailRequest;
class CMPXMedia;

// CONSTANTS

// CLASS DECLARATION


/**
 *  MGlxDataSourceRequestObserver class 
 *
 *  Returns the result of a Request.
 *	@ingroup collection_component_design
 *  @internal reviewed 11/07/2007 by Dave Schofield
 */
class MGlxDataSourceRequestObserver 
	{
public:
	/**
	 * Asynchronous call to retrieve 
	 * 
	 * @param aListId list for this query relates to
	 * @return error code
	 */
	virtual void HandleResponse(CMPXMedia* aResponse, CGlxRequest* aRequest, const TInt& aError) = 0;
	};
		
#endif //_MGLXDATASOURCEREQUESTOBSERVER_H_