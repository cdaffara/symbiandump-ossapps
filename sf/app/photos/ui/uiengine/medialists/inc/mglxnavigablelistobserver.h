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
* Description:    Observer for navigable list
*
*/




#ifndef __M_GLXNAVIGABLELISTOBSERVER_H__
#define __M_GLXNAVIGABLELISTOBSERVER_H__

#include <e32base.h>
#include "glxlistdefs.h"
#include "mglxitemlistobserver.h"

/**
 * Observer for navigable list
 *
 * @author Aki Vanhatalo
 *
 * @internal reviewed 17/07/2007 by Kimmo Hoikka
 *
 * @ingroup mlm_media_list_manager_design
 */
class MGlxNavigableListObserver : public MGlxItemListObserver
    {
public:
    /**
     * Nofication of focus having been changed
     *
     * @param aType backward/forward/unknown
     * @param aNewIndex New focus index
     * @param aOldIndex Old focus index
     */
    virtual void HandleFocusChanged( NGlxListDefs::TFocusChangeType aType, 
        TInt aNewIndex, TInt aOldIndex ) = 0;
        
    /**
     * Notification that an item has been selected/deselected
     *
     * @param aIndex Index of the item that has been selected/deselected
     * @param aSelected Boolean to indicate selection/deselection
     */
    virtual void HandleItemSelected( TInt aIndex, TBool aSelected ) = 0;
    };
    
#endif // __M_GLXNAVIGABLELISTOBSERVER_H__

