/*
* Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   Interface for signal strength handler.
*
*/



#ifndef MSIGNALSTRENGTHHANDLER_H
#define MSIGNALSTRENGTHHANDLER_H

//  INCLUDES
#include <e32base.h>

// FORWARD DECLARATIONS
class MSignalStrengthObserver;

// CLASS DECLARATION

/**
*  Interface for a class which handles changes
*  in phone signal strength.
*/
class MSignalStrengthHandler
    {
    public:     // Constructors and destructor

        /**
        * Destructor.
        */
        virtual ~MSignalStrengthHandler() {};

    public:     // New functions

        /**
         * Returns ETrue if signal observing is supported, EFalse otherwise.
         * @return ETrue if signal observing is supported, EFalse otherwise.
         */
        virtual TBool ObservingSupported() = 0;
        
        /**
         * Returns ETrue if signal strength requests are supported, EFalse otherwise.
         * @return ETrue if signal strength requests are supported, EFalse otherwise.
         */
        virtual TBool RequestsSupported() = 0;

        /**
         * Sets the signal strength observer. May leave with KErrNotSupported
         * if signal strength observing is not supported.
         * @param aObserver Observer for signal strength. Ownership is not passed.
         */
        virtual void SetSignalStrengthObserverL( MSignalStrengthObserver* aObserver ) = 0;

        /**
         * Starts observing the signal strength. May leave with KErrNotSupported
         * if signal strength observing is not supported.
         */
        virtual void StartObservingL() = 0;
 
        /**
         * Stops the signal strength observing.
         */
        virtual void StopObservingL() = 0;

        /**
         * Returns the current signal strength. May leave with KErrNotSupported
         * if signal strength requests are not supported.
         * @return The current signal strength.
         */
        virtual TInt SignalStrengthL() = 0;
        
        /**
         * Returns the current bar value. May leave with KErrNotSupported
         * if bar value requests are not supported.
         * @return The current signal strength.
         */
        virtual TInt BarValueL() = 0;
        
    };

#endif      // MSIGNALSTRENGTHHANDLER_H

// End of File
