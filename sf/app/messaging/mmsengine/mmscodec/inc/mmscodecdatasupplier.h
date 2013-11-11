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
* Description:   Data supplier API for chunked sending of large messages
*
*/



#ifndef MMMSCODECDATASUPPLIER_H
#define MMMSCODECDATASUPPLIER_H

#include <e32base.h>

/**
 *  Defines the interface for chunked data supply.
 *
 *  When MMS messages are sent in chunks this provides the interface
 *  that can be called to get next encoded data chunk
 *
 *  @since v3.1
 */
class MMmsCodecDataSupplier
    {

public:

    /**
     * Give poiner to next encoded data chunk.
     *
     * @since v3.1
     * @param aDataPart pointer to the next encoded data part
     * @param aLastDataChunk 
     *     ETrue if this is the last data chunk
     *     EFalse if this is not the last data chunk
     * @return error code, KErrNone if all is well.
     *     If returns some other error, transaction must be cancelled.
     */
    virtual TInt GetNextDataPart(
        TPtrC8& aDataPart, TBool& aLastDataChunk ) = 0;
    
    /**
     * Empty buffer and prepare next data part (synchronous encoding).
     * When needed, caller will tell HTTP stack that the next data part is
     * ready by calling RHTTPTransaction::NotifyNewRequestBodyPartL()
     *
     * @since v3.1
     * @return error code, KErrNone if all is well.
     *     If returns some other error, transaction must be cancelled.
     */
    virtual TInt ReleaseData() = 0;
    
    /**
     * Return the overall data size.
     *
     * @since v3.1
     * @return The overall data size if it is known, otherwise KErrNotFound
     */
    virtual TInt OverallDataSize() = 0;
    
    /**
     * Reset the data supplier.
     * Data supplier must start the data generation from the beginning again.
     *
     * @since v3.1
     * @return KErrNone if reset is successful, other error code if data
     *     supplier cannot be reset
     */
    virtual TInt ResetSupplier() = 0;

    };

#endif // MMMSCODECDATASUPPLIER_H
