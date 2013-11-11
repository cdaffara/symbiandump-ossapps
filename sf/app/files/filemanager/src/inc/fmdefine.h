/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
#ifndef FMDEFINE_H
#define FMDEFINE_H

// define for filemanager error
#define  FmErrNone                       0  // No Error
#define  FmErrNotFound                  -1  // General error for things requested is not found 
#define  FmErrGeneral                   -2  // General error
#define  FmErrAlreadyExists             -3  // General for already existed. for example if unlock a drive but it has already been unlocked
#define  FmErrAccessDenied              -4  // General for Access is denied, for example, format a drive but access is denied.
#define  FmErrCannotRemove              -5  // General error for can not performance remove
#define  FmErrCannotCopy                -6  // General error for can not performance copy
#define  FmErrDiskFull                  -7  // Error for disk is full
#define  FmErrFileUsed                  -8  // Error for file is in use
#define  FmErrFolderUsed                -9  // Error for folder is in use
#define  FmErrTypeFormatFailed          -10 // Error reported when format failed
#define  FmErrTypeFormatFailedUsed      -11 // Error reported when user try to format a drive but it is in use 
#define  FmErrAlreadyStarted            -12 // General error for operation is already started
#define  FmErrCancel                    -13 // General error for operation canceled
#define  FmErrWrongParam                -14 // General error for wrong parameter
#define  FmErrUnKnown                   -15 // Unknown error
#define  FmErrIsNotFileOrFolder         -16 // Error for a path which is not a fild or folder.
#define  FmErrCannotMakeDir             -17 // Error for can not create directory.
#define  FmErrSrcPathDoNotExist         -18 // Error for source path is not exist.
#define  FmErrDestPathDoNotExist        -19 // Error for destination path is not exist.
#define  FmErrCopyDestToSubFolderInSrc  -20 // If user try to copy a folder to its sub folder, error will occur
#define  FmErrMoveDestToSubFolderInSrc  -21 // If user try to move a folder to its sub folder, error will occur
#define  FmErrNotSupported              -22 // feature not supported, for example, lock an drive which can not be locked
#define  FmErrNotReady                  -23 // Error when drive is not ready( means drive is not present )
#define  FmErrInUse                     -24 // Error for drive is in use.
#define  FmErrPermissionDenied          -25 // Permission Denied 
#define  FmErrBadName                   -26 // Bad Drive or file/folder name, currently used when rename drive
#define  FmErrPathNotFound              -27 // Error for path is not found
#define  FmErrRemoveDefaultFolder       -28 // Error when user try to remove default folder, such as C:/data/images...
#define  FmErrLocked                    -29 // Disk is locked
#define  FmErrCorrupt                   -30 // Disk is corrupt
#define  FmErrDisMounted                -31 // e.g. If disk dis mounted while backuping.
#define  FmErrDriveNotAvailable         -32 // e.g. if user want to access MMC but it is not available
#define  FmErrPathDenied                -33 // if user want to access path but it is denied, e.g c:/private
#define  FmErrDriveDenied               -34 // if user want to access drive but it is denied, e.g D:/
#define  FmErrPathNotExist              -35 // path not existed.

#define FmEstimateUpperLimit 90 // available mem/backup size*100%=90%, User selectable continuation
#define FmEstimateLowerLimit 10 // available mem/backup size*100%=10%, Backup will be interrupted

#define FmPlaceholderString " " // Placeholder for HbLabel because layout will be wrong when HbLabel is empty

#define Drive_C ( QString( "C:" ) + QDir::separator() ) 
#define Drive_D ( QString( "D:" ) + QDir::separator() )
#define Drive_Z ( QString( "Z:" ) + QDir::separator() )
#define Folder_C_Data ( QString( "C:" ) + QDir::separator() + QString( "Data" ) + QDir::separator() )

// define  contact data path for view details.
#define FmViewDetail_Contacts ( "\\private\\10003a73\\SQLite__Contacts.cdb" )
// define message data path for view details
#define FmViewDetail_Messages ( "\\private\\1000484b\\Mail2" )

#define FmMaxLengthofDriveName           11
#define FmMaxLengthofDrivePassword       8

// used to match un-zeroLength string, empty character such as space is acceptable
#define Regex_ValidUnZeroLength QString( "^.+$" )

// used to match un-empty string, and is not totally empty characters.
#define Regex_ValidUnEmpty QString( "^.*[^\\s].*$" )

// this is regexp for valid file/folder name: no \/:*?"<>| and is not totally empty characters.
// file name can not end with "." , but it is not include in this RegExp. It should be checked in Regex_ValidNotEndWithDot
// this expression is composed by two expressions:
// ^.*[^\\s].*$  used to match un-empty string and is not totally empty characters.
// [^\\\\/:*?\"<>|] used to math valid file/folder name
// merge the two regex together:
// valid file/folder name and is not totally empty.
#define Regex_ValidFileFolderName QString( "^[^\\\\/:*?\"<>|]*[^\\\\/:*?\"<>|\\s][^\\\\/:*?\"<>|]*$" )

// is not end with dot( trim blank characters in the end first )
#define Regex_ValidNotEndWithDot QString( "^.*[^\\.\\s][\\s]*$" )

// use slash(/) not backslash(\) or QDir::separator() because QRegExp can not recognize BackSlash(\)
#define RegexWidecard_SystemFolder      QString( "?:/system/*" )
#define RegexWidecard_SysFolder         QString( "?:/sys/*" )
#define RegexWidecard_PrivateFolder     QString( "?:/private/*" )
#define RegexWidecard_ResourceFolder    QString( "?:/resource/*" )

#define Char_Slash       QChar( '/' )
#define Char_BackSlash   QChar( '\\' )

#define Pre_Suffix_Dot   QChar('.')

// used for handle back event
enum FmEventResponse
{
    BackWasNotConsumed  = 0,
    BackWasConsumed     = 1
};

#endif 
