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
* Description:
*
*/

#include "launcherdllparser.h"
#include "launchertraces.h"

// ---------------------------------------------------------------------------

CLauncherDLLParser::~CLauncherDLLParser()
    {
    LOGSTRING("Launcher: CLauncherDLLParser::~CLauncherDLLParser");
    }

// ---------------------------------------------------------------------------

CLauncherDLLParser::CLauncherDLLParser()
    {
    LOGSTRING("Launcher: CLauncherDLLParser::CLauncherDLLParser");
    }

// ---------------------------------------------------------------------------

CLauncherDLLParser* CLauncherDLLParser::NewL()
    {
    LOGSTRING("Launcher: CLauncherDLLParser::NewL");
    CLauncherDLLParser* self = CLauncherDLLParser::NewLC();    
    CleanupStack::Pop();
    return self;
    }

// ---------------------------------------------------------------------------

CLauncherDLLParser* CLauncherDLLParser::NewLC()
    {
    LOGSTRING("Launcher: CLauncherDLLParser::NewLC");
    CLauncherDLLParser* self = new (ELeave) CLauncherDLLParser;
    CleanupStack::PushL(self);
    self->ConstructL();
    return self;
    }

// ---------------------------------------------------------------------------

void CLauncherDLLParser::ConstructL()
    {
    
    }

// ---------------------------------------------------------------------------

void CLauncherDLLParser::ParseL( RFs& aFileSession, RFile& aFile, CLauncherDLLElement& aElement )
    {
    TFileName dllName;
    TFileName fullName;
    TUid tmpUID;
    TInt offset = 0;
        
    aFile.Name(dllName);
    aFile.FullName(fullName);
    aElement.SetNameL(dllName);
    
    TBool isFileInRom = aFileSession.IsFileInRom(fullName) != 0;
    
    /**
     * Read UID1
     */
    offset = UID1_OFFSET;
    aFile.Seek(ESeekStart, offset); // Set file position   
    tmpUID.iUid = ReadUint32L(aFile);    
    aElement.SetUID1L(tmpUID);    
    
    /**
     * Read UID2
     */
    offset = UID2_OFFSET;
    aFile.Seek(ESeekStart, offset); // Set file position
    tmpUID.iUid = ReadUint32L(aFile);    
    aElement.SetUID2L(tmpUID);
        
    /**
     * Read UID3
     */
    offset = UID3_OFFSET;
    aFile.Seek(ESeekStart, offset); // Set file position
    tmpUID.iUid = ReadUint32L(aFile);    
    aElement.SetUID3L(tmpUID);
        
    /**
     * Read SID
     */
    if( isFileInRom )
        offset = SID_OFFSET_ROM;
    else
        offset = SID_OFFSET_ROFS;
    aFile.Seek(ESeekStart, offset); // Set file position
    tmpUID.iUid = ReadUint32L(aFile);    
    aElement.SetSIDL(tmpUID);
    
    /**
     * Read Capability
     */
    if( isFileInRom )
        offset = CAPABILITY_OFFSET_ROM;
    else
        offset = CAPABILITY_OFFSET_ROFS;
    aFile.Seek(ESeekStart, offset); // Set file position
    TUint32 tmpVal = ReadUint32L(aFile);
    aElement.SetCapabilityL(tmpVal);    
    }

// ---------------------------------------------------------------------------

TUint32 CLauncherDLLParser::ReadUint32L( RFile& aFile )
    {
    TBuf8<4> tempBuf;    
    User::LeaveIfError(aFile.Read(tempBuf, 4));
    const TUint8* ptr = tempBuf.Ptr();    
    return TUint32(ptr[0] | (ptr[1]<<8) | (ptr[2]<<16) | (ptr[3]<<24));    
    }
