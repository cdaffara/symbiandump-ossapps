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
* Description:  Functional implentation of pointer event service.
*
*/

#ifndef CPOINTEREVENTHANDLER_H
#define CPOINTEREVENTHANDLER_H

// INCLUDES
#include <HtiServicePluginInterface.h>
#include <w32std.h>
#include "MultiTouchPointerEventHandler.h"
// CONSTANTS

// MACROS

// DATA TYPES

// FUNCTION PROTOTYPES

// FORWARD DECLARATIONS

// CLASS DECLARATION


/**
*  Functional implentation of pointer event service.
*/
class CPointerEventHandler : 
	public CActive,
	public MHtiMultiTouchObserver
    {
    public:

        enum TError
            {
            EMissingCommand          = 0x01,
            EUnrecognizedCommand     = 0x02,
            ENotReadyCommand         = 0x03,
            EInvalidParameters       = 0x9A
            };

        enum TCommand
            {
            ETapScreen = 0x10,
            ETapAndDrag = 0x11,
            ETapAndDragMultipoint = 0x12,
            EPressPointerDown = 0x13,
            ELiftPointerUp = 0x14,
			EAdvancedTapScreen = 0x15, //for advanced pointer
			EPinchZoom = 0x16, //for advanced pointer
			EMultiTouch = 0x17, //for advanced pointer
            EResultOk = 0xFF // only for response message
            };

        enum TPointerState
            {
            EPointerUp,
            EPointerDown,
            EPointerMoving
            };

        struct TAdvancedPointer
            {
            TUint8 PointerNum;
            TInt   X;
            TInt   Y;
            TInt   Z;            
            };			

			

        /**
        * Two-phased constructor.
        */
        static CPointerEventHandler* NewL();

        /**
        * Destructor.
        */
        virtual ~CPointerEventHandler();

        /**
        * Called by the HtiKeyEventServicePlugin when a pointer event
        * command is received.
        * @param aMessage message body destinated to the servive
        * @param aPriority message priority
        */
        void ProcessMessageL( const TDesC8& aMessage,
            THtiMessagePriority aPriority );

        /**
        * Sets the dispatcher to send outgoing messages to.
        * @param aDispatcher pointer to dispatcher instance
        */
        void SetDispatcher( MHtiDispatcher* aDispatcher );


    protected: // Functions from base classes

        // From CActive
        void RunL();
        TInt RunError(TInt aError);
        void DoCancel();
        
        // From MHtiMultiTouchObserver
        void NotifyMultiTouchComplete();

    private:

        /**
        * C++ default constructor.
        */
        CPointerEventHandler();

        /**
        * 2nd phase constructor.
        */
        void ConstructL();

        // Helper methods
        void HandleTapScreenL( const TDesC8& aData );
        void HandleTapAndDragL( const TDesC8& aData );
        void HandleTapAndDragMultipointL( const TDesC8& aData );
        void HandlePointerDownOrUpL( const TDesC8& aData );
        
        //for advanced pointer
		void HandleAdvancedTapScreenL( const TDesC8& aData ); 
        void HandlePinchZoomL( const TDesC8& aData ); 
        void HandleMultiTouchL( const TDesC8& aData );

        void SendOkMsgL();
        void SendErrorMessageL( TInt aError, const TDesC8& aDescription );

        void ChangePointerStateL();
        void MoveToNextPointL();
        void PointerDown();
        void PointerUp();
        void PointerMove();
        void SimulatePointerEvent( TRawEvent::TType aType );
        TBool IsMultitouch();
        void AdvanceAddMiddlePointL(TInt aPointNumber,TInt aX1,TInt aY1, TInt aZ1,TInt aX2,TInt aY2, TInt aZ2 , TInt aStepCount );
        void AdvancedAddDelayArray(TTimeIntervalMicroSeconds32 aDelay , TInt aStepCount );
        TBool AdvancedStartDelay();


    private: // data

        RWsSession iWsSession;
        MHtiDispatcher* iDispatcher; // referenced

        RTimer iTimer;
        TBool  iReady;
        TUint8 iCommand;
        TInt   iX;
        TInt   iY;
        TInt   iTapCount;
        TTimeIntervalMicroSeconds32 iEventDelay;
        TTimeIntervalMicroSeconds32 iActionDelay;
        TPointerState iState;
        RArray<TInt>* iCoords;
		RPointerArray<TAdvancedPointer> iAdvancedPointers;
		
		RPointerArray<TAdvancedPointer> iAdvPointerMoveArray;
		RPointerArray<TTimeIntervalMicroSeconds32> iDelayArray;
		
		CMultiTouchPointerEventHandler* iMultiTouchHandler;
		};

#endif // CKEYEVENTHANDLER_H
