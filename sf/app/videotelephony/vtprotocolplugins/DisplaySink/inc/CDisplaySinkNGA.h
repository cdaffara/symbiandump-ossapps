/*
* Copyright (c) 2004-2006 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Displaysink implementation for NGA.
*
*/

#ifndef CDISPLAYSINKNGA_H
#define CDISPLAYSINKNGA_H

//  INCLUDE FILES
#include "CDisplaySink.h"
#include "TDisplaySinkParamsNGA.h"

// CLASS DECLARATION

/**
*  Displaysink implementation for NGA.
*
*  @lib displaysink.lib
*  @since Series 60 3.1
*/
NONSHARABLE_CLASS( CDisplaySinkNGA ) : public CDisplaySink
    {
public: // static constructor

    /**
    * Static constructor that may leave.
    */
    static CDisplaySinkNGA* NewL( TDisplaySinkParams& aParams,
        const TDesC8& aInitData );

private: // constructors and destructor

    /**
    * C++ default constructor.
    */
    CDisplaySinkNGA();

    /**
    * destructor
    */
    ~CDisplaySinkNGA();

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

private: // MDisplaySink

    /**
    * @see MDisplaySink::NextFrame
    */
    void NextFrame();

private: // new methods

    /**
    * Updates expected frame size.
    * @exception May leave with KErrNotSupported, if bitmap sizes do not match.
    */
    void UpdateExpectedFrameSizeL();

private: // new classes

    // Class submit buffer related data
    class TBitmapEntryNGA : public TBitmapEntry
        {
    public: // data

        // Pointer to Surface Buffer pointer
        //TVideoFrameBuffer** iBuffer;
        TUint8**  iBuffer;
        };

private: // data

    // Array of bitmap related structures
    TBitmapEntryNGA iBitmapTable[ KBitmapTableCount ];

    // Store for saving new parameters in UpdateSinkParamsL() call
    TDisplaySinkParamsNGA iNewParams;
    };

#endif // CDISPLAYSINKNGA_H

// End of File
