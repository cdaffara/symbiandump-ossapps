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
* Description:    Media userinterface
*
*/




#ifndef __M_GLXMEDIAUSER_H__
#define __M_GLXMEDIAUSER_H__

#include <e32std.h>

#include <mpxattribute.h>

#include "glxmediaid.h"

class CGlxMedia;

/**
 * Interface for media object users
 * @author Aki Vanhatalo
 *
 * @internal reviewed 17/07/2007 by Kimmo Hoikka
 *
 * @ingroup mlm_media_list_manager_design
 */
class MGlxMediaUser
    {
public:
    /**
     * For a given item, get all the attributes that may be requested for it
     *
     * @param aIndex index in media list of item
     * @param aAttributes Ordered list of attributes (ordered by AttributeOrder)
     * @return aAttributes with any new attributes added
     */
    virtual void GetRequiredAttributesL( TInt aIndex, RArray< TMPXAttribute >& aAttributes ) = 0;
    
    /**
     * Called to notify the list that attributes for the item are available
     *
     * @param aIndex index in media list of item that has attributes available
     * @param aAttributes list of attributes available
     */
    virtual void HandleAttributesAvailableL( TInt aIndex, 
        const RArray< TMPXAttribute >& aAttributes ) = 0;
    
    /**
     * Called to notify that the callee should remove all references to the 
     * media object (as it is about to be deleted)
     * @param aIndex index in media list of item to which references should be removed
     */
    virtual void RemoveReference( TInt aIndex ) = 0;

    /**
     * Notify of error
     *
     * @param aError Error code
     */
    virtual void HandleError( TInt aError ) = 0;
    };
    
#endif // __M_GLXMEDIAUSER_H__
