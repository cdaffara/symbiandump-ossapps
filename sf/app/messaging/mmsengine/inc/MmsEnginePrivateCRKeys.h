/*
* Copyright (c) 2004-2009 Nokia Corporation and/or its subsidiary(-ies).
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
*     Definitions of private keys related to MmsEngine settings in CenRep
*
*/



#ifndef MMSENGINEPRIVATECRKEYS_H
#define MMSENGINEPRIVATECRKEYS_H

#include <MmsEngineDomainCRKeys.h>

const TUid KCRUidMmsEngineRegistryId = {0x101FB0D5};

/**
* KMmsServerSettingsVersion 
* Defines the settings version.
* Default value: 30
*/
const TUint32 KMmsEngineSettingsVersion =               0x00000001;

/**
* KMmsEngineDefaultService 
* Defines the entry id of the default service.
* Default value: 0
*/
const TUint32 KMmsEngineDefaultServiceId =              0x00000002;

/**
* KMmsEngineNotificationFolderId
* Defines the folder entry id of the notification folder.
* Default value: 0
*/
const TUint32 KMmsEngineNotificationFolderId =          0x0000001B;

/**
* KMmsEngineMMBoxFolderId
* Defines the folder entry id of the mmbox folder.
* Default value: 0
*/
const TUint32 KMmsEngineMMBoxFolderId =                 0x0000001C;

/**
* KMmsEngineApplicationFolderId
* Defines the folder entry id of the application folder.
* Default value: 0
*/
const TUint32 KMmsEngineApplicationFolderId =           0x00000020;


/**
* KMmsEngineAllowPersonal
* Defines whether fetching personal messages are allowed or not.
* Possible values:
* 0 ( no )
* 1 ( yes )
* Default value: 1
*/
const TUint32 KMmsEngineAllowPersonal =                 0x00000003;

/**
* KMmsEngineAllowAdvertisements 
* Defines whether fetching advertisements are allowed or not. 
* Possible values:
* 0 ( no )
* 1 ( yes )
* Default value: 1
*/
const TUint32 KMmsEngineAllowAdvertisement =            0x00000004;

/**
* KMmsEngineAllowInfomational 
* Defines whether fetching of informational messages are allowed or not.
* Possible values:
* 0 ( no )
* 1 ( yes )
* Default value: 1
*/
const TUint32 KMmsEngineAllowInformational =            0x00000005;

/**
* KMmsEngineAllowAnonymous 
* Defines whether fetching of anonymous messages are allowed or not.
* Possible values:
* 0 ( no )
* 1 ( yes )
* Default value: 1
*/
const TUint32 KMmsEngineAllowAnonymous =                0x00000006;

/**
* KMmsEngineDeliveryReportWanted 
* Defines whether a delivery report is requested for sent messages.
* Possible values:
* 128 ( yes )
* 129 ( no )
* Default value: 129
*/
const TUint32 KMmsEngineDeliveryReportWanted =          0x00000007;

/**
* KMmsEngineDeliveryReportSendingAllowed 
* Defines whether to allow network to send a delivery report to sender
* after receiving a message.
* Possible values:
* 128 ( yes )
* 129 ( no )
* Default value: 128
*/
const TUint32 KMmsEngineDeliveryReportSendingAllowed =  0x00000008;

/**
* KMmsEngineExpiryInterval 
* Defines the validity period in hours that is requested from the network
* when sending a message.
* Hours are valid in cen rep, seconds in code.
* Recommended values:
* 0 ( = maximum allowed by MMSC )
* 1 ( 1 hour )
* 6 ( 6 hours )
* 24 ( 24 hours )
* 72 ( 3 days = 72 hours )
* 168 ( 1 week = 168 hours )
* Default value: 0
*/
const TUint32 KMmsEngineExpiryInterval =                0x00000009;

/**
* KMmsEngineExpiryOvershoot 
* Defines a period of time in seconds that the expiry time (reported in
* the received notification) can be overshoot and message fetching still tried.
* Default value: 300
*/
const TUint32 KMmsEngineExpiryOvershoot =               0x0000000A;

