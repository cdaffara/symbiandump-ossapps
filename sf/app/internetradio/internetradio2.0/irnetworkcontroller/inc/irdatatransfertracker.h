/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Tracks data transmitted through a network connection.
*
*/


#ifndef C_IRDATATRANSFERTRACKER_H
#define C_IRDATATRANSFERTRACKER_H

#include <mhttpfilter.h>

#include "irdatatransferobserver.h"


/**
 * Tracks data transmitted through a network connection.
 *
 * Data sent or received over HTTP sessions are logged with the implemented HTTP filter.
 * For raw sockets, the implementation requires that the user of the socket notifies
 * him of data transfer explicitly.
 */
NONSHARABLE_CLASS( CIRDataTransferTracker ): public CBase,
                                             public MIRDataTransferTracker,
                                             public MHTTPFilter
    {
public:

    /**
     * Two-phased constructor.
     *
     * @return  The created object. Transfers ownership.
     */
    static CIRDataTransferTracker*  NewL( );

    /**
     * Destructor.
     */
    ~CIRDataTransferTracker();

// from base class MIRDataTransferTracker

   void SetObserver( MIRDataTransferObserver* );
    const TIRDataTransferPckg& TransferredData() const;
    void BindL( RHTTPSession , TIRTransferCategory );
    void RawDataTransferredL( TInt , TInt , TIRTransferCategory );

// from base class MHTTPFilter

    void MHFRunL( RHTTPTransaction aTransaction, const THTTPEvent& aEvent );

private:

    /**
     * Constructor.
     */
    CIRDataTransferTracker();

    /**
     * Second-phase constructor.
     */
    void ConstructL() const;

    /**
     * Calculates the header field sizes in bytes from the specified headers.
     *
     * @param   aTransaction        HTTP transaction for the headers.
     * @param   aHeaders            Headers to calculate the size from.

     * @return  Size of the headers in bytes.
     */
    TInt HeaderSize( const RHTTPTransaction aTransaction, RHTTPHeaders aHeaders );

    /**
     * Parses the header sizes (both request and response) of the supplied HTTP transaction,
     * logging it under the specified category.
     *
     * @param   aTransaction        Transaction to parse the headers from.
     * @param   aCategory           Transfer category to record the data under.
     */
    void ParseHeaderSizeL( const RHTTPTransaction aTransaction, TIRTransferCategory aCategory );

    /**
     * Parses the body size of the supplied HTTP transaction, logging it under the specified category.
     *
     * @param   aTransaction        Transaction to parse the headers from.
     * @param   aCategory           Transfer category to record the data under.
     */
    void ParseBodySizeL( const RHTTPTransaction aTransaction, TIRTransferCategory aCategory );

    /**
     * Processes raw data sent and/or received via the connection.
     *
     * @param   aBytesSent          The number of bytes that were sent.
     * @param   aBytesReceived      The number of bytes that were received.
     * @param   aCategory           Transfer category to record the data under.
     */
    void ProcessDataL( TInt aBytesSent, TInt aBytesReceived, TIRTransferCategory aCategory );

    /**
     * Dispatches the notification of the data transfer conditionally.
     *
     * If the threshold between the last notification's data transfer amount and the current
     * data transfer amount is not exceeded, then no notification is dispatched.
     */
    void DispatchNotificationL();

private:

    /**
     * Data transfer observer.
     * Not owned.
     */
    MIRDataTransferObserver* iObserver;

    /**
     * Persistent storage for the data amounts transferred.
     */
    MIRDataTransferTracker::TIRDataTransferPckg iPckg;

    /**
     * The amount of bytes transferred since last notification was dispatched.
     */
    TInt iBytesSinceLastNotification;

    /**
     * The flag to determine Byte Counter crossed first threshold
     */
    TBool bThresholdCrossed;
    };

#endif // C_IRDATATRANSFERTRACKER_H
