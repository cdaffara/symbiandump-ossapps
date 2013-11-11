/*
* Copyright (c) 2004 - 2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Factory for creating call control
*
*/



#ifndef CVTCTLFACTORY_H
#define CVTCTLFACTORY_H

//  INCLUDES
#include <e32base.h>
#include "VtCtlDefs.h"

// CONSTANTS
_LIT( KVtCtlLibraryName, "CSCallCtrl" );

// FORWARD DECLARATIONS
class MVtCtlEventObserver;
class MVtCtlCommandObserver;
class MVtCtlCallControl;

/**
* Factory class for instantiating call control.
*
*  @lib CSCallCtrl
*  @since 2.6
*/
NONSHARABLE_CLASS( CVtCtlFactory ) : public CBase
    {
    public:

        /**
        * Instantiates call control
        * @param aEventObserver
        * @param aCommandObserver
        * @return call control
        */
        virtual MVtCtlCallControl* CreateCallControlL(
            MVtCtlEventObserver& aEventObserver );

        /**
        * Destructs call control created with CreateCallControlL.
        * @param aCallControl pointer to call control instance
        */
        virtual void Delete( MVtCtlCallControl* aCallControl );
    };

/**
* Creates call control factory
* @return factory
*/
IMPORT_C CVtCtlFactory* CreateVtCtlFactory();

#endif      // CVTCTLFACTORY_H
            
// End of File
