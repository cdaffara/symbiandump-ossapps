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
* Description:   Observer for signal strength handler
*
*/



#ifndef MSIGNALSTRENGTHOBSERVER_H
#define MSIGNALSTRENGTHOBSERVER_H

//  INCLUDES
#include <e32base.h>

// CLASS DECLARATION

/**
*  Interface for a class which observes if there are any changes
*  in phone signal strength.
*/
class MSignalStrengthObserver
    {
    public:     // Constructors and destructor

        /**
        * Destructor.
        */
        virtual ~MSignalStrengthObserver() {};

    public:     // New functions

        /**
         * Notifies about updates in signal bars and strength.
         * @param aNewSignalValue New value for the signal strength.
         * @param aNewBarValue New value for the signal bars.
         */
        virtual void SignalStrengthAndBarUpdatedL( TInt aNewSignalValue, TInt aNewBarValue ) = 0;
    };

#endif      // MSIGNALSTRENGTHOBSERVER_H

// End of File
