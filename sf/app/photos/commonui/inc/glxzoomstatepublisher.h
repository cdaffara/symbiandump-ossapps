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
* Description:   Utility class for publishing the zoom state
*
*/



#ifndef __CGLXZOOMSTATEPUBLISHER_H__
#define __CGLXZOOMSTATEPUBLISHER_H__

/**
 * @internal reviewed 5/02/2007 by Alex Birkett
 */
 
// INCLUDES
#include <e32std.h>

// CLASS DECLARATION

/**
 * NGlxZoomStatePublisher
 * Utility to publish the zooming state of photos
 */
namespace NGlxZoomStatePublisher
    {
    /**
     * Publishes the zooming state
     * @param aEnabled ETrue if it is possible to zoom
     */
    IMPORT_C void PublishStateL( TBool aEnabled );
    }

#endif // __CGLXZOOMSTATEPUBLISHER_H__
