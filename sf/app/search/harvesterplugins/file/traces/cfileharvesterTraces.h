/*
* Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
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
* Description: 
*
*/
// Created by TraceCompiler 2.3.0
// DO NOT EDIT, CHANGES WILL BE LOST

#ifndef __CFILEHARVESTERTRACES_H__
#define __CFILEHARVESTERTRACES_H__

#define KOstTraceComponentID 0x2001f703

#define CFILEHARVESTER_CFILEHARVESTER_ENTRY 0x8a0001
#define CFILEHARVESTER_CFILEHARVESTER_EXIT 0x8a0002
#define CFILEHARVESTER_STARTL_ENTRY 0x8a0003
#define CFILEHARVESTER_STARTL_EXIT 0x8a0004
#define CFILEHARVESTER_GETNEXTFOLDERL_ENTRY 0x8a0005
#define CFILEHARVESTER_GETNEXTFOLDERL_EXIT 0x8a0006
#define CFILEHARVESTER_GETFILEIDL_ENTRY 0x8a0007
#define CFILEHARVESTER_GETFILEIDL_EXIT 0x8a0008
#define CFILEHARVESTER_RUNL_ENTRY 0x8a0009
#define CFILEHARVESTER_RUNL_EXIT 0x8a000a
#define CFILEHARVESTER_RESUMEREQUEST_ENTRY 0x8a003a
#define CFILEHARVESTER_RESUMEREQUEST_EXIT 0x8a003b
#define CFILEHARVESTER_ADDIGNOREPATHSL 0x860001
#define DUP1_CFILEHARVESTER_ADDIGNOREPATHSL 0x860002
#define DUP2_CFILEHARVESTER_ADDIGNOREPATHSL 0x860003
#define CFILEHARVESTER_REMOVEIGNOREPATHS 0x860004
#define DUP1_CFILEHARVESTER_REMOVEIGNOREPATHS 0x860005
#define DUP2_CFILEHARVESTER_REMOVEIGNOREPATHS 0x860006
#define CFILEHARVESTER_REMOVEIGNOREPATH 0x860007
#define CFILEHARVESTER_CHECKPATH 0x860008
#define CFILEHARVESTER_GETNEXTFOLDERL 0x860009
#define DUP1_CFILEHARVESTER_GETNEXTFOLDERL 0x86000a
#define DUP2_CFILEHARVESTER_GETNEXTFOLDERL 0x86000b
#define CFILEHARVESTER_DOCANCEL 0x86000c
#define CFILEHARVESTER_RUNERROR 0x86000d
#define CFILEHARVESTER_SETNEXTREQUEST 0x86000e


#ifndef __KERNEL_MODE__
#ifndef __OSTTRACEGEN1_TUINT32_CONST_TDESC16REF__
#define __OSTTRACEGEN1_TUINT32_CONST_TDESC16REF__

inline TBool OstTraceGen1( TUint32 aTraceID, const TDesC16& aParam1 )
    {
    TBool retval;
    TInt size = aParam1.Size();
    // BTrace assumes that parameter size is atleast 4 bytes
    if (size % 4 == 0)
        {
        TUint8* ptr = ( TUint8* )aParam1.Ptr();
        // Data is written directly and length is determined from trace message length
        retval = OstSendNBytes( EXTRACT_GROUP_ID(aTraceID), EOstTrace, KOstTraceComponentID, aTraceID, ptr, size );
        }
    else
        {
        TUint8 data[ KOstMaxDataLength ];
        TUint8* ptr = data;
        if (size > KOstMaxDataLength)
            {
            size = KOstMaxDataLength;
            }
        TInt sizeAligned = ( size + 3 ) & ~3;
        memcpy( ptr, aParam1.Ptr(), size );
        ptr += size;
        // Fillers are written to get 32-bit alignment
        while ( size++ < sizeAligned )
            {
            *ptr++ = 0;
            }
        ptr -= sizeAligned;
        size = sizeAligned;
        // Data is written directly and length is determined from trace message length
        retval = OstSendNBytes( EXTRACT_GROUP_ID(aTraceID), EOstTrace, KOstTraceComponentID, aTraceID, ptr, size );
        }
    return retval;
    }

#endif // __OSTTRACEGEN1_TUINT32_CONST_TDESC16REF__

#endif


#endif

// End of file

