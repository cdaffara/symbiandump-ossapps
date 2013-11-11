/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  This file contains the header file of the MPECallOwner class.
*
*/



#ifndef MPECALLOWNER_H
#define MPECALLOWNER_H

//INCLUDES
#include <mpecall.h>

// CLASS DECLARATION

/**
*  Abstract base class for call owner.
*
*  @lib CallHandling.lib
*/
class MPECallOwner
    {
    public:
        
        /**
        * returns ETrue if call object found by state, NULL if not found.
        * @param aState, the wanted call state.
        */ 
        virtual TBool IsCallByState( 
            const TPEState aState ) const = 0;
        
        /**
        * returns ETrue if call object found by call ID, NULL if not found.
        * @param aCallId, the wanted call id.
        */    
        virtual TBool IsCallByCallId( 
            const TInt aCallId ) const = 0;
 
        /**
        * returns pointer to MPECall object by State, NULL if not found.
        * @param aState, the wanted call state.
        */ 
        virtual MPECall* CallByState( 
            const TPEState aState ) const = 0;
        
        /**
        * returns pointer to MPECall object by CallId, NULL if not found.
        * @param aCallId, the wanted call id.
        */    
        virtual MPECall* CallByCallId( 
            const TInt aCallId ) const = 0;
            
        /**
        * returns pointer to MPECall object by CallId, NULL if not found.
        * @param aName, the wanted name.
        */    
        virtual MPECall* CallByName( 
            const TName& aName ) const = 0;
        
        /**
        * Return active call count, other then idle state.
        * @param None
        */ 
        virtual TInt ActiveCallCount() const = 0;
    };

#endif // MPECALLOWNER_H

//End of file
