/*
* Copyright (c) 2008-2008 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Client information for a single call
*
*/


#ifndef CPECLIENTINFORMATION_H_
#define CPECLIENTINFORMATION_H_

#include <e32base.h>
#include "mpeclientinformation.h"

/**
 * Holds client information for a single call
 * 
 * This information is specific on a per-call basis.
 */
NONSHARABLE_CLASS( CPEClientInformation ) 
    : public CBase, public MPEClientInformation
    {
    
public:
        
        /**
         * Two-phased constructor.
         * 
         * @return  The newly created object. Ownership is transferred to the caller.
         */
        IMPORT_C static CPEClientInformation* NewL();
        
        /**
         * Destructor.
         */
        virtual ~CPEClientInformation();
        
protected:
        
        /**
         * Constructor.
         */
        CPEClientInformation();
    };

#endif /*CPECLIENTINFORMATION_H_*/
