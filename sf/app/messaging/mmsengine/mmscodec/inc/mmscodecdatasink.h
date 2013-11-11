/*
* Copyright (c) 2005-2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   Data sink API for chunked receiving of large messages
*
*/



#ifndef MMMSCODECDATASINK_H
#define MMMSCODECDATASINK_H

#include <e32base.h>


/**
 *  Defines the interface for chunked data supply
 *
 *  When MMS messages are sent in chunks this provides the interface
 *  that can be called to get next encoded data chunk
 *
 *  @since v3.1
 */
class MMmsCodecDataSink
    {

public:

    /**
     * Gives the next data part to be decoded.
     * Decoding is synchronous. When the code returns, data can be released.
     *
     * @since v3.1
     * @param aBuffer flat buffer containing the data to be decoded.
     * @param aPosition start of data to be decoded.
     *     Decoding will start from aPosition.
     *     At return aPosition will point to the data that has not been
     *     decoded yet (in case data ends with an incomplete header) or
     *     past the end of data if everything has been decoded.
     *     The caller can discard the beginning of data up to aPosition.
     *     If there is data left in the buffer it must be shifted to the
     *     beginning of buffer and new data appended to the end to allow
     *     an incomplete header to be decoded at next round.
     * @param aLastChunk ETrue if the data chunk in the buffer is the last one.
     * @return error code, KErrNone if decoding succeeds
     *     If returns some other error, transaction must be cancelled.
     */
    virtual TInt NextDataPart(
        CBufFlat& aBuffer,
        TInt& aPosition,
        TBool aLastDataChunk ) = 0;
        
    /**
     * Release data sink and free any resources
     * This function must be called after last data chunk has been processed
     * This function must also be called if the process is terminated by an error
     *
     * @since v3.1
     */
    virtual void RelaseDataSink() = 0;    
    
    /**
     * Reset data sink.
     * Delete all attachments from current message.
     * Reset MMS headers
     * Start message decoding from the beginning
     * 
     * This function must be called if transaction is interrupted and restarted
     *
     * @since v3.1
     */
    virtual void ResetDataSink() = 0;    
    
    };

#endif // MMMSCODECDATASINK_H
