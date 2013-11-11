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
* Description:   Allows change the screen size
*
*/



#ifndef __GLX_RESOLUTION_MANAGER__
#define __GLX_RESOLUTION_MANAGER__

#include <e32base.h>


// Forward Declarations
class CGlxResolutionUtility;
    
/**
 * Class Description
 * Restricted Resolution Implementation
 * Dont use this class directly, 
 * use CGlxResolutionUtility instead
 * @author Loughlin
 */
NONSHARABLE_CLASS (CGlxResolutionManager) : public CBase
    {
public:
     /**
     * Symbian Constructor
     * @return a CGlxResolutionManager object.
     */
    IMPORT_C static CGlxResolutionManager* NewL();
    
     /**
     * Destructor
     */
    IMPORT_C ~CGlxResolutionManager();
    
    
private:
     /**
     * C++ Constructor
     */
    CGlxResolutionManager();

public:
    /**
     * Sets the screen size
     * @param The screens size 
     */
    IMPORT_C void SetScreenSize( TSize aScrnSz );

    
private:

    /// Onw: the client side resolution implementation 
    CGlxResolutionUtility* iResUtility;
    };

#endif // __GLX_RESOLUTION_MANAGER__

