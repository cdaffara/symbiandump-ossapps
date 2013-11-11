/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Declarations for class CSPCallArray
*
*/


#ifndef CSPCALLARRAY_H
#define CSPCALLARRAY_H

#include <e32base.h>

#include "mcspcallinformation.h"

class CSPCall;

/**
* Handles call adding from calls not done by the plugin.
*
*  @lib csplugin.dll
*/
class CSPCallArray: public CBase,
                    public MCSPCallInformation
    {
    
    public: //Constructors and descructor    
    
        /**
        * Two-phased constructing for the array.
        *
        */
        static CSPCallArray* NewL( );

        /**
        * C++ default destructor
        */
        virtual ~CSPCallArray( );

        /**
         * Remove call from array
         * @param aCall call to be removed
         * @return result of removal
         */        
        TInt Add( CSPCall* aCall );
        
        /**
         * Remove call from array
         * @param aCall call to be removed
         * @return result of removal
         */
        TInt Remove( CSPCall* aCall );
        
        /**
         * Get call count.
         */
        TInt GetCallCount( );
        
        /**
         * Get call by index.
         */
        CSPCall* Get( TInt aIndex );
        
// from base class MCSPCallInformation

        /**
        * From MCSPCallInformation
        * Finds call pointer specified by call name.
        * @param aCallName
        * @return corresponding call pointer or NULL if not found.
        */
        CSPCall* FindCall( const TName& aCallName );
    
    private:
        /**
        * C++ default constructor
        * @param aObserver the observer for status change (incoming call)
        * @param aLine the line associated with the call
        * @param aLineId line identifier
        */
        CSPCallArray( );
                                          
    private: // data
        
        /**
        * Pointer array of call objects.     
        */
        RPointerArray<CSPCall> iCallArray;
        
    };

#endif // CSPCALLARRAY_H
