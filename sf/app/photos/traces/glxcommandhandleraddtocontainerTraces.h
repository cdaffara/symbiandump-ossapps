// Created by TraceCompiler 2.1.2
// DO NOT EDIT, CHANGES WILL BE LOST

#ifndef __GLXCOMMANDHANDLERADDTOCONTAINERTRACES_H__
#define __GLXCOMMANDHANDLERADDTOCONTAINERTRACES_H__

#define KOstTraceComponentID 0x20000a0d

#define DUP1_GLXQUERYCONTENTWIDGET_GLXQUERYCONTENTWIDGET_ENTRY 0x8a0007
#define DUP1_GLXQUERYCONTENTWIDGET_GLXQUERYCONTENTWIDGET_EXIT 0x8a0008
#define GLXQUERYCONTENTWIDGET_GLXQUERYCONTENTWIDGET_ENTRY 0x8a0009
#define GLXQUERYCONTENTWIDGET_GLXQUERYCONTENTWIDGET_EXIT 0x8a000a
#define GLXCOMMANDHANDLERADDTOCONTAINER_GLXCOMMANDHANDLERADDTOCONTAINER_ENTRY 0x8a000b
#define GLXCOMMANDHANDLERADDTOCONTAINER_GLXCOMMANDHANDLERADDTOCONTAINER_EXIT 0x8a000c
#define DUP1_GLXCOMMANDHANDLERADDTOCONTAINER_GLXCOMMANDHANDLERADDTOCONTAINER_ENTRY 0x8a000d
#define DUP1_GLXCOMMANDHANDLERADDTOCONTAINER_GLXCOMMANDHANDLERADDTOCONTAINER_EXIT 0x8a000e
#define GLXCOMMANDHANDLERADDTOCONTAINER_CREATECOMMANDL_ENTRY 0x8a000f
#define GLXCOMMANDHANDLERADDTOCONTAINER_CREATECOMMANDL_EXIT 0x8a0010
#define GLXCOMMANDHANDLERADDTOCONTAINER_GETSELECTIONLIST_ENTRY 0x8a0011
#define GLXCOMMANDHANDLERADDTOCONTAINER_GETSELECTIONLIST_EXIT 0x8a0012
#define GLXCOMMANDHANDLERADDTOCONTAINER_CREATENEWMEDIA_ENTRY 0x8a0013
#define GLXCOMMANDHANDLERADDTOCONTAINER_CREATENEWMEDIA_EXIT 0x8a0014
#define GLXCOMMANDHANDLERADDTOCONTAINER_CREATECOMMANDL 0x860001
#define GLXCOMMANDHANDLERADDTOCONTAINER_GETSELECTIONLIST 0x860002


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


#endif

// End of file