/**
* KMmsEngineSenderVisibility 
* Defines whether network sends our number to the message recipient.
* This feature has to be supported also by the network.
* Possible values:
* 0   ( network desides )
* 128 ( hide sender identity )
* 129 ( show sender identity )
* Default value: 0
*/
const TUint32 KMmsEngineSenderVisibility =              0x0000000D;

/**
* KMmsEngineReadReplyReportWanted
* Defines whether read report is requested when sending a message.
* Possible values:
* 128 ( yes )
* 129 ( no )
* Default value: 129
*/
const TUint32 KMmsEngineReadReplyReportWanted =         0x0000000E;

/**
* KMmsEngineReadReplyReportSendingAllowed
* Defines whether to allow sending a read report related to a received message.
* Possible values:
* 0 ( no )
* 1 ( yes )
* Default value: 1
*/
const TUint32 KMmsEngineReadReplyReportSendingAllowed = 0x0000000F;

/**
* KMmsEngineLogEmailRecipients
* Defines whether email recipients should get an entry into log
* when sending a message (i.e. entry having a "Pending" status).
* This feature depends also upon KMmsEngineDeliveryReport setting.
* (email gateways often do not support delivery reporting)
* Possible values:
* 0 ( no )
* 1 ( yes )
* Default value: 1
*/
const TUint32 KMmsEngineLogEmailRecipients =            0x00000012;

/**
* KMmsEngineReceivingModeForeign
* Defines the receive mode when roaming.
* Possible values:
* 1 ( Automatic )
* 2 ( Manual )
* 3 ( Postpone )
* 4 ( Reject )
* Default value: 2
*/
const TUint32 KMmsEngineReceivingModeForeign =          0x00000014;

/**
* KMmsEngineReceivingModeHome 
* Defines the receive mode when in home network.
* Possible values:
* 1 ( Automatic )
* 2 ( Manual )
* 3 ( Postpone )
* 4 ( Reject )
* Default value: 1
*/
const TUint32 KMmsEngineReceivingModeHome =             0x00000015;

/**
* KMmsEnginePseudoMMBox 
* Defines the default value of the X-StoredInMMBox header value used
* if the header is not present in the MM Notification.
* (i.e. if received notification really specifies this,
* it will override this setting)
* Possible values:
* 0 ( No MMBox by default)
* 1 ( Message is in MMBox by default)
* Default value: 0
*/
const TUint32 KMmsEnginePseudoMMBox =                   0x00000016;

/**
* KMmsEngineShowSentTime
* Defines whether received mms messages show time when the message was sent or
* when the mms message was received to the device.
* Note this affects only received mms messages.  
* 
* Possible values:
* 0 ( no, received mms message shows time when the message arrives to device is shown. ) 
* 1 ( yes, received mms message shows time when the message was sent. )
* Default value: 0
*/
const TUint32 KMmsEngineShowSentTime = 0x00000021;

/**
* KMmsEngineMessagePriority 
* Defines message priority code
* Possible values:
* 128 ( Low )
* 129 ( Normal )
* 130 ( High )
* Default value: 129
*/
const TUint32 KMmsEngineMessagePriority =               0x00000017;

/**
* KMmsEngineMessageClass 
* Defines message class
* Possible values:
* 128 ( Personal )
* 129 ( Advertisement )
* 130 ( Informational )
* 131 ( Auto )
* Default value: 128
*/
const TUint32 KMmsEngineMessageClass =                  0x00000018;

/**
* KMmsEngineLocalMode 
* Defines whether MMS engine should run in local or normal mode
* Possible values:
* 0 ( EFalse )
* 1 ( ETrue )
* Default value: 0
*/
const TUint32 KMmsEngineLocalMode =                     0x0000001D;

/**
* KMmsEngineDecodeLog 
* Defines whether MMS engine should produce extra logging when decoding messages
* Possible values:
* 0 ( EFalse )
* 1 ( ETrue )
* Default value: 0

*/
const TUint32 KMmsEngineDecodeLog =                     0x0000001E;

/**
* KMmsEngineBinaryDump 
* Defines whether MMS engine should dump the binary messages into file when decoding
* Possible values:
* 0 ( EFalse )
* 1 ( ETrue )
* Default value: 0
*/
const TUint32 KMmsEngineBinaryDump =                    0x0000001F;

