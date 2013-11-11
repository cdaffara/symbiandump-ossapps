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
* Description:    Media pool interface
*
*/




#ifndef __M_GLXMEDIAPOOL_H__
#define __M_GLXMEDIAPOOL_H__

#include <e32std.h>

#include "glxmediaid.h"

class CGlxMedia;

/**
 * Interface to return media items
 * @author Aki Vanhatalo
 *
 * @internal reviewed 17/07/2007 by Kimmo Hoikka
 *
 * @ingroup mlm_media_list_manager_design
 */
class MGlxMediaPool
    {
public:
    /**
     * @param aIdSpaceId Id of Id space of aId
     * @param aId Id of media object
     * @return media object
     */
    virtual CGlxMedia* Media( const TGlxIdSpaceId& aIdSpaceId,
        const TGlxMediaId& aId ) const = 0;
    };
    
#endif // __M_GLXMEDIAPOOL_H__
