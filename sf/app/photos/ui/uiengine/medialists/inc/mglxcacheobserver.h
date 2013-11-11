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
* Description:    Media item cache observer interface
*
*/




#ifndef M_GLXCACHEOBSERVER_H
#define M_GLXCACHEOBSERVER_H

#include <mpxattribute.h>
#include "glxmediaid.h"

class CGlxMedia;

/**
 *  MGlxCacheObserver
 *
 *  Media item cache observer interface
 *
 *  @lib glxmedialists.lib
 */
class MGlxCacheObserver
	{
public:	
	/**
	 * Called when new attributes for a media item are available
	 * @param aIdSpaceId The Id Space of the item
	 * @param aMediaId The media id of the item
	 * @param aAttributes The new attributes
	 * @param aMedia The media item
	 */
    virtual void HandleAttributesAvailableL(const TGlxIdSpaceId& aIdSpaceId, 
                        const TGlxMediaId& aMediaId, 
                        const RArray<TMPXAttribute>& aAttributes, 
                        const CGlxMedia* aMedia) = 0;
                        
	/**
	 * Called when a media item needs to be deleted
	 * @param aMediaId The media id of the item to be deleted
	 */
    virtual void CleanupMedia(const TGlxMediaId& aMediaId) = 0;
	};

#endif // M_GLXCACHEOBSERVER_H



