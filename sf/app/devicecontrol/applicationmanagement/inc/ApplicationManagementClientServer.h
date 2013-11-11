/*
* Copyright (c) 2000 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description: Implementation of applicationmanagement components
*
*/
 

#ifndef __APPLICATIONMANAGEMENTCLIENTSERVER_H__
#define __APPLICATIONMANAGEMENTCLIENTSERVER_H__

#include <e32std.h>

_LIT(KApplicationManagementServerName,"applicationmanagementserver");
_LIT(KApplicationManagementServerImg,"\\applicationmanagementserver.exe");		// DLL/EXE name

const TUid KApplicationManagementServerUid={0x200267FB};

namespace NApplicationManagement
	{
	
//const TInt KMaxReturnMessageLength = 100;
//const TInt KElementIdMaxLength = 100;

const TUint KMessageDelimiterChar = '|' ;


const TInt KAMVerMajor = 1;
const TInt KAMVerMinor = 0;
const TInt KAMVerBuild = 0;


enum TApplicationManagementMessages
	{
	EAddTrust,
	EDeplCompCountOperation,
	EDeplCompIdsOperation,
//	EDeplCompDeliverStartOperation,
	EDeplCompDeliverCompleteOperation,
	EDeplCompGetOperation,
	EDeplCompGetDataOperation,
	EDeplCompGetDataLengthOperation,
	EDeplCompAttrUpdateOperation, // updates "simple" common attribute
	EDeplCompUpdateDataOperation, // updates "larger" attrbute, such as data
	EDeplCompUpdateStreamedDataOperation,// 10
	EInstallOperation, 
	EUpdateOperation,
	EDeplCompRemoveOperation,
	EDeplCompDownloadOperation,
	EDeplCompActivateOperation,
	EDeplCompDeactivateOperation,
	EDeplCompDeliverStartOperation2,
	EDeplCompUpdateOperation, // updates whole component
	EDeplCompGetUserIdOperation, // returns the user id
	EDeplCompStartDownloadOperation,  // 20
	EGetStateChangingIdsCountOperation,
	EGetStateChangingIdsOperation, // gets list of ids that are potentially changing state
	ECompleteStateChangeOperation, // completes change of state
	EDeplCompGetTemporaryInstFile,
	EDeplCompGetTargetURI,
	EDeplCompGetFinalResultForGA,
	EDeplCompGenericAlertSent,
	ECheckStatusNodes,
    EPerformRfs,
	ELast
	
	};

	
	}
#endif// __ApplicationManagementCLIENTSERVER_H__
	
//  End of File
