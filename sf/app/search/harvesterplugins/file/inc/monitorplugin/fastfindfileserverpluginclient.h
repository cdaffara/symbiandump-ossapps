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
* Description:  Monitors file creations, modifications and deletions.
 *
*/


#ifndef FASTFINDFILESERVERPLUGINCLIENT_H
#define FASTFINDFILESERVERPLUGINCLIENT_H

enum TFastFindFSPOperation
	{
	EFastFindFSPOpEnable,
	EFastFindFSPOpDisable,
	EFastFindFSPOpRegisterNotification,
	EFastFindFSPOpAddNotificationPath,
	EFastFindFSPOpRemoveNotificationPath,
	EFastFindFSPOpAddIgnorePath,
	EFastFindFSPOpRemoveIgnorePath,
	EFastFindFSPOpNotificationCancel,
	};

class TFastFindFSPStatus
	{
	public:
		TInt iFileEventType;
		TInt iDriveNumber;
		TFileName iFileName;
		TFileName iNewFileName;
        TUid iProcessId;
	};

enum TFastFindFileEventType
	{
	EFastFindFileCreated,
	EFastFindFileRenamed,
	EFastFindFileModified,
	EFastFindFileReplaced,
	EFastFindFileDeleted,
	EFastFindDriveFormatted,
	EFastFindFileUnknown,
	EFastFindDirCreated,
	EFastFindDirDeleted,
	};

typedef TPckgBuf<TFastFindFSPStatus> TFastFindFSPStatusPckg;

const TInt KFastFindFSPluginPosition = 0x2001F6FA; // 0x2000FB14;

//#ifndef __FASTFINDFILESERVERPLUGIN_H__
//#define __FASTFINDFILESERVERPLUGIN_H__

#include <f32file.h>
#include <f32file_private.h>

/**
 * 
 */
class RFastFindFSPEngine : public RPlugin
	{
	public:

		/**
		 * Register...
		 * @param aFastFindFSPStatus ...
		 * @param aStat  TRequestStatus for asyncronous operation.
		 */
		inline void RegisterNotification( TFastFindFSPStatusPckg& aFastFindFSPStatus,
				TRequestStatus& aStat);

		/**
		 * Adds a new path to monitor.
		 * @param aPath  Path to add.
		 */
		inline void AddNotificationPath( const TDesC& aPath );

		/**
		 * Removes a path from list of monitored paths.
		 * @param aPath  Path to remove.
		 */
		inline void RemoveNotificationPath( const TDesC& aPath );

		/**
		 * Adds a new ignored path.
		 * @param aPath Path descriptor.
		 */
		inline void AddIgnorePath( const TDesC& aPath );

		/**
		 * Removes a ignored path from the list.
		 * @param aPath Path descriptor.
		 */
		inline void RemoveIgnorePath( const TDesC& aPath );

		/**
		 * Enable plugin.
		 * @return Error code.
		 */
		inline TInt Enable();

		/**
		 * Disable plugin.
		 * @return Error code.
		 */
		inline TInt Disable();

		/**
		 * Cancel notifications.
		 */
		inline void NotificationCancel();
	};

#include "fastfindfileserverpluginclient.inl"  // inlined methods

#endif // FASTFINDFILESERVERPLUGINCLIENT_H

