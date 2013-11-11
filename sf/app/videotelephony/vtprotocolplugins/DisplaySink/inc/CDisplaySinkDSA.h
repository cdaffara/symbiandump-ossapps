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
* Description:    DisplaySink subsystem.
*
*/




#ifndef CDISPLAYSINKDSA_H
#define CDISPLAYSINKDSA_H

//  INCLUDE FILES

#include "CDisplaySink.h"
#include "TDisplaySinkParamsDSA.h"

// CLASS DECLARATION

/**
*  Displaysink implementation for Direct Screen access.
*
*  @lib displaysink.lib
*  @since Series 60 3.1
*/
NONSHARABLE_CLASS( CDisplaySinkDSA ) : public CDisplaySink,
    public MVtImageRotatorObserver
    {
public: // static constructor

    /**
    * Static constructor that may leave.
    */
    static CDisplaySinkDSA* NewL( TDisplaySinkParams& aParams,
        const TDesC8& aInitData );

private: // constructors and destructor

    /**
    * C++ default constructor.
    */
    CDisplaySinkDSA();

    /**
    * destructor
    */
    ~CDisplaySinkDSA();

private: // from CDisplaySink

    /**
    * @see CDisplaySink::BaseConstructL
    */
    void BaseConstructL( TDisplaySinkParams& aParams,
        const TDesC8& aInitData );

    /**
    * @see CDisplaySink::BaseConvert
    */
    TBool BaseConvert( const TVtMMFDataBuffer& aBuffer );

    /**
    * @see CDisplaySink::BaseSinkThreadLogonL
    */
    void BaseSinkThreadLogonL();

    /**
    * @see CDisplaySink::BaseSinkThreadLogoff
    */
    void BaseSinkThreadLogoff();

    /**
    * @see CDisplaySink::BaseUpdateSinkParamsL
    */
    void BaseUpdateSinkParamsL( const TDisplaySinkParams& aParams );

    /**
    * @see CDisplaySink::BaseDoUpdateParamsL
    */
    void BaseDoUpdateParamsL();

    /**
    * @see CDisplaySink::BaseDoUpdateParamsL
    */
    void BaseSetVideoFrameSizeL( const TSize& aSize );

    /**
    * @see CDisplaySink::BaseSizesMatch
    */
    TBool BaseSizesMatch( const TSize& aSize );

    /**
    * @see CDisplaySink::BitmapEntry
    */
    TBitmapEntry& BitmapEntry( TInt aIndex );

    /**
    * @see CDisplaySink::BaseEmptyBufferL
    */
    void BaseEmptyBufferL( TVtMMFDataBuffer& aVTMMFBuffer );

    /**
    * @see CDisplaySink::BaseSetBitmapAvailable
    */
    void BaseSetBitmapAvailable( TBitmapNo aBitmapNo );

    /**
    * @see CDisplaySink::BaseVideoFrameSizeChangedL
    */
    void BaseVideoFrameSizeChangedL( const TSize& aNewSize );

private: // from MVtImageRotatorObserver

    /**
    * @see MVtImageRotatorObserver::RotationFinished
    */
    void RotationFinished( TInt aError );

private: // new methods

    /**
    * Updates expected frame size.
    * @exception May leave with KErrNotSupported, if bitmap sizes do not match.
    */
    void UpdateExpectedFrameSizeL();

    /**
    * Converts image. Calls DoConvertL() to do the task.
    * @param "aBitmapNo" CFbsBitmap number to use for converted image.
    * @param "aConverted" Reference to boolean that will be ETrue if buffer
    * was converted, EFalse otherwise.
    * @return KErrNone or one of the system wide error codes.
    */
    void ConvertL( const TPtrC8& aBuffer, TBool& aConverted );

    /**
    * Convert YUV420Planar image to a CFbsBitmap.
    * @param "aBitmapNo" CFbsBitmap number to use for converted image.
    * @return KErrNone or one of the system wide error codes.
    */
    void DoConvertL( const TPtrC8& aBuffer, TBitmapNo aBitmapNo );

private: // new classes

    // Class containing bitmap related data
    class TBitmapEntryDSA : public TBitmapEntry
        {
    public: // data

        // Handle to target bitmap
        TInt iHandle; // QCIF
        };

private: // data

    // Array of bitmap related structures
    TBitmapEntryDSA iBitmapTable[ KBitmapTableCount ];

    // Converter for bitmap 1, created into Protocol thread's heap
    CVTYUVFbsBitmapConverter* iYUVBitmapConverter1; // owned

    // Converter for bitmap 2, created into Protocol thread's heap
    CVTYUVFbsBitmapConverter* iYUVBitmapConverter2; // owned

    // Converter for bitmap, created into VT thread's heap
    CVTYUVFbsBitmapConverter* iYUVBitmapConverterVT;

    // Pointer to image rotator instance.
    CVtImageRotator* iRotator; // owned

    // Source YUV image for image transforms
    CVtImageIYUV* iSourceImg;

    // Target YUV image for image transforms
    CVtImageIYUV* iTargetImg;

    // Rotation angle
    CVtImageRotator::TRotationAngle iRotationAngle;

    // Store for saving new parameters in UpdateSinkParamsL() call
    TDisplaySinkParamsDSA iNewParams;
    };

#endif // CDISPLAYSINKDSA_H

// End of File
