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
* Description:  This file contains the header file of the CPECallArrayOwner object
*
*/


#ifndef CPECALLARRAYOWNER_H
#define CPECALLARRAYOWNER_H

//INCLUDES
#include <pevirtualengine.h>
#include <mccecall.h>
#include <mpecallowner.h>
#include "cpesinglecall.h"

// CONSTANTS
// None

// MACROS
// None

// DATA TYPES
// None.

// FUNCTION PROTOTYPES
// None. 

// FORWARD DECLARATIONS

// CLASS DECLARATION
class MPEMessageSender;
class CCCE;

/**
*  Create call handling requests to Call Command object
*  @lib CallHandling.lib
*  @since Series60_4.0
*/
class CPECallArrayOwner 
        : public CBase,
          public MPECallOwner
    {
    public:
        /**
        * Destructor.
        */
        virtual ~CPECallArrayOwner();
        
        /**
        * Two-phased constructor.
        */
        static CPECallArrayOwner* NewL( 
            CCCE& iConvergedCallEngine, 
            MPEMessageSender& aOwner );

    private: //Constructors and descructor

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL(
            CCCE& iConvergedCallEngine, 
            MPEMessageSender& aOwner );
    
        /**
        * C++ default constructor.
        */
        CPECallArrayOwner();

    public:    //New functions
    
        /**
        * 
        * @param aCallId, the call id number
        */    
        CPESingleCall* CreateCallL( MPEMessageSender& aOwner );
        
        /**
        * 
        * @param aCallId, the call id number
        */    
        CPESingleCall* GetCallObject( const TInt aCallId ) const;

        /**
        * 
        * @param aIndex, index to call array
        */    
        CPESingleCall* GetCallObjectByIndex( const TInt aIndex ) const;

        /**
        * 
        * @param aCallId, the call id number
        */    
        TInt DeleteCallObject( const TInt aCallId );
        
        /**
        * Return call count
        * @param None
        */ 
        TInt Count() const;
        
        /**
        * Create data call object
        * @param aOwner
        */ 
        CPESingleCall* CreateDataCallL( MPEMessageSender& aOwner );
        
        /**
        * returns CPESingleCall call object by State, NULL if not found.
        * @param aState
        */ 
        CPESingleCall* CallPointerByState( TPEState aState ) const;
        
        /**
        * returns CPESingleCall call object by Name, NULL if not found.
        * @param aState
        */
        CPESingleCall* CallPointerByName( const TName& aName ) const;
       
    public:    // MPECallArrayOwner
        
        /**
        * returns ETrue if call object found by state, NULL if not found.
        * @param aState, the wanted call state.
        */ 
        IMPORT_C TBool IsCallByState( 
            const TPEState aState ) const;
        
        /**
        * returns ETrue if call object found by call ID, NULL if not found.
        * @param aCallId, the wanted call id.
        */    
        IMPORT_C TBool IsCallByCallId( 
            const TInt aCallId ) const;
            
        /**
        * returns pointer to MPECall object by CallId, NULL if not found.
        * @param aName, the wanted name.
        */    
        IMPORT_C MPECall* CallByName( 
            const TName& aName ) const;
        
        /**
        * Return active call count, other then idle state.
        * @param None
        */ 
        IMPORT_C TInt ActiveCallCount() const;
        
        /**
        * returns pointer to MPECall object by CallId, NULL if not found.
        * @param aCallId, the call id number
        */    
        IMPORT_C MPECall* CallByCallId( const TInt aCallId ) const;
        
        /**
        * returns pointer to MPECall object by state, NULL if not found.
        * @param aState
        */ 
        IMPORT_C MPECall* CallByState( TPEState aState ) const;

    private:
        
        /**
        * Create emergency call object
        * @param aOwner
        */    
        CPESingleCall* CreateEmergencyCallL( MPEMessageSender& aOwner );
        
    private: // Data

        // RPointerArray object which contains CPESingleCall objects.
        RPointerArray<CPESingleCall>* iCallArray; 

    };

#endif // CPECALLARRAYOWNER_H

//    End of File
