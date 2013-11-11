// Copyright (c) 1999-2009 Nokia Corporation and/or its subsidiary(-ies).
// All rights reserved.
// This component and the accompanying materials are made available
// under the terms of "Eclipse Public License v1.0"
// which accompanies this distribution, and is available
// at the URL "http://www.eclipse.org/legal/epl-v10.html".
//
// Initial Contributors:
// Nokia Corporation - initial contribution.
//
// Contributors:
//
// Description:
// Enumerations used to communicate the point at which a client
// requires an imap connection operation to complete.
// 
//

#if !defined(__IMAPCONNECTCOMPLETIONSTATES_H__)
#define __IMAPCONNECTCOMPLETIONSTATES_H__

/**
@internalComponent
@released
*/
enum TImapConnectionCompletionState
	{
	EAfterConnect=1,
	EAfterFullSync,
	EAfterDisconnection
	};

#endif
