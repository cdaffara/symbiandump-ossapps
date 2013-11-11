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
* Description: CSocketsRead is an active obejct that reads data from
* an already open socket and calculates some metrics from the data read
*
*/

// INCLUDE FILES
#include "SocketsRead.h"
#include "SocketsEngine.h"
#include "uinotify.h"
#include "Utils.h"


// ---------------------------------------------------------
// CSocketsRead::NewL(MUINotify& aConsole, RSocket& aSocket)
// EPOC two phased constructor
// ---------------------------------------------------------
//
CSocketsRead* CSocketsRead::NewL(MUINotify& aConsole, RSocket& aSocket, CSocketsEngine& aEngine)
{
    CSocketsRead* self = CSocketsRead::NewLC(aConsole, aSocket, aEngine);
    CleanupStack::Pop(self);
    return self;
}

// ---------------------------------------------------------
// CSocketsRead::NewLC(MUINotify& aConsole, RSocket& aSocket)
// EPOC two phased constructor
// ---------------------------------------------------------
//
CSocketsRead* CSocketsRead::NewLC(MUINotify& aConsole, RSocket& aSocket, CSocketsEngine& aEngine)
{
    CSocketsRead* self = new (ELeave) CSocketsRead(aConsole, aSocket, aEngine);
    CleanupStack::PushL(self);
    self->ConstructL();
    return self;
}

// ---------------------------------------------------------
// CSocketsRead::CSocketsRead(MUINotify& aConsole, RSocket& aSocket)
// Constructor
// ---------------------------------------------------------
//
CSocketsRead::CSocketsRead(MUINotify& aConsole, RSocket& aSocket, CSocketsEngine& aEngine)
: CActive(/*EPriorityStandard*/EPriorityHigh),
iEngine(aEngine),
iSocket(aSocket),
iConsole(aConsole)
{
}

// ---------------------------------------------------------
// CSocketsRead::~CSocketsRead()
// Destructor
// ---------------------------------------------------------
//
CSocketsRead::~CSocketsRead()
{
    Cancel();
}

// ---------------------------------------------------------
// CSocketsRead::ConstructL()
// EPOC two-phased constructor
// ---------------------------------------------------------
//
void CSocketsRead::ConstructL()
{
    CActiveScheduler::Add(this);
}


// ---------------------------------------------------------
// CSocketsRead::Start(TInetAddr* aAddress, TUint aProtocol)
// Initiate a read from socket.
// ---------------------------------------------------------
//
void CSocketsRead::Start(TInetAddr* aAddress, TUint aProtocol)
{
    iAddress = aAddress;
    iProtocol = aProtocol;

    // Initiate a new read from socket into iBuffer
    if (!IsActive())
    {
        IssueRead();
    }
}

// ---------------------------------------------------------
// CSocketsRead::StartRAWRead()
// Start RAW read
// ---------------------------------------------------------
//

void CSocketsRead::StartRAWRead(TInetAddr* aAddress, TUint aProtocol)
	{
    iAddress = aAddress;
    iProtocol = aProtocol;
    iBodySize = 0; // REMEMBER TO DO THIS, OR OTHERWISE WE THINK THIS IS HTTP TRAFFIC
    iDoPerformance = ETrue;
	iUdpPacketCounter = 0;
	
    // Initiate a new read from socket into iBuffer
    iConsole.PrintNotify(_L("Getting data...\n"));
    iDoCount = ETrue;
    iStartTime.UniversalTime();
    iReceivedBytes = 0;
    
    if (!IsActive())
    {
        IssueRead();
	}
}


// ---------------------------------------------------------
// CSocketsRead::IssueRead()
// Receive data.
// ---------------------------------------------------------
//
void CSocketsRead::IssueRead()
{
    // Initiate a new read from socket into iBuffer
    ASSERT(!IsActive());
    
    switch(iProtocol)
    {
    case KProtocolInetTcp:
        iSocket.RecvOneOrMore(iBuffer, 0, iStatus, iDummyLength);
        break;
    case KProtocolInetUdp:
    	iBuffer.Zero();
        iSocket.RecvFrom(iBuffer, *iAddress, 0, iStatus/*, iDummyLength*/);
        break;
    }    
    SetActive();
}


// ---------------------------------------------------------
// CSocketsRead::SetPerformance(const TBool aValue)
// Set performance calculation on/off
// ---------------------------------------------------------
//
void CSocketsRead::SetPerformance(const TBool aValue)
{
    iDoPerformance = aValue;
    iReceivedBytes = 0;
    iDoCount = EFalse;
    
    //TInt recBuf;
    //iSocket.SetOpt(KSORecvBuf, KSOLSocket, 200000);
    //iSocket.GetOpt(KSORecvBuf, KSOLSocket, recBuf);
}


