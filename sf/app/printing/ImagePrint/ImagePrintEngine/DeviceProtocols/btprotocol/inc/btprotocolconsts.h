/*
* Copyright (c) 2004-2007 Nokia Corporation and/or its subsidiary(-ies).
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



#ifndef BTPROTOCOLCONSTS_H
#define BTPROTOCOLCONSTS_H

#include <btsdp.h>

////////////////////////////////////////////////////////////////////////////////////////
//
// Obex header HI values from http://www.it.lut.fi/~doc/bluetooth/OBEX12.pdf
//
////////////////////////////////////////////////////////////////////////////////////////
const TUint KBtObexHeaderHiCount			=	0xC0;	//Number of objects (used by Connect)
const TUint KBtObexHeaderHiName				=	0x01;	//name of the object (often a file name)
const TUint KBtObexHeaderHiType				=	0x42;	//type of object - e.g. text, html, binary, manufacturer specific
const TUint KBtObexHeaderHiLength			=	0xC3;	//the length of the object in bytes
const TUint KBtObexHeaderHiTimeIso			=	0x44;	//date/time stamp – ISO 8601 version - preferred
const TUint KBtObexHeaderHiTime4B			=	0xC4;	//date/time stamp – 4 byte version (for compatibility only)
const TUint KBtObexHeaderHiDescription		=	0x05;	//text description of the object
const TUint KBtObexHeaderHiTarget			=	0x46;	//name of service that operation is targeted to
const TUint KBtObexHeaderHiHTTP				=	0x47;	//an HTTP 1.x header
const TUint KBtObexHeaderHiBody				=	0x48;	//a chunk of the object body.
const TUint KBtObexHeaderHiEnd				=	0x49;	//of Body
const TUint KBtObexHeaderHiWho				=	0x4A;	//identifies the OBEX application, used to tell if talking to a peer
const TUint KBtObexHeaderHiConnectionId		=	0xCB;	//an identifier used for OBEX connection multiplexing
const TUint KBtObexHeaderHiAppParameters	=	0x4C;	//extended application request & response information
const TUint KBtObexHeaderHiAuthChallenge	=	0x4D;	//authentication digest-challenge
const TUint KBtObexHeaderHiAuthResponse		=	0x4E;	//authentication digest-response
const TUint KBtObexHeaderHiObjectClass		=	0x4F;	//OBEX Object class of object
// Reserved			=	0x10 to 0x2F	//this range includes all combinations of the upper 2 bits
// User Defined		=	0x30 to 0x3F	//this range includes all combinations of the upper 2 bits

// Obex tag values
const TUint KBtObexTagOffset	=	0x01;
const TUint KBtObexTagCount		=	0x02;
const TUint KBtObexTagJobId		=	0x03;
const TUint KBtObexTagSize		=	0x04;

// Obex tag lengths in bytes
const TUint KBtDataLength4		=	0x04;	//Length of I4 = 4 bytes integer
const TUint KBtTripletLength	=	6;	//Length of tag + length + data = 1 + 1 + 4


////////////////////////////////////////////////////////////////////////////////////////
//
// Mime types
//
////////////////////////////////////////////////////////////////////////////////////////
_LIT8(KBtMimeTypeSoap, 			"x-obex/bt-SOAP");
_LIT8(KBtMimeTypeXhtml, 		"application/vnd.pwg-xhtml-print+xml\0"); //"application/xhtml+xml");
_LIT8(KBtMimeTypeReferenced, 	"x-obex/referencedobject");
_LIT8(KBtMimeTypeJpeg, 			"image/jpeg");
_LIT8(KBtMimeAnyImg, 			"image/*" );

////////////////////////////////////////////////////////////////////////////////////////
//
// Obex constants
//
// According to http://www.bluetooth.org/assigned-numbers/baseband.htm
// these should be the correct numbers, however the Series 60
// implementation of Bluetooth uses the numbers as below
//
////////////////////////////////////////////////////////////////////////////////////////
//#define KBTMjrDevClassImaging			0x6
//#define KBTMinDevClassPrinter			0x80
//#define KBTMjrSvcClassRendering		0x20
//#define KBTMjrSvcClassObjectTransfer	0x80

/** If the Major Device Class includes Imaging, the discovered Bluetooth device MAY be a BPP printer. */
#define KBTMjrDevClassImaging			0x600
/** If the Minor Device Class includes Printing, the discovered Bluetooth device is LIKELY a BPP printer. */
#define KBTMinDevClassPrinter			0x20
/** If the Major Service Class does not include Rendering, the discovered Bluetooth device is NOT a BPP printer. */
#define KBTMjrSvcClassRendering			0x200000
/** If the Major Service Class includes ObjectTransfer, the discovered Bluetooth device is NOT a BPP printer. */
#define KBTMjrSvcClassObjectTransfer	0x800000

