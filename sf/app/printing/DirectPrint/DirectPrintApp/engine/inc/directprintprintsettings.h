/*
* Copyright (c) 2010 Kanrikogaku Kenkyusho, Ltd.
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of the License "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Kanrikogaku Kenkyusho, Ltd. - Initial contribution
*
* Contributors:
*
* Description:
* {Description of the file}
*
*/

// Protection against nested includes
#ifndef DIRECTPRINTPRINTSETTINGS_H
#define DIRECTPRINTPRINTSETTINGS_H

// System includes
#include <e32std.h>
#include <e32base.h>

// Forward declarations
class CDirectPrintBaseCapability;

/**
 *
 * Interface to use the device discovery functionality of Image Print Engine
 *
 */
class CDirectPrintPrintSettings : public CBase
    {
    public:     // Constructors and destructors

        /**
         *  Constructor
         */
        CDirectPrintPrintSettings();

        /**
         *  Destructor
         */
        virtual ~CDirectPrintPrintSettings();

    public:     // New methods

        CDirectPrintPrintSettings* CloneL();

	public:		// Data

        // Capability list
        RPointerArray<CDirectPrintBaseCapability> iCapabilities;
    };

#endif //  DIRECTPRINTPRINTSETTINGS_H

//  End of File
