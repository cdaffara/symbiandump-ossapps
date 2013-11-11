/*
 * Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies). 
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

// INCLUDE FILES
#include <e32svr.h>

#include "SocketsWrite.h"
#include "TimeOutTimer.h"
#include "ConnTest.pan"
#include "uinotify.h"
#include "Utils.h"
#include "conntest.hrh"
#include "datasendnotifyhandler.h"

// CONSTANTS
static const TInt KTimeOut = 30000000; // 30 seconds time-out


// ---------------------------------------------------------
// CSocketsWrite::NewL(MUINotify& aConsole, RSocket& aSocket, MDataSendNotifyHandler& aSendHandler)
// EPOC two phased constructor
// ---------------------------------------------------------
//
CSocketsWrite* CSocketsWrite::NewL( MUINotify& aConsole, 
        RSocket& aSocket, 
        MDataSendNotifyHandler& aSendHandler )
    {
    CSocketsWrite* self = CSocketsWrite::NewLC(aConsole, aSocket, aSendHandler);
    CleanupStack::Pop(self);
    return self;
    }


// ---------------------------------------------------------
// CSocketsWrite::NewLC(MUINotify& aConsole, RSocket& aSocket, MDataSendNotifyHandler& aSendHandler)
// EPOC two phased constructor
// ---------------------------------------------------------
//
CSocketsWrite* CSocketsWrite::NewLC( MUINotify& aConsole, 
        RSocket& aSocket, 
        MDataSendNotifyHandler& aSendHandler )
    {
    CSocketsWrite* self = new (ELeave) CSocketsWrite(aConsole, aSocket, aSendHandler);
    CleanupStack::PushL(self);
    self->ConstructL();
    return self;
    }


// ---------------------------------------------------------
// CSocketsWrite::CSocketsWrite(MUINotify& aConsole, RSocket& aSocket, MDataSendNotifyHandler& aSendHandler)
// Constructor
// ---------------------------------------------------------
//
CSocketsWrite::CSocketsWrite( MUINotify& aConsole, 
        RSocket& aSocket, 
        MDataSendNotifyHandler& aSendHandler ):
        CActive(EPriorityStandard),
        iSocket(aSocket),
        iConsole(aConsole),
        iPackets(0),
        iPacketSize(0),
        iReqBodySubmitBufferPtr(0,0),
        iSendHandler(aSendHandler)
    {
    }


// ---------------------------------------------------------
// CSocketsWrite::~CSocketsWrite()
// Destructor
// ---------------------------------------------------------
//
CSocketsWrite::~CSocketsWrite()
    {
    Cancel();
    delete iReqBodySubmitBuffer;
    delete iTimer;
    iTimer = NULL;
    if(iTransferBuffer)
        {
        delete iTransferBuffer;
        iTransferBuffer = NULL;
        }
    if(iWriteBuffer)
        {
        delete iWriteBuffer;
        iWriteBuffer = NULL;
        }
    }


// ---------------------------------------------------------
// CSocketsWrite::ConstructL()
// EPOC two-phased constructor
// ---------------------------------------------------------
//
void CSocketsWrite::ConstructL()
    {
    CActiveScheduler::Add(this);

    iTransferBuffer = HBufC8::NewL(KMaxSendBuffer);
    iWriteBuffer = HBufC8::NewL(KMaxSendBuffer);

    iTimeOut = KTimeOut; 
    iTimer = CTimeOutTimer::NewL(10, *this);
    iWriteStatus = EWaiting;
    }


// ---------------------------------------------------------
// CSocketsWrite::RunL()
// Called when request has completed.
// ---------------------------------------------------------
//
void CSocketsWrite::RunL()
    {
    if(!iFlood)
        {   
        RDebug::Print(_L("ConnTest: CSocketsWrite::RunL - iStatus = %d"), iStatus.Int());
        }
    // Active object request complete handler
    if (iStatus == KErrNone)
        {
        switch(iWriteStatus)
            {
            // Character has been written to socket
            case ESending:
                if(!iFlood)
                    SendNextPacket();
                else
                    DoFloodWrite();
                break;
            default:
                User::Panic(KPanicSocketsWrite, EConnTestBadStatus);
                break;
            };
        }
    else 
        {
        // Error: pass it up to user interface
        iTimer->Cancel();
        TBuf<50> err;
        err.Format(_L("\nCSocketsWrite error %d\n"), iStatus.Int());
        //iConsole.ErrorNotify(_L("\nCSocketsWrite error"), iStatus.Int());
        iConsole.PrintNotify(err);
        iWriteStatus = ECommsFailed;
        }
    }


// ---------------------------------------------------------
// CSocketsWrite::DoCancel()
// Cancel ongoing requests.
// ---------------------------------------------------------
//
void CSocketsWrite::DoCancel()
    {	
    RDebug::Print(_L("CSocketsWrite::DoCancel"));

    // Cancel asychronous write request
    iSocket.CancelWrite();

    iTimer->Cancel();
    }


// ---------------------------------------------------------
// CSocketsWrite::IssueWriteL()
// Add data into buffer for sending.
// ---------------------------------------------------------
//
void CSocketsWrite::IssueWriteL(const TDesC8& aData, TInetAddr* aAddress, TUint aProtocol)
    {
    RDebug::Print(_L("ConnTest: CSocketsWrite::IssueWriteL - aData.Length = %d"), aData.Length());

    iFlood = EFalse;
    iAddress = aAddress;
    iProtocol = aProtocol;

    // Write data to a stream socket
    if ((aData.Length() + iTransferBuffer->Length()) > iTransferBuffer->Des().MaxLength())
        {
        RDebug::Print(_L("ConnTest: CSocketsWrite::IssueWriteL - data doesn't fit in the transferbuffer"));
        RDebug::Print( _L( 
                "ConnTest: CSocketsWrite::IssueWriteL - data length = %d, buffer length = %d, buffer max length = %d"),
                aData.Length(), iTransferBuffer->Length(), iTransferBuffer->Des().MaxLength() );
        // Not enough space in buffer
        User::Leave(KErrOverflow);
        }

    // Add new data to buffer
    iTransferBuffer->Des().Append(aData);

    if (!IsActive())
        {
        SendNextPacket();
        }
    }

// ---------------------------------------------------------
// CSocketsWrite::IssueWriteL()
// Flood data over socket
// ---------------------------------------------------------
//
void CSocketsWrite::IssueWriteL( const TDesC8& aData, 
        TInetAddr* aAddress, 
        TUint aProtocol, 
        TInt aCount )
    {
    RDebug::Print(_L("ConnTest: CSocketsWrite::IssueWriteL - flood, packet count=%d"), aCount);
    RDebug::Print(_L("ConnTest: CSocketsWrite::IssueWriteL - aData.Length = %d"), aData.Length());
    iFlood = ETrue;
    iCount = 0;
    iPackets = aCount;
    iPacketSize = aData.Length();

    iAddress = aAddress;
    iProtocol = aProtocol;

    // Write data to a stream socket
    if (aData.Length() > iTransferBuffer->Des().MaxLength())
        {
        // Not enough space in buffer
        User::Leave(KErrOverflow);
        }

    // Add new data to buffer
    iTransferBuffer->Des().Zero();
    iTransferBuffer->Des().Append(aData);
    iWriteBuffer->Des().Copy(*iTransferBuffer);
    if (!IsActive())
        {
        DoFloodWrite();
        }
    }

// -------------------------------------------------------------------------
// CSocketsWrite::IssueWriteL()
// Add data into buffer for sending POST request, initializes body data.
// -------------------------------------------------------------------------
//
void CSocketsWrite::IssueWriteL( const TDesC8& aData, 
        TInetAddr* aAddress, 
        TUint aProtocol, 
        TInt aPacketSize, 
        TInt aPackets )
    {
    RDebug::Print(_L("ConnTest: CSocketsWrite::IssueWriteL - aData.Length = %d"), aData.Length());
    iFlood = EFalse;

    iPackets = aPackets;
    iPacketSize = aPacketSize;

    delete iReqBodySubmitBuffer;
    iReqBodySubmitBuffer = NULL;
    iReqBodySubmitBuffer = HBufC8::NewMaxL(KSendDataSize);
    iReqBodySubmitBufferPtr.Set(iReqBodySubmitBuffer->Des());

    // Create body chunk
    Utils::CreateDataChunk(iReqBodySubmitBufferPtr, iPacketSize);
    iDataChunkCount = 0;

    IssueWriteL(aData, aAddress, aProtocol);
    }

// ---------------------------------------------------------
// CSocketsWrite::SendNextPacket()
// Write data from buffer to socket.
// ---------------------------------------------------------
//
void CSocketsWrite::SendNextPacket()
    {
    RDebug::Print(_L("ConnTest: CSocketsWrite::SendNextPacket - iTransferBuffer.Length = %d, iPackets = %d"), iTransferBuffer->Length(), iPackets);

    iTimer->Cancel(); // Cancel TimeOut timer
    iWriteStatus = EWaiting;

    if (iTransferBuffer->Length() > 0)
        {
        // Move data from transfer buffer to actual write buffer
        iWriteBuffer->Des().Copy(*iTransferBuffer);
        iTransferBuffer->Des().Zero();

        switch(iProtocol)
            {
            case KProtocolInetTcp:
                iSocket.Write(*iWriteBuffer, iStatus); // Initiate actual write
                break;

            case KProtocolInetUdp:
                iSocket.SendTo(*iWriteBuffer, *iAddress, 0, iStatus);
                break;
            }
        iSendHandler.NotifySend(iWriteBuffer->Length());
        // Request timeout
        iTimer->After(iTimeOut);
        SetActive();
        iWriteStatus = ESending;
        }
    else if(iPackets)
        {
        // We are sending body data, i.e. this is POST request
        if(iDataChunkCount == 0)
            {
            iConsole.PrintNotify(_L("Sending body...\n"));
            iSentBytes = 0;
            iStartTime.UniversalTime();
            }

        TBool noMoreData = iDataChunkCount < iPackets ? EFalse : ETrue;
        ++iDataChunkCount;

        if(noMoreData)
            {
            // Throughput calculation
            TBuf8<128> b(_L8("Body sent\n"));
            Utils::CalculateThroughput(b, iStartTime, iSentBytes);

            b.Append(_L("\n\n"));
            iConsole.PrintNotify(b);

            iPackets = 0;
            iPacketSize = 0;
            iSentBytes = 0;

            return;
            }

        iSentBytes += iReqBodySubmitBufferPtr.Length();
        iSocket.Write(iReqBodySubmitBufferPtr, iStatus); 
        SetActive();
        iWriteStatus = ESending;

        }
    }

// ---------------------------------------------------------
// CSocketsWrite::DoFloodWrite()
// Floods the data to the socket
// ---------------------------------------------------------
//
void CSocketsWrite::DoFloodWrite()
    {
    //RDebug::Print(_L("ConnTest: CSocketsWrite::DoFloodWrite: %d"),iCount); // eats CPU
    iTimer->Cancel(); // Cancel TimeOut timer
    iWriteStatus = EWaiting;
    if (iCount != iPackets)
        {
        if( iTransferBuffer->Length() > 0)
            {
            switch(iProtocol)
                {
                case KProtocolInetTcp:
                    iSocket.Write(*iWriteBuffer, iStatus); // Initiate actual write
                    break;

                case KProtocolInetUdp:
                    {
                    TUint32* seqNumberPointer = (TUint32*)(iTransferBuffer->Des().Ptr());
                    *seqNumberPointer = ByteOrder::Swap32( iCount ); // put sequence number into to the packet
                    iSocket.SendTo(*iTransferBuffer, *iAddress, 0, iStatus);
                    break;
                    }
                default:
                    iConsole.ErrorNotify(_L("Unsupproted protocol\n\n"),KErrNotSupported );
                    return;
                }

            iCount++;
            iTimer->After(iTimeOut);
            SetActive();
            iWriteStatus = ESending;
            }
        }
    else
        {
        iTransferBuffer->Des().Zero();
        iWriteBuffer->Des().Zero();
        iSendHandler.NotifySend(iPacketSize*iPackets);   
        iFlood = EFalse;
        iCount = 0;
        iPackets = 0;
        iPacketSize = 0; 	    
        }
    }

// ---------------------------------------------------------
// CSocketsWrite::TimerExpired()
// Timeout, show error notification.
// ---------------------------------------------------------
//
void CSocketsWrite::TimerExpired()
    {
    Cancel();
    iWriteStatus = ECommsFailed;
    iConsole.ErrorNotify(_L("Write operation timed out\n"), KErrTimedOut);
    }