/** Nokia 6600: If the Major Device Class includes Imaging, the discovered Bluetooth device MAY be a BPP printer. */
#define KBTMjrDevClassImaging_V2			0x600
/** Nokia 6600: If the Minor Device Class includes Printing, the discovered Bluetooth device is LIKELY a BPP printer. */
#define KBTMinDevClassPrinter_V2			0x80
/** Nokia 6600: If the Major Service Class does not include Rendering, the discovered Bluetooth device is NOT a BPP printer. */
#define KBTMjrSvcClassRendering_V2			0x40000
/** Nokia 6600: If the Major Service Class does not includes ObjectTransfer, the discovered Bluetooth device is NOT a BPP printer. */
#define KBTMjrSvcClassObjectTransfer_V2		0x100000

/** If the Major Device Class includes Computer, the discovered Bluetooth device MAY support Object Transfer. */
#define KBTMjrDevClassComputer 0x100
/** The device is a Desktop Computer. */
#define KBTMinDevClassDesktop  0x4
/** The device is a Server Computer. */
#define KBTMinDevClassServer   0x8
/** The device is a Laptop Computer. */
#define KBTMinDevClassLaptop   0xC


// According to https://www.bluetooth.org/foundry/assignnumb/document/service_discovery
/** UID for OBEX Object Push service in a Bluetooth device SDP query. Required for OPP. */
#define KBTSDPOBEXObjectPush    0x1105
/** UID for the BPP Direct Printing service in a Bluetooth device SDP query. Required for BPP. */
#define KBTSDPDirectPrinting	0x1118
/** UID for the BPP Reference Printing service in a Bluetooth device SDP query. Required for BPP. */
#define KBTSDPReferencePrinting	0x1119
/** UID for the BPP Direct Printing Reference Objects service in a Bluetooth device SDP advertise. Required for BPP. */
#define KBTSDPDPROService 		0x1120
/** UID for the BPP Printing Status service in a Bluetooth device SDP advertise. Required for BPP. */
#define KBTSDPPrintingStatus 	0x1123
/** UID for L2CAP in a Bluetooth device SDP query. Required for BPP. */
#define KBTSDPL2CAP				0x0100
/** UID for RFCOMM in a Bluetooth device SDP query. Required for BPP. */
#define KBTSDPRFCOMM			0x0003
/** UID for Object Exchange in a Bluetooth device SDP query. Required for BPP. */
#define KBTSDPObex				0x0008

// Additions to universal attributes in btsdp.h
static const TSdpAttributeID KSdpAttrIdAdditionalProtocolDescriptorLists 	= 0x000D;
static const TSdpAttributeID KSdpAttrIdDocFormatsSupported 					= 0x0350;
static const TSdpAttributeID KSdpAttrIdModelID 								= 0x0358;
static const TSdpAttributeID KSdpAttrIdMaxWidth								= 0x0362;
static const TSdpAttributeID KSdpAttrIdMaxLen 								= 0x0364;
static const TSdpAttributeID KSdpAttrIdAllDone 								= 0xFFFFFF;
// own constants for UID bitmask
static const TUint KBtMaskRFCOMM 			= 0;
static const TUint KBtMaskL2CAP 			= 1;
static const TUint KBtMaskObex		 		= 2;
static const TUint KBtMaskObjectPush	 	= 4;
static const TUint KBtMaskDirectPrinting 	= 8;
static const TUint KBtMaskPrintingStatus 	= 16;
static const TUint KBtMaskROService			= 32;

static const TUint KBtMaskObexLevel		= KBtMaskRFCOMM | KBtMaskL2CAP | KBtMaskObex;


////////////////////////////////////////////////////////////////////////////////////////
//
// Obex server constants
//
////////////////////////////////////////////////////////////////////////////////////////
const TUint KRfcommChannel = 1;
const TUint KServiceClass = 0x01007779; // Own UID

_LIT( KServiceName,"DirectPrintingReferenceObjectsService" );
_LIT( KServiceDescription,"Direct Printing Reference Objects Service" );

// Used by ObexFileServerImpl.h
const TInt KSimultainousSocketsOpen = 8;

////////////////////////////////////////////////////////////////////////////////////////
//
// Misc
//
////////////////////////////////////////////////////////////////////////////////////////
const TInt KPendingLimit = 6; // 6 x 5 sec = 30 sec total to wait answer to request from printer
const TInt KOppPendingLimit = 24; // 24 X 5 sec = 120 sec total to wait opp printing. 
// to get local BT name from central repository
const TUid KCRUidBluetoothLocalNameSettings = {0x10204DAC};
const TUint32 KBTLocalName = 0x00000001;

/** XHTML file name*/
_LIT(KXHTMLFileName, 	"c:\\system\\data\\xhtml\\Bt_Print_%d.xhtml");
_LIT8(KXHTMLFileName8, 	"c:\\system\\data\\xhtml\\Bt_Print_%d.xhtml");

/* Time out value for OBEX commands = 20 second */
const TInt KObexCmdTimeOut = 20000000;

#endif // BTPROTOCOLCONSTS_H

//  End of File

