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
* Description: CSocketsRead is an active obejct that reads data from 
* an already open socket and calculates some metrics from the data read
*
*/

#ifndef __SOCKETSREAD_H__
#define __SOCKETSREAD_H__

#include <in_sock.h>
#include "conntest.hrh"

// CONSTANTS

const TInt KMaximumPacketCount = 4000; // Count to 4000. Fails if ploss is over 20% (1000pckts)

// FORWARD DECLARATIONS
class MUINotify;
class CSocketsEngine;

// CLASS DECLARATION

/** 
*  This class handles reading data from the socket.
*  In this implementation, any data read is simply
*  displayed as text on the console.
*/
class CSocketsRead : public CActive
{
public:

    /**
    * Create a CSocketsRead object
    * @param aConsole console to use for ui output
    * @param aSocket socket to read from
    * @result a pointer to the created instance of CSocketsRead
    */
    static CSocketsRead* NewL(MUINotify& aConsole, RSocket& aSocket, CSocketsEngine& aEngine);
    
    /**
    * Create a CSocketsRead object
    * @param aConsole console to use for ui output
    * @param aSocket socket to read from
    * @result a pointer to the created instance of CSocketsRead
    */
    static CSocketsRead* NewLC(MUINotify& aConsole, RSocket& aSocket, CSocketsEngine& aEngine);
    
    /**
    * Destroy the object and release all memory objects
    */
    ~CSocketsRead();
    
    /**
    * Initiate the read process.
    * @param aAddress Address where data is sent (needed by UDP)
    * @param aProtocol Protocol to be used (TCP or UDP)
    */
    void Start(TInetAddr* aAddress, TUint aProtocol);

    /**
    * Set throughput calculation on or off.
    * @param aValue If ETrue, measure throughput
    */
    void SetPerformance(const TBool aValue);

	/**
	* Start RAW read
	*/
    void StartRAWRead(TInetAddr* aAddress, TUint aProtocol);
    
protected: // from CActive
           
    /**
    * Cancel any outstanding operation
    */
    void DoCancel();
    
    /**
    * Called when operation complete
    */
    void RunL();	
    
private:

    /**
    * Perform the first phase of two phase construction 
    * @param aConsole console to use for ui output
    * @param aSocket socket to read from
    */
    CSocketsRead(MUINotify& aConsole, RSocket& aSocket, CSocketsEngine& aEngine);
    
    /**
    * Perform the second phase construction of a CSocketsRead 
    */
    void ConstructL();
    
    /**
    * Read data from socket
    */
    void IssueRead();
	
	/**
	* Calculate packet loss
	* @return ploss percentage
	*/
	TInt32 CalculatePacketLoss();
    
private: // Member variables
//    enum { KReadBufferSize = 4096 }; // 4K
    //enum { KReadBufferSize = 10240 }; // 10K
    // Member variables
    CSocketsEngine&         iEngine;  // connection engine
    RSocket&                iSocket;  // socket to read data from
    MUINotify&              iConsole; // console for displaying text etc
    TBuf8<KReadDataSize>  iBuffer;  // buffer for receiving data
    TSockXfrLength          iDummyLength; // dummy - length of data read is written here
    TInt                    iReceivedBytes; 
    TInt32					iUdpPacketCounter;
    TBool                   iDoCount; 
    TBool                   iDoPerformance; 
    TInetAddr*              iAddress;
    TUint                   iProtocol;
    TTime                   iStartTime;
    TBuf8<2048>             iHeaders;  // buffer for HTTP headers
    TInt                    iBodySize;
};

#endif // __SOCKETSREAD_H__

