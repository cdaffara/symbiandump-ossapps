/*
* Copyright (c) 2004 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  DisplaySink subsystem.
*
*/


#ifndef MDISPLAYSINK_H
#define MDISPLAYSINK_H

//  INCLUDE FILES

#include <e32base.h>

// FORWARD DECLARATIONS

class MVTVideoSink;

/**
*  MDisplaySinkObserver
*
*  Sink observer API. Currently only frame size changes are reported using
*  this IF.
*
*  @lib DisplaySink.lib
*/
class MDisplaySinkObserver
    {
public:
    /**
    * When incoming video frame size changes, this callback will be called.
    * @param "aFrom" Current frame size.
    * @param "aTo" New frame size.
    * @return KErrNone if successful, one of the system wide error codes
    * @exception May leave with one of the system wide error codes.
    * otherwise.
    */
    virtual void VideoFrameSizeChangedL(
        const TSize& aFrom,
        const TSize& aTo ) = 0;
    };

/**
*  Input parameters for Display Sink
*
*  @lib DisplaySink.lib
*/
class TDisplaySinkParams
    {
public:

    /**
    * Enumeration for limiting frame rate.
    */
    enum TFrameRateLimit
        {
        /** Don't limit frame rate (default) */
        EFrameRateLimitNone = 0,
        /** Limit frame rate to 1.0 frames per second */
        EFrameRateLimit1_0 = 1000,
        /** Limit frame rate to 2.5 frames per second */
        EFrameRateLimit2_5 = 400,
        /** Limit frame rate to 5.0 frames per second */
        EFrameRateLimit5_0 = 200,
        /** Limit frame rate to 7.5 frames per second */
        EFrameRateLimit7_5 = 133,
        /** Limit frame rate to 10.0 frames per second */
        EFrameRateLimit10_0 = 100,
        /** Limit frame rate to 12.5 frames per second */
        EFrameRateLimit12_5 = 80,
        /** Limit frame rate to 15.0 frames per second */
        EFrameRateLimit15_0 = 67,
        };

	/**
	* Flags for selecting correct type sink.
	*/
	enum TFlags
		{
		/** Create instance of DSA version of sink */
		EDisplaySinkDSA = ( 1 << 0 ),
		/** Create instance of DP version of sink */
		EDisplaySinkDP = ( 1 << 1 ),
		/** Create instance of NGA version of sink */
		EDisplaySinkNGA = ( 1 << 2 )
		};

public:        // Data

    /**
    * C++ constructor for initializing some of the variables.
    */
    IMPORT_C TDisplaySinkParams();

    // Thread id that gets notified by sink
    TThreadId iThreadId;

    // Pointer to iStatus member of the Active Object to notify after video
    // frame is converted
    TRequestStatus* iRequestStatusPtr;

    // Pointer to Display Sink observer interface
    MDisplaySinkObserver* iObserver;

    // Frame rate limitation
    TFrameRateLimit iFrameRateLimit;

    // Sink flags
    TUint32 iFlags;
    };

/**
*  MDisplaySink
*
*  @lib DisplaySink.lib
*/
class MDisplaySink
    {
public: // data

    // Enumeration for the bitmaps.
    enum TBitmapNo
        {
        EFirstBitmap = 0,
        ESecondBitmap
        };

public: // new methods

    /**
    * Deletes instance and frees all memory.
    */
    virtual void Destroy() = 0;

    /**
    * Mark the bitmap available for use.
    * @param "aBitmapNo" One of the enumerated TBitmapNo values.
    */
    virtual void SetBitmapAvailable( TBitmapNo aBitmapNo ) = 0;

    /**
    * Cancels sink so that it does not convert bitmaps even it receives data.
    */
    virtual void Release() = 0;

    /**
    * Updates display sink parameters, e.g. when display mode changes.
    * @param "aParams" Structure that contains new bitmap handles.
    * @param "aUpdated" Reference to boolean variable that will be set to
    * ETrue when display sink parameter update has been completed.
    * @exception May leave with one of the system wide error codes (e.g. in
    * out of memory situaton).
    */
    virtual void UpdateSinkParamsL( const TDisplaySinkParams& aParams,
        TBool& aUpdated ) = 0;

    /**
    * Pauses sending of remote video to VT engine.
    */
    virtual void Pause() = 0;

    /**
    * Resumes sending of remote video to VT engine.
    */
    virtual void Resume() = 0;

    /**
    * Operator returns this instance .
    */
    virtual operator MVTVideoSink&() = 0;

    /**
    * This method is called by the VT engine when engine has finished handling
    * of previous frame and is ready to handle a new one.
    */
    virtual void NextFrame() = 0;

    };

/**
* Creates instance of displaysink.
*/
IMPORT_C MDisplaySink* CreateSinkL( TDisplaySinkParams& aParams,
    const TDesC8& aInitData );

#endif // MDISPLAYSINK_H

// End of file
