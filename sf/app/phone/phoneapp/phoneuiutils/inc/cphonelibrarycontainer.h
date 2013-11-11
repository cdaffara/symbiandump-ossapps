/*
* Copyright (c) 2005 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Container for factory library handles.
*
*/

#ifndef __CPHONELIBRARYCONTAINER_H
#define __CPHONELIBRARYCONTAINER_H

//  INCLUDES
#include <e32base.h>
#include <e32std.h>

/**
*  Container for factory library handles.
*
*  @since 4.0
*
*/
class CPhoneLibraryContainer : public CBase
    {    
    public:     // Construction / destruction

        /**
        * NewL function for creating the container.
        * @param None
        * @return None
        */
        IMPORT_C static CPhoneLibraryContainer* NewL();

        /**
        * Virtual Destructor
        * @param None
        * @return None
        */
        IMPORT_C virtual ~CPhoneLibraryContainer();

    public:     // New functions

        /**
        * Adds library handle to container. NOTE: Ownership of handle is 
        * moved to container! After calling that method given object isn't 
        * allowed to close or destroy.
        * @param Referece to library handle
        * @return None
        */
        IMPORT_C void AddLibraryL( const RLibrary& aLibraryHandle );

    private:  // Construction / destruction

        /**
        * Protected constructor.
        * @param None
        * @return None
        */
        CPhoneLibraryContainer();

    private: // Data

        RArray<RLibrary> iLibraries;
    };

#endif      // __CPHONELIBRARYCONTAINER_H
// End of File
