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



#ifndef _MGLXDATASOURCEUPDATEOBSERVER_H_
#define _MGLXDATASOURCEUPDATEOBSERVER_H_

// INCLUDES
#include <e32cmn.h>
#include "glxmediaid.h"
#include <mpxmessage2.h>

// FORWARD DECLARATIONS

// CONSTANTS

// CLASS DECLARATION
		
/**
 *  MGlxDataSourceUpdateObserver class 
 *
 *  Returns update notifications.
 *	@ingroup collection_component_design
 *  @internal reviewed 11/07/2007 by Dave Schofield
 */
class MGlxDataSourceUpdateObserver 
	{
public:
	/**
	 * Notification that new items have been found in the data source (file 
	 * system, etc.)
	 *
	 * @param aItems ids of the items that were been found. 
	 */
	virtual void HandleMessage(CMPXMessage& aMessage) = 0;
		
	};

#endif //_MGLXDATASOURCEUPDATEOBSERVER_H_