// ---------------------------------------------------------
// CSocketsRead::RunL()
// Called when request has completed.
// ---------------------------------------------------------
//
void CSocketsRead::RunL()
    {
    // Active object request complete handler
    if (iStatus == KErrNone)
        {
        // Character has been read from socket
        if(iDoPerformance)
            {
            if(!iDoCount)
                {
                iHeaders.Append(iBuffer.Left(iHeaders.MaxLength() - iHeaders.Length()));
                iHeaders.LowerCase();

                TInt end = iHeaders.Find(_L8("\r\n\r\n"));
                TInt emptyLine = iBuffer.Find(_L8("\r\n\r\n"));

                if(!(end == KErrNotFound))
                    {
                    TInt cl = iHeaders.Find(_L8("content-length:"));

                    if(!(cl == KErrNotFound))
                        {
                        TBuf8<512> temp;
                        temp = iHeaders.Mid(cl, end - cl);

                        TInt st = temp.Locate(':');
                        TInt endcl = temp.Find(_L8("\r\n"));

                        if ( endcl != KErrNotFound )
                            {
                            // not the last line in headers.
                            temp = temp.Mid(st+1, endcl - st);                            
                            }
                        else
                            {
                            temp = temp.Mid(st+1);                            
                            }
                        
                        temp.Trim();

                        TLex8 lex(temp);
                        TInt err = lex.Val(iBodySize);

                        if(err == KErrNone)
                            {
                            iConsole.PrintNotify(_L("Getting body...\n"));
                            iDoCount = ETrue;
                            iStartTime.UniversalTime();
                            iReceivedBytes = iBuffer.Length() - emptyLine - 4; 
                            }
                        }
                    }
                }
            else
                {          
                iReceivedBytes += iBuffer.Length();
                if (iProtocol == KProtocolInetUdp)
                    {
                    if (iUdpPacketCounter == 0)
                        iStartTime.UniversalTime(); // Start time calculation from the first received packet.
                    iUdpPacketCounter++;
                    if (iUdpPacketCounter == KMaximumPacketCount)
                        {
                        TBuf8<128> b(_L8("Got data\n"));
                        TInt ploss = CalculatePacketLoss();
                        Utils::CalculateThroughput(b, iStartTime, iReceivedBytes);
                        iEngine.Disconnect();
                        b.AppendFormat(_L8("Ploss: %d %%\n"),ploss);
                        iConsole.PrintNotify(b);
                        return; // do not issue any request.
                        }
                    else if (iUdpPacketCounter > KMaximumPacketCount)
                        {
                        iConsole.PrintNotify(_L8("Unexpected state\n"));
                        iEngine.Disconnect();
                        iSocket.Close();
                        return;
                        }

                    }
                }

            // Check if all the data has been received
            if(iReceivedBytes == iBodySize)
                {
                // Throughput calculation
                TBuf8<128> b(_L8("Got body\n"));
                Utils::CalculateThroughput(b, iStartTime, iReceivedBytes);
                iConsole.PrintNotify(b);
                iHeaders.Zero();
                iDoCount = EFalse;
                }
            }
        else
            {            
            iConsole.PrintNotify(iBuffer);
            }

        IssueRead(); // Immediately start another read
        }
    else
        {
        if( iStatus.Int() == KErrEof || 
                iStatus.Int() == KErrDisconnected || 
                iStatus.Int() == KErrCancel )
            {
            TBuf<64> text(_L("HTTP Get Completed"));
            iEngine.StopTickCount(text);
            // Remote server closed the socket
            if (iBodySize == 0) // We don't know how much we are expecting data so we calculate throughput here
                {
                iReceivedBytes += iBuffer.Length();
                // Throughput calculation
                TBuf8<128> b(_L8("Got body\n"));
                Utils::CalculateThroughput(b, iStartTime, iReceivedBytes);
                iConsole.PrintNotify(b);
                }
            TBuf<64> note;
            note.Format(_L("\nSocket closed: (%d)\n"), iStatus.Int());
            iEngine.Disconnect();
            iConsole.PrintNotify(note);
            }
        else if(iStatus.Int() == KErrTimedOut)
            {
            // LastSocketActivityTimeout has been elapsed
            TBuf<64> note;
            note.Copy(_L("\nLastSocketActivityTimeOut elapsed\n\n"));
            iEngine.Disconnect();
            iConsole.PrintNotify(note);
            }
        else
            {
            // Error: pass it up to user interface
            TBuf<50> err;
            err.Format(_L("\nCSocketsRead error %d\n"), iStatus.Int());
            iConsole.PrintNotify(err);
            }
        iSocket.Close();  // error is always fatal.      
        }	
    }

// ---------------------------------------------------------
// CSocketsRead::DoCancel()
// Cancel ongoing requests.
// ---------------------------------------------------------
//
void CSocketsRead::DoCancel()
{
    // Cancel asychronous read request
    iSocket.CancelRead();
    // If we were accepting raw UDP-traffic, we should stop and analyse.
    if (iBodySize == 0 && iProtocol == KProtocolInetUdp && iReceivedBytes != 0)
	{
        TBuf8<128> b(_L8("Got body\n"));
        Utils::CalculateThroughput(b, iStartTime, iReceivedBytes);
        iConsole.PrintNotify(b);
	}
}

// ---------------------------------------------------------
// CSocketsRead::CalculatePacketLoss()
// Calculates packet loss rate
// @return ploss percentage
// ---------------------------------------------------------
//
TInt32 CSocketsRead::CalculatePacketLoss()
{
	TUint32* seqNumberPointer = (TUint32*)(iBuffer.Ptr());
	TUint32 sequenceNumberOfThisPacket = ByteOrder::Swap32( *seqNumberPointer );
	if(sequenceNumberOfThisPacket != 0)
	    return (100 - (100 * iUdpPacketCounter) / sequenceNumberOfThisPacket);
	else return 100;
}

// EOF
