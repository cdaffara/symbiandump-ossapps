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
* Description:  Implementation of ECOM plug-in service interface providing
*                process and application control service.
*
*/



#ifndef HTIAPPSERVICEPLUGIN_H__
#define HTIAPPSERVICEPLUGIN_H__

//  INCLUDES
#include "../../../symbian_version.hrh"

#include <e32base.h>
#include <apgcli.h>
#include <w32std.h>
#if ( SYMBIAN_VERSION_SUPPORT < SYMBIAN_4 )
#include <SWInstDefs.h>
#include <swi/sisregistryentry.h>
#else
#include <usif/usifcommon.h>
#endif
#include <HtiServicePluginInterface.h>

// CONSTANTS

// MACROS

// DATA TYPES

// FUNCTION PROTOTYPES

// FORWARD DECLARATIONS
class CDesC8ArrayFlat;
class TApaTask;

// CLASS DECLARATION

NONSHARABLE_CLASS (CHtiAppControl) : public CHTIServicePluginInterface
    {
protected:
    //commands
    enum TAppCommand
        {
        //commands
        EProcessLastCommand = 0x0F, //!!! used to sort process control from
                                    //!!! app control

        //app control
        EStartApp               = 0x10,
        EStartApp_u             = 0x11,
        EStartApp_uid           = 0x12,
        EStartApp_uid_u         = 0x13,
        EStartDoc               = 0x14,
        EStartDoc_u             = 0x15,
        EStatusApp              = 0x16,
        EStatusApp_u            = 0x17,
        EStatusDoc              = 0x18,
        EStatusDoc_u            = 0x19,
        EStatusApp_uid          = 0x1A,
        EStopApp                = 0x1C,
        EStopApp_u              = 0x1D,
        EStopDoc                = 0x1E,
        EStopDoc_u              = 0x1F,
        EStopApp_uid            = 0x20,
        EListApps               = 0x24,
        EListApps_u             = 0x25,
        EListInstalledApps      = 0x26,
        EListInstalledApps_u    = 0x27,

        EAppLastCommand     = 0x2F, //!!! used to sort process control from
                                    //!!! app control
        EInstall            = 0x30,
        EInstall_u          = 0x31,
        EUnInstall          = 0x32,
        EUnInstallName_u    = 0x33,
        EUnInstallName      = 0x34,

        ESisLastCommand = 0x3F, //!!! used to sort sis control from
                                //!!! app control
        //responses
        EOk             = 0xF0,
        ENotFound       = 0xF1,
        EAlreadyRunning = 0xF2,
        EAlreadyStopped = 0xF3,
        ERunning        = 0xF4,
        EKilled         = 0xF5,
        EPanic          = 0xF6
        };

public:
    static CHtiAppControl* NewL();

    // Interface implementation
    void ProcessMessageL(const TDesC8& aMessage,
                         THtiMessagePriority aPriority);

protected:
    CHtiAppControl();
    void ConstructL();

    virtual ~CHtiAppControl();
    
    void HandleAppControlL(const TDesC8& aMessage);
    void HandleInstallerControlL( const TDesC8& aMessage );
    /**
    * Extracts string from incoming request and convert it to unicode
    * for non-unicode request
    * aResult should have enough maz length and it will contain
    * Function returns either offset for a next parameter in aRequest
    * or some symbian error code
    */
    TInt ParseString( const TDesC8& aRequest,
                      TInt anOffset,
                      TBool aUnicode,
                      TDes& aResult);

    //command handlers
    void HandleStartProcessL( const TDesC& aProgramName,
                              const TDesC& aCmdLine,
                              TBool aStoreProcessHandle );

    void HandleStopProcessL( RProcess& aProcess );
    void HandleStatusProcessL( RProcess& aProcess );

    void HandleListProcessesL( const TDesC& aMatch );

    void HandleStartAppL( TApaAppInfo &aAppInfo, const TDesC& aDocName );

    void HandleListAppsL( TBool aIncludeHidden,
                          TBool aIncludeSystem,
                          TBool aUnicode );

    void HandleListInstalledAppsL( TBool aUnicode );

    //construct and send short message
    void SendMessageL(TAppCommand aResponseCode, const TDesC8& aMsg = KNullDesC8 );

    /**
    * Helper to send error message
    * @return KErrNone or some system-wide error code
    */
    inline TInt SendErrorMsg( TInt anError, const TDesC8& aMsg );

    /**
    *
    */
    TInt OpenProcessL( RProcess& aProcess, const TDesC& aMatch );
    TInt OpenProcessL( RProcess& aProcess, const TUint32 aProcessId );

#if ( SYMBIAN_VERSION_SUPPORT < SYMBIAN_4 )
    SwiUI::TPolicy ConvertToPolicy( const TInt8 aValue );
    TInt GetPackageUidL( const TDesC& aPackageName, TInt aMimeIndex );
#else
    Usif::TSifPolicy ConvertToSifPolicy( const TInt8 aValue );
    TInt GetComponentIdFromUid(const TInt32 aUid);
    TInt GetComponentIdFromPackageName(const TDesC& aPackageName);
#endif

    TBool ValidateInstallParams( const TDesC8& aParams, TBool aIsUnicode );

    /**
    * Find application based on its full path name and fill in aAppInfo
    * @return KErrNone or KErrNotFound
    */
    TBool FindAppL( TApaAppInfo &aAppInfo, const TDesC& aAppFullName );
    TBool FindAppL( TApaAppInfo &aAppInfo, const TInt32 aUid );

    /**
     * Find a task running the application specified by the UID as a root app
     * (i.e. not embedded.
     * @param aAppUid the UID of the application to find
     * @return the found task or an empty task if not found
     */
    TApaTask FindRunningRootApp( TUid aAppUid );

    /**
    * Convert string to T 32bit int (e.g. TUint, TInt32)
    */
    template<class T> T Parse32( const TDesC8& a32int);

protected:

    // Session to Window Server
    RWsSession iWs;

    // Session to Application Architecture Server
    RApaLsSession iAppServer;

    // A flag that is set to EFalse if connecting to Window Server or
    // Application Architecture Server fails. In that case all application
    // related commands will return a "not supported" error.
    TBool iIsAppCmdsSupported;

    // Container array for process handles for querying the process exit codes
    RArray<RProcess> iProcessHandleArray;

    // possible mimetypes for silent software uninstallation
    CDesC8ArrayFlat* iMimeTypes;

#if ( SYMBIAN_VERSION_SUPPORT < SYMBIAN_4 )
    SwiUI::TInstallOptions iInstOpts;
    SwiUI::TInstallOptionsPckg iInstOptsPckg;
    SwiUI::TUninstallOptions iUnInstOpts;
    SwiUI::TUninstallOptionsPckg iUnInstOptsPckg;
    RPointerArray<Swi::CSisRegistryPackage> iAugmentations;
    TInt iAugmentationIndex;
#endif
    };

#endif

// End of file
