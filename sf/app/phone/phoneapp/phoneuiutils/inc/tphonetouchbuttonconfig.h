/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description: Touch button configuration.
*
*/

#ifndef TPHONETOUCHBUTTONCONFIG_H
#define TPHONETOUCHBUTTONCONFIG_H

#include <e32std.h>
#include "phoneui.hrh"

const TInt    KTelButtonsConfigKeyCount = 1; 

/**
 *  Touch button configuration.
 *
 *
 *
 *  @lib PhoneUiUtils.lib
 *  @since S60 v5.0
 */
class TPhoneTouchButtonConfig
    {
public:
    TPhoneTouchButtonConfig();

    /**
     * Read configuration from resource file.
     *
     * @since S60 5.0
     * @return KErrNone if successful, otherwise error code.
     */
    TInt ReadConfiguration();
   
    /**
     * Returns resource id from configuration.
     *
     * @since S60 5.0
     * @param aButtons Requested configuration.
     * @param aDefaultResourceId This is returned, if config is not found.  
     * @return Resource id.
     */
    TInt ResourceId( TPhoneUIConfigurableButtons aButtons, 
                     TInt aDefaultResourceId ) const;

private:
    void ReadCenRepKeysL( 
            TFixedArray<TInt,KTelButtonsConfigKeyCount>& aKeyValues ) const;
    void ReadConfigResourceL( 
            const TFixedArray<TInt,KTelButtonsConfigKeyCount>& aKeyValues );
    
    inline TInt MapToCenRepKey( TInt aButtons ) const;
    
private:
    TFixedArray<TInt,EPhoneButtonConfigCount> iConfiguration;
    };

#endif // TPHONETOUCHBUTTONCONFIG_H
            
// End of File
