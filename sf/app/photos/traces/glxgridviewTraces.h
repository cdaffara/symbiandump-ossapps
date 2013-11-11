// Created by TraceCompiler 2.1.2
// DO NOT EDIT, CHANGES WILL BE LOST

#ifndef __GLXGRIDVIEWTRACES_H__
#define __GLXGRIDVIEWTRACES_H__

#define KOstTraceComponentID 0x200009ef

#define GLXGRIDVIEW_GLXGRIDVIEW_ENTRY 0x8a0052
#define GLXGRIDVIEW_GLXGRIDVIEW_EXIT 0x8a0053
#define GLXGRIDVIEW_ACTIVATE_ENTRY 0x8a0054
#define GLXGRIDVIEW_ACTIVATE_EXIT 0x8a0055
#define GLXGRIDVIEW_DEACTIVATE_ENTRY 0x8a0056
#define GLXGRIDVIEW_DEACTIVATE_EXIT 0x8a0057
#define GLXGRIDVIEW_SETMODEL_ENTRY 0x8a0058
#define GLXGRIDVIEW_SETMODEL_EXIT 0x8a0059
#define GLXGRIDVIEW_ADDTOOLBAR_ENTRY 0x8a005a
#define GLXGRIDVIEW_ADDTOOLBAR_EXIT 0x8a005b
#define GLXGRIDVIEW_GETANIMATIONITEM_ENTRY 0x8a005c
#define GLXGRIDVIEW_GETANIMATIONITEM_EXIT 0x8a005d
#define GLXGRIDVIEW_LOADGRIDVIEW_ENTRY 0x8a005e
#define GLXGRIDVIEW_LOADGRIDVIEW_EXIT 0x8a005f
#define DUP1_GLXGRIDVIEW_GLXGRIDVIEW_ENTRY 0x8a0060
#define DUP1_GLXGRIDVIEW_GLXGRIDVIEW_EXIT 0x8a0061
#define GLXGRIDVIEW_SETVISVALWINDOWINDEX 0x850002
#define DUP1_GLXGRIDVIEW_SETVISVALWINDOWINDEX 0x850003
#define DUP2_GLXGRIDVIEW_SETVISVALWINDOWINDEX 0x850004
#define DUP3_GLXGRIDVIEW_SETVISVALWINDOWINDEX 0x850005
#define GLXGRIDVIEW_ENABLEMARKING 0x86001f
#define GLXGRIDVIEW_DISABLEMARKING 0x860020
#define GLXGRIDVIEW_HANDLEUSERACTION 0x860021
#define DUP1_GLXGRIDVIEW_HANDLEUSERACTION 0x860022
#define DUP2_GLXGRIDVIEW_HANDLEUSERACTION 0x860023
#define GLXGRIDVIEW_GETSELECTIONMODEL 0x860024
#define GLXGRIDVIEW_ITEMDESTROYED 0x860025
#define GLXGRIDVIEW_ITEMCHANGE 0x860026
#define GLXGRIDVIEW_ADDVIEWCONNECTION 0x860027
#define GLXGRIDVIEW_REMOVEVIEWCONNECTION 0x860028
#define GLXGRIDVIEW_ITEMSELECTED 0x860029
#define GLXGRIDVIEW_INDICATELONGPRESS 0x86002a
#define EVENT_DUP1_GLXGRIDVIEW_ITEMSELECTED_START 0x8b0001
#define EVENT_DUP1_GLXGRIDVIEW_ITEMSELECTED_STOP 0x8b0002


inline TBool OstTraceGen2( TUint32 aTraceID, TInt aParam1, TUint aParam2 )
    {
    TBool retval = BTraceFiltered8( EXTRACT_GROUP_ID(aTraceID), EOstTraceActivationQuery, KOstTraceComponentID, aTraceID );
    if ( retval )
        {
        TUint8 data[ 8 ];
        TUint8* ptr = data;
        *( ( TInt* )ptr ) = aParam1;
        ptr += sizeof ( TInt );
        *( ( TUint* )ptr ) = aParam2;
        ptr += sizeof ( TUint );
        ptr -= 8;
        retval = OstSendNBytes( EXTRACT_GROUP_ID(aTraceID), EOstTrace, KOstTraceComponentID, aTraceID, ptr, 8 );
        }
    return retval;
    }

inline TBool OstTraceGen2( TUint32 aTraceID, TInt32 aParam1, TUint32 aParam2 )
    {
    TBool retval = BTraceFiltered8( EXTRACT_GROUP_ID(aTraceID), EOstTraceActivationQuery, KOstTraceComponentID, aTraceID );
    if ( retval )
        {
        TUint8 data[ 8 ];
        TUint8* ptr = data;
        *( ( TInt* )ptr ) = aParam1;
        ptr += sizeof ( TInt );
        *( ( TUint* )ptr ) = aParam2;
        ptr += sizeof ( TUint );
        ptr -= 8;
        retval = OstSendNBytes( EXTRACT_GROUP_ID(aTraceID), EOstTrace, KOstTraceComponentID, aTraceID, ptr, 8 );
        }
    return retval;
    }


#endif

// End of file

