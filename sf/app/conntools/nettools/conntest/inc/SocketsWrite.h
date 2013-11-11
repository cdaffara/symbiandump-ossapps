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
* Description: CSocketsWrite is an active object which implements data 
* sending through an already open socket using UDP or TCP
*
*/

#ifndef __SOCKETSWRITE_H__
#define __SOCKETSWRITE_H__

#include <in_sock.h>
#include "TimeOutNotify.h"
#include "conntest.hrh"

// FORWARD DECLARATIONS
class CTimeOutTimer;
class MUINotify;
class MDataSendNotifyHandler;

// CLASS DECLARATION

/**
*  This class handles writing data to the socket.
*  Data to be written is accumulated in iTransferBuffer,
*  and is then transferred to iWriteBuffer for the actual
*  write to the socket.
*/
class CSocketsWrite : public CActive, public MTimeOutNotify
{
public:

    /**
    * Create a CSocketsWrite object
    * @param aConsole console to use for ui output
    * @param aSocket socket to write to
    * @param aSendHandler handler to notify UDP packet sends
    * @result a pointer to the created instance of CSocketsWrite
    */
    static CSocketsWrite* NewL( MUINotify& aConsole, 
                                RSocket& aSocket, 
                                MDataSendNotifyHandler& aSendHandler );
    
    /**
    * Create a CSocketsWrite object
    * @param aConsole console to use for ui output
    * @param aSocket socket to write to
    * @param aSendHandler handler to notify UDP packet sends
    * @result a pointer to the created instance of CSocketsWrite
    */
    static CSocketsWrite* NewLC( MUINotify& aConsole, 
                                 RSocket& aSocket, 
                                 MDataSendNotifyHandler& aSendHandler );
    
    /**
    * Destroy the object and release all memory objects
    */
    ~CSocketsWrite();
    
    /**
    * Write the data to the socket (buffered)
    * @param aData the data to be written
    * @param aAddress Address where data is sent (needed by UDP)
    * @param aProtocol Protocol to be used (TCP or UDP)
    */
    void IssueWriteL(const TDesC8& aData, TInetAddr* aAddress, TUint aProtocol);
    
    /**
    * Write the data to the socket flooded
    * @param aData the data to be written
    * @param aAddress Address where data is sent (needed by UDP)
    * @param aProtocol Protocol to be used (TCP or UDP)
    * @param aCount The packet will be sent aCount times
    */
    void IssueWriteL(const TDesC8& aData, TInetAddr* aAddress, TUint aProtocol, TInt aCount);
    
    /**
    * Write the data to the socket (buffered)
    * @param aData the data to be written
    * @param aAddress Address where data is sent (needed by UDP)
    * @param aProtocol Protocol to be used (TCP or UDP)
    */
    void IssueWriteL( const TDesC8& aData, 
                      TInetAddr* aAddress, 
                      TUint aProtocol, 
                      TInt aPacketSize, 
                      TInt aPackets );
    
protected: // from CActive
          
    /**
    * Cancel any outstanding operation
    */
    void DoCancel();
    
    /**
    * Called when operation complete
    */
    void RunL();	
    
    // From MTimeOutNotify
    void TimerExpired(); 
    
private:

    /**
    * Perform the first phase of two phase construction 
    * @param aConsole console to use for ui output
    * @param aSocket socket to write to
    * @param aSendHandler handler to notify UDP packet sends
    */
    CSocketsWrite( MUINotify& aConsole, 
                   RSocket& aSocket, 
                   MDataSendNotifyHandler& aSendHandler );
    
    /**
    * Perform the second phase construction of a CSocketsWrite 
    */
    void ConstructL();
    
    /**
    * Handle a 'write buffer empty' situation.
    */    
    void SendNextPacket();

    /**
    * Calls the actual write when flooding
    */
    void DoFloodWrite();
    
    
private: // Member data
    enum TWriteState 
    {
        ESending, EWaiting ,ECommsFailed
    };
    
    
    // Member variables
    RSocket&                iSocket;
    MUINotify&              iConsole; // console for displaying text etc
    //TBuf8<KMaxSendBuffer>   iTransferBuffer; // Accumulate data to send in here
    //TBuf8<KMaxSendBuffer>   iWriteBuffer; // Holds data currently being sent to socket
    HBufC8*                 iTransferBuffer;
    HBufC8*                 iWriteBuffer;
    CTimeOutTimer*          iTimer;
    TInt                    iTimeOut;
    TWriteState             iWriteStatus;
    TInetAddr*              iAddress;
    TUint                   iProtocol;
	TInt                    iPackets;
	TInt                    iPacketSize;
    TInt                    iSentBytes;
	TTime                   iStartTime;
	TInt                    iDataChunkCount;
	HBufC8*                 iReqBodySubmitBuffer;
	TPtr8                   iReqBodySubmitBufferPtr;
	MDataSendNotifyHandler& iSendHandler;
	TBool                   iFlood; // Flood mode sending
	TInt                    iCount; // Times to flood the packet
};

#endif // __SOCKETSWRITE_H__

