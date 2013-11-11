/*
* Copyright (c) 2005-2007 Nokia Corporation and/or its subsidiary(-ies).
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


#ifndef MUIMODE_H
#define MUIMODE_H

#include <e32def.h>
#include <e32base.h>

class  MDiscovery;

/**
 *
 * Base class for UI mode classes
 *
 */
class MUIMode
    {           
	public:
   
    virtual ~MUIMode(){};
    
    public: // New methods
                
        /**
         * Called when applaction starts. Finds the default printer or 
         * starts the full discovery if it is not available
         */
        virtual void SearchDefaultPrinterL() = 0;
        
        /**
         * Called when specific protocol is needed. 
         */
        virtual TBool ChangeDefaultPrinterL( TInt aProtocol ) = 0;
        
        
    };

#endif //  MUIMODE_H

//  End of File
