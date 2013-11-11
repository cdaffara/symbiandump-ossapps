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
* Description:  common email address object
*
*/



#ifndef CFSMAILADDRESS_H
#define CFSMAILADDRESS_H

#include <e32std.h>
#include <e32base.h>

//<cmail>
#include "CFSMailCommon.h"
//</cmail>

//<qmail>
#include <QExplicitlySharedDataPointer>

class NmAddressPrivate;
class NmAddress;
//</qmail>

/**
 *  email address handling
 *
 *  @lib FSFWCommonLib
 *  @since S60 v3.1
 */

NONSHARABLE_CLASS ( CFSMailAddress ) : public CBase
{
    public:

    /**
     * Two-phased constructor.
     *
     */
   	 IMPORT_C static CFSMailAddress* NewL();

    /**
     * Two-phased constructor.
     *
     * @param aNmAddress address with data to be shared
     */
    // <qmail>
    IMPORT_C static CFSMailAddress* NewL( const NmAddress& aNmAddress );
    // </qmail>
    /**
     * Two-phased constructor.
     *
     */
  	 IMPORT_C static CFSMailAddress* NewLC();

    /**
     * destructor
     */
  	 IMPORT_C ~CFSMailAddress();
      	
    /**
     * email address accessor
     *
     * @return email address owned by class
     */
  	 IMPORT_C TDesC& GetEmailAddress() const;
        
    /**
     * email address mutator
     *
     * @param aAddress email address to be stored
     */
  	 IMPORT_C void SetEmailAddress(const TDesC& aAddress);
  	 
    /**
     * display name accessor
     *
     * @return stored display name owned by class
     */
  	 IMPORT_C TDesC& GetDisplayName() const;
        
    /**
     * display name mutator
     *
     * @param aDisplayName display name to be stored
     */
  	 IMPORT_C void SetDisplayName(const TDesC& aDisplayName);

    /**
     * returns the NmAddress object
     *
     * @return NmAddress
     */
    // <qmail>
    IMPORT_C NmAddress GetNmAddress();
    // </qmail>
    
    private:
    // <qmail>        
     /**
      * constructor
      */
     CFSMailAddress();
    // </qmail> 
 

 	/**
     * Two-phased constructor
     */
  	 void ConstructL();

    /**
     * Two-phased constructor
     */
    // <qmail>
    void ConstructL( const NmAddress& aNmAddress );
    // </qmail>    

 private: // data

	// <qmail>
  	/**
     * email address - pointer descriptor to access QString object
     */
    mutable TPtrC  iEmailAddressPtr;

  	/**
     * display name - pointer descriptor to access QString object
     */
    mutable TPtrC  iDisplayNamePtr;

    /**
     * Reference to QT side of the address data object.
     */
    QExplicitlySharedDataPointer<NmAddressPrivate> iNmPrivateAddress;
    // </qmail>
};

#endif
