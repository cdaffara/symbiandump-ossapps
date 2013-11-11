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

#ifndef __APPLICATIONSPLUGINTRACES_H__
#define __APPLICATIONSPLUGINTRACES_H__

#define KOstTraceComponentID 0x2001a9d7

#define CAPPLICATIONSPLUGIN_NEWL_ENTRY 0x8a0001
#define CAPPLICATIONSPLUGIN_NEWL_EXIT 0x8a0002
#define CAPPLICATIONSPLUGIN_HANDLEAPPLISTEVENT_ENTRY 0x8a0003
#define CAPPLICATIONSPLUGIN_HANDLEAPPLISTEVENT_EXIT 0x8a0004
#define CAPPLICATIONSPLUGIN_PAUSEPLUGINL_ENTRY 0x8a0005
#define CAPPLICATIONSPLUGIN_PAUSEPLUGINL_EXIT 0x8a0006
#define CAPPLICATIONSPLUGIN_RESUMEPLUGINL_ENTRY 0x8a0007
#define CAPPLICATIONSPLUGIN_RESUMEPLUGINL_EXIT 0x8a0008
#define CAPPLICATIONSPLUGIN_CAPPLICATIONSPLUGIN_ENTRY 0x8a0009
#define CAPPLICATIONSPLUGIN_CAPPLICATIONSPLUGIN_EXIT 0x8a000a
#define CAPPLICATIONSPLUGIN_CONSTRUCTL_ENTRY 0x8a000b
#define CAPPLICATIONSPLUGIN_CONSTRUCTL_EXIT 0x8a000c
#define CAPPLICATIONSPLUGIN_STARTPLUGINL_ENTRY 0x8a000d
#define CAPPLICATIONSPLUGIN_STARTPLUGINL_EXIT 0x8a000e
#define CAPPLICATIONSPLUGIN_STARTHARVESTINGL_ENTRY 0x8a000f
#define CAPPLICATIONSPLUGIN_STARTHARVESTINGL_EXIT 0x8a0010
#define _ADDAPPLICATIONINFOL_ENTRY 0x8a0011
#define _ADDAPPLICATIONINFOL_EXIT 0x8a0012
#define DUP1__ADDAPPLICATIONINFOL 0x860004
#define CAPPLICATIONSPLUGIN_ISAPPHIDDENL 0x860005
#define DUP1_CAPPLICATIONSPLUGIN_ISAPPHIDDENL 0x860006
#define CAPPLICATIONSPLUGIN_CREATEAPPLICATIONSINDEXITEML 0x860007
#define DUP1_CAPPLICATIONSPLUGIN_CREATEAPPLICATIONSINDEXITEML 0x860008
#define CAPPLICATIONSPLUGIN_HANDLEAPPLISTEVENT 0x860009
#define DUP2_CAPPLICATIONSPLUGIN_CREATEAPPLICATIONSINDEXITEML 0x86000a
#define CAPPLICATIONSPLUGIN_DELAYEDCALLBACKL 0x86000b
#define _ADDAPPLICATIONINFOL 0x86000c


#ifndef __KERNEL_MODE__
#ifndef __OSTTRACEGEN2_TUINT32_CONST_TDESC16REF_CONST_TDESC16REF__
#define __OSTTRACEGEN2_TUINT32_CONST_TDESC16REF_CONST_TDESC16REF__

inline TBool OstTraceGen2( TUint32 aTraceID, const TDesC16& aParam1, const TDesC16& aParam2 )
    {
    TBool retval = BTraceFiltered8( EXTRACT_GROUP_ID(aTraceID), EOstTraceActivationQuery, KOstTraceComponentID, aTraceID );
    if ( retval )
        {
        TInt length = 0;
        // Check that parameter lenght is not too long
        TInt length1 = aParam1.Size();
        if ((length + length1 + sizeof ( TUint32 )) > KOstMaxDataLength)
            {
            length1 = KOstMaxDataLength - (length + sizeof ( TUint32 ));
            }
        TInt lengthAligned1 = ( length1 + 3 ) & ~3;
        if (lengthAligned1 > 0)
            {
            length = length + sizeof ( TUint32 ) + lengthAligned1;
            }
        // Check that parameter lenght is not too long
        TInt length2 = aParam2.Size();
        if ((length + length2 + sizeof ( TUint32 )) > KOstMaxDataLength)
            {
            length2 = KOstMaxDataLength - (length + sizeof ( TUint32 ));
            }
        TInt lengthAligned2 = ( length2 + 3 ) & ~3;
        if (lengthAligned2 > 0)
            {
            length = length + sizeof ( TUint32 ) + lengthAligned2;
            }
        TUint8 data[ KOstMaxDataLength ];
        TUint8* ptr = data;
        // Set length to zero and calculate it againg
        // when adding parameters
        length = 0;
        if (length1 > 0)
            {
            // Number of elements is written before data
            // In case of Unicode string, number of elements is half of length
            *( ( TUint32* )ptr ) = length1 / (aParam1.Size() / aParam1.Length());
            ptr += sizeof ( TUint32 );
            memcpy( ptr, aParam1.Ptr(), length1 );
            ptr += length1;
            // Fillers are written to get 32-bit alignment
            while ( length1++ < lengthAligned1 )
                {
                *ptr++ = 0;
                }
            length += sizeof ( TUint32 ) + lengthAligned1;
            }
        else if (length + sizeof ( TUint32 ) <= KOstMaxDataLength)
            {
            *( ( TUint32* )ptr ) = 0;
            ptr += sizeof ( TUint32 );
            length += sizeof ( TUint32 );
            }
        if (length2 > 0)
            {
            // Number of elements is written before data
            // In case of Unicode string, number of elements is half of length
            *( ( TUint32* )ptr ) = length2 / (aParam2.Size() / aParam2.Length());
            ptr += sizeof ( TUint32 );
            memcpy( ptr, aParam2.Ptr(), length2 );
            ptr += length2;
            // Fillers are written to get 32-bit alignment
            while ( length2++ < lengthAligned2 )
                {
                *ptr++ = 0;
                }
            length += sizeof ( TUint32 ) + lengthAligned2;
            }
        else if (length + sizeof ( TUint32 ) <= KOstMaxDataLength)
            {
            *( ( TUint32* )ptr ) = 0;
            ptr += sizeof ( TUint32 );
            length += sizeof ( TUint32 );
            }
        ptr -= length;
        retval = OstSendNBytes( EXTRACT_GROUP_ID(aTraceID), EOstTrace, KOstTraceComponentID, aTraceID, ptr, length );
        }
    return retval;
    }

#endif // __OSTTRACEGEN2_TUINT32_CONST_TDESC16REF_CONST_TDESC16REF__

#endif


#endif

// End of file

