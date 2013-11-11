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

#ifndef CVCXTESTTIMER_H
#define CVCXTESTTIMER_H

//  INCLUDES
#include <e32base.h>

// CONSTANTS

// MACROS

// DATA TYPES

// FUNCTION PROTOTYPES

// FORWARD DECLARATIONS
class MVcxTestTimerObserver;

// CLASS DECLARATION

/**
*
*
*
*
*  @since
*/
class CVcxTestTimer : public CActive
    {
    public:  // Constructors and destructor

        /**
        * Two-phased constructor.
        */
        static CVcxTestTimer* NewL(MVcxTestTimerObserver& aObserver, TInt aId);

        /**
        * Destructor.
        */
        virtual ~CVcxTestTimer();


    public: // New functions
		void After(TTimeIntervalMicroSeconds32 aInterval);
		void CancelTimer( );

    protected: //from base classes
	    void RunL();
	    void DoCancel();

    private:

        /**
        * C++ default constructor.
        */
        CVcxTestTimer(MVcxTestTimerObserver& aObserver, TInt aId);

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL();

    private:    // Data
	    RTimer iTimer;
	    MVcxTestTimerObserver& iObserver;
	    TInt  iId;
	    TBool iRestartAfterCancel;
	    TTimeIntervalMicroSeconds32 iInterval;
    };


#endif      // CVCXTESTTIMER_H

// End of File
