/*
* Copyright (c) 2002-2006 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description: Packet probe hook
*
*/

#include <e32std.h>
#include <e32base.h>
#include <f32file.h>
#include <e32svr.h>
#ifdef SYMBIAN_ENABLE_SPLIT_HEADERS
#include <es_prot_internal.h>
#endif


#include "family.h"
#include "prt.h"
#include "sap.h"

_LIT(KTcpDumpFolder, "tcpdump\\");
_LIT(KProbeDumpFile, "probe.cap");
_LIT(KLogFolder,     "c:\\logs\\");


CProtocolProbe::CProtocolProbe(TUint aId) : 
iId(aId), 
iDumpCb(CActive::EPriorityStandard),
iFileServerOpen(EFalse),
iFileOpen(EFalse),
iBufCreated(EFalse)
	{

	iTimeOrigin.UniversalTime();

	TCallBack cbFunc(DumpCb, this);
	iDumpCb.Set(cbFunc);
	}

void CProtocolProbe::InitL(TDesC& aTag)
	{

	CProtocolBase::InitL(aTag);
	}

void CProtocolProbe::StartL()
	{
	}

CProtocolProbe::~CProtocolProbe()
	{
	if(iFileOpen)
	    {
		iFile.Close();
		iFileOpen = EFalse;
	    }
	if(iFileServerOpen)
		{
		iFs.Close();
		iFileServerOpen = EFalse;
		}	
    if(iBufCreated)
        {
        iBuf.Close();
        iBufCreated = EFalse;
        }
	}

CProtocolProbe *CProtocolProbe::NewL(TUint aId)
	{

	return new (ELeave) CProtocolProbe(aId);
	}

void CProtocolProbe::Identify(TServerProtocolDesc& anEntry, TUint aId)
	{

	anEntry.iName=_S("probe");
	if (aId > 1)
		{
		anEntry.iName.AppendNum(aId-1);
		}

	anEntry.iAddrFamily = KAfProbe;
	anEntry.iSockType = KSockDatagram;
	anEntry.iProtocol = aId;
	anEntry.iVersion = TVersion(1, 0, 0);
	anEntry.iByteOrder = EBigEndian;
	anEntry.iServiceInfo = KSIDatagram | KSIConnectionLess;
	anEntry.iNamingServices = 0;
	anEntry.iSecurity = KSocketNoSecurity;
	anEntry.iMessageSize = 0xffff;
	anEntry.iServiceTypeInfo = ESocketSupport;
	anEntry.iNumSockets = KUnlimitedSockets;
	}

void CProtocolProbe::Identify(TServerProtocolDesc *aDesc) const
	{

	Identify(*aDesc, iId);
	}

CServProviderBase* CProtocolProbe::NewSAPL(TUint aProtocol)
	{

	if (aProtocol != KSockDatagram)
		{
		User::Leave(KErrNotSupported);
		}

	CProviderProbe* sap = new (ELeave) CProviderProbe(this);
	sap->iNext = iList;
	iList = sap;
	return sap;
	}

//
// CProtocolProbe::CancelSAP
// *************************
// Disconnect SAP from the protocol
//
void CProtocolProbe::CancelSAP(CServProviderBase *aSAP)
	{
	
	CProviderProbe **h, *sap;
	for (h = &iList; (sap = *h) != NULL; h = &sap->iNext)
		if (sap == aSAP)
			{
			*h = sap->iNext;
			break;
			}
	}

// CProtocolProbe::NetworkAttachedL
// ********************************
// When network becomes available, do the hooking!
//
void CProtocolProbe::NetworkAttachedL()
	{

	NetworkService()->BindL(this, MIp6Hook::BindPostHook());
	NetworkService()->BindL(this, MIp6Hook::BindPostHook()+1);

	// initialise dump file
	if(iFileOpen)
		{
		iFile.Close();
		iFileOpen = EFalse;
		}
	if(iFileServerOpen)
		{
		iFs.Close();
		iFileServerOpen = EFalse;
		}

	User::LeaveIfError(iFs.Connect());
    iFileServerOpen = ETrue;
    TBuf<50> filename;
    filename.Append(KLogFolder);
    filename.Append(KTcpDumpFolder);
    filename.Append(KProbeDumpFile);
	User::LeaveIfError(iFile.Replace(iFs, filename, EFileWrite));
    iFileOpen = ETrue;

    // allocate buffer
    if(!iBufCreated)
        {
        iBuf.CreateL(65535);
        iBufCreated = ETrue;
        }
    
	LibcapDumpFileHeader();
	}

//
//	CProtocolProbe::Dump
//	***********************
//	Log the packet to file
//
void CProtocolProbe::Dump(RMBufChain &aPacket)
	{

	RMBufPacket packet;
	packet.Assign(aPacket);
	RMBufPktInfo* info = packet.Unpack();

	TUint32 secs = static_cast<TUint32>(info->iProtocol);
	TUint32 micros = static_cast<TUint32>(info->iFlags);

    iBuf.SetMax();
    packet.CopyOut(iBuf, 0);
    if(iBuf.Length() != info->iLength)
        {
        iBuf.SetLength(info->iLength);
        }
	LibcapDump(iBuf, secs, micros);
	packet.Free();
	}

