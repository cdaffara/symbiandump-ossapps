// Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
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
//
#include "ccsbackuphandler.h"
#include <e32base.h>
#include <e32property.h>
#include <connect/sbdefs.h>
#include "ccsserver.h"
#include "ccsdebug.h"

_LIT(KDbFileNamePath,"c:\\private\\10281e17\\[2002A542]conversations.db");


// ========================== Member Functions ===============================

// ----------------------------------------------------------------------------
// CCsBackUpHandler::NewL 
// ----------------------------------------------------------------------------
CCsBackUpHandler* CCsBackUpHandler::NewL(CCsServer& aServer)
{
    PRINT ( _L("Enter CCsBackUpHandler::NewL") );
    CCsBackUpHandler* self = new (ELeave) CCsBackUpHandler(aServer);
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);
    PRINT ( _L("Exit CCsBackUpHandler::NewL") );
    return (self);
    
}

// ----------------------------------------------------------------------------
// CCsBackUpHandler::CCsBackUpHandler 
// ----------------------------------------------------------------------------
CCsBackUpHandler::CCsBackUpHandler(CCsServer& aServer): iServer(aServer)
{
}

// ----------------------------------------------------------------------------
// CCsBackUpHandler::~CCsBackUpHandler 
// destructor
// ----------------------------------------------------------------------------
CCsBackUpHandler::~CCsBackUpHandler()
{
    // Unregister ourselves with backup server
    if (iBackup) 
    {
        iBackup->DeregisterFile(iFileName);
        delete iBackup;
    }
}

// ----------------------------------------------------------------------------
// CCsBackUpHandler::ConstructL 
// The 2-phase constructor
// ----------------------------------------------------------------------------
void CCsBackUpHandler::ConstructL()
{
    PRINT ( _L("Enter CCsBackUpHandler::ConstructL") );
    iFileName = KDbFileNamePath;
    // Create backup session
    iBackup = CBaBackupSessionWrapper::NewL();
    // Register with the backup server
    iBackup->RegisterFileL(iFileName, *this);
    PRINT ( _L("Exit  CCsBackUpHandler::ConstructL 2") );
    PRINT1 ( _L("KDbFileNamePath: %S"), &iFileName);
}

// ----------------------------------------------------------------------------
// CCsBackUpHandler::ChangeFileLockL 
// Call back from the backup engine observer when a backup/restore is taking place
// ----------------------------------------------------------------------------
void CCsBackUpHandler::ChangeFileLockL(const TDesC& aFileAffected, TFileLockFlags aFlags)
{
    PRINT1 ( _L("ChangeFileLockL: %d"), aFlags);
    PRINT1 ( _L("ChangeFileLockL affected file : %S"), &aFileAffected);
    switch (aFlags) {
    case ETakeLock:
    {
        OpenConversationsDbL();
        break;
    }
    case EReleaseLockNoAccess:    
    case EReleaseLockReadOnly:
    {
        CloseConversationsDbL();
        break;
    }
    default:
    {
        __ASSERT_DEBUG(EFalse,User::Invariant());
        break;
    }
    }
}

// ----------------------------------------------------------------------------
// CCsBackUpHandler::CloseConversationsDbL 
// Unloads the plugins, so that the DB handle is closed
// ----------------------------------------------------------------------------
void CCsBackUpHandler::CloseConversationsDbL()
{
    iServer.DeletePlugins();
}

// ----------------------------------------------------------------------------
// CCsBackUpHandler::OpenConversationsDbL 
// Loads the plugins, so that the DB handle is again opened
// ----------------------------------------------------------------------------
void CCsBackUpHandler::OpenConversationsDbL()
{
    iServer.LoadPlugins();
}
