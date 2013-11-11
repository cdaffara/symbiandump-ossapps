/*
* Copyright (c) 2003-2005 Nokia Corporation and/or its subsidiary(-ies).
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
*     global definitions for mmscodec
*
*/



#ifndef MMSCODEC_H
#define MMSCODEC_H

#include <mmsconst.h>

//
// The following is a gapless table where the corresponding
// assigned numbers must run from 0 to KMmsUnAssigned - 1.
// If new content types are added to the table, and the assigned
// numbers are not contiguous dummy entries must be inserted, but
// if these would become too numerous, a two-table system must be
// constructed.
// At the moment one table is enough, as all well-known content types
// have contiguous assigned numbers.
//
const TUint8 KMmsUnAssigned = 0x37;
const TInt KNumberContentTypes = KMmsUnAssigned;

static const TText8* const KContentTypeTable[] = 
    {
    _S8( "*/*" ),                                       //0x00
    _S8( "text/*" ),                                    //0x01
    _S8( "text/html" ),                                 //0x02
    _S8( "text/plain" ),                                //0x03
    _S8( "text/x-hdml" ),                               //0x04
    _S8( "text/x-ttml" ),                               //0x05
    _S8( "text/x-vCalendar" ),                          //0x06
    _S8( "text/x-vCard" ),                              //0x07
    _S8( "text/vnd.wap.wml" ),                          //0x08
    _S8( "text/vnd.wap.wmlscript" ),                    //0x09
    _S8( "text/vnd.wap.channel" ),                      //0x0A
    _S8( "Multipart/*" ),                               //0x0B
    _S8( "Multipart/mixed" ),                           //0x0C
    _S8( "Multipart/form-data" ),                       //0x0D
    _S8( "Multipart/byteranges" ),                      //0x0E
    _S8( "multipart/alternative" ),                     //0x0F
    _S8( "application/*" ),                             //0x10
    _S8( "application/java-vm" ),                       //0x11    
    _S8( "application/x-www-form-urlencoded" ),         //0x12
    _S8( "application/x-hdmlc" ),                       //0x13
    _S8( "application/vnd.wap.wmlc" ),                  //0x14
    _S8( "application/vnd.wap.vmlscriptc" ),            //0x15
    _S8( "application/vnd.wap.channelc" ),              //0x16
    _S8( "application/vnd.wap.uaprof" ),                //0x17
    _S8( "application/vnd.wap.wtls-ca-certificate" ),   //0x18
    _S8( "application/vnd.wap.wtls-user-certificate" ), //0x19
    _S8( "application/x-x509-ca-cert" ),                //0x1A
    _S8( "application/x-x509-user-cert" ),              //0x1B
    _S8( "image/*" ),                                   //0x1C
    _S8( "image/gif" ),                                 //0x1D
    _S8( "image/jpeg"),                                 //0x1E
    _S8( "image/tiff" ),                                //0x1F
    _S8( "image/png" ),                                 //0x20
    _S8( "image/vnd.wap.wbmp" ),                        //0x21
    _S8( "application/vnd.wap.multipart.*" ),           //0x22
    _S8( "application/vnd.wap.multipart.mixed" ),       //0x23
    _S8( "application/vnd.wap.multipart.form-data" ),   //0x24
    _S8( "application/vnd.wap.multipart.byteranges" ),  //0x25
    _S8( "application/vnd.wap.multipart.alternative" ), //0x26
    _S8( "application/xml" ),                           //0x27
    _S8( "text/xml" ),                                  //0x28
    _S8( "application/vnd.wap.wbxml" ),                 //0x29
    _S8( "application/x-x968-cross-cert" ),             //0x2A
    _S8( "application/x-x968-ca-cert" ),                //0x2B
    _S8( "application/x-x968-user-cert" ),              //0x2C
    _S8( "text/vnd.wap.si" ),                           //0x2D
    _S8( "application/vnd.wap.sic" ),                   //0x2E
    _S8( "text/vnd.wap.sl" ),                           //0x2F
    _S8( "text/vnd.wap.slc" ),                          //0x30
    _S8( "text/vnd.wap.co" ),                           //0x31
    _S8( "application/vnd.wap.coc" ),                   //0x32
    _S8( "application/vnd.wap.multipart.related" ),     //0x33
    _S8( "application/vnd.wap.sia" ),                   //0x34
    _S8( "text/vnd.wap.connectivity-xml" ),             //0x35
    _S8( "application/vnd.wap.connectivity-wbxml" ),    //0x36
    // This must always be the last one.
    // This is the type that all unassigned numbers get:
    // Even if we have an actual assigned number for
    // application/octet-stream some day, there must be
    // this duplicate entry at the end of the table to
    // handle mapping of unknown assigned numbers
    _S8( "application/octet-stream" )                   //KMmsUnAssigned
    };

#endif
