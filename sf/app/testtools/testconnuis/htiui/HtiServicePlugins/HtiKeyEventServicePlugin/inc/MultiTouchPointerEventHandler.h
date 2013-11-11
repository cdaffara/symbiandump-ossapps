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
* Description:  Functional implementation of advanced pointer multitouch service.
*
*/

#ifndef CMULTITOUCHPOINTEREVENTHANDLER_H
#define CMULTITOUCHPOINTEREVENTHANDLER_H

// INCLUDES
#include <HtiServicePluginInterface.h>

// CONSTANTS

// MACROS

// DATA TYPES

// FUNCTION PROTOTYPES

// FORWARD DECLARATIONS

// CLASS DECLARATION
class CMultiTouchPointerEvent;

/**
* Observer interface for Multi touch event.
*/
class MHtiMultiTouchObserver
    {
public:
    /**
    * Notify multi touch event completed
    */
    virtual void NotifyMultiTouchComplete() = 0;

    };
/**
*  Functional implentation of pointer event service.
*/
class CMultiTouchPointerEventHandler: public CBase
    {
    public:
        /**
        * Two-phased constructor.
        */
        static CMultiTouchPointerEventHandler* NewL(MHtiMultiTouchObserver& aObserver);

        /**
        * Destructor.
        */
        virtual ~CMultiTouchPointerEventHandler();      


        TBool HandleMultiTouchL( const TDesC8& aData );

        /**
         * Callback function called by CMultiTouchPointerEvent to notify CMultiTouchPointerEventHandler 
         * that this touch action(pointer sequence) is conpleted.
         * @param aTouchNumber touch number ordinal
         */
        void NotifyTouchComplete(TUint8 aTouchNumber);        
     
        void SimulateEvent(TUint8 aTouchNumber,TInt aX, TInt aY, TInt aZ, TRawEvent::TType aEventType);
        
        void Clear();
        
    private:

        /**
        * C++ default constructor.
        */
        CMultiTouchPointerEventHandler(MHtiMultiTouchObserver& aObserver);

        /**
        * 2nd phase constructor.
        */
        void ConstructL();
        
        TBool ParsePoint( const TDesC8& aData, TInt& aoffset,
        		TInt& aX, TInt& aY, TInt& aZ, 
        		TTimeIntervalMicroSeconds32& aWait, TTimeIntervalMicroSeconds32& aHold );
        
        TBool ParseMove( const TDesC8& aData, TInt& aoffset,
        		TTimeIntervalMicroSeconds32& aDragTime, TInt& aStepCount );

    private: // data
        RWsSession iWsSession;
        RPointerArray<CMultiTouchPointerEvent> iTouchPointerArray;
        TUint8 iFinishedCount;
        
        MHtiMultiTouchObserver& iObserver;
		};

#endif // CKEYEVENTHANDLER_H
