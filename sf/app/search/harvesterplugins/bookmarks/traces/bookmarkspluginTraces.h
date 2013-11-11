// Created by TraceCompiler 2.1.2
// DO NOT EDIT, CHANGES WILL BE LOST

#ifndef __BOOKMARKSPLUGINTRACES_H__
#define __BOOKMARKSPLUGINTRACES_H__

#define KOstTraceComponentID 0x2001a9d3

#define CBOOKMARKSPLUGIN_STARTHARVESTINGL 0x860001
#define DUP1_CBOOKMARKSPLUGIN_STARTHARVESTINGL 0x860002
#define CBOOKMARKSPLUGIN_DELAYEDCALLBACKL 0x860003
#define DUP1_CBOOKMARKSPLUGIN_DELAYEDCALLBACKL 0x860004
#define DUP2_CBOOKMARKSPLUGIN_DELAYEDCALLBACKL 0x860005
#define DUP3_CBOOKMARKSPLUGIN_DELAYEDCALLBACKL 0x860006
#define CBOOKMARKSPLUGIN_GETDOMAINNAMEL 0x860007
#define DUP1_CBOOKMARKSPLUGIN_GETDOMAINNAMEL 0x860008
#define CBOOKMARKSPLUGIN_DOINDEXINGL 0x860009
#define DUP1_CBOOKMARKSPLUGIN_DOINDEXINGL 0x86000a
#define DUP2_CBOOKMARKSPLUGIN_DOINDEXINGL 0x86000b
#define DUP3_CBOOKMARKSPLUGIN_DOINDEXINGL 0x86000c
#define DUP4_CBOOKMARKSPLUGIN_DOINDEXINGL 0x86000d
#define DUP5_CBOOKMARKSPLUGIN_DOINDEXINGL 0x86000e
#define CBOOKMARKSPLUGIN_CREATEBOOKMARKSINDEXITEML 0x86000f
#define DUP1_CBOOKMARKSPLUGIN_CREATEBOOKMARKSINDEXITEML 0x860010
#define DUP2_CBOOKMARKSPLUGIN_CREATEBOOKMARKSINDEXITEML 0x860011
#define CBOOKMARKSPLUGIN_HANDLEFAVOURITESDBEVENTL 0x860012
#define DUP1_CBOOKMARKSPLUGIN_HANDLEFAVOURITESDBEVENTL 0x860013
#define CBOOKMARKSPLUGIN_DOINDEXINGL_ENTRY 0x8a0001
#define CBOOKMARKSPLUGIN_DOINDEXINGL_EXIT 0x8a0002


inline TBool OstTraceGen2( TUint32 aTraceID, TInt aParam1, TInt aParam2 )
    {
    TBool retval = BTraceFiltered8( EXTRACT_GROUP_ID(aTraceID), EOstTraceActivationQuery, KOstTraceComponentID, aTraceID );
    if ( retval )
        {
        TUint8 data[ 8 ];
        TUint8* ptr = data;
        *( ( TInt* )ptr ) = aParam1;
        ptr += sizeof ( TInt );
        *( ( TInt* )ptr ) = aParam2;
        ptr += sizeof ( TInt );
        ptr -= 8;
        retval = OstSendNBytes( EXTRACT_GROUP_ID(aTraceID), EOstTrace, KOstTraceComponentID, aTraceID, ptr, 8 );
        }
    return retval;
    }

inline TBool OstTraceGen2( TUint32 aTraceID, TInt32 aParam1, TInt32 aParam2 )
    {
    TBool retval = BTraceFiltered8( EXTRACT_GROUP_ID(aTraceID), EOstTraceActivationQuery, KOstTraceComponentID, aTraceID );
    if ( retval )
        {
        TUint8 data[ 8 ];
        TUint8* ptr = data;
        *( ( TInt* )ptr ) = aParam1;
        ptr += sizeof ( TInt );
        *( ( TInt* )ptr ) = aParam2;
        ptr += sizeof ( TInt );
        ptr -= 8;
        retval = OstSendNBytes( EXTRACT_GROUP_ID(aTraceID), EOstTrace, KOstTraceComponentID, aTraceID, ptr, 8 );
        }
    return retval;
    }


#ifndef __KERNEL_MODE__
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
#endif


#ifndef __KERNEL_MODE__
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
#endif


#endif

// End of file

