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

#ifndef __CCALENDARPLUGINTRACES_H__
#define __CCALENDARPLUGINTRACES_H__

#define KOstTraceComponentID 0x2001f704

#define CCALENDARPLUGIN_DELAYEDCALLBACKL 0x860001
#define CCALENDARPLUGIN_CALCHANGENOTIFICATION 0x860002
#define CCALENDARPLUGIN_HANDLECHANGEDENTRYL 0x860003
#define DUP1_CCALENDARPLUGIN_HANDLECHANGEDENTRYL 0x860004
#define DUP2_CCALENDARPLUGIN_HANDLECHANGEDENTRYL 0x860005
#define DUP3_CCALENDARPLUGIN_HANDLECHANGEDENTRYL 0x860006
#define CCALENDARPLUGIN_CREATEENTRYL 0x860007
#define DUP1_CCALENDARPLUGIN_CREATEENTRYL 0x860008
#define DUP2_CCALENDARPLUGIN_CREATEENTRYL 0x860009
#define DUP8_CCALENDARPLUGIN_CREATEENTRYL 0x86000f
#define DUP9_CCALENDARPLUGIN_CREATEENTRYL 0x860010
#define CCALENDARPLUGIN_ADDTOFIELDEXCERPTL 0x860011
#define DUP3_CCALENDARPLUGIN_CREATEENTRYL 0x860012
#define CCALENDARPLUGIN_ADDTOFIELDEXCERPTL_ENTRY 0x8a0001
#define CCALENDARPLUGIN_ADDTOFIELDEXCERPTL_EXIT 0x8a0002
#define CCALENDARPLUGIN_PAUSEPLUGINL_ENTRY 0x8a0003
#define CCALENDARPLUGIN_PAUSEPLUGINL_EXIT 0x8a0004
#define CCALENDARPLUGIN_RESUMEPLUGINL_ENTRY 0x8a0005
#define CCALENDARPLUGIN_RESUMEPLUGINL_EXIT 0x8a0006
#define CCALENDARPLUGIN_OVERWRITEORADDTOQUEUEL_ENTRY 0x8a0007
#define CCALENDARPLUGIN_OVERWRITEORADDTOQUEUEL_EXIT 0x8a0008
#define DUP1_CCALENDARPLUGIN_OVERWRITEORADDTOQUEUEL_EXIT 0x8a0009
#define CCALENDARPLUGIN_INDEXQUEUEDITEMS_ENTRY 0x8a000a
#define CCALENDARPLUGIN_INDEXQUEUEDITEMS_EXIT 0x8a000b


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

