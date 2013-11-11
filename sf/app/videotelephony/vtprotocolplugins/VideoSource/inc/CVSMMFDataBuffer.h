/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Video Source subsystem.
*
*/


#ifndef CVSMMFDATABUFFER_H
#define CVSMMFDATABUFFER_H

//  INCLUDE FILES

#include <mmfdatabuffer.h>

// FORWARD DECLARATIONS

class MFrameBuffer;
class MCameraBuffer;

/**
*  API of data buffer that allows provider's buffer contents to
*  be given to consumer without copying.
*
*  @lib videosource.lib
*  @since 5.0
*/
class MVSMMFDataBuffer
    {

public:

    /**
    * Sets the ECAM frame buffer.
    * @param "aFrameBuffer" MFrameBuffer pointer.
    */
    virtual void SetFrameBufferL( MFrameBuffer* aFrameBuffer ) = 0;

	/**
    * Sets the ECAM camera buffer.
    * @param "aFrameBuffer" MFrameBuffer pointer.
    */
    virtual void SetFrameBufferL( MCameraBuffer* aCameraBuffer ) = 0;

    /**
    * Calls Release() for MFrameBuffer and sets aFrameBuffer to NULL.
    */
    virtual void ReleaseL() = 0;

    /**
    * Sets start offset and length of internal iPtr variable (when
    * pointing to iData).
    */
    virtual void SetBufferOffset( TInt aOffset ) = 0;

    /**
    * Reallocates the max size in bytes.
    * @param "aMaxBufferSize" New size in bytes.
    * @exception Can leave with one of the system wide error codes.
    */
    virtual void ReAllocBufferL( TInt aMaxBufferSize ) = 0;

    /**
    * Checks whether buffer needs reset or not.
    * @return ETrue if buffer needs reset, EFalse otherwise.
    */
    virtual TBool NeedsReset() const = 0;

    /**
    * Sets needs reset state.
    * @param aNeedsReset New reset state.
    */
    virtual void SetNeedsReset( TBool aNeedsReset ) = 0;

    /**
    * Resets buffer (i.e. clears data).
    */
    virtual void Reset() = 0;

    };

/**
*  Implementation of data buffer that allows provider's buffer contents to
*  be given to consumer without copying.
*
*  @lib videosource.lib
*/
class CVSMMFDataBuffer : public CMMFDataBuffer, public MVSMMFDataBuffer
    {
    public: // new methods

        /**
        * Static factory function to create instance of this class.
        * @exception Can leave with one of the system wide error codes.
        * @return Pointer to new instance.
        */
        IMPORT_C static CVSMMFDataBuffer* NewL();

        /**
        * Static factory function to create instance of this class.
        * @param "aMaxBufferSize" Buffer size
        * @exception Can leave with one of the system wide error codes.
        * @return Pointer to new instance.
        */
        IMPORT_C static CVSMMFDataBuffer* NewL( TInt aMaxBufferSize );

        /**
        * Destructor.
        */
        IMPORT_C ~CVSMMFDataBuffer();

    public: // from CMMFDataBuffer

        /**
        * @see CMMFDataBuffer::Data
        */
        virtual TDes8& Data();

        /**
        * @see CMMFDataBuffer::Data
        */
        virtual const TDesC8& Data() const;

        /**
        * @see CMMFDataBuffer::SetStatus
        */
        virtual void SetStatus( TBufferStatus aStatus );

        /**
        * @see CMMFDataBuffer::SetRequestSizeL
        */
        virtual void SetRequestSizeL( TInt aSize );

        /**
        * @see CMMFDataBuffer::BufferSize
        */
        virtual TUint BufferSize() const;

        /**
        * @see CMMFDataBuffer::SetPosition
        */
        virtual void SetPosition( TUint aPosition );

    public: // from MVSMMFDataBuffer

        /**
        * @see MVSMMFDataBuffer::SetFrameBufferL
        */
        void SetFrameBufferL( MFrameBuffer* aFrameBuffer );

        /**
        * @see MVSMMFDataBuffer::SetFrameBufferL
        */
        void SetFrameBufferL( MCameraBuffer* aCameraBuffer );

        /**
        * @see MVSMMFDataBuffer::ReleaseL
        */
        void ReleaseL();

        /**
        * @see MVSMMFDataBuffer::SetBufferOffset
        */
        void SetBufferOffset( TInt aOffset );

        /**
        * @see MVSMMFDataBuffer::ReAllocBufferL
        */
        void ReAllocBufferL( TInt aMaxBufferSize );

        /**
        * @see MVSMMFDataBuffer::NeedsReset
        */
        TBool NeedsReset() const;

        /**
        * @see MVSMMFDataBuffer::SetNeedsReset
        */
        void SetNeedsReset( TBool aNeedsReset );

        /**
        * @see MVSMMFDataBuffer::Reset
        */
        void Reset();


    private:    // Constructors and destructor

        /**
        * Constructor.
        */
        CVSMMFDataBuffer();

        /**
        * 2nd phase constructor.
        * @param "aMaxBufferSize" Buffer size in bytes.
        */
        void ConstructL( TInt aMaxBufferSize );

    public: // Data

    protected:  // Data

    private:    // Data

        // Pointer to ECAM buffer
        MFrameBuffer* iFrameBuffer;

        // Pointer to ECAM buffer
        MCameraBuffer* iCameraBuffer;

        // Pointer to data buffer ( n/a for ECAM )
        TUint8* iData;

        // TPtr8 pointing to iData or ECAM buffer data content
        TPtr8 iPtr;

        // Max buffer size ( n/a for ECAM )
        TInt iMaxBufferSize;

        // ETrue if buffers needs reset, EFalse otherwise
        TBool iNeedsReset;

        // ETrue if buffer is set, EFalse otherwise
        TBool iIsBufferSet;

    };

#endif // CVSMMFDATABUFFER_H
