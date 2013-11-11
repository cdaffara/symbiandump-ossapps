/*
* Copyright (c) 2002-2007 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  This file contains the header file of the class CPEAudioFactory 
*                which is used to create CTelephonyAudioRouting
*
*/


#ifndef CPEAUDIOFACTORY_H
#define CPEAUDIOFACTORY_H

//  INCLUDES
#include <e32base.h>

// CONSTANTS
// None.

// MACROS
// None.

// DATA TYPES
// None.

// FUNCTION PROTOTYPES
// None.

// FORWARD DECLARATIONS
class CTelephonyAudioRouting;
class MTelephonyAudioRoutingObserver;
class CPEAudioToneUtility;
class MMdaAudioToneObserver;


// CLASS DECLARATION

/**
*  This file contains the header file of the class CPEAudioFactory, 
*  which is used to create CTelephonyAudioRouting
*
*  @lib audiohandling.lib
*  @since Series60_5.0
*/  

class CPEAudioFactory : public CBase
    {
    public: 
        
        IMPORT_C static CPEAudioFactory* NewLC();
                
       
        /**
        * creates CTelephonyAudioRouting.
        * @since S60 v5.0
        * @param  aAudioRoutingObserver, needed when creating CTelephonyAudioRouting.
        */
        IMPORT_C virtual CTelephonyAudioRouting* CreateTelephonyAudioRoutingL( 
            MTelephonyAudioRoutingObserver& aAudioRoutingObserver );
        
        /**
         * Creates CPEAudioToneUtility.
         * @since S60 v5.0
         * @param aToneObserver Observer of the CMdaAudioToneUtility
         */
        IMPORT_C virtual CPEAudioToneUtility* CreateAudioToneUtilityL(
            MMdaAudioToneObserver& aToneObserver );
    
protected:
        IMPORT_C CPEAudioFactory();
    };    
         
#endif      // CPEAudioFactory_H  
            
// End of File
