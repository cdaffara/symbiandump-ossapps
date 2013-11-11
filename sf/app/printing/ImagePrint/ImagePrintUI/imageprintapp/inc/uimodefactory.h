/*
* Copyright (c) 2004-2007 Nokia Corporation and/or its subsidiary(-ies).
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


#ifndef UIMODEFACTORY_H
#define UIMODEFACTORY_H

#include <e32def.h>
#include <e32base.h>

#include "cimageprintappui.h"
#include "muimodeobserver.h"
#include "muimode.h"

class CAiwMode;
class CIdleMode;
class CIFFactory;

/**
 * Factory class for UI mode objects
 */
class UIModeFactory
    {
    public: // Constructors and destructors
                
		/**
		 * Creates the UI mode specified with parametres
		 *
		 * @param aAiwMode Whether Aiw mode is the one to be created
		 * @param aUIModeObserver Pointer to UI mode observer
		 * @param aIFFactory Interface factory to be used
		 *
		 * @return The newly created UI mode object
		 */                
        static MUIMode* CreateL( TBool aAiwMode,
                                 MUIModeObserver* aUIModeObserver,
                                 CIFFactory& aIFFactory );
    
    protected:
    
        ~UIModeFactory();
    };

#endif // UIMODEFACTORY_H

//  End of File
