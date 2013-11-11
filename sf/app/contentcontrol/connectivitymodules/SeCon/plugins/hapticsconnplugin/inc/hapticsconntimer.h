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
* Description:  Connection shutdown timer for haptics server connection.
*
*/


#ifndef C_HAPTICSCONNTIMER_H
#define C_HAPTICSCONNTIMER_H

#include <e32base.h>

#include "hapticsmsghandler.h"

NONSHARABLE_CLASS ( CHapticsConnTimer ) : public CTimer
	{
	public:
        /**
         * 2-phased constructor.
         */
        static CHapticsConnTimer* NewL( MHapticsConnectionShutdown* aCallback );

        /**
         * Destructor.
         */
        virtual ~CHapticsConnTimer();

		/**
	 	* Starts the timer
	 	*/
        void Start();
        
    private: // From CActive.
        /**
         * Notifies client on timeout.
         */
        void RunL();
        			
    private:
        /**
         * Constructor.
         */
        CHapticsConnTimer( MHapticsConnectionShutdown* aCallback );

        /**
         * 2nd phase constructor.
         */    
	    void ConstructL();

    private:
        /**
         * Callback interface to notify about timeout.
         * Not own.
         */        
        MHapticsConnectionShutdown* iCallback;
	};
	
#endif // C_HAPTICSCONNTIMER_H

// End of file
