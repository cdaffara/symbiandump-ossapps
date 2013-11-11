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
* Description:  Data transfer interfaces.
*
*/


#ifndef M_IRDATATRANSFEROBSERVER_H
#define M_IRDATATRANSFEROBSERVER_H

#include <e32def.h>

class RHTTPSession;
class MIRDataTransferObserver;


/**
 * Data transfer tracker interface.
 *
 * Provides means to attach itself to an existing network session to track the data amounts
 * transferred through it, and also an API through which raw data amounts can be manually
 * inputted (this is mainy the case when raw sockets are used directly).
 *
 * For getting notifications from the application's data transfer amounts on the core side,
 * this class supports an observer interface through which data transfer events are notified.
 */
NONSHARABLE_CLASS( MIRDataTransferTracker )
    {
public:

    /**
     * Supported data transfer categories.
     */
    enum TIRTransferCategory
        {

        /**
         * The category for the data is unknown.
         */
        EIRTransferCategoryUnknown,

        /**
         * The category for the data is SDS.
         */
        EIRTransferCategoryIsds,

        /**
         * The category for the data is content show.
         */
        EIRTransferCategoryAudio

        };

    /**
     * Simple struct to keep hold of all the different data amounts transferred.
     *
     * The actual total amount of bytes of transferred data through the connection
     * is the amount of total bytes sent added to the amount of total bytes received.
     */
    NONSHARABLE_STRUCT( TIRDataTransferPckg )
        {

        /**
         * The total amount of bytes sent originating from all traffic.
         * This is simply the sum of all the categorized data sent amounts.
         */
        TInt iBytesSentTotal;

        /**
         * The total amount of bytes received originating from all traffic.
         * This is simply the sum of all the categorized data received amounts.
         */
        TInt iBytesReceivedTotal;

        /**
         * The total amount of bytes sent originating from unknown traffic.
         */
        TInt iBytesSentUnknown;

        /**
         * The total amount of bytes received originating from unknown traffic.
         */
        TInt iBytesReceivedUnknown;

        /**
         * The total amount of bytes sent originating from station directory traffic.
         */
        TInt iBytesSentIsds;

        /**
         * The total amount of bytes received originating from station directory traffic.
         */
        TInt iBytesReceivedIsds;

        /**
         * The total amount of bytes sent originating from content show traffic.
         */
        TInt iBytesSentAudio;

        /**
         * The total amount of bytes received originating from content show traffic.
         */
        TInt iBytesReceivedAudio;

        };

public:

    /**
     * Sets the observer that is notified of data transfer events.
     *
     * @param   aObserver           The observer to set, or NULL if none.
     */
    virtual void SetObserver( MIRDataTransferObserver* aObserver ) = 0;

    /**
     * Returns the current data transfer amounts.
     *
     * This always reflects the current situation, and thereby does not
     * take the threshold mechanism into account.
     *
     * @return  The current data transfer amounts.
     */
    virtual const TIRDataTransferPckg& TransferredData() const = 0;

    /**
     * Binds the tracker to an existing and opened HTTP session.
     *
     * @param   aSession            Session to bind the tracker to.
     * @param   aCategory           Transfer category to record the data under.
     */
    virtual void BindL( RHTTPSession aSession, TIRTransferCategory aCategory ) = 0;

    /**
     * Notifies of raw bytes transferred through the connection.
     *
     * Raw sockets cannot be observer automatically, so if they are used, then manual
     * notification is required on the user's part.
     *
     * @param   aBytesSent          Bytes sent through the raw socket.
     * @param   aBytesReceived      Bytes received through the raw socket.
     * @param   aCategory           Transfer category to record the data under.
     */
    virtual void RawDataTransferredL( TInt aBytesSent,
             TInt aBytesReceived, TIRTransferCategory aCategory ) = 0;

    };

/**
 * Interface which all data transfer events are passed through.
 */
class MIRDataTransferObserver
    {

public:

    /**
     * Invoked when a data transfer event occurs.
     *
     * The amount of data supplied as the parameter is the absolute amount of data transferred during
     * the life time of the connection. The threshold after which the call back is invoked is internal,
     * resulting in that it may not always be instantly called unless a predefined amount of data has
     * already been transferred since the last call back was made.
     *
     * @param   aData           Data amounts that have been transferred through the connection.
     */
    virtual void HandleDataTransferEventL( 
                const MIRDataTransferTracker::TIRDataTransferPckg& aData ) = 0;
    };

#endif // M_IRDATATRANSFEROBSERVER_H
