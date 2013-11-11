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
* Description:    Observer of list of media items
*
*/




#ifndef __M_GLXITEMLISTOBSERVER_H__
#define __M_GLXITEMLISTOBSERVER_H__

#include <e32std.h>

/**
 * Observer for item list
 *
 * @author Aki Vanhatalo
 *
 * @internal reviewed 17/07/2007 by Kimmo Hoikka
 *
 * @ingroup mlm_media_list_manager_design
 */
class MGlxItemListObserver 
    {
public:
    /**
     * Notification of item addition
     * @param aAddedAtIndex Index at which items were added
     * @param aCount count of items that were added
     */
    virtual void HandleItemsAdded( TInt aAddedAtIndex, TInt aCount ) = 0;
    
    /**
     * Notification of item removal
     * @param aRemovedFromIndex Index at which items were added
     * @param aCount count of items that were added
     */
    virtual void HandleItemsRemoved( TInt aRemovedFromIndex, TInt aCount ) = 0;
    };
    
#endif // __M_GLXITEMLISTOBSERVER_H__
