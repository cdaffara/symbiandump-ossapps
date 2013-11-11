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
* Description:  Class for call detaction
*
*/

// Version : %version: 1 %

#ifndef MPXCALLDETECTOR_H
#define MPXCALLDETECTOR_H

#include <e32base.h>
#include <e32property.h>

class MMPXCallDetectorObserver
{
    public:
        virtual void CallDetectedL() = 0;
};

/**
 *  Detector for incoming call
 */
NONSHARABLE_CLASS( CMPXCallDetector ) : public CActive
{
    public: // Constructors and destructor

        /**
        * Two-phased constructor.
        */
        static CMPXCallDetector* NewL( MMPXCallDetectorObserver* aObserver );

        /**
        * Destructor.
        */
        ~CMPXCallDetector();
        
    public: // New functions

        /**
        * Request call notifications
        *
        */
        void RequestNotification();

    private:

        /**
        * C++ default constructor.
        */
        CMPXCallDetector( MMPXCallDetectorObserver* aObserver );

        /**
        * Symbian OS 2nd phase constructor.
        */
        void ConstructL();
    
        /**
        * From CActive
        * @see CActive for more information
        */
        void RunL();
        
        /**
        * From CActive
        * @see CActive for more information
        */
        void DoCancel();

    private:

        // Call observer
        MMPXCallDetectorObserver*           iObserver;
        RProperty                           iTsyProperty;
};

#endif // CALLDETECTOR_H
