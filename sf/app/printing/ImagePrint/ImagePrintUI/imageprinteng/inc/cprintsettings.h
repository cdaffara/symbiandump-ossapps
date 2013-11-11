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


#ifndef CPRINTSETTINGS_H
#define CPRINTSETTINGS_H

#include <e32std.h>
#include <e32base.h>

class CBaseCapability;

/**
 *
 * Interface to use the device discovery functionality of Image Print Engine
 *
 */
class CPrintSettings
    : public CBase
    {			
    public:     // Constructors and destructors

        /**
         *  Constructor
         */
        CPrintSettings();

        /**
         *  Destructor
         */
        virtual ~CPrintSettings();
        
    public:     // New methods
    
        CPrintSettings* CloneL();                 
            
	public:		// Data

        // Capability list
        RPointerArray<CBaseCapability> iCapabilities;        
    };

#endif //  CPRINTSETTINGS_H

//  End of File
