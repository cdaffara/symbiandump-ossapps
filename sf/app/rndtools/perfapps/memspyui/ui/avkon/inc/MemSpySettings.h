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

#ifndef MEMSPYSETTINGS_H
#define MEMSPYSETTINGS_H

// System includes
#include <e32base.h>
#include <badesca.h>
#include <f32file.h>

// Engine includes
#include <memspy/engine/memspyengineoutputsink.h>
#include <memspy/engine/memspyenginehelpersysmemtrackerconfig.h>

// Classes referenced
class CMemSpyEngine;

//cigasto
class RMemSpySession;


class CMemSpySettings : public CBase
    {
public:
    //static CMemSpySettings* NewL( RFs& aFsSession, CMemSpyEngine& aEngine );
	static CMemSpySettings* NewL( RFs& aFsSession, RMemSpySession& aSession );
    ~CMemSpySettings();

private:
    //CMemSpySettings( RFs& aFsSession, CMemSpyEngine& aEngine );
    CMemSpySettings( RFs& aFsSession, RMemSpySession& aSession );
    void ConstructL();

public: // API
    inline RFs& FsSession() { return iFsSession; }
    void StoreSettingsL();
    
    void SetSinkType( TMemSpySinkType aType ) { iSinkType = aType; }

private: // Settings methods
    void RestoreSettingsL();
    void GetSettingsFileNameL( TDes& aFileName );
    void GetSettingsPathL( TDes& aPath );
    RFile SettingsFileLC( TBool aReplace = EFalse );
    
    //new methods according to new architecture
    void GetSwmtConfig( TMemSpyEngineHelperSysMemTrackerConfig& aConfig );
    void SetSwmtConfig( TMemSpyEngineHelperSysMemTrackerConfig& aConfig );  

private: // Data members
    RFs& iFsSession;    
    RMemSpySession& iMemSpySession;
    
private: //Settings
    TMemSpySinkType iSinkType;
    TMemSpyEngineHelperSysMemTrackerConfig iSwmtConfig;
    RArray<TUid> iUidList;
    };


#endif
