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
* Description:   Command request
*
*/



#ifndef GLXCOMMANDREQUEST_H_
#define GLXCOMMANDREQUEST_H_

// INCLUDES
#include <glxmediaid.h>
#include <mpxcommand.h>

#include "glxrequest.h"
#include "mglxdatasourceupdateobserver.h"
// FORWARD DECLARATIONS

// CONSTANTS

// CLASS DECLARATION

/**
 *  CGlxCommandRequest class 
 *
 *  @internal reviewed 11/07/2007 by Dave Schofield
 */
class CGlxCommandRequest : public CGlxRequest
	{
public:
	/** 
	 * Two-phase constructor: 
	 * @param aCommand the MPX command to be requested.
	 * @param aCollectionPluginUid The UID of the calling collection plugin.
	 * @param aObserver Oberver to inform of progress notifications
	 */
	static CGlxCommandRequest* NewL(const CMPXCommand& aCommand, const TUid aCollectionPluginUid, MGlxDataSourceUpdateObserver& aObserver);

	/**
	 * Destructor
	 */
	~CGlxCommandRequest();
	
	/**
	 * Returns a reference to the CMPXCommand object.
	 * @return a reference to the CMPXCommand object.
	 */
	inline const CMPXCommand& Command() const;
	
	/**
	 * Returns a reference to the MGlxDataSourceUpdateObserver passed in on construction.
	 * @return a reference to the MGlxDataSourceUpdateObserver passed in on construction.
	 */
	inline MGlxDataSourceUpdateObserver& DataSourceUpdateObserver();
	
private:
	/**
	 * Constructor.

	 * @param aCollectionPluginUid The UID of the calling collection plugin.
	 * @param aObserver Oberver to inform of progress notifications
	 */
	CGlxCommandRequest(TUid aCollectionPluginUid, MGlxDataSourceUpdateObserver& aObserver);
	
	/**
	 * Second stage constructor.
	 * @param aCommand the MPX command to be requested.
	 */
	void ConstructL(const CMPXCommand& aCommand);

private:
	/**
	 * a CMpxCommand
	 * (owned)
	 */
	CMPXCommand* iCommand;
	
	/**
	 * Data source update observer. 
	 * Used for progress updates.
	 */
	MGlxDataSourceUpdateObserver& iDataSourceUpdateObserver;
	};

#include "glxcommandrequest.inl"

#endif // GLXCOMMANDREQUEST_H_