TInt CProtocolProbe::Send(RMBufChain &aPacket, CProtocolBase* aSrc)
	{

	Queue(aPacket);
	return CProtocolPosthook::Send(aPacket, aSrc);
	}

void CProtocolProbe::Process(RMBufChain &aPacket, CProtocolBase* aSrc)
	{

	Queue(aPacket);
	CProtocolPosthook::Process(aPacket, aSrc);
	}

void CProtocolProbe::LibcapDumpFileHeader()
//
//  Dump file header in a format compatible with Libcap
//
//  Format is:
//  
//  struct FileHeader
//      {
//	    TUint32 magic; 
//	    TUint16 version_major;
//	    TUint16 version_minor;
//	    TUint32 thiszone;	/* gmt to local correction */
//	    TUint32 sigfigs;	/* accuracy of timestamps */
//	    TUint32 snaplen;	/* max length saved portion of each pkt */
//	    TUint32 linktype;	/* data link type (LINKTYPE_*) */
//      };
//
//  Note LINKTYPE specified in libpcap/bpf/net/bpf.h (see www.tcpdump.org)
//
	{

	TBuf8<sizeof(TUint32)*5+sizeof(TUint16)*2> fileHeader;
	*((TUint32*) &(fileHeader.Ptr()[0])) = 0xa1b2c3d4;
	*((TUint16*) &(fileHeader.Ptr()[4])) = 0x02; 
	*((TUint16*) &(fileHeader.Ptr()[6])) = 0x04; 
	*((TUint32*) &(fileHeader.Ptr()[8])) = 0x00;
	*((TUint32*) &(fileHeader.Ptr()[12])) = 0x00;
	*((TUint32*) &(fileHeader.Ptr()[16])) = 0xffff;
	*((TUint32*) &(fileHeader.Ptr()[20])) = 12;       // DLT_RAW		12	/* raw IP */
	fileHeader.SetLength(fileHeader.MaxLength());

	iFile.Write(fileHeader);
	}

void CProtocolProbe::LibcapDump(const TDesC8& aBuffer, TUint32 aTimeStampSecs, TUint32 aTimeStampMicros)
//
//  Dumps a packet in a format compatbible with Libcap
//
//  For each record the format is:
//
//  struct record 
//      {
//      TUint32 sec;	/* time stamp - secs*/
//      TUint32 usec;	/* time stamp - microsecs*/
//      TUint32 captureLen;	/* length packet captured */
//      TUint32 packetLen;	/* total length of packet*/
//      };
//
//  Byte ordering of the header is little endian
//  Byte ordering of the packet is network byte order (big endian)
//
	{
	
	TBuf8<sizeof(TUint32)*4> recordHeader;
	recordHeader.FillZ();

	*((TUint32*) &(recordHeader.Ptr()[0])) = aTimeStampSecs;
	*((TUint32*) &(recordHeader.Ptr()[4])) = aTimeStampMicros;
	*((TUint32*) &(recordHeader.Ptr()[8])) = aBuffer.Length();
	*((TUint32*) &(recordHeader.Ptr()[12])) = aBuffer.Length();
	recordHeader.SetLength(recordHeader.MaxLength());

	iFile.Write(recordHeader);
	iFile.Write(aBuffer);
	}

void CProtocolProbe::Queue(RMBufChain &aPacket)
//
//  Takes a copy of aPacket, adds it to the queue and triggers the callback
//
	{

	RMBufPacketBase copy;
	TRAPD(err, copy.CopyPackedL(aPacket));
	if (err == KErrNone)
		{

		//
		// Calculate packet time-stamp
		//
		TTime newTime;
		newTime.UniversalTime();
		TTimeIntervalMicroSeconds interval = newTime.MicroSecondsFrom(iTimeOrigin);

		const TInt KMicrosInASecond = 1000000;

		// To avoid possible overflow caused by microsecond accuracy (32 bits
		// of microseconds is just a bit over an hour), use 64-bit variables
		// in calculation of timestamp
		TInt64 secs = interval.Int64() / KMicrosInASecond;

		//
		// Reuse the protocol and flags fields of
		// RMBufPktInfo to store the time-stamp
		//
		RMBufPktInfo* info = RMBufPacket::PeekInfoInChain(copy);
		// Just let the seconds overflow in case of timestamp is over the
		// integer scope (nothing reasonable to do in this case anyway)
		info->iProtocol = static_cast<TInt>( secs );
		info->iFlags = static_cast<TUint>( interval.Int64() - ( secs * KMicrosInASecond ));

		iQueue.Append(copy);
		iDumpCb.CallBack();
		}
	else
		{
		copy.Free();
		}
	}

TInt CProtocolProbe::DumpCb(TAny* aThisPtr)
//
//  Callback function - calls DumpQueuedPackets()
//
	{

	CProtocolProbe* self = static_cast<CProtocolProbe*>(aThisPtr);
	self->DumpQueuedPackets();
	return KErrNone;
	}

void CProtocolProbe::DumpQueuedPackets()
//
//  Dumps all packets on the queue to the log file
//
	{

	RMBufPacketBase packet;
	while(iQueue.Remove(packet))
		{
		Dump(packet);
		}
	}
