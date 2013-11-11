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


inline TInt RFastFindFSPEngine::Enable()
	{
	return DoControl( EFastFindFSPOpEnable );
	}

inline TInt RFastFindFSPEngine::Disable()
	{
	return DoControl( EFastFindFSPOpDisable );
	}

inline void RFastFindFSPEngine::RegisterNotification(
		TFastFindFSPStatusPckg& aFastFindFSPStatus, TRequestStatus& aStat )
	{
	DoRequest( EFastFindFSPOpRegisterNotification, aStat, aFastFindFSPStatus );
	}

inline void RFastFindFSPEngine::AddNotificationPath( const TDesC& aPath )
	{
	TFastFindFSPStatusPckg pckg;
	TRequestStatus rs;
	TFastFindFSPStatus& status = pckg();

	status.iFileName.Zero();
	status.iFileName.Copy( aPath );

	DoRequest( EFastFindFSPOpAddNotificationPath, rs, pckg );
	User::WaitForRequest( rs );
	}

inline void RFastFindFSPEngine::RemoveNotificationPath( const TDesC& aPath )
	{
	TFastFindFSPStatusPckg pckg;
	TRequestStatus rs;
	TFastFindFSPStatus& status = pckg();

	status.iFileName.Zero();
	status.iFileName.Copy( aPath );

	DoRequest( EFastFindFSPOpRemoveNotificationPath, rs, pckg );
	User::WaitForRequest( rs );
	}

inline void RFastFindFSPEngine::AddIgnorePath (const TDesC& aPath )
	{
	TFastFindFSPStatusPckg pckg;
	TRequestStatus rs;
	TFastFindFSPStatus& status = pckg();

	status.iFileName.Zero();
	status.iFileName.Copy( aPath );

	DoRequest( EFastFindFSPOpAddIgnorePath, rs, pckg );
	User::WaitForRequest( rs );
	}

inline void RFastFindFSPEngine::RemoveIgnorePath( const TDesC& aPath )
	{
	TFastFindFSPStatusPckg pckg;
	TRequestStatus rs;
	TFastFindFSPStatus& status = pckg();

	status.iFileName.Zero();
	status.iFileName.Copy( aPath );

	DoRequest( EFastFindFSPOpRemoveIgnorePath, rs, pckg );
	User::WaitForRequest( rs );
	}

inline void RFastFindFSPEngine::NotificationCancel()
	{
	DoCancel( EFastFindFSPOpNotificationCancel );
	}