/**
* KMmsEncodingType
* Defines target encoding type to be used to send MMS message. 
* Currently text objects/attachments can be encoded in different encoding type specified,
* provided corresponding conversion plugins are available  
* Possible values:
* 0  (Default encoding mechanism)
* 36 (Korean specific-KSC5601 encoding)
* Default value: 0
*/
const TUint32 KMmsEncodingType =                        0x00000023;

///////////////////////////////////////////////////////////////////////////////
//
// Following constants are for schedule settings
//
///////////////////////////////////////////////////////////////////////////////

const TUint32 KMmsScheduleVersion =                     0x00010000;
const TUint32 KMmsSchedulePriority =                    0x00010001;
const TUint32 KMmsScheduleValidityPeriod =              0x00010002;
const TUint32 KMmsScheduleIntervalType =                0x00010003;
const TUint32 KMmsScheduleLatency =                     0x00010004;
const TUint32 KMmsScheduleSendLongInterval =            0x00010005;     	
const TUint32 KMmsScheduleSendShortInterval =           0x00010006;
const TUint32 KMmsScheduleSendVariableIntervalsCount =  0x00010007;
const TUint32 KMmsScheduleSendVariableIntervalArrayBase	= 0x00011000;
const TUint32 KMmsScheduleRecvLongInterval =            0x00010008;     	
const TUint32 KMmsScheduleRecvShortInterval =           0x00010009;
const TUint32 KMmsScheduleRecvVariableIntervalsCount =  0x0001000A;
const TUint32 KMmsScheduleRecvVariableIntervalArrayBase	= 0x00012000;
const TUint32 KMmsSchedulePendingConditionsTimeout      = 0x0001000B;

///////////////////////////////////////////////////////////////////////////////
//
// Following constants are for schedule error action settings
//
///////////////////////////////////////////////////////////////////////////////

const TUint32 KMmsScheduleErrorActionsVersion =         0x00030000;
const TUint32 KMmsScheduleSendErrorActionsCount	=       0x00030001;
const TUint32 KMmsScheduleRecvErrorActionsCount	=       0x00030002;
// Number of errors that cause an immediate failure (no retries)
const TUint32 KMmsImmediateFailureErrorCount =          0x00030003;

// Error action bases

// This error action is used in sending when error code is not found
// from the action list
const TUint32 KMmsScheduleSendErrorDefaultAction =      0x00031000;

// Start of error actions that require different handling for sending.
// These are complete structures defining number of retries and intervals.
// Immediate failure cases that are common for sending and receiving
// are in a separate list
const TUint32 KMmsScheduleSendErrorActionsBase =        0x00032000;

// This error action is used in receiving when error code is not found
// from the action list
const TUint32 KMmsScheduleRecvErrorDefaultAction =      0x00033000;

// Start of error actions that require different handling for receiving.
// These are complete structures defining number of retries and intervals.
// Immediate failure cases that are common for sending and receiving
// are in a separate list.
const TUint32 KMmsScheduleRecvErrorActionsBase =        0x00034000;

// List for error codes that cause an immediate failure
// This list is common for sending and receiving
// This is just a list of error codes, no action information is defined for these
// Each error only takes one line (one key), so no spacing for these needs to 
// be defined
const TUint32 KMmsImmediateFailureErrorBase =           0x00035000;

// Elements of the error action structure
// These are used for errors in lists starting from
// KMmsScheduleSendErrorActionsBase and KMmsScheduleRecvErrorActionsBase
const TUint32 KMmsErrorActionVersion =                  0x00000000; // obsolete
const TUint32 KMmsErrorActionError =                    0x00000001;
const TUint32 KMmsErrorActionMaxRetries	=               0x00000002;    	
const TUint32 KMmsErrorActionRetries =                  0x00000003;
const TUint32 KMmsErrorActionAction =                   0x00000004;
const TUint32 KMmsErrorActionRetrySpacing =             0x00000005;


#endif      // MMSENGINEPRIVATECRKEYS_H
