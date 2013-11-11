// Copyright (c) 1998-2009 Nokia Corporation and/or its subsidiary(-ies).
// All rights reserved.
// This component and the accompanying materials are made available
// under the terms of "Eclipse Public License v1.0"
// which accompanies this distribution, and is available
// at the URL "http://www.eclipse.org/legal/epl-v10.html".
//
// Initial Contributors:
// Nokia Corporation - initial contribution.
//
// Contributors:
//
// Description:
// Test Enviroment for protocol code
// 
//

#ifndef _TESTENV_H_
#define _TESTENV_H_

#include <e32std.h>
#include <e32test.h>
#include <c32comm.h>
#include <in_sock.h>

extern RTest Test;
extern RSocketServ SocketServer;
extern RHeap* TestHeap;

const TInt KEchoPort = 7;
const TInt KChargenPort = 19;
const TInt KDiscardPort = 9;
const TInt KTestPort = 9;

const TInt KSmallBufSize = 256;
const TInt KLargeBufSize = 2048;
const TInt KHugeBufSize = 16384;

typedef TBuf8<KSmallBufSize> TSmallBuf;
typedef TBuf8<KLargeBufSize> TLargeBuf;
typedef TBuf8<KHugeBufSize> THugeBuf;


void CommInitL(TBool aEnhanced);
TInt AsyncStart();

#define TEST(a) DoTest((a), __FILE__, __LINE__, 0)
#define TESTE(a,b) DoTest((a), __FILE__, __LINE__, b)
void WaitKey();
void DoTest(TBool aCondition, char* aFile, TInt aLine, TInt aErr);

void SockStart();
void SockClose();

#define IPADDR(a,b,c,d) (TUint32)(((a)<<24)|((b)<<16)|((c)<<8)|(d))


void StripeMem(TUint8 *aBuf, TInt aStartPos, TInt anEndPos, TUint aStartChar, TUint anEndChar, TInt aOffset=0);
void StripeDes(TDes8 &aBuf, TInt aStartPos, TInt anEndPos, TUint aStartChar, TUint anEndChar, TInt aOffset=0);
TBool CheckMem(TUint8 *aBuf, TInt aStartPos, TInt anEndPos, TUint aStartChar, TUint anEndChar, TInt aOffset=0);
TBool CheckDes(TDes8 &aBuf, TInt aStartPos, TInt anEndPos, TUint aStartChar, TUint anEndChar, TInt aOffset=0);
void StripeMem32(TUint *aBuf, TInt aStartPos, TInt aEndPos);
void StripeDes32(TDes8 &aBuf, TInt aStartPos, TInt anEndPos);


class TIoBuf : public TPtr8
	{
public:
	TIoBuf();
	~TIoBuf();
	TInt Alloc(TUint aSize);
	};

void OutputInetAddr(TDes& aBuf, TUint32 aAddr);
void GetHostByName(TNameRecord& aRecord, const TDesC& aHost);
void GetHostByAddr(TNameRecord& aRecord, const TInetAddr& aAddr);
void GetHostName(THostName& aName);
void GetHostAddr(TInetAddr& aAddr);

#endif
