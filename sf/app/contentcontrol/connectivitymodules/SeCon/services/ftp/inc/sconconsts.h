/*
* Copyright (c) 2005-2010 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Constant definitions for File Transfer Controller
*
*/


#ifndef _SCONCONSTS_H_
#define _SCONCONSTS_H_

// Define this to show C-drive logs folder to user
#define SCON_SHOW_LOGS_FOLDER

_LIT( K_C_ROOT, "C:\\" );
_LIT( KSConLogsPath, "logs\\" );     // logs folder

// Following folders on MMC are exluded from folder listing
_LIT( KSConResource, "resource\\" ); // resource folder on MMC
_LIT( KSConSys, "sys\\" );           // sys folder on MMC
_LIT( KSConPrivate, "private\\" );   // private folder on MMC
_LIT( KSConSystem, "system\\" );     // system folder on MMC

// Suite needs access to SuiteConf.xml file
_LIT( KSConAllowedPath, "Z:\\Private\\101F7C87" );

// For folder listing
_LIT8( KSConXmlDocBegin,
       "<?xml version=\"1.0\"?>\n<!DOCTYPE folder-listing SYSTEM \"obex-folder-listing.dtd\"\n  [ <!ATTLIST folder mem-type CDATA #IMPLIED>\n  <!ATTLIST folder label CDATA #IMPLIED> ]>\n<folder-listing version=\"1.0\">\n" );
_LIT8( KSConXmlParentFolder, "   <parent-folder />\n" );
_LIT8( KSConXmlFileNameBegin,"   <file name=\"" );
_LIT8( KSConXmlFolderNameBegin,"   <folder name=\"" );
_LIT8( KSConXmlDriveNameBegin,"   <drive name=\"" );
_LIT8( KSConXmlType,"\" type=\"" );
_LIT8( KSConXmlModified,"\" modified=\"" );
_LIT8( KSConXmlSize,"\" size=\"" );
_LIT8( KSConXmlUserAttributes,"\" user-perm=\"" );
_LIT8( KSConXmlMemoryType,"\" mem-type=\"" );
_LIT8( KSConXmlMemoryLabel,"\" label=\"" );

_LIT8( KSConXmlTypeWord, "x-epoc/x-app268450404" );
_LIT8( KSConXmlTypeSheet, "x-epoc/x-app268450429" );

_LIT8( KSConXmlUserEntryArchive,"RWD" );
_LIT8( KSConXmlUserEntryReadOnly,"R" );
_LIT8( KSConXmlUserEntryDrive, "RW" );

_LIT8( KSConXmlFileEnd,"\"/>\n" );
_LIT8( KSConXmlFolderListEnd,"</folder-listing>" );
_LIT8( KSConDriveCName,"C:" );
_LIT8( KSConDriveZName,"Z:" );
_LIT8( KSConFolderListType, "x-obex/folder-listing" );

const TInt KSConMemTypeMaxLength = 5;
_LIT8( KSConMemoryTypeDev, "DEV" ); // prefix for device memory
_LIT8( KSConMemoryTypeMmc, "MMC" ); // prefix for MMC memory

// Date field format
_LIT( KSConXmlDate, "%F%Y%M%DT%H%T%SZ" );
const TInt KSConDateMaxLength = 16;

//File extensions
_LIT( KSConEpocWordExt, ".ewd" );
_LIT( KSConEpocSheetExt, ".sht" );

_LIT8( KFind1,"&" );
_LIT8( KReplace1, "&amp;" );
_LIT8( KReplace2, "&lt;" );
_LIT8( KReplace3, "&gt;" );
_LIT8( KReplace4, "&quot;" );
_LIT8( KReplace5, "&apos;" );

_LIT( KSConResourceName, "z:\\Resource\\sconftp.rsc" );

const TInt KSConBufSize = 1024;
const TInt KSConSetPathDontCreate = 0x01;   // Flags used to indicate SetPath commands
const TInt KSConSetPathRoot = 0x02;         // Flags used to indicate SetPath commands
const TInt KSConSetPathForward = 0x02;      // Flags used to indicate SetPath commands
const TInt KSConSetPathBack = 0x03;         // Flags used to indicate SetPath commands
const TInt KSConXmlBufSize = 30;
const TInt KSConDriveNameLength = 2;
const TInt KSConMaxMimeTypeLength = 25;
const TInt KSConBufferSize = 262144; // 256KB

const TUid KSConSheetUid = { 0x10003a7d };  // Sheet document uid
const TUid KSConWordUid = { 0x10003a64 };   // Word document uid

const TInt KDriveLength = 3;

#endif

