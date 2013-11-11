/*
* Copyright (c)  Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  MR organizer definition
*
*/


#ifndef M_MRINFOORGANIZER_H
#define M_MRINFOORGANIZER_H

#include <e32base.h>

/**
 * MMROrganizer defines meeting request organizer.
 */
class MMROrganizer
    {
public: // Destruction

    /**
     * Virtual destructor.
     */
    virtual ~MMROrganizer()  { }

public: // Interface
    /**
     * Sets MR organizer email address.
     * @param aAddress Email address.
     */
    virtual void SetAddressL(const TDesC& aAddress ) = 0;

    /**
     * Fetches MR organizer email address.
     * @return MR Organzier email address. KNullDesC, if address is not set.
     */     
    virtual const TDesC& Address() const = 0;
    
    /**
     * Sets MR organizer common name.
     * @param aCommonName MR organizer common name.
     */
    virtual void SetCommonNameL(const TDesC& aCommonName) = 0; 
    
    /**
     * Fetches MR Organizer common name.
     * @return MR Organzer common name. KNullDesC, if common name is not set.
     */
    virtual const TDesC& CommonName() const = 0;
    };


#endif // M_MRINFOORGANIZER_H
