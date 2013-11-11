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
* Description:  Implementation of ECOM plug-in service interface. Provides
*                system info service.
*
*/


#ifndef HTISYSINFOPLUGIN_H
#define HTISYSINFOPLUGIN_H

// INCLUDES
#include "../../../symbian_version.hrh"

#include <e32property.h>
#include <f32file.h>
#include <HtiServicePluginInterface.h>

// FORWARD DECLARATIONS
class CHtiLightsController;
class CHtiPropertySubscriber;
class CMGXFileManager;
class CMGXFileNotificationHandler;
class MDesCArray;

// CLASS DECLARATION
/**
* System info service plugin.
*/
class CHtiSysInfoServicePlugin : public CHTIServicePluginInterface
    {
    public:

        static CHtiSysInfoServicePlugin* NewL();

        // Interface implementation
        void ProcessMessageL( const TDesC8& aMessage,
                              THtiMessagePriority aPriority );

        void NotifyMemoryChange( TInt aAvailableMemory );

#if ( SYMBIAN_VERSION_SUPPORT < SYMBIAN_4 )
        static TInt HandleAllowSSPropertyChange( TAny* aPtr );
#endif

    protected:

        CHtiSysInfoServicePlugin();
        void ConstructL();
        virtual ~CHtiSysInfoServicePlugin();


    private: // private helper methods

        void HandleGetHalAttrL( const TDesC8& aMessage );
        void HandleGetImeiL( const TDesC8& aMessage );
        void HandleGetSwVersionL( const TDesC8& aMessage );
        void HandleGetLangVersionL( const TDesC8& aMessage );
        void HandleGetSwLangVersionL( const TDesC8& aMessage );
        void HandleGetUserAgentStringL( const TDesC8& aMessage );
        void HandleGetFreeRamL( const TDesC8& aMessage );
        void HandleGetUsedRamL( const TDesC8& aMessage );
        void HandleGetTotalRamL( const TDesC8& aMessage );
        void HandleEatRamL( const TDesC8& aMessage );
        void HandleReleaseRamL( const TDesC8& aMessage );
        void HandleGetFreeDiskSpaceL( const TDesC8& aMessage );
        void HandleGetUsedDiskSpaceL( const TDesC8& aMessage );
        void HandleGetTotalDiskSpaceL( const TDesC8& aMessage );
        void HandleEatDiskSpaceL( const TDesC8& aMessage );
        void HandleReleaseDiskSpaceL( const TDesC8& aMessage );
        void HandleSetHomeTimeL( const TDesC8& aMessage );
        void HandleGetHomeTimeL( const TDesC8& aMessage );
        void HandleLightsCommandL( const TDesC8& aMessage );
        void HandleScreenSaverCommandL( const TDesC8& aMessage );
        void HandleScreenSaverTimeoutCommandL( const TDesC8& aMessage );
        void HandleNetworkModeCommandL( const TDesC8& aMessage );
        void HandleIrActivateCommandL( const TDesC8& aMessage );
        void HandleBtPowerCommandL( const TDesC8& aMessage );
        void HandleBtSettingsCommandL( const TDesC8& aMessage );
        void HandleBtDeletePairingsL( const TDesC8& aMessage );
        void HandleKeyLockToggleL( const TDesC8& aMessage );
        void HandleAutoKeyGuardTimeL( const TDesC8& aMessage );
        void HandleEmptyDrmRightsDbL( const TDesC8& aMessage );
        void HandleBatteryStatusL( const TDesC8& aMessage );
        void HandleSignalStrengthL( const TDesC8& aMessage );
        void HandleSetDateTimeFormatL( const TDesC8& aMessage );
        void HandleHsdpaCommandL( const TDesC8& aMessage );
        void HandleUpdateMediaGalleryL( const TDesC8& aMessage );
        void HandleActivateSkinL( const TDesC8& aMessage );
        void HandleSetLanguageL( const TDesC8& aMessage );

        void SetDefaultNumberModeL(TInt aMode, TInt aNbrModeType);
        void ParseTimeDataL( const TDesC8& aTimeData, TTime& aResult );
        TInt CleanUpTempFiles();
        TBool CanTurnBluetoothOnL( const TBool aUseForce );
        TInt CreatFileToEatDiskSpace( TFileName aPath, TInt64 aSpaceToEat  );

    private: // private data
        RFs iFs;
        CFileMan* iFileMan;
        HBufC8* iMemEater;
        HBufC8* iReply;
        CHtiLightsController* iLightsController;
        CHtiPropertySubscriber* iAllowSSSubscriber;
#if ( SYMBIAN_VERSION_SUPPORT < SYMBIAN_4 )
        TInt iAllowSSValue;
        RProperty iAllowSSProperty;
        TBool iAllowSSPropertyAttached;
#endif
        TBool iGalleryUpdateSupported;
    };


// CLASS DECLARATION
/**
*  Helper class to wait the async requests.
*/
class CAsyncWaiter : public CActive
    {
    public:
        static CAsyncWaiter* NewL( TInt aPriority = EPriorityStandard );
        static CAsyncWaiter* NewLC( TInt aPriority = EPriorityStandard );
        ~CAsyncWaiter();

        void StartAndWait();
        TInt Result() const;

    private:
        CAsyncWaiter( TInt aPriority );

        // from CActive
        void RunL();
        void DoCancel();

    private:
        CActiveSchedulerWait* iWait;
        TInt iResult;
        
    };


// CLASS DECLARATION
/**
*  Copied from MGXFileManagerFactory.h
*/
class MGXFileManagerFactory
    {
    public:
        static CMGXFileManager* NewFileManagerL( RFs& aFs );
        static CMGXFileNotificationHandler* NewFileNotificationHandlerL();
    };


// CLASS DECLARATION
/**
*  Copied from CMGXFileManager.h
*/
class CMGXFileManager : public CBase
    {
    public:
        virtual TBool SuccessFileNameL( const TDesC& aFileName,
                                        TDriveNumber aDrive ) = 0;
        virtual void UpdateL() = 0;
        virtual void UpdateL( const TDesC& aFullPath ) = 0;
        virtual void UpdateL( const TDesC& aOldName,
                              const TDesC& aNewName ) = 0;
        virtual void UpdateL( const MDesCArray& aFileNameArray ) = 0;
    };


#endif // HTISYSINFOPLUGIN_H

// End of file
