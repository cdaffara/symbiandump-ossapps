/*
* Copyright (c) 2005-2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  ConML parser/generator
*
*/



#ifndef __SCONCONMLTAGS_H__
#define __SCONCONMLTAGS_H__

// -----------------------------------------------------------------------------
// Constants
// -----------------------------------------------------------------------------
const TInt32 KSConConMLPublicId = 0x8F;
const TUint8 KSConConMLVersion = 0x02;
const TUint32 KSConConMLUTF8 = 0x6a;

_LIT8(KSConConMLPublicIdStr, "-//CONML//DTD ConML 1.0//EN");

// -----------------------------------------------------------------------------
// TConMLTags
// -----------------------------------------------------------------------------
enum TConMLTags
    {
        EConMLAll = 5,
        EConMLApplication,
        EConMLApplications,
        EConMLBUROptions,
        EConMLCancel,
        EConMLComplete,
        EConML,
        EConMLData,
        EConMLDataOwners,
        EConMLDataOwnerStatus,
        EConMLDelayToPrepareData,
        EConMLDeviceInfo,
        EConMLDrive,
        EConMLDrives,
        EConMLExecute,
        EConMLFile,
        EConMLFiles,
        EConMLGetDataOwnerStatus,
        EConMLGetDataSize,
        EConMLGetStatus,
        EConMLHasFiles,
        EConMLID,
        EConMLIncType,
        EConMLInstall,
        EConMLInstParams,
        EConMLListDataOwners,
        EConMLListInstalledApps,
        EConMLListPublicFiles,
        EConMLMaxObjectSize,
        EConMLModified,
        EConMLMoreData,
        EConMLName,
        EConMLPackageInfo,
        EConMLParam,
        EConMLPartialType,
        EConMLProgress,
        EConMLReboot,
        EConMLRequestData,
        EConMLRequiresReboot,
        EConMLResults,
        EConMLSetBURMode,
        EConMLSetInstParams,
        EConMLSID,
        EConMLSize,
        EConMLStatus,
        EConMLSupplyData,
        EConMLSupportedMethods,
        EConMLSupportsInc,
        EConMLTask,
        EConMLTransferDataType,
        EConMLType,
        EConMLUID,
        EConMLUnInstall,
        EConMLUpdateDeviceInfo,
        EConMLUserPerm,
        EConMLValue,
        EConMLVersion,
        EConMLGetMetadata,
        EConMLFilename
        };
// -----------------------------------------------------------------------------
_LIT8( KConMLElements, 
       "<0>|<1>|<2>|<3>|<4>|All|Application|Applications|BUROptions|Cancel"
       "|Complete|ConML|Data|DataOwners|DataOwnerStatus|DelayToPrepareData"
       "|DeviceInfo|Drive|Drives|Execute|File|Files|GetDataOwnerStatus"
       "|GetDataSize|GetStatus|HasFiles|ID|IncType|Install|InstParams"
       "|ListDataOwners|ListInstalledApps|ListPublicFiles|MaxObjectSize|"
       "Modified|MoreData|Name|PackageInfo|Param|PartialType|Progress|"
       "Reboot|RequestData|RequiresReboot|Results|SetBURMode|SetInstParams|SID|"
       "Size|Status|SupplyData|SupportedMethods|SupportsInc|Task|"
       "TransferDataType|Type|UID|UnInstall|UpdateDeviceInfo|UserPerm|"
       "Value|Version|GetMetadata|Filename" );
       
_LIT8(KXMLTagStart, "<");
_LIT8(KXMLTagStartEndTag, "</");
_LIT8(KXMLTagEndNoContent, "/>");
_LIT8(KXMLTagEnd, ">");
_LIT8(KXMLElemenentSeparator, "|");
_LIT8(KXMLNamespaceBegin, " xmlns='");
_LIT8(KXMLNamespaceEnd, "'");
_LIT8(KXMLIndent, "   ");
_LIT8(KXMLNewLine, "\r\n");

#endif