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

#ifndef __CFILEMONITORTRACES_H__
#define __CFILEMONITORTRACES_H__

#define KOstTraceComponentID 0x2001f703

#define CFILEMONITOR_CFILEMONITOR_ENTRY 0x8a000b
#define CFILEMONITOR_CFILEMONITOR_EXIT 0x8a000c
#define CFILEMONITOR_RUNL_ENTRY 0x8a000d
#define CFILEMONITOR_RUNL_EXIT 0x8a000e
#define CFILEMONITOR_STARTMONITORING_ENTRY 0x8a000f
#define CFILEMONITOR_STARTMONITORING_EXIT 0x8a0010
#define CFILEMONITOR_INITIALIZE_ENTRY 0x8a0011
#define CFILEMONITOR_INITIALIZE_EXIT 0x8a0012
#define DUP1_CFILEMONITOR_INITIALIZE_EXIT 0x8a0013
#define CFILEMONITOR_RUNERROR 0x86000f
#define DUP1_CFILEMONITOR_RUNL 0x860010
#define DUP4_CFILEMONITOR_RUNL 0x860011
#define DUP2_CFILEMONITOR_RUNL 0x860012
#define DUP3_CFILEMONITOR_RUNL 0x860013
#define DUP5_CFILEMONITOR_RUNL 0x860014
#define DUP6_CFILEMONITOR_RUNL 0x860015
#define DUP7_CFILEMONITOR_RUNL 0x860016
#define DUP8_CFILEMONITOR_RUNL 0x860017
#define DUP9_CFILEMONITOR_RUNL 0x860018
#define DUP10_CFILEMONITOR_RUNL 0x860019
#define DUP11_CFILEMONITOR_RUNL 0x86001a
#define DUP12_CFILEMONITOR_RUNL 0x86001b
#define DUP13_CFILEMONITOR_RUNL 0x86001c
#define CFILEMONITOR_STARTMONITORING 0x86001d
#define CFILEMONITOR_RESETSTATUS 0x86001e
#define CFILEMONITOR_INITIALIZE 0x86001f
#define DUP1_CFILEMONITOR_INITIALIZE 0x860020
#define CFILEMONITOR_ADDNOTIFICATIONPATHSL 0x860021
#define DUP1_CFILEMONITOR_ADDNOTIFICATIONPATHSL 0x860022
#define DUP2_CFILEMONITOR_ADDNOTIFICATIONPATHSL 0x860023
#define DUP3_CFILEMONITOR_ADDNOTIFICATIONPATHSL 0x860024
#define CFILEMONITOR_REMOVENOTIFICATIONPATHS 0x860025
#define DUP1_CFILEMONITOR_REMOVENOTIFICATIONPATHS 0x860026
#define DUP2_CFILEMONITOR_REMOVENOTIFICATIONPATHS 0x860027
#define DUP3_CFILEMONITOR_REMOVENOTIFICATIONPATHS 0x860028


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

