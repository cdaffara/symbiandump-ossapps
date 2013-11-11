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
* Description:  Functional implementation of one touch(pointer sequence) event service.
*
*/

#ifndef CMULTITOUCHPOINTER_UNIT_H
#define CMULTITOUCHPOINTER_UNIT_H

// INCLUDES
#include <HtiServicePluginInterface.h>
#include <w32std.h>

// CONSTANTS

// MACROS

// DATA TYPES

// FUNCTION PROTOTYPES

// FORWARD DECLARATIONS

// CLASS DECLARATION
class CMultiTouchPointerEventHandler;

/**
*  Functional implentation of pointer event service.
*/
class CMultiTouchPointerEvent : public CActive
    {
    public:

        /**
        * Two-phased constructor.
        */
        static CMultiTouchPointerEvent* NewL(TUint8 aTouchNumber,
                CMultiTouchPointerEventHandler* aEventHandler);

        /**
        * Destructor.
        */
        virtual ~CMultiTouchPointerEvent();       


        /**
         * Touch active object start function
         * @param aDelayTime initial wait time to start "finger" down for touch
         */
        void StartTouch();
        
        void AddPointL(TTimeIntervalMicroSeconds32 aTime,
        		TInt aX, TInt aY, TInt aZ, TRawEvent::TType aEventType);
        
        void PrintInfo();
        
        /**
         * Insert a dragging event between two given pointers
         * Interpolate pointer sequence with step aStepCount and dragging time.
         * @param aX1, aY1, aZ1 3D coordinate of start point
         * @param aX2, aY2, aZ2 3D coordinate of end point
         * @param aDragTime dragging time between start point and end point
         * @param aStepCount step count between two points
         */
        void InterpolatePointL(TInt aX1,TInt aY1, TInt aZ1, TInt aX2,TInt aY2, TInt aZ2, 
                TTimeIntervalMicroSeconds32 aDragTime, TInt aStepCount);
        
    protected: // Functions from base classes

        // From CActive
        void RunL();
        TInt RunError(TInt aError);
        void DoCancel();

    private:

        /**
        * C++ default constructor.
        */
        CMultiTouchPointerEvent(TUint8 aTouchNumber, CMultiTouchPointerEventHandler* aEventHandler);

        /**
        * 2nd phase constructor.
        */
        void ConstructL();

        void  SimulatePointerEvent();
        
        /**
         * Insert an advanced pointer to pointer sequence array for a touch
         * @param aX, aY, aZ pointer 3D coordinate
         * @param aEventType event type
         */
        void InsertPointArrayL(TInt aX, TInt aY, TInt aZ, TRawEvent::TType aEventType);

        /**
         * Insert time duration to time array
         * @param aDelayTime time duation to wait for
         */
        void InsertTimeArrayL(TTimeIntervalMicroSeconds32 aDelayTime); 
        
    private: // data

        struct TAdvancedPointer
            {
            TInt             X;
            TInt             Y;
            TInt             Z; 
            TRawEvent::TType EventType;
            };  
        
        CMultiTouchPointerEventHandler* iEventHandler;
        TUint8 iTouchNumber;

        RTimer iTimer;

		RPointerArray<TAdvancedPointer> iAdvPointerArray;
		RPointerArray<TTimeIntervalMicroSeconds32> iTimeArray;
		};

#endif // CMULTITOUCHPOINTER_UNIT_H
