/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of the License "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description: 
*
*/

#ifndef CVCXTESTACTIVEWAIT_H
#define CVCXTESTACTIVEWAIT_H

//  INCLUDES
#include <e32base.h>

// CONSTANTS

// MACROS

// DATA TYPES

// FUNCTION PROTOTYPES

// FORWARD DECLARATIONS

// CLASS DECLARATION

/**
*
*
*
*
*  @since
*/
class CVcxTestActiveWait : public CBase
    {
    public:  // Constructors and destructor
        /**
        * Two-phased constructor.
        */
        static CVcxTestActiveWait * NewL();

        /**
        * Destructor.
        */
        virtual ~CVcxTestActiveWait ();


	enum TVcxTestActiveWaitCmd
       	{
       	EActiveWaitStart,
       	EActiveWaitStop
       	};

    public: // New functions

    	/*
    	* To start and stop CActiveSchedulerWait
    	*/

		void ActiveWait( TVcxTestActiveWaitCmd aActiveWaitCmd );

		/*
		 * Returns ETrue if CActiveSchedulerWait is running.
		 */
		TBool IsWaiting();

        /*
         * Starts active wait.
         */
        TInt Start();

        /*
         * Stops active wait.
         */
        TInt Stop();


    protected: // From base classes

    private:

        /**
        * C++ default constructor.
        */
        CVcxTestActiveWait ();

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL();

    private:    // Data
		CActiveSchedulerWait* iActiveSchedulerWait;
    };


#endif      // CVCXTESTACTIVEWAIT_H

// End of File
