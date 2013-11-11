/*
* Copyright (c) 2007-2008 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description: This file defines class CFSFWImplementation.
*
*/


#ifndef __CFSFWIMPLEMENTATION_H
#define __CFSFWIMPLEMENTATION_H

#include <e32std.h>
#include <e32base.h>

// forward declarations
class CFSMailPluginManager;

/**
 *  email framework
 *
 *  @lib FSFWCommonLib
 *  @since S60 v3.1
 */
NONSHARABLE_CLASS ( CFSFWImplementation ) : public CBase
{
 public:
  	
    /**
     * Destructor.
     */  
     ~CFSFWImplementation();

    /**
     * Two-phased constructor.
     *
     * @param aConfiguration loaded plugins
     */
     static CFSFWImplementation* NewL(TInt aConfiguration);
  	
    /**
     * Two-phased constructor.
     *
     * @param aConfiguration loaded plugins
     */
     static CFSFWImplementation* NewLC(TInt aConfiguration);

    /**
     * plugin manager accessor
     *
     * @returns plugin manager reference
     */
     CFSMailPluginManager& GetPluginManager( );

protected:

    /**
     * ConstructL
     */
     void ConstructL(TInt aConfiguration);

private:

    /**
     * C++ default constructor.
     */
     CFSFWImplementation();

private: // data

    /** plugin manager */
     CFSMailPluginManager*	iPluginManager;
};

#endif // __CFSFWIMPLEMENTATION_H
