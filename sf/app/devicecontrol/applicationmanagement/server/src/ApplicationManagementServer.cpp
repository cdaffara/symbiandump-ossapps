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

#define __INCLUDE_CAPABILITY_NAMES__
#include <e32svr.h>
#include <badesca.h>
#include <e32cmn.h>
#include <bautils.h> 
#include <featmgr.h>
#include <imcvcodc.h>
#include <uikon.hrh>
#include <e32property.h>
#include <driveinfo.h>
#include <SWInstDefs.h>
#include <PolicyEngineClient.h>
#include <PolicyEngineXACML.h>
#include <eikenv.h>
#include <hbapplication.h>
#include <apgwgnam.h>
#include <nsmldmconst.h>
#include <DevManInternalCRKeys.h>
#include <apgtask.h>
#include <apgwgnam.h>
#include "ampskeys.h"
#include "amprivateCRKeys.h"
#include "ApplicationManagementConst.h"
#include "ApplicationManagementCommon.h"
#include "ApplicationManagementClientServer.h"
#include "ApplicationManagementServer.h"
#include "AMDeploymentComponent.h"
#include "amsmlhelper.h"
#include "ApplicationManagementUtility.h"
#include "ApplicationManagementUtilityFactory.h"
#include "debug.h"
#include "coemain.h"
#include "AMPreInstallApp.h"
#include "amview.h"
#include "appmgmtnotifier.h"

using namespace NApplicationManagement;

#ifdef __AM_CONNECT_INSTALLER_ONSTARTUP_	

#pragma message("AM Connecting installer at startup")
#else
#pragma message("AM Connecting installer when needed")
#endif
// Needed only for jad+jar installation  
_LIT( KInstallDir, "c:\\temp\\" );
_LIT8(KMIDletJarURL, "MIDlet-Jar-URL");
//OMA SCOMO Specific
_LIT8(KDownloadState, "./SCOMO/Download/");
_LIT8(KDeliveredState, "./SCOMO/Inventory/Delivered/");
_LIT8(KDeployedState, "./SCOMO/Inventory/Deployed/");

//Package all the above together into a policy
CPolicyServer::TPolicy KApplicationManagementSecurityPolicy;

// ------------------------------------------------------------------------------------------------
// CApplicationManagementServer::ComponentDownloadComplete()
// ------------------------------------------------------------------------------------------------
void CApplicationManagementServer::ComponentDownloadComplete(
        CDeploymentComponent *aComponent, TInt iapid, HBufC8 *aServerId)
    {
    TInt err = KErrNone;
    TRAP( err, DownloadCompleteL( aComponent, iapid, aServerId ));
    }

// ------------------------------------------------------------------------------------------------
// CApplicationManagementServer::DownloadCompleteL()
// ------------------------------------------------------------------------------------------------

void CApplicationManagementServer::DownloadCompleteL(
        CDeploymentComponent *aComponent, TInt iapid, HBufC8 *aServerId)
    {
    RDEBUG( "ApplicationManagementServer DownloadComplete - start" );
    __ASSERT_DEBUG( aComponent, User::Invariant() );
    iComp = aComponent;
    iStorage->UpdateL(*aComponent);
    TInt errdownload = KErrNone;
    TInt errInstall = KErrNone;
    TInt status = KStatusSuccess;

    if (!aComponent->DownloadFailed())
        {
        CDeploymentComponent *old = DoDataUpdateCheckL(*aComponent);
        TDownloadTarget trgt = aComponent->DownloadTarget();
        switch (trgt)
            {
            case EDeliver:
                {
                break;
                }
            case EInstall:
            case EInstallAndActivate:
                {

                TRAP(errInstall,DoTheInstallL( *aComponent));
RDEBUG_2( "ApplicationManagementServer DownloadComplete - start %d", errInstall);

                if (trgt == EInstall && errInstall == KErrNone)
                    {
                    iStorage->DeactivateL(*aComponent);
                    }
                break;
                }

            case EUpdate:
            case EUpdateAndActivate:
                {
                if (old)
                    {
                    FullUpdateL(*aComponent, *old);
                    if (trgt == EUpdate)
                        {
                        iStorage->DeactivateL(*aComponent);
                        }
                    }
                break;
                }
            default:
                {
                break;
                }
            }
        }
    else
        {
        errdownload = aComponent->DownloadStatus();
        }

    RDEBUG( "ApplicationManagementServer DownloadComplete - Start Send to background" );
    //Download Completed/ Failed or installation success/ failure Send server to Background.

    iOMASCOMOEnabled = IsOMASCOMOEnabledL();
    
    m_Window->lower();
    
    if (iOMASCOMOEnabled)
        {
        status = GetErrorStatus(errdownload, errInstall);

        TInt ASyncSupported = -1;

        CRepository *repository = CRepository::NewLC(
                KUidPSApplicationManagementKeys);
        repository->Get(KAsyncEnabled, ASyncSupported);
        CleanupStack::PopAndDestroy();

        // For Sync Reporting
        if (!ASyncSupported)
            {
            // Update Central Repository with proper error code for Sync

            CRepository *repository = CRepository::NewLC(
                    KUidPSApplicationManagementKeys);
            repository->Set(KAMSyncStatus, status);
            CleanupStack::PopAndDestroy();

            RProperty counter;

            TInt r = counter.Attach(KUidPSApplicationManagementKeys,
                    KSyncNotifier, EOwnerThread);
            User::LeaveIfError(r);

            TInt err = counter.Set(KErrCancel);
            User::LeaveIfError(err);
            counter.Close();
            }
        else// Do AsyncReporting
            {

            RDEBUG( "CApplicationManagementServer::ASync Reporting Step 1" );

            // Get deployement component entries
            TInt internalid = aComponent->InternalId();

            RDEBUG( "CApplicationManagementServer::ASync Reporting Step 2" );

            TBuf8<256> targetURI;

            TDeploymentComponentState state = aComponent->State();

            RDEBUG( "CApplicationManagementServer::ASync Reporting Step 3" );

            if (state == EDCSDownload)
                {

                RDEBUG( "CApplicationManagementServer::ASync EDCSDownload Start" );

                targetURI.Append(KDownloadState);
                targetURI.Append(aComponent->UserId());

                RDEBUG( "CApplicationManagementServer::ASync EDCSDownload End" );

                }
            else if (state == EDCSDelivered)
                {
                RDEBUG( "CApplicationManagementServer::ASync EDCSDelivered Start" );

                targetURI.Append(KDeliveredState);
                targetURI.Append(aComponent->UserId());

                RDEBUG( "CApplicationManagementServer::ASync EDCSDelivered End" );

                }
            else if (state == EDCSActive || state == EDCSInactive)
                {

                RDEBUG( "CApplicationManagementServer::ASync EDCSActive Start" );

                targetURI.Append(KDeployedState);
                targetURI.Append(aComponent->UserId());

                RDEBUG( "CApplicationManagementServer::ASync EDCSActive End" );
                }

            //Save Entry to database with status and target URI of deployment component
            TInt iapidval = 2;
            TRAPD( err1, SmlHelper::GetDefaultIAPFromDMProfileL( iapid ) );
            if (err1 == KErrNone)
                {
                RDEBUG_2("Application Mgmt Server service::DownloadCompleteL iapid to : %d", iapid );
                }

            TInt index;
            iAMServerDB->SaveToDatabaseL(index, internalid, status,
                    targetURI, iapidval);

            RDEBUG( "CApplicationManagementServer::ASync SaveToDatabaseL completed" );

            EnableDMNetworkMonL(iapid, aServerId);
            }
        }
    int value = 0 ;
    int err = RProperty::Set(TUid::Uid(KOmaDMAppUid), KDownloadActive, value);
    RDEBUG_2("RProperty set (%d)", err  );
    if (iDownloadMngr->DownloadCount() == 0)
        {

        RDEBUG_2( "CApplicationManagementServer::ConstructL - ERROR leaving cannot connect installer: %d", iSessionCount );

        // Is session count is not modified whenever there is error then at the end of download complete decrement
        //  the session count.

        if (status != KStatusSuccess && iSessionCount != 0)
            {
            iSessionCount--;
            }

        if (iSessionCount == 0)
            {
            iShutdown.Start();    
            }
        }
    aComponent->SetDriveSelectionStatus(false);
    RDEBUG( "ApplicationManagementServer DownloadComplete - end" );
    }

void CApplicationManagementServer::InstallationCancelled()
	{
		iInstaller.CancelOperation();
	}
// ------------------------------------------------------------------------------------------------
// CApplicationManagementServer::IsOMASCOMOEnabledL()
// ------------------------------------------------------------------------------------------------

TBool CApplicationManagementServer::IsOMASCOMOEnabledL()
    {
    RDEBUG( "CApplicationManagementServer::IsOMASCOMOEnabledL Start" );

    TInt adapterValue;

    CRepository *repository = CRepository::NewLC(
            KCRUidPrivateApplicationManagementKeys);
    repository->Get(KAMAdapterValue, adapterValue);
    CleanupStack::PopAndDestroy();

    RDEBUG( "CApplicationManagementServer::IsOMASCOMOEnabledL End" );

    if (adapterValue == 0 || adapterValue == 1)
        return EFalse;
    else
        return ETrue;
    }

// ------------------------------------------------------------------------------------------------
// CApplicationManagementServer::GetErrorStatus()
// ------------------------------------------------------------------------------------------------
TInt CApplicationManagementServer::GetErrorStatus(TInt aErrorDownload,
        TInt aErrorInstall)
    {
    TInt status = KStatusSuccess;

    if (aErrorDownload != KErrNone)
        {
        if (aErrorDownload == KStatusUserCancelled || aErrorDownload
                == KStatusDowloadFailedOOM || aErrorDownload
                == KStatusUnSupportedEnvironment || aErrorDownload
                == KStatusAlternateDownldAuthFail || aErrorDownload
                == KStatusAltDowldUnavailable)
            {
            status = aErrorDownload;
            }
        else
            status = KStatusDownloadFailed;
        }

    else if (aErrorInstall != KErrNone)
        {
        if (aErrorInstall == SwiUI::KSWInstErrSecurityFailure)
            status = KStatusPkgValidationFailed;
        if (aErrorInstall == SwiUI::KSWInstErrInsufficientMemory)
            status = KStatusInstallFailedOOM;
	  if (aErrorInstall == KStatusUserCancelled)
		status = KStatusUserCancelled;
        else
            status = KStatusInstallFailed;

        }

    return status;

    }

// ------------------------------------------------------------------------------------------------
// CApplicationManagementServer::EnableDMNetworkMonL()
// ------------------------------------------------------------------------------------------------
void CApplicationManagementServer::EnableDMNetworkMonL(TInt iapid,
        HBufC8 *aServerId)
    {
    TInt retryenabled = 1;
    _LIT( KNetMon,"\\dmnetworkmon.exe" );

    // Enable DM Network Monitoring for retry of Generic alert in case of N/W loss

    CRepository *repository = CRepository::NewLC(
            KCRUidDeviceManagementInternalKeys);
    TInt err_val = repository->Set(KDevManEnableDMNetworkMon, retryenabled);
    RDEBUG_2( "CApplicationManagementServer::EnableDMNetworkMonL Retry enabled err: %d", err_val );
    RDEBUG_2( "CApplicationManagementServer::EnableDMNetworkMonL Retry enabled value: %d", retryenabled );

    err_val = repository->Set(KDevManIapIdKey, iapid);
    RDEBUG_2( "CApplicationManagementServer::EnableDMNetworkMonL devmaniapid: %d", err_val );
    RDEBUG_2( "CApplicationManagementServer::EnableDMNetworkMonL iap id: %d", iapid );

    err_val = repository->Set(KDevManServerIdKey, *aServerId);
    RDEBUG_2( "CApplicationManagementServer::EnableDMNetworkMonL serveridkey: %d", err_val );

    TBuf<256> buf1;
    buf1.Copy(*aServerId);
    RDEBUG_2( "Server ID -  '%S'", &buf1);

    CleanupStack::PopAndDestroy();

    // create NetMon EXE
    RProcess rp;
    TInt err = rp.Create(KNetMon, KNullDesC);
    User::LeaveIfError(err);
    TRequestStatus stat;
    rp.Rendezvous(stat);

    if (stat != KRequestPending)
        rp.Kill(0); // abort startup
    else
        rp.Resume(); // logon OK - start the server
    TInt r = (rp.ExitType() == EExitPanic) ? KErrGeneral : stat.Int();
    rp.Close();

    }

// ------------------------------------------------------------------------------------------------
// CApplicationManagementServer::CApplicationManagementServer
// ------------------------------------------------------------------------------------------------

inline CApplicationManagementServer::CApplicationManagementServer() :
    CPolicyServer(0, KApplicationManagementSecurityPolicy, ESharableSessions)
    {
    }

// ------------------------------------------------------------------------------------------------
// CApplicationManagementServer::AddSession()
// ------------------------------------------------------------------------------------------------

void CApplicationManagementServer::AddSession()
    {
    // A new session is being created
    RDEBUG( "ApplicationManagementServer: CApplicationManagementServer::AddSession" );
    ++iSessionCount;

    // Cancel the shutdown timer if it was running
    iShutdown.Cancel();
    }

// ------------------------------------------------------------------------------------------------
// CApplicationManagementServer::DropSession()
// ------------------------------------------------------------------------------------------------

void CApplicationManagementServer::DropSession()
    {
    // A session is being destroyed
    RDEBUG( "ApplicationManagementServer: CApplicationManagementServer::DropSession" );

    // Start the shutdown timer if it is the last session
    if (--iSessionCount == 0)
        {
        if (iDownloadMngr->DownloadCount() > 0)
            {
            RDEBUG( "ApplicationManagementServer: CApplicationManagementServer::DropSession download in progress" );
            }
        else
            {
            iShutdown.Start();
            }
        }
    }

// -----------------------------------------------------------------------------------------------------------
// CApplicationManagementServer::AddDownloadL( CDeploymentComponent *aComponent )
// -----------------------------------------------------------------------------------------------------------

void CApplicationManagementServer::AddDownloadL(
        CDeploymentComponent *aComponent)
    {
    __ASSERT_DEBUG( aComponent, User::Invariant() );

    RDEBUG( "CApplicationManagementServer::AddDownloadL Start" );

    iDownloadMngr->AddDownloadL(aComponent);

    RDEBUG( "CApplicationManagementServer::AddDownloadL End" );
    }

// ------------------------------------------------------------------------------------------------
// CApplicationManagementServer
// -----------------------------------------------------------------------------

CServer2* CApplicationManagementServer::NewL(HbMainWindow *mainWindow)
    {
    //specifies all connect attempts should pass
    KApplicationManagementSecurityPolicy.iOnConnect
            = CPolicyServer::EAlwaysPass;
    KApplicationManagementSecurityPolicy.iRangeCount
            = KApplicationManagementRangeCount; //number of ranges                                   
    KApplicationManagementSecurityPolicy.iRanges
            = KApplicationManagementRanges;
    KApplicationManagementSecurityPolicy.iElementsIndex
            = KApplicationManagementSecurityElementsIndex;
    KApplicationManagementSecurityPolicy.iElements
            = KApplicationManagementSecurityElements;

    RDEBUG( "ApplicationManagementServer: CApplicationManagementServer::NewLC" );

    CApplicationManagementServer* self =
            new (ELeave) CApplicationManagementServer;
    CleanupStack::PushL(self);

    self->ConstructL(mainWindow);
    CleanupStack::Pop();
    return self;

    }

// -----------------------------------------------------------------------------
// CApplicationManagementServer::~CApplicationManagementServer
// ------------------------------------------------------------------------------------------------

CApplicationManagementServer::~CApplicationManagementServer()
    {
    RDEBUG_3("CApplicationManagementServer::~CApplicationManagementServer 0x%X - 0x%X", reinterpret_cast<TUint>(this ),
            reinterpret_cast<TUint>(this)+sizeof( CApplicationManagementServer ) );
    FeatureManager::UnInitializeLib();
    iInstaller.Close();
    delete iStorage;

    delete iUtility;
    delete iAMServerDB;

    RProperty::Delete(KUidPSApplicationManagementKeys, KAMServerUIEnabled);

    RProperty::Delete(KUidPSApplicationManagementKeys, KAMOperationNonSilent);

    RProperty::Delete(KUidPSApplicationManagementKeys, KSyncNotifier);

    RProperty::Delete(KUidPSApplicationManagementKeys, KDMUINotClosed);

    iInstallFile.Close();
    iInstallRFs.Close();

    delete iDownloadMngr;

    }

// ------------------------------------------------------------------------------------------------
// CApplicationManagementServer::HandleAllClientsClosed
// ------------------------------------------------------------------------------------------------
void CApplicationManagementServer::HandleAllClientsClosed()
    {
    // Do Nothing
    RDEBUG( "CApplicationManagementServer::HandleAllClientsClosed" );
    }

// ------------------------------------------------------------------------------------------------
// CApplicationManagementServer::ConstructL
// ------------------------------------------------------------------------------------------------

void CApplicationManagementServer::ConstructL(HbMainWindow* aWindow)
    {

    // 2nd phase construction - ensure the timer and server objects are running
    RDEBUG( "CApplicationManagementServer::ConstructL - Start" );

    iOMASCOMOEnabled = ETrue;
    mUserCancelled = 0;
    TRAPD( errf, FeatureManager::InitializeLibL() );
    if (errf != KErrNone)
        {
        User::Leave(errf);
        }
    m_Window = aWindow;
    RDEBUG( "CApplicationManagementServer::ConstructL - start" );
    StartL(KApplicationManagementServerName);
    iStorage = CDeliveryComponentStorage::NewL();

    iUtility
            = ApplicationManagementUtilityFactory::CreateManagementUtilityL();
    iDownloadMngr = CAMDownloadStore::NewL(*this);
    iAMServerDB = CAMDownloaddb::NewL();

    // Define P&S Keys
    RProperty::Define(KUidPSApplicationManagementKeys, KAMServerUIEnabled,
            RProperty::EInt);

    RProperty::Define(KUidPSApplicationManagementKeys, KAMOperationNonSilent,
            RProperty::EInt);

    RProperty::Define(KUidPSApplicationManagementKeys, KSyncNotifier,
            RProperty::EInt);

    RProperty::Define(KUidPSApplicationManagementKeys, KDMUINotClosed,
            RProperty::EInt);

    RProperty syncproperty;

    //Ignore error status
    syncproperty.Attach(KUidPSApplicationManagementKeys, KSyncNotifier,
            EOwnerThread);
    syncproperty.Set(KErrNone);
    syncproperty.Close();

#ifdef __AM_CONNECT_INSTALLER_ONSTARTUP_	
    TInt err( iInstaller.Connect() );
    if ( err != KErrNone )
        {
        RDEBUG_2( "CApplicationManagementServer::ConstructL - ERROR leaving cannot connect installer: %d", err );
        User::LeaveIfError( err );
        }
#endif	
    iShutdown.ConstructL();
    // ensure that the server still exits even if the 1st client fails to connect
    iShutdown.Start();

    RDEBUG( "CApplicationManagementServer::ConstructL - end" );
    }
//---------------------------------------------------------------------------
// Create a new client session. 
//---------------------------------------------------------------------------
CSession2* CApplicationManagementServer::NewSessionL(
        const TVersion& aVersion, const RMessage2& /*aMessage */) const
    {
    RDEBUG( "ApplicationManagementServer: CApplicationManagementServer::NewSessionL" );
    TVersion ver(KAMVerMajor, KAMVerMinor, KAMVerBuild);
    if (!User::QueryVersionSupported(ver, aVersion))
        {
        RDEBUG( "CApplicationManagementServer::NewSessionL(): wrong version offered -> BAIL OUT!" );
        User::Leave(KErrNotSupported);
        }
    return new (ELeave) CApplicationManagementSession();
    }

// ------------------------------------------------------------------------------------------------
// CApplicationManagementServer::CustomSecurityCheckL()
// ------------------------------------------------------------------------------------------------

CPolicyServer::TCustomResult CApplicationManagementServer::CustomSecurityCheckL(
        const RMessage2& /*aMsg*/, TInt& /*aAction*/, TSecurityInfo& /*aMissing*/)
    {
    return EPass;
    }

// ------------------------------------------------------------------------------------------------
// CApplicationManagementServer::InstallCompleteL()
// ------------------------------------------------------------------------------------------------

void CApplicationManagementServer::InstallCompleteL(
        const CDeploymentComponent& aCompo, const TBool &aInstallSuccess)
    {
    RDEBUG_2("CApplicationManagementServer::InstallCompleteL start (%d)", aInstallSuccess);

    iInstallFile.Close();
    // if java jad + jar install, remove files from c:\temp directory
    if (CDeploymentComponent::IsJavaMimeL(aCompo.MetaData().MimeType() ) && CDeploymentComponent::IsJavaMimeL(aCompo.Data().MimeType() ) )
        {
        TFileName fn, fn2;
        GenerateTempFileNameL(iInstallRFs, aCompo, fn, fn2);
        RDEBUG_3("CApplicationManagementServer::InstallCompleteL delete files: (%S) (%S)", &fn, &fn2);
        User::LeaveIfError(iInstallRFs.Delete(fn));
        User::LeaveIfError(iInstallRFs.Delete(fn2));
        }
         TInt silentsession = 0;
        TInt r2 = RProperty::Get(KPSUidNSmlSOSServerKey, KNSmlDMSilentJob,
                silentsession);

	  if(r2 == KErrNone)
		CApplicationManagementUtility::iSilentSession = silentsession;

        if (!CApplicationManagementUtility::iSilentSession)
        	{
    AppMgmtNotifier* note = new AppMgmtNotifier(m_appName);
    if (aInstallSuccess)
        {
        note->showInstallSuccessNote();
        // Remove temporary files from private directory 
        const_cast<CDeploymentComponent&> (aCompo).ResetDataL(iInstallRFs);
        }
    else if(mUserCancelled == 0)
        {
        note->showInstallFailedNote();
        m_Window->lower();
        }
        
    delete note;
     }
     else
     	{
     		 if (aInstallSuccess)
        {
        
        const_cast<CDeploymentComponent&> (aCompo).ResetDataL(iInstallRFs);
        m_Window->lower();
        }
     	}
     
    iInstallRFs.Close();

    iInstallInProgress = EFalse;
#ifndef __AM_CONNECT_INSTALLER_ONSTARTUP_	
    iInstaller.Close();
#endif
    RDEBUG("CApplicationManagementServer::InstallCompleteL end");
    }

// ------------------------------------------------------------------------------------------------
// CApplicationManagementServer::GenerateTempJarFileNameL()
// ------------------------------------------------------------------------------------------------

void CApplicationManagementServer::GenerateTempJarFileNameL(RFs &aFs,
        const TDesC& aMetaSource, TFileName &aFileName) const
    {
    if (aMetaSource != KNullDesC)
        {
        RFile file;
        User::LeaveIfError(file.Open(aFs, aMetaSource, EFileRead));
        CleanupClosePushL(file);
        TFileName temp;
        temp = ReadJarFileNameL(file);
        GenerateTempFileNameL(aFs, temp, aFileName);
        CleanupStack::PopAndDestroy(&file);
        }
    }

// ------------------------------------------------------------------------------------------------
// CApplicationManagementServer::GenerateTempFileNameL()
// ------------------------------------------------------------------------------------------------

void CApplicationManagementServer::GenerateTempFileNameL(const RFs &aFs,
        const TDesC& aSource, TFileName &aFileName) const
    {
    if (aSource != KNullDesC)
        {
        aFileName.Copy(KInstallDir);
        TParse p;
        User::LeaveIfError(aFs.Parse(aSource, p));
        aFileName.Append(p.NameAndExt());
        }
    }

// ------------------------------------------------------------------------------------------------
// CApplicationManagementServer::GenerateTempFileNameL()
// ------------------------------------------------------------------------------------------------

void CApplicationManagementServer::GenerateTempFileNameL(RFs &aFs,
        const CDeploymentComponent& aCompo, TFileName& aSource,
        TFileName &aMetaSource, TFileName &aFileName,
        TFileName &aMetaFileName) const
    {
    if (!BaflUtils::FolderExists(aFs, KInstallDir))
        {
        aFs.MkDirAll(KInstallDir);
        }

    aMetaSource.Copy(aCompo.MetaData().DataFileName());
    GenerateTempFileNameL(aFs, aMetaSource, aMetaFileName);
    aSource.Copy(aCompo.Data().DataFileName());
    GenerateTempJarFileNameL(aFs, aMetaSource, aFileName);
    }

// ------------------------------------------------------------------------------------------------
// CApplicationManagementServer::GenerateTempFileNameL()
// ------------------------------------------------------------------------------------------------	

void CApplicationManagementServer::GenerateTempFileNameL(RFs &aFs,
        const CDeploymentComponent& aCompo, TFileName &aFileName,
        TFileName &aMetaFileName) const
    {
    TFileName ftemp, msource;
    GenerateTempFileNameL(aFs, aCompo, ftemp, msource, aFileName,
            aMetaFileName);
    }

// ------------------------------------------------------------------------------------------------
// CApplicationManagementServer::ReadJarFileNameL()
// ------------------------------------------------------------------------------------------------

TFileName CApplicationManagementServer::ReadJarFileNameL(RFile& aFile) const
    {
    TFileName jarFileName;
    HBufC8* lineBuffer = HBufC8::NewLC(KMaxFileName + 72); // lets hope all lines fit to this
    TPtr8 linePtr(lineBuffer->Des());
    TInt length(0);
    aFile.Size(length);
    HBufC8* data = HBufC8::NewLC(length);
    TPtr8 ptr(data->Des());
    aFile.Read(ptr);
    RDesReadStream stream(*data);
    CleanupClosePushL(stream);
    TInt colonIndex = -1;
    TInt err(KErrNone);
    TInt found(KErrNotFound);
    const TInt KJadNameValueSeparator = 0x3A; //':' 

    for (;;)
        {
        TRAP( err, ReadLineL(stream, linePtr) );
        if (err == KErrNone)
            {
            RDEBUG8_2("Read line: (%S)", &linePtr);
            found = linePtr.Find(KMIDletJarURL);
            if (found != KErrNotFound && found == 0) // found
                {
                TPtrC8 filename(linePtr);
                colonIndex = linePtr.Locate(KJadNameValueSeparator);
                if (colonIndex != KErrNotFound)
                    {
                    filename.Set(linePtr.Mid(colonIndex + 1)); // get jar filename, format is: MIDlet-Jar-URL: bomber.jar
                    jarFileName.Copy(filename);
                    jarFileName.Trim();
                    break;
                    }
                }
            }
        else
            {
            User::Leave(err);
            }
        }
    CleanupStack::PopAndDestroy(3, lineBuffer); // stream, data
    return jarFileName;
    }

// ------------------------------------------------------------------------------------------------
// CApplicationManagementServer::ReadLineL()
// ------------------------------------------------------------------------------------------------

void CApplicationManagementServer::ReadLineL(RReadStream& aStream,
        TDes8& aLineBuffer) const
    {
    TBuf8<1> input;
    const TUint32 LF = 10;
    const TUint32 CR = 13;
    TBool endOfLine(EFalse);
    TInt errRead(KErrNone);

    // reset
    aLineBuffer.Zero();

    while (!endOfLine)
        {
        TRAP(errRead, aStream.ReadL(input, 1));

        if (errRead == KErrEof)
            {
            // EOF
            User::Leave(KErrEof);
            }
        // check special characters	
        if (input[0] == LF)
            {
            // End of line
            endOfLine = ETrue;
            }
        else if (input[0] == CR)
            {
            // Only end of line if not followed by a LF
            }
        else
            {
            // We put any other character in the buffer
            if (aLineBuffer.Length() < aLineBuffer.MaxLength())
                {
                aLineBuffer.Append(input[0]);
                }
            }
        }
    }

// ------------------------------------------------------------------------------------------------
// CApplicationManagementServer::PrepareInstallL()
// ------------------------------------------------------------------------------------------------	

TBool CApplicationManagementServer::PrepareInstallL(
        const CDeploymentComponent &aComponent, TFileName &aFileName)
    {
    RDEBUG( "CApplicationManagementServer::PrepareInstallL" );
    TBool res(ETrue);

#ifndef __AM_CONNECT_INSTALLER_ONSTARTUP_
    TInt err(iInstaller.Connect());
    if (err != KErrNone)
        {
        RDEBUG_2( "CApplicationManagementServer::PrepareInstallL - ERROR leaving cannot connect installer: %d", err );
        User::LeaveIfError(err);
        }
    CleanupClosePushL(iInstaller);
#endif

    RDEBUG( "CApplicationManagementServer::PrepareInstallL - connected to installer" );
    // Copy data file name (install file)
    if (aComponent.Data().DataFileName().Length() > 0)
        {
        aFileName.Copy(aComponent.Data().DataFileName());
        }
    else
        {
        // data file has not been created for component!	
        User::Leave(KErrNotFound);
        }
    // Open RFs session
    User::LeaveIfError(iInstallRFs.Connect());
    User::LeaveIfError(iInstallRFs.ShareProtected());

    // set installoptions and correct mime type
    using namespace SwiUI;
    iInstallOptions = aComponent.InstallOpts().iOptions;
    TInstallReq req;
    if (aComponent.Data().MimeType() == SwiUI::KSisMimeType)
        {
        //convert if MIME type is application/vnd.symbian.install to x-epoc/x-sisx-app
        //as installer doesn't support application/vnd.symbian.install MIME type from S60 3rd edition onwards.
        req.iMIME = SwiUI::KSisxMimeType;
        }
    else
        {
        req.iMIME = aComponent.Data().MimeType();
        }

    if (CDeploymentComponent::IsJavaMimeL(aComponent.MetaData().MimeType()))
        {
        // Check if both jad and jar files already exist in the phone	
        if (CDeploymentComponent::IsJavaMimeL(aComponent.Data().MimeType())
                && aComponent.MetaData().MimeType() == SwiUI::KJadMIMEType)
            {
            // Move both files to temporary location and use jad to install java application
            CFileMan *fm = CFileMan::NewL(iInstallRFs);
            CleanupStack::PushL(fm);
            TFileName source, metasource, metatarget;
            aFileName.Copy(KNullDesC8()); // reset
            GenerateTempFileNameL(iInstallRFs, aComponent, source,
                    metasource, aFileName, metatarget);

            RDEBUG_2( "CApplicationManagementServer::PrepareInstallL - temp jar file '%S'", &aFileName );
            RDEBUG_2( "CApplicationManagementServer::PrepareInstallL - temp jad file '%S'", &metatarget );

            // Copy datafile (jar)
            TInt copyer(fm->Copy(source, aFileName));

            // Copy metadatafile (jad)
            TInt copyer2(fm->Copy(metasource, metatarget));

            CleanupStack::PopAndDestroy(fm);

            RDEBUG_3( "CApplicationManagementServer::PrepareInstallL - (%d)-(%d)", copyer, copyer2 );

            if (copyer == KErrNone && copyer2 == KErrNone)
                {
                res = ETrue;
                aFileName = metatarget;
                req.iMIME = aComponent.MetaData().MimeType();
                }
            else
                {
                User::Leave(KErrWrite);
                }
            }
        else if (aComponent.MetaData().MimeType() == SwiUI::KJadMIMEType)
            {
            aFileName.Copy(aComponent.MetaData().DataFileName());
            req.iMIME = aComponent.MetaData().MimeType();
            RDEBUG_2( "CApplicationManagementServer::PrepareInstallL - Using Jad file to install java application: %S", &aFileName );
            }
        }

    if (aComponent.GetIAP() > -1)
        {
        req.iIAP = aComponent.GetIAP();
        RDEBUG_2( "CApplicationManagementServer::PrepareInstallL: setting iap to %d", req.iIAP );
        }
    else
        {
        TInt iap = KErrNotFound;
        TRAPD( err, SmlHelper::GetDefaultIAPFromDMProfileL( iap ) );
        if (err == KErrNone)
            {
            req.iIAP = iap;
            RDEBUG_2("	-> using DM profile default IAP: %d", iap);
            }
        else
            {
            RDEBUG_2("**** ERROR while fetching DM profile default iap: %d", err );
            }
        }

    iInstallReq = SwiUI::TInstallReqPckg(req);
    RDEBUG8_2( "CApplicationManagementServer::PrepareInstallL: mime type is (%S)", &req.iMIME);
    RDEBUG_2( "CApplicationManagementServer::PrepareInstallL - Using temp file '%S'", &aFileName );

    User::LeaveIfError(iInstallFile.Open(iInstallRFs, aFileName, EFileRead));
    TInt sz;
    User::LeaveIfError(iInstallFile.Size(sz));
    if (!(sz > 0))
        {
        res = EFalse;
        RDEBUG( "CApplicationManagementServer::PrepareInstallL - ERROR size of source file is 0!" );
        }

#ifndef __AM_CONNECT_INSTALLER_ONSTARTUP_   
    CleanupStack::Pop(&iInstaller);
#endif

    RDEBUG( "CApplicationManagementServer::PrepareInstallL end" );

    iAppname = aFileName;
    
    return res;
    }

// ------------------------------------------------------------------------------------------------
// CApplicationManagementServer::InstallL()
// ------------------------------------------------------------------------------------------------	

void CApplicationManagementServer::InstallL(
        const CDeploymentComponent &aComponent, TRequestStatus &aStatus)
    {
    RDEBUG( "CApplicationManagementServer::InstallL (async): begin" );

    if (iInstallInProgress == EFalse)
        {
        TFileName fn;
        TInt type = EInstallWaitDlg;
        TInt silentsession = 0;
        TInt r2 = RProperty::Get(KPSUidNSmlSOSServerKey, KNSmlDMSilentJob,
                silentsession);

	  if(r2 == KErrNone)
		CApplicationManagementUtility::iSilentSession = silentsession;

        if (!CApplicationManagementUtility::iSilentSession)
            {
            m_Dlg = new AMWaitProgDialog(m_appName, *this);
            m_Dlg->createWaitDialog(type);
            if (CApplicationManagementUtility::mHidden == 1)
                {
                m_Dlg->startIndicator();
                }
            else
                {
                m_Window->raise();
                CApplicationManagementUtility::mCurrDlg->setVisible(true);
                }
            }
        if (PrepareInstallL(aComponent, fn))
            {	  
            if (m_Dlg)
                m_Dlg->registerInstallRequest(aStatus);
            delete iArgu;
            iArgu = NULL;
            iArgu = Usif::COpaqueNamedParams::NewL();
            iArgu->AddIntL(Usif::KSifInParam_InstallSilently, 1);
            TAMInstallOptions opts = aComponent.InstallOpts();
            TChar drive = opts.iOptions.iDrive;
            RFs iFs;
            TInt driveNumber;
            iFs.CharToDrive(drive,driveNumber);

            iArgu->AddIntL(Usif::KSifInParam_Drive, driveNumber);  

            delete iResults;
            iResults = NULL;
            iResults = Usif::COpaqueNamedParams::NewL();

            RDEBUG_3( "CApplicationManagementServer::InstallL: Install '%S' sizeof opts: %d", &fn, sizeof (aComponent.InstallOpts()));
		TRAPD( err ,iInstaller.Install( iInstallFile, *iArgu, *iResults, aStatus ) );

            RDEBUG_2( "CApplicationManagementServer::InstallL: status: %d", err);
            User::LeaveIfError(err);
            iInstallInProgress = ETrue;
            }
        else
            {
            RDEBUG( "CApplicationManagementServer::InstallL - PrepareInstallL failed, leaving" );
            User::Leave(KErrArgument);
            }
        }
    else
        {
        RDEBUG( "CApplicationManagementServer::InstallL (async): previous install still in progress...bailing out" );
        TRequestStatus * status = &aStatus;
        User::RequestComplete(status, KErrInUse);
        }
    RDEBUG( "CApplicationManagementServer::InstallL (async): end" );
    }

// ------------------------------------------------------------------------------------------------
// CApplicationManagementServer::PrepareInstallL()
// ------------------------------------------------------------------------------------------------	

void CApplicationManagementServer::InstallL(
        const CDeploymentComponent &aComponent)
    {
    RDEBUG( "CApplicationManagementServer::InstallL: begin" );

    if (iInstallInProgress == EFalse)
        {
        TFileName fn;
        if (PrepareInstallL(aComponent, fn))
            {
            iInstallInProgress = ETrue;
            RDEBUG_2( "CApplicationManagementServer::InstallL: Install '%S' ", &fn);
            SwiUI::TInstallOptionsPckg pkg(aComponent.InstallOpts().iOptions);
            iInstallInProgress = EFalse;
            }
        else
            {
            RDEBUG( "CApplicationManagementServer::InstallL - PrepareInstallL failed, leaving" );
            User::Leave(KErrArgument);
            }
        }
    RDEBUG( "CApplicationManagementServer::InstallL: end" );
    }

// ------------------------------------------------------------------------------------------------
// CApplicationManagementServer::UninstallL()
// ------------------------------------------------------------------------------------------------	

void CApplicationManagementServer::UninstallL(
        const CDeploymentComponent &aComponent, TRequestStatus &aStatus)
    {
    RDEBUG( "CApplicationManagementServer::UninstallL: begin" );
    if (iInstallInProgress == EFalse)
        {
        TInt err(iInstaller.Connect());
        User::LeaveIfError(err);
        RDEBUG_2( "CApplicationManagementServer::UninstallL: UninstallL '0x%X' ", aComponent.Uid());
        using namespace SwiUI;
        iUninstallOptions = aComponent.UninstallOpts();

        delete iArgu;
        iArgu = NULL;
        iArgu = Usif::COpaqueNamedParams::NewL();
        iArgu->AddIntL( Usif::KSifInParam_InstallSilently, 1 );

        delete iResults;
        iResults = NULL;
        iResults = Usif::COpaqueNamedParams::NewL();
        TRAP( err ,iInstaller.Uninstall( aComponent.GetComponentId(), *iArgu, *iResults, aStatus ));

        RDEBUG_2( "CApplicationManagementServer::UninstallL: UninstallL result '0x%X'", err );
        User::LeaveIfError(err);
        iInstallInProgress = ETrue;
        }
    else
        {
        RDEBUG( "CApplicationManagementServer::UninstallL: previous uninstall still in progress...bailing out" );
        TRequestStatus * status = &aStatus;
        User::RequestComplete(status, KErrInUse);
        }

    RDEBUG( "CApplicationManagementServer::UninstallL: end" );
    }
// ------------------------------------------------------------------------------------------------
// CApplicationManagementServer::PerformRFSL()
// ------------------------------------------------------------------------------------------------

void CApplicationManagementServer::PerformRFSL()
    {
    RDEBUG( "CApplicationManagementServer::PerformRFSL: begin" );
    const RComponentIdArray &arrt = iStorage->GetComponentIds();
    TCertInfoPckg *corcert = NULL;
    for (TInt i(0); i < arrt.Count(); i++)
        {
        RDEBUG_3( "CApplicationManagementServer::PerformRFSL processing dc %d of %d", i, arrt.Count() );
        CDeploymentComponent &c = iStorage->ComponentL(arrt[i]);

        const RCertArray &certs = iStorage->Certificates();
        TInt index = c.Owner();
        TCertInfoPckg *certp = NULL;
        if (index >= 0)
            {
            certp = certs[c.Owner()];
            }
        if (!corcert && certp)
            {
            if (CheckCertL((*certp)()))
                {
                corcert = certp;
                RDEBUG_2( "CApplicationManagementServer::PerformRFSL Found CORP Cert! Removing %d ", i );
                RemoveInternalL(c);
                }
            else
                {
                RDEBUG_2( "CApplicationManagementServer::PerformRFSL skipping %d because not corp dc", i );
                }
            }
        else
            {
            if (corcert == certp)
                {
                RDEBUG_2( "CApplicationManagementServer::PerformRFSL Removing %d ", i );
                RemoveInternalL(c);
                }
            else
                {
                RDEBUG_2( "CApplicationManagementServer::PerformRFSL skipping %d because not corp dc", i );
                }
            }
        }
    RDEBUG( "CApplicationManagementServer::PerformRFSL: end" );
    }

// ------------------------------------------------------------------------------------------------
// CApplicationManagementServer::RemoveInternalL()
// ------------------------------------------------------------------------------------------------


void CApplicationManagementServer::RemoveInternalL(
        const CDeploymentComponent &aCompo, TBool aDoUninstall /* = ETrue */)
    {
    RDEBUG_2("CApplicationManagementServer: RemoveInternalL - Remove id: (%d)", aCompo.InternalId() );
    TInt err(KErrNone);
    TDeploymentComponentName name = aCompo.ComponentName();
    TBuf<KDeploymentComponentNameMaxLength> ne;
    ne.Copy(name);
    m_appName = QString::fromUtf16(ne.Ptr(), ne.Length());

    RDEBUG("CApplicationManagementServer: RemoveInternalL - step1");

    TInt err1 = KErrNone;
    if (!err1)
        {
        RDEBUG("CApplicationManagementServer: RemoveInternalL - step2");

        aCompo.SetStatusNode(EDelivered_RemoveProgress);

        TDeploymentComponentState st(aCompo.State());
        if (st == EDCSDelivered || st == EDCSDownload)
            {
            RDEBUG("CApplicationManagementServer: RemoveInternalL - step3 start");
            TRAP(err,iStorage->RemoveL( aCompo.InternalId() ));
            RDEBUG("CApplicationManagementServer: RemoveInternalL - step4 start");
            }
        else if (st == EDCSActive || st == EDCSInactive)
            {
            RDEBUG("CApplicationManagementServer: RemoveInternalL - step5 start");
            if (aDoUninstall)
                {
                if (aCompo.Uid() != TUid::Null())
                    {
                    TInt nonSilentOperation = KErrNotFound;
                    iOMASCOMOEnabled = IsOMASCOMOEnabledL();
                    if (!iOMASCOMOEnabled)
                        nonSilentOperation = 0;
                    RDEBUG("CApplicationManagementServer: RemoveInternalL - step5 start 1");

                    TInt silentsession = 0;
                    RProperty::Get(KPSUidNSmlSOSServerKey, KNSmlDMSilentJob,
                            silentsession);

                    CApplicationManagementUtility::iSilentSession
                            = silentsession;
                    TRequestStatus s1 = KRequestPending;
                    AppMgmtNotifier* note = new AppMgmtNotifier(m_appName);
                    
                    // displaying uninstall confirm notes
                    if (!CApplicationManagementUtility::iSilentSession)
                        {
                        CDialogWait* wait = CDialogWait::NewL(); 
                        note->showUninstallDialog(aCompo, wait->iStatus);     
                        m_Window->raise();   
                        wait->StartWait();
                        s1=wait->iStatus;
                        delete wait;
                        }
                    if (s1 != KStatusUserCancelled)
                        {
                        TInt type = EUninstallWaitDlg;
                        TRequestStatus s;
                        if (!CApplicationManagementUtility::iSilentSession)
                            {
                            m_Dlg = new AMWaitProgDialog(m_appName, *this);
                            m_Dlg->createWaitDialog(type);
                            m_Window->raise();
                            CApplicationManagementUtility::mCurrDlg->setVisible(
                                        true);
                                
                            }
                        CDialogWait* wait1 = CDialogWait::NewL();           
                     
                        TRAP(err,UninstallL( aCompo, wait1->iStatus ));
                        //wait till uninstall completes
                        wait1->StartWait();
                        if (!CApplicationManagementUtility::iSilentSession)
                            m_Dlg->closeAMWaitDialog();
                        m_Window->lower();
                        s = wait1->iStatus;
                        delete wait1;
                        iInstallInProgress = EFalse;
                        // Capturing Installer errors,if any
                        err = s.Int();
                        if (s.Int() == KErrNone)
                            {
                            note->showUnInstallSuccessNote();
                            RDEBUG("CApplicationManagementServer::RemoveInternalL Uninstalled");
                            }
                        else
                            {
                            note->showUnInstallFailedNote();
                            if (s.Int() == KErrNotFound)
                                {
                                RDEBUG( "CApplicationManagementServer: RemoveInternalL WARNING component was not found by uninstaller" );
                                }
                            else
                                {
                                RDEBUG_2("CApplicationManagementServer: RemoveInternalL ERROR uninstall failed %d", s.Int() );
                                }
                            }
                        }
                    else
                        {
                        err = SwiUI::KSWInstErrUserCancel;
                        aCompo.SetStatusNode(EDelivered_RemoveFailed);
                        RDEBUG( "CApplicationManagementServer: RemoveInternalL User cancelled" );
                        }
                    delete note;
                    }
                else
                    {
                    RDEBUG( "CApplicationManagementServer: RemoveInternalL ERROR component is deployed but has null uid - it cannot be uninstalled!" );
                    }
                }
            else
                {
                RDEBUG_2("CApplicationManagementServer: RemoveInternalL - Skipped uninstall of %d", aCompo.InternalId() );
                }
            /*
             *   if (unstall is sucesss) then remove component from storage
             *   
             */
            if (err == KErrNone)
                {
                iStorage->RemoveL(aCompo.InternalId());
                }
            }
        else
            {
            RDEBUG_2("CApplicationManagementServer: RemoveInternalL ERROR called with illegal state component id %d", aCompo.State() );
            User::Leave(KErrArgument);
            }
        /*  
         * if error set state to remove failed
         * Finally call userleaveiferror()
         * */
        if (err != KErrNone)
            {
            aCompo.SetStatusNode(EDelivered_RemoveFailed);
            }
        }
    else
        {
        err = SwiUI::KSWInstErrUserCancel;
        aCompo.SetStatusNode(EDelivered_RemoveFailed);
        }

    User::LeaveIfError(err);
    }
    

// ------------------------------------------------------------------------------------------------
// CApplicationManagementServer::CheckCertL()
// ------------------------------------------------------------------------------------------------

TBool CApplicationManagementServer::CheckCertL(const TCertInfo &aInfo) const
    {
    TBool go(EFalse);

    if (FeatureManager::FeatureSupported(KFeatureIdSapPolicyManagement))
        {
        //	#ifdef __SAP_POLICY_MANAGEMENT
        RPolicyEngine peng;

        // The management session
        //
        TInt err(peng.Connect());

        if (err == KErrNone)
            {
            CleanupClosePushL(peng);
            RPolicyRequest rq;
            err = rq.Open(peng);
            if (err == KErrNone)
                {
                CleanupClosePushL(rq);

                TRequestContext rc;

                using namespace PolicyEngineXACML;
                using namespace NPolicyConstants;

                rc.AddSubjectAttributeL(KTrustedSubject, aInfo);
                rc.AddResourceAttributeL(KResourceId, KApplicationManagement,
                        KStringDataType);

                TResponse resp;
                err = rq.MakeRequest(rc, resp);
                if (err == KErrNone)
                    {
                    if (resp.GetResponseValue() == EResponsePermit
#ifdef _DEBUG
                            || resp.GetResponseValue()
                                    == EResponseNotApplicable
#endif 
                    )
                        {
                        go = ETrue;
                        }
                    else
                        {
                        RDEBUG_2( "CApplicationManagementSession::CheckCertL; Policy response denies access! %d", resp.GetResponseValue());
                        }
                    }
                else
                    {
                    RDEBUG_2( "CApplicationManagementSession::CheckCertL; ERROR making policy request! %d", err);
                    }
                CleanupStack::PopAndDestroy(&rq);
                }
            else
                {
                RDEBUG_2( "CApplicationManagementSession::CheckCertL; ERROR Failed to open policy engine session! %d", err);
                }
            CleanupStack::PopAndDestroy(&peng);
            }
        else
            {
            RDEBUG_2( "CApplicationManagementSession::CheckCertL; ERROR Failed to connect policy engine! %d", err);
            }
        User::LeaveIfError(err);
        }
    else
        {
        RDEBUG( "CApplicationManagementSession::CheckCertL; No PolicyManagement by featmgr-> cert ok!" );
        go = ETrue;
        }
    return go;
    }

// ------------------------------------------------------------------------------------------------
// CApplicationManagementServer::HasUidL()
// ------------------------------------------------------------------------------------------------	

TBool CApplicationManagementServer::HasUidL(const TUid &aUid,
        CDeploymentComponent *& aCompo, CDeploymentComponent *aIgnored /* = NULL */)
    {
    TBool found(EFalse);
    const RComponentIdArray &arrt = iStorage->GetComponentIds();
    TInt count(arrt.Count());
    for (TInt i(0); i < count; i++)
        {
        CDeploymentComponent &compo = iStorage->ComponentL(arrt[i]);
        if (compo.Uid() == aUid)
            {
            if (&compo != aIgnored)
                {
                aCompo = &compo;
                found = ETrue;
                RDEBUG_3( "ApplicationManagementServer: UidExistsL 0x%X - Found NOT ignored %d", aUid.iUid, compo.InternalId() );
                }
            else
                {
                RDEBUG( "ApplicationManagementServer: UidExistsL - Found ignored " );
                }
            }
        }
    if (!found)
        {
        found = FindInstalledSisUidL(aUid);
        if (!found)
            {
            found = FindInstalledJavaUidL(aUid);
            }
        }
    return found;
    }

// ------------------------------------------------------------------------------------------------
// CApplicationManagementServer::FindInstalledSisUidL()
// ------------------------------------------------------------------------------------------------	

TBool CApplicationManagementServer::FindInstalledSisUidL(const TUid &aUid) const
    {
    TBool found(EFalse);
    Swi::RSisRegistrySession sisses;
    TInt r(sisses.Connect());
    User::LeaveIfError(r);
    CleanupClosePushL(sisses);

    RArray<TUid> uids;
    sisses.InstalledUidsL(uids);
    CleanupClosePushL(uids);

    TInt uidc(uids.Count());
    for (TInt j(0); found == EFalse && j < uidc; j++)
        {
        if (uids[j] == aUid)
            {
            found = ETrue;
            }
        }
    CleanupStack::PopAndDestroy(&uids);
    CleanupStack::PopAndDestroy(&sisses);

    return found;
    }

// ------------------------------------------------------------------------------------------------
// CApplicationManagementServer::FindInstalledJavaUidL()
// ------------------------------------------------------------------------------------------------	

TBool CApplicationManagementServer::FindInstalledJavaUidL(const TUid &aUid) const
    {
    TBool found(EFalse);
    iUtility->RefreshJavaRegistryL();
    found = iUtility->FindInstalledJavaUidL(aUid);
    return found;
    }

// ------------------------------------------------------------------------------------------------
// CApplicationManagementServer::FindNewUid()
// ------------------------------------------------------------------------------------------------	

TUid CApplicationManagementServer::FindNewUid(const RArray<TUid> &aUidsOrig,
        const RArray<TUid> &aUidsNew) const
    {
    TUid ret(TUid::Null());
    TInt c1(aUidsOrig.Count());
    TInt c2(aUidsNew.Count());
    if (c2 >= c1)
        {
        for (TInt i(c2 - 1); i >= 0 && !ret.iUid; i--)
            {
            if (aUidsOrig.Find(aUidsNew[i]) == KErrNotFound)
                {
                ret = aUidsNew[i];
                }
            }
        }
    return ret;
    }

// ------------------------------------------------------------------------------------------------
// CApplicationManagementServer::JavaUidsL()
// ------------------------------------------------------------------------------------------------	

void CApplicationManagementServer::JavaUidsL(RArray<TUid> &aUids) const
    {
    iUtility->RefreshJavaRegistryL();
    iUtility->JavaUidsL(aUids);
    }

void CApplicationManagementServer::CheckforDuplicateMidletsL(
        TMidletParameters& amidletParameters)
    {
    RDEBUG( "CApplicationManagementServer::CheckforDuplicateMidletsL: Start");
    const RComponentIdArray &arrt = iStorage->GetComponentIds();
    TInt cont(arrt.Count());
    for (TInt i( 0); i < cont; i++)
     {
     CDeploymentComponent &compo = iStorage->ComponentL(arrt[i]);
     if (((compo.State() == EDCSActive) || (compo.State() == EDCSInactive ))
     && (compo.MidletName()== amidletParameters.iMidletName)
     && (compo.MidletVendor()== amidletParameters.iMidletVenorName))
     {
     //remove old one
     iStorage->RemoveL(compo.InternalId() );
     }
     }

    RDEBUG( "CApplicationManagementServer::CheckforDuplicateMidletsL: End");
    }
// ------------------------------------------------------------------------------------------------
// CApplicationManagementServer::JavaInstallL()
// ------------------------------------------------------------------------------------------------	

void CApplicationManagementServer::JavaInstallL(CDeploymentComponent &aCompo)
    {
    RDEBUG( "CApplicationManagementServer::JavaInstallL: Start");

    //TRequestStatus s;
    TInt err(KErrNone);
    // set state to install progressf
    CDialogWait* wait = CDialogWait::NewL();
    aCompo.SetStatusNode(EDelivered_InstallProgress);
    TRAP(err,InstallL( aCompo,wait->iStatus ));
    if (err != KErrNone)
        {
        aCompo.SetStatusNode(EDelivered_InstalledFailedWithData);
        delete wait;
        User::Leave(err);
        }
    wait->StartWait();
    if (!CApplicationManagementUtility::iSilentSession && CApplicationManagementUtility::mCurrDlg)
        m_Dlg->closeAMWaitDialog();
    m_Window->lower();
    TRequestStatus s1 = wait->iStatus;
    
    RDEBUG_2( "ApplicationManagementSession: JavaInstallL 1 failed with code %d",
                s1.Int() );

    TInt usifdeplid = -1;
        if (iResults && wait->iStatus.Int()!=KErrCancel)
	  {
            iResults->GetIntByNameL(Usif::KSifOutParam_ComponentId, usifdeplid);
    		RDEBUG_2( "ApplicationManagementSession: JavaInstallL usifdeplid %d",
                usifdeplid);

	  }
      RDEBUG( "wait->StartWait() CApplicationManagementServer::javainstallL Step3");

        aCompo.SetComponentId(usifdeplid);

         s1 = wait->iStatus;
        delete wait;
        RDEBUG_2("ApplicationManagementSession: javainstallL with code %d",
                        s1.Int() );
    if (s1 == KErrCancel)
        {
        RDEBUG_2( "ApplicationManagementSession: JavaInstallL failed with code %d",
                s1.Int() );
        // set state to install failed with data
        aCompo.SetStatusNode(EDelivered_InstalledFailedWithData);
        User::Leave(s1.Int());
        }
    else
        {
        TMidletParameters midletParameters;
        iUtility->RefreshJavaRegistryL();
        //Ignore if any error comes
        TRAPD(err,iUtility->GetInstalledMidletParametersL(midletParameters));
        TRAP(err,CheckforDuplicateMidletsL(midletParameters));

        if (err != KErrNone)
            {
            aCompo.SetStatusNode(EDelivered_InstalledFailedWithData);
            User::Leave(err);
            }
        else
            {
            aCompo.SetStatusNode(EIdle);
            }
        aCompo.SetUid(midletParameters.iMidletUid);

        RDEBUG( "CApplicationManagementServer::JavaInstallL: Step1");
        if (aCompo.ComponentVersion() == KNullDesC8)
            {
            aCompo.SetVersionL(midletParameters.bufVersion);

            }

        TBool status = EFalse;

        //Since "C" drive is not removable

        if (midletParameters.iDrive != EDriveC)
            {
            status = IsInstalledAppRemovableL(midletParameters.iDrive);
            }
        if (status)
            {
            RDEBUG( "IsInstalledAppRemovableL(midletParameters.iDrive); True");
            }
        else
            {
            RDEBUG( "IsInstalledAppRemovableL(midletParameters.iDrive); False");
            }

        //check for status drive
        aCompo.SetAppRemovableStatus(status);
        }

    RDEBUG( "CApplicationManagementServer::JavaInstallL: End");
    }

// ------------------------------------------------------------------------------------------------
// CApplicationManagementServer::SisInstallL()
// ------------------------------------------------------------------------------------------------	

void CApplicationManagementServer::SisInstallL(
        const CDeploymentComponent &aCompo)
    {
    TRequestStatus s;
    CDialogWait* wait = CDialogWait::NewL();
    // set state to install progress
    aCompo.SetStatusNode(EDelivered_InstallProgress);
    TInt err(KErrNone);
    TRAP(err,InstallL( aCompo,wait->iStatus ));
    if (err != KErrNone)
        {
        aCompo.SetStatusNode(EDelivered_InstalledFailedWithData);
        delete wait;
        User::Leave(err);
        }
    RDEBUG_2("ApplicationManagementSession: SisInstallL failed with code before startwait %d",
            wait->iStatus.Int() );
    wait->StartWait();
    RDEBUG_2("ApplicationManagementSession: SisInstallL failed with code after startwait %d",
                wait->iStatus.Int() );
    RDEBUG( "wait->StartWait() CApplicationManagementServer::SisInstallL Step1");
    if (!CApplicationManagementUtility::iSilentSession && CApplicationManagementUtility::mCurrDlg)
        m_Dlg->closeAMWaitDialog();
    delete m_Dlg;
    m_Dlg=NULL;
    RDEBUG( "wait->StartWait() CApplicationManagementServer::SisInstallL Step2");
    
    m_Window->lower();
    
    TInt usifdeplid = -1;
    if (iResults && wait->iStatus.Int()!=KErrCancel)
        iResults->GetIntByNameL(Usif::KSifOutParam_ComponentId, usifdeplid);
    
    RDEBUG( "wait->StartWait() CApplicationManagementServer::SisInstallL Step3");

    iComp->SetComponentId(usifdeplid);

    TRequestStatus s1 = wait->iStatus;
    delete wait;
    RDEBUG_2("ApplicationManagementSession: SisInstallL with code %d",
                    s1.Int() );
    
    if(s1 == KErrCancel)
        {
        s1 = KStatusUserCancelled;
        RDEBUG( "KErrCancel visited operation cancelled");
        mUserCancelled=1;
        }
    if (s1 != KErrNone)
        {
        RDEBUG_2("ApplicationManagementSession: SisInstallL failed with code %d",
                s1.Int() );
        // set state to install failed with data
        aCompo.SetStatusNode(EDelivered_InstalledFailedWithData);
        User::Leave(s1.Int());
        }
    else
        {
        // set state to install sucessful
        aCompo.SetStatusNode(EIdle);
        RDEBUG("ApplicationManagementSession: SisInstallL successfull" );
        }
    RDEBUG( "CApplicationManagementServer::SisInstallL: End");
    }

// ------------------------------------------------------------------------------------------------
// CApplicationManagementServer::DoTheInstallL()
// ---------------------------------------------------- --------------------------------------------	

void CApplicationManagementServer::DoTheInstallL(CDeploymentComponent &aCompo)
    {
    RDEBUG( "CApplicationManagementServer::DoTheInstallL: Start");
    TInt error(KErrNone);
    iComp = &aCompo;
    
    TDeploymentComponentName name = aCompo.ComponentName();
    if(name.Compare(KNullDesC8())==0)
        {
        name = aCompo.UserId();
        }
    TBuf<KDeploymentComponentNameMaxLength> nameBuf;
    nameBuf.Copy(name);
    m_appName = QString::fromUtf16(nameBuf.Ptr(), nameBuf.Length());

    TRequestStatus stat = KErrCompletion;
    if (!aCompo.DriveSelected()
            && !CApplicationManagementUtility::iSilentSession)
        {
        // displaying install confirm notes
        CDialogWait* ao = CDialogWait::NewL();
        AppMgmtNotifier* note = new AppMgmtNotifier();
        int err = KErrNone;
        TRAP( err, QT_TRYCATCH_LEAVING(note->showInstallDialog(iComp, ao->iStatus)));
        ao->StartWait();
        stat = ao->iStatus;
        delete ao;
        m_Window->lower();
        delete note;
        }
    aCompo.SetDriveSelectionStatus(false);
    if (stat == KErrCompletion)
        {
        if (CDeploymentComponent::IsJavaMimeL(aCompo.Data().MimeType())
                || CDeploymentComponent::IsJavaMimeL(
                        aCompo.MetaData().MimeType()))
            {
            TRAP( error, JavaInstallL( aCompo ) );
            }
        else
            {
            TRAP( error, SisInstallL( aCompo ) );
            if(error==KErrNone)
                {
                SetSisAppVersionAndDriveL(aCompo);
                }
            }
      


        if (error == KErrNone)
            {
            InstallCompleteL(aCompo, ETrue);
            CDeploymentComponent *old = DoDataUpdateCheckL(aCompo);
            if (old)
                {
                if (aCompo.State() == EDCSDelivered)
                    {
                    if (old->State() == EDCSActive || old->State()
                            == EDCSInactive)
                        {
                        RDEBUG("AMSession: DoTheInstallL ** deleting old node **" );
                        iStorage->RemoveL(old->InternalId());
                        }
                    }
                }
            iStorage->InstalledL(aCompo);
            }
        else
            {
            InstallCompleteL(aCompo, EFalse);
            User::Leave(error);
            }
        }
    else
        {
        aCompo.SetStatusNode(EDelivered_InstallFailedwithOutData);
        }
    
    RDEBUG( "CApplicationManagementServer::DoTheInstallL: End");
    }

// ------------------------------------------------------------------------------------------------
// CApplicationManagementServer::DoDataUpdateCheckL()
// ------------------------------------------------------------------------------------------------	

CDeploymentComponent *CApplicationManagementServer::DoDataUpdateCheckL(
        CDeploymentComponent &aCompo)
    {
    CDeploymentComponent *old = NULL;
    if (aCompo.Uid() != TUid::Null() && HasUidL(aCompo.Uid(), old, &aCompo))
        {
        // check for Uid component.. Legacy code removed.
        }
    return old;
    }

// ------------------------------------------------------------------------------------------------
// CApplicationManagementServer::SetDataL()
// ------------------------------------------------------------------------------------------------

void CApplicationManagementServer::SetDataL(CDeploymentComponent &aCompo,
        const TDesC8 &aData, const TDesC8 &aMime)
    {
    aCompo.SetDataL(aData, aMime);
    DoDataUpdateCheckL(aCompo);
    }

// ------------------------------------------------------------------------------------------------
// CApplicationManagementServer::FullUpdateL()
// ------------------------------------------------------------------------------------------------

void CApplicationManagementServer::FullUpdateL(const TUint32 sid,
        const TUint32 tid)
    {
    CDeploymentComponent &scompo = iStorage->ComponentL(sid);
    CDeploymentComponent &tcompo = iStorage->ComponentL(tid);
    FullUpdateL(scompo, tcompo);
    }

// ------------------------------------------------------------------------------------------------
// CApplicationManagementServer::FullUpdateL()
// ------------------------------------------------------------------------------------------------

void CApplicationManagementServer::FullUpdateL(CDeploymentComponent &scompo,
        const CDeploymentComponent &tcompo)
    {
    if (scompo.State() == EDCSDelivered)
        {
        if (tcompo.State() == EDCSActive || tcompo.State() == EDCSInactive)
            {
            TRequestStatus s2;
            InstallL(scompo, s2);
            User::WaitForRequest(s2);
            if (s2 != KErrNone)
                {
                RDEBUG_2( "ApplicationManagementSession: FullUpdateL failed with code %d", s2.Int() );
                User::Leave(s2.Int());
                }
            else
                {
                InstallCompleteL(scompo);
                // Before deleting old node ,copy node values which are not set in new node from old node
                if (scompo.ComponentId() == KNullDesC8)
                    scompo.SetIdL(tcompo.ComponentId());

                if (scompo.ComponentName() == KNullDesC8)
                    scompo.SetNameL(tcompo.ComponentName());

                if (scompo.ComponentVersion() == KNullDesC8)
                    scompo.SetVersionL(tcompo.ComponentVersion());

                //Tag tcompo state for deletion. Cant delete now, since nsmldmtree woudnt be updated
                //Remove the const'ness since state needs to be updated.

                iStorage->SetDeploymentComponentState(
                        const_cast<CDeploymentComponent &> (tcompo),
                        EDCSDelete);

                iStorage->InstalledL(scompo);
                RDEBUG( "ApplicationManagementSession: FullUpdateL ok" );
                }
            }
        else
            {
            RDEBUG_2( "ApplicationManagementSession: ERROR FullUpdateL called for illegal state component: %d!", scompo.State() );
            User::Leave(KErrArgument);
            }
        }
    else
        {
        RDEBUG( "ApplicationManagementSession: ERROR FullUpdateL called for illegal state message!" );
        User::Leave(KErrArgument);
        }
    }

// -------------------------------------------------------------------------------------------------------------------
// CApplicationManagementServer::IsDataFileB64EncodedL()
// -------------------------------------------------------------------------------------------------------------------
TBool CApplicationManagementServer::IsDataFileB64EncodedL(RFile &aFile,
        TInt& aLength)
    {
    RDEBUG("CApplicationManagementServer::IsDataFileB64EncodedL()  : start");
    TBool encoded = ETrue; // assume b64 encoded
    TInt bufSize(KBase64BufSize);
    TInt offset(0);
    HBufC8 *dataBuf = HBufC8::NewLC(bufSize);
    TPtr8 dataPtr = dataBuf->Des();
    aFile.Size(aLength);
    aFile.Seek(ESeekStart, offset);

    while (encoded)
        {
        aFile.Read(dataPtr);
        if (!dataPtr.Length())
            {
            break;
            }
        offset = 0;
        aFile.Seek(ESeekCurrent, offset);
        RDEBUG_2("OFFSET: %d", offset);
        encoded = CheckB64Encode(dataPtr);
        }
    RDEBUG_2("Encoded: %d", encoded);
    CleanupStack::PopAndDestroy(dataBuf);
    offset = 0;
    aFile.Seek(ESeekStart, offset);

    RDEBUG("CApplicationManagementServer::IsDataFileB64EncodedL()  : end");
    return encoded;
    }

// -------------------------------------------------------------------------------------------------------------------
// CApplicationManagementServer::CheckB64Encode( const TDesC8& aData )
// -------------------------------------------------------------------------------------------------------------------
TBool CApplicationManagementServer::CheckB64Encode(const TDesC8& aData)
    {
    TInt inputIndex = 0;
    TInt inputLen = aData.Length();

    while (inputIndex < inputLen)
        {
        const TUint8& p = aData[inputIndex];

        if ((p >= 48 && p <= 57) || (p >= 65 && p <= 90) || (p >= 97 && p
                <= 122) || p == 43 || p == 47 || p == 61 || p == 10 || p
                == 13)
            {
            }
        else
            {
            return EFalse;
            }

        inputIndex++;

        }

    return ETrue;
    }

// ------------------------------------------------------------------------------------------------
// CApplicationManagementServer::GetAMServerDownloadDBL()
// ------------------------------------------------------------------------------------------------	

void CApplicationManagementServer::GetAMServerDownloadDBL(TUint32 internalid,
        TInt& finalresult, TDes8& aTargetURI)
    {
    RDEBUG( "CApplicationManagementServer::GetAMServerDownloadDBL(): Step1" );

    TBuf<50> internalidbuf;

    RDEBUG( "CApplicationManagementServer::GetAMServerDownloadDBL(): Step2" );
    internalidbuf.Num(internalid);

    RDEBUG( "CApplicationManagementServer::GetAMServerDownloadDBL(): Step3" );

    RPointerArray<CAMDbItem> aItemArray;

    RDEBUG( "CApplicationManagementServer::GetAMServerDownloadDBL(): Step4" );

    iAMServerDB->GetEntryForLUIDL(aItemArray, internalidbuf);

    RDEBUG( "CApplicationManagementServer::GetAMServerDownloadDBL(): Step5" );

    if (aItemArray.Count() != 0)
        {
        RDEBUG( "CApplicationManagementServer::GetAMServerDownloadDBL(): Step6" );

        finalresult = aItemArray[0]->iResult;

        RDEBUG( "CApplicationManagementServer::GetAMServerDownloadDBL(): Step7" );

        if (finalresult < KErrNone)
            User::Leave(KErrGeneral);

        RDEBUG( "CApplicationManagementServer::GetAMServerDownloadDBL(): Step8" );

        if (*aItemArray[0]->iTargetURI == KNullDesC8())
            User::Leave(KErrGeneral);

        RDEBUG( "CApplicationManagementServer::GetAMServerDownloadDBL(): Step9" );
        aTargetURI.Copy(*aItemArray[0]->iTargetURI);

        }
    RDEBUG( "CApplicationManagementServer::GetAMServerDownloadDBL(): Step10" );
    aItemArray.ResetAndDestroy();

    }

// ------------------------------------------------------------------------------------------------
// CApplicationManagementServer::DeleteGenericAlertForIDL()
// ------------------------------------------------------------------------------------------------	

void CApplicationManagementServer::DeleteGenericAlertForIDL(
        TUint32 internalid)
    {
    iAMServerDB->DeleteFromDatabaseL(internalid);
    }

// -------------------------------------------------------------------------------------------------------------------
// CApplicationManagementServer::DecodeB64DataFileL( RFile& aDataFile, RFile& aDestinationFile )
// -------------------------------------------------------------------------------------------------------------------
void CApplicationManagementServer::DecodeB64DataFileL(RFile& aSourceFile,
        RFile& aDestinationFile)
    {
    RDEBUG( "CApplicationManagementServer::DecodeB64DataFileL(): Start" );
    // Create buffers
    //TInt b64bufferSize = 131072;
    TInt decodedBufferSize = ((KBase64BufSize * 3) / 4 + 16);
    HBufC8 *dataBuf = HBufC8::NewLC(KBase64BufSize);
    TPtr8 dataPtr = dataBuf->Des();
    HBufC8* targetBuf = HBufC8::NewLC(decodedBufferSize);
    TPtr8 targetPtr(targetBuf->Des());

    TInt offset(0);
    TInt length(0);
    TBool reading(ETrue);
    aSourceFile.Size(length);
    RDEBUG_2( "CApplicationManagementServer::DecodeB64DataFileL(): b64 file total size: (%d)", length );
    aSourceFile.Seek(ESeekStart, offset);

    // Initialize decoder
    TImCodecB64 B64Coder;
    B64Coder.Initialise();

    while (reading)
        {
        aSourceFile.Read(dataPtr);
        if (!dataPtr.Length())
            {
            // EOF
            break;
            }
        offset = 0;
        aSourceFile.Seek(ESeekCurrent, offset);
        RDEBUG_2("OFFSET: %d", offset);

        // decode data
        B64Coder.Decode(dataPtr, targetPtr);
        // write decoded data to temporary file
        aDestinationFile.Write(targetPtr);
        }
    CleanupStack::PopAndDestroy(targetBuf);
    CleanupStack::PopAndDestroy(dataBuf);

    offset = 0;
    aDestinationFile.Seek(ESeekStart, offset);
    RDEBUG( "CApplicationManagementServer::DecodeB64DataFileL(): end" );
    }

// -------------------------------------------------------------------------------------------------------------------
// CApplicationManagementServer::DecodeB64DataLC( const TDesC8& aData )
// -------------------------------------------------------------------------------------------------------------------
HBufC8* CApplicationManagementServer::DecodeB64DataLC(const TDesC8 &aData)
    {
    RDEBUG( "CApplicationManagementServer::DecodeB64DataLC(): Start" );
    TImCodecB64 B64Coder;
    B64Coder.Initialise();
    HBufC8 *target = HBufC8::NewLC((aData.Length() * 3) / 4 + 16);
    TPtr8 targetPtr(target->Des());
    B64Coder.Decode(aData, targetPtr);
    RDEBUG( "CApplicationManagementServer::DecodeB64DataLC(): End" );
    return target;
    }

// -------------------------------------------------------------------------------------------------------------------
//  CApplicationManagementServer::EncodeDataL( const TDesC8& aData, CBufBase &aTarget )
// -------------------------------------------------------------------------------------------------------------------
void CApplicationManagementServer::EncodeDataL(const TDesC8& aData,
        CBufBase &aTarget)
    {
    TInt outLen(aData.Length() * 2);
    aTarget.ExpandL(0, outLen);
    TImCodecB64 B64Coder;
    B64Coder.Initialise();
    TPtr8 target(aTarget.Ptr(0));
    TInt decoded(B64Coder.Encode(aData, target));
    TInt s(target.Length());
    aTarget.ResizeL(s);
    aTarget.Compress();
    if (!decoded)
        {
        RDEBUG( "CApplicationManagementServer::EncodeDataL(): Encoding b64 failed?" );
        }
    }

// -------------------------------------------------------------------------------------------------------------------
// CApplicationManagementSession::CApplicationManagementSession
// -------------------------------------------------------------------------------------------------------------------
inline CApplicationManagementSession::CApplicationManagementSession()
    {
    RDEBUG_2( "CApplicationManagementSession::CApplicationManagementSession 0x%x", this);
    }

// -------------------------------------------------------------------------------------------------------------------
// CApplicationManagementSession::CApplicationManagementSession
// -------------------------------------------------------------------------------------------------------------------

inline CApplicationManagementSession::CApplicationManagementSession(
        const TCertInfo &aInfo) :
    iCertInfo(aInfo)
    {
    RDEBUG( "CApplicationManagementSession::CApplicationManagementSession (cert) " );
    }

// -------------------------------------------------------------------------------------------------------------------
// CApplicationManagementSession::Server()
// -------------------------------------------------------------------------------------------------------------------

inline CApplicationManagementServer& CApplicationManagementSession::Server() const
    {
    return *static_cast<CApplicationManagementServer*> (const_cast<CServer2*> (CSession2::Server()));
    }

// -------------------------------------------------------------------------------------------------------------------
// CApplicationManagementSession::CreateL()
// -------------------------------------------------------------------------------------------------------------------
// 2nd phase construct for sessions - called by the CServer framework

void CApplicationManagementSession::CreateL()
    {
    RDEBUG_2( "ApplicationManagementSession: CApplicationManagementSession::CreateL: 0x%x", this );

    Server().AddSession();
    }

// -------------------------------------------------------------------------------------------------------------------
// CApplicationManagementSession::~CApplicationManagementSession()
// -------------------------------------------------------------------------------------------------------------------

CApplicationManagementSession::~CApplicationManagementSession()
    {
    RDEBUG_3("CApplicationManagementSession::~CApplicationManagementSession 0x%X - 0x%X", reinterpret_cast<TUint>(this),
            reinterpret_cast<TUint>(this)+sizeof( CApplicationManagementSession ) );
    RDEBUG_2("CApplicationManagementSession::~CApplicationManagementSession iCertInfo offs 0x%X",
            reinterpret_cast<TUint>(&this->iCertInfo) - reinterpret_cast<TUint>(this) );

    Server().DropSession();
    }

// -------------------------------------------------------------------------------------------------------------------
// CApplicationManagementSession::ServiceL()
// -------------------------------------------------------------------------------------------------------------------

void CApplicationManagementSession::ServiceL(const RMessage2& aMessage)
    {
    if (aMessage.Function() == EPerformRfs)
        {
        TRAPD( err, Server().PerformRFSL() );
        aMessage.Complete(err);
        }
    else
        {
        TBool handled(EFalse);
        TInt err(KErrNone);
        TRAP( err, handled = PreDispatchMessageL( aMessage ) );
        if (!handled)
            {
            TRAP(err,DispatchMessageL(aMessage));
            }

        // Some of the functions contains operations that already complete message
        // doing Complete() twice will cause panic
        if (aMessage.Handle())
            {
            aMessage.Complete(err);
            }
        }
    }

// CApplicationManagementSession::PreDispatchMessageL()
// -------------------------------------------------------------------------------------------------------------------

TBool CApplicationManagementSession::PreDispatchMessageL(
        const RMessage2 &aMessage)
    {
    TBool ret(EFalse);
    // These succeed regardless of certificate....
    switch (aMessage.Function())
        {
        case EGetStateChangingIdsCountOperation:
            {
            // gets count of items list of ids that are potentially changing state
            StateChangeComponentIdsCountL(aMessage);
            ret = ETrue;
            break;
            }
        case EGetStateChangingIdsOperation:
            {
            // gets list of ids that are potentially changing state
            StateChangeComponentIdsL(aMessage);
            ret = ETrue;
            break;
            }
        case ECompleteStateChangeOperation: // completes change of state
            {
            StateChangeCompleteL(aMessage);
            ret = ETrue;
            break;
            }
        default:
            {
            break;
            }
        }
    return ret;
    }

// -------------------------------------------------------------------------------------------------------------------
// CApplicationManagementSession::DispatchMessageL()
// -------------------------------------------------------------------------------------------------------------------

void CApplicationManagementSession::DispatchMessageL(
        const RMessage2& aMessage)
    {
    RDEBUG_2( "CApplicationManagementSession::DispatchMessageL; %d" ,
            aMessage.Function() );

    TBool go(EFalse);
    if (iTrustAdded)
        {
        go = Server().CheckCertL(iCertInfo);
        }
    else
        {
        go = ETrue;
        }

    if (go)
        {
        switch (aMessage.Function())
            {
            case EAddTrust:
                {
                RDEBUG_2( "CApplicationManagementSession::DispatchMessageL; EAddTrust this=0x%x", this );
                AddTrustL(aMessage);
                break;
                }
            case EDeplCompCountOperation:
                {
                ComponentCountL(aMessage);
                return;
                }
            case EDeplCompIdsOperation:
                {
                ComponentIdsL(aMessage);
                return;
                }
            case EDeplCompDeliverCompleteOperation:
                {
                DeliverCompleteL(aMessage);
                break;
                }
            case EDeplCompGetOperation:
                {
                GetComponentL(aMessage);
                break;
                }
            case EInstallOperation:
                {
                InstallL(aMessage);
                break;
                }
            case EDeplCompAttrUpdateOperation:
                {
                UpdateL(aMessage);
                break;
                }
            case EDeplCompGetDataOperation:
                {
                GetDataL(aMessage);
                break;
                }
            case EDeplCompGetDataLengthOperation:
                {
                GetDataLengthL(aMessage);
                break;
                }
            case EDeplCompRemoveOperation:
                {
                RemoveL(aMessage);
                break;
                }
            case EDeplCompDeliverStartOperation2:
                {
                Deliver2L(aMessage);
                break;
                }
            case EDeplCompUpdateDataOperation:
                {
                UpdateDataL(aMessage);
                break;
                }
            case EDeplCompUpdateStreamedDataOperation:
                {
                UpdateStreamedDataL(aMessage);
                break;
                }
            case EDeplCompDownloadOperation:
                {
                DownloadL(aMessage);
                break;
                }
            case EDeplCompUpdateOperation:
                {
                FullUpdateL(aMessage);
                break;
                }
            case EDeplCompGetUserIdOperation:
                {
                GetUserIdL(aMessage);
                break;
                }
            case EDeplCompStartDownloadOperation:
                {
                StartDownloadL(aMessage);
                break;
                }
            case EDeplCompDeactivateOperation:
                {
                DeactivateL(aMessage);
                break;
                }
            case EDeplCompActivateOperation:
                {
                ActivateL(aMessage);
                break;
                }
            case EDeplCompGetTemporaryInstFile:
                {
                GetTemporaryInstFileL(aMessage);
                break;
                }

            case EDeplCompGetFinalResultForGA:
                {
                GetFinalResultForGAL(aMessage);
                break;

                }
            case EDeplCompGenericAlertSent:
                {
                GenericAlertSentForIdL(aMessage);
                break;
                }
            case ECheckStatusNodes:
                {
                CheckStatusNodesValuesL();
                break;
                }

            default:
                {
                User::Leave(KErrNotSupported);
                break;
                }
            }
        }
    else
        {
        RDEBUG_2( "CApplicationManagementSession::DispatchMessageL; Policy prevented action this=0x%x", this );
        User::Leave(KErrAccessDenied);
        }
    }

// -------------------------------------------------------------------------------------------------------------------
// CApplicationManagementSession::GenericAlertSentForIdL()
// -------------------------------------------------------------------------------------------------------------------

void CApplicationManagementSession::GenericAlertSentForIdL(
        const RMessage2& aMessage) const
    {
    TUint32 luid;
    TPckg<TUint32> pk(luid);
    TInt read(aMessage.Read(0, pk));

    Server().DeleteGenericAlertForIDL(luid);

    }

// -------------------------------------------------------------------------------------------------------------------
// CApplicationManagementSession::GetFinalResultForGAL()
// -------------------------------------------------------------------------------------------------------------------

void CApplicationManagementSession::GetFinalResultForGAL(
        const RMessage2& aMessage)
    {

    RDEBUG( "CApplicationManagementSession: GetFinalResultForGAL Step 1" );
    TUint32 luid;
    TPckg<TUint32> pk(luid);

    TInt finalresult = KErrNone;

    RDEBUG( "CApplicationManagementSession: GetFinalResultForGAL Step 2" );

    HBufC8 *targetURI = HBufC8::NewL(512);
    TPtr8 ptr = targetURI->Des();

    RDEBUG( "CApplicationManagementSession: GetFinalResultForGAL Step 3" );

    TInt read(aMessage.Read(0, pk));

    RDEBUG( "CApplicationManagementSession: GetFinalResultForGAL Step 4" );

    Server().GetAMServerDownloadDBL(luid, finalresult, ptr);

    RDEBUG( "CApplicationManagementSession: GetFinalResultForGAL Step 5" );

    RDEBUG_2( "ApplicationManagementServer: GetFinalResultForGAL Count is %d", finalresult );
    TPckg<TInt> p1(finalresult);
    aMessage.WriteL(1, p1);

    RDEBUG( "CApplicationManagementSession: GetFinalResultForGAL Step 6" );
    aMessage.WriteL(2, ptr);

    RDEBUG( "CApplicationManagementSession: GetFinalResultForGAL Step 7" );
    if (targetURI)
        delete targetURI;

    }

// -------------------------------------------------------------------------------------------------------------------
// CApplicationManagementSession::ComponentCountL()
// -------------------------------------------------------------------------------------------------------------------

void CApplicationManagementSession::ComponentCountL(const RMessage2& aMessage)
    {
    RDEBUG( "CApplicationManagementSession: ComponentCountL" );
    TDeploymentComponentState st;
    TPckg<TDeploymentComponentState> pk(st);

    TInt read(aMessage.Read(1, pk));
    RDEBUG_3( "CApplicationManagementSession: ComponentCountL State is %d, read %d", st, read);

    const RComponentIdArray &arrt = Server().Storage()->GetComponentIds();
    RComponentIdArray arr;
    CleanupClosePushL(arr);
    if ((st == EDCSActive) || (st == EDCSInactive))
        {
        ActiveComponentsL(arrt, arr, st);
        }
    else
        {
        TInt count(arrt.Count());
        for (TInt i(0); i < count; i++)
            {
            CDeploymentComponent &compo = Server().Storage()->ComponentL(
                    arrt[i]);
            if (st == EDCSNone || compo.State() == st)
                {
                arr.Append(arrt[i]);
                }
            }
        }
    TInt n(arr.Count());
    CleanupStack::PopAndDestroy(&arr);

    RDEBUG_2( "ApplicationManagementServer: ComponentCountL Count is %d", n );
    TPckg<TInt> p(n);

    aMessage.WriteL(0, p);

    }

// -------------------------------------------------------------------------------------------------------------------
// CApplicationManagementSession::ActiveComponentsL()
// -------------------------------------------------------------------------------------------------------------------

void CApplicationManagementSession::ActiveComponentsL(
        const RComponentIdArray &aArrt, RComponentIdArray &aArr,
        TDeploymentComponentState &aState) const
    {
    Swi::RSisRegistrySession sisses;
    TInt r(sisses.Connect());
    User::LeaveIfError(r);
    CleanupClosePushL(sisses);

    RArray<TUid> uids;
    sisses.InstalledUidsL(uids);
    CleanupClosePushL(uids);
    TInt cont(aArrt.Count());
    TInt uidc(uids.Count());
    RArray<TUid> juids;
    Server().JavaUidsL(juids);
    CleanupClosePushL(juids);
    TInt juidc(juids.Count());
    RComponentIdArray rem;
    CleanupClosePushL(rem);
    for (TInt i(0); i < cont; i++)
        {
        CDeploymentComponent &compo =
                Server().Storage()->ComponentL(aArrt[i]);
        TDeploymentComponentState state = compo.State();
        if (state == aState)
            {
            TBool add(EFalse);
            const TUid &uid(compo.Uid());
            for (TInt j(0); add == EFalse && j < uidc; j++)
                {

                if (uids[j] == uid)
                    {
                    RDEBUG( "CApplicationManagementSession::ActiveComponentsL Entered" );

                    // Check if this registry entry is present
                    Swi::RSisRegistrySession sisSession;
                    User::LeaveIfError(sisSession.Connect());
                    CleanupClosePushL(sisSession);

                    Swi::RSisRegistryEntry sisEntry;

                    //Opens the base package entry by specifying a UID. 
                    User::LeaveIfError(sisEntry.Open(sisSession, compo.Uid()));
                    CleanupClosePushL(sisEntry);

                    add = sisEntry.IsPresentL();

                    if (add)
                        {
                        // If Present update the Version, name and other info to that deployement component. Since application would have got an update
                        // by some other means
                        TVersion version = sisEntry.VersionL();
                        TBuf8<KVersionLength> pkgDes;
                        pkgDes.AppendNum(version.iMajor);
                        pkgDes.Append(KLiteralPeriod);
                        pkgDes.AppendNum(version.iMinor);

                        HBufC *packagename = sisEntry.PackageNameL();
                        TBuf8<512> packagename8;
                        packagename8.Copy(*packagename);
                        delete packagename;

                        compo.SetNameL(packagename8);
                        compo.SetVersionL(pkgDes);

                        Server().Storage()->UpdateL(compo);

                        RDEBUG( "CApplicationManagementSession::ActiveComponentsL ETrue" );

                        }
                    else
                        RDEBUG( "CApplicationManagementSession::ActiveComponentsL EFalse" );
                    CleanupStack::PopAndDestroy(2, &sisSession);
                    }
                }
            if (!add) // go throug java suites
                {
                for (TInt k(0); add == EFalse && k < juidc; k++)
                    {
                    if (juids[k] == uid)
                        {
                        add = ETrue;
                        }
                    }
                }
            if (add)
                {
                aArr.Append(aArrt[i]);
                }
            else
                {
                RDEBUG8_3( "ApplicationManagementServer: ActiveComponentCountL skipping 0x%x, %S",
                        compo.Uid() ,&compo.ComponentId() );
                rem.Append(aArrt[i]);
                }
            }
        }
    TInt rc(rem.Count());
    for (TInt t(0); t < rc; t++)
        {
        RDEBUG_2( "ApplicationManagementServer: ActiveComponentCountL removing non existing %d", rem[t] );
        CDeploymentComponent &compo = Server().Storage()->ComponentL(rem[t]);
        if (!compo.AppRemovable())
            Server().RemoveInternalL(compo, EFalse);
        }
    CleanupStack::PopAndDestroy(&rem);
    CleanupStack::PopAndDestroy(&juids);
    CleanupStack::PopAndDestroy(&uids);
    CleanupStack::PopAndDestroy(&sisses);
    }

// -------------------------------------------------------------------------------------------------------------------
// CApplicationManagementSession::ComponentIdsL()
// -------------------------------------------------------------------------------------------------------------------

void CApplicationManagementSession::ComponentIdsL(const RMessage2& aMessage) const
    {
    RDEBUG( "CApplicationManagementSession: ComponentIdsL" );
    TDeploymentComponentState st;
    TPckg<TDeploymentComponentState> pk(st);

    TInt read(aMessage.Read(1, pk));
    RDEBUG_3( "CApplicationManagementSession: ComponentIdsL State is %d, read %d", st, read);

    const RComponentIdArray &arrt = Server().Storage()->GetComponentIds();
    TInt buflen(0);
    TUint32* buf = NULL;
    if (st == EDCSNone)
        {
        buflen = arrt.Count();
        buf = new (ELeave) TUint32[buflen];
        CleanupArrayDeletePushL(buf);
        for (TInt i(0); i < buflen; i++)
            {
            RDEBUG_2("ApplicationManagementSession: ComponentIdsL Adding %d!!", arrt[i]);
            buf[i] = arrt[i];
            }
        }
    else
        {
        RComponentIdArray arr;
        if (st == EDCSActive)
            {
            ActiveComponentsL(arrt, arr, st);
            }
        else
            {
            TInt artc(arrt.Count());
            for (TInt i(0); i < artc; i++)
                {
                CDeploymentComponent &compo = Server().Storage()->ComponentL(
                        arrt[i]);
                if (compo.State() == st)
                    {
                    arr.Append(arrt[i]);
                    }
                }
            }
        buflen = arr.Count();
        buf = new (ELeave) TUint32[buflen];
        CleanupArrayDeletePushL(buf);

        TUint32 len(aMessage.GetDesMaxLength(0) / 4);
        if (buflen == len)
            {
            RDEBUG( "CApplicationManagementSession: ComponentIdsL Right count!!" );
            }
        else
            {
            RDEBUG_3( "CApplicationManagementSession: ComponentIdsL Wrong count (is %d, should be %d)!!", len, buflen);
            if (len < buflen)
                {
                buflen = len;
                }
            }
        for (TInt i(0); i < buflen; i++)
            {
            RDEBUG_2( "CApplicationManagementSession: ComponentIdsL Adding %d!!",
                    arr[i] );
            buf[i] = arr[i];
            }
        arr.Close();
        }

    TPtrC8 p((TUint8*) buf, buflen * sizeof(TUint32));
    aMessage.WriteL(0, p);

    CleanupStack::PopAndDestroy(buf);
    }

#ifdef _DEBUG
HBufC* Des8to16LC(const TDesC8 &aDes)
    {
    HBufC *b = HBufC::NewLC(aDes.Length());
    TPtr p(b->Des());
    p.Copy(aDes);
    return b;
    }

HBufC* ToStringLC(TDeploymentComponent &aComp)
    {
    HBufC *b = HBufC::NewLC(aComp.iId.Length() + aComp.iName.Length()
            + aComp.iVersion.Length() + 10 + 50);
    TPtr p(b->Des());
    _LIT( KFormat, "Id: '%S', Name: '%S', Version: '%S', State: %d, OldState: %d");
    p.Format(KFormat, &*Des8to16LC(aComp.iId), &*Des8to16LC(aComp.iName),
            &*Des8to16LC(aComp.iVersion), aComp.iState, aComp.iOldState);
    CleanupStack::PopAndDestroy(3);
    return b;
    }
#endif

// -------------------------------------------------------------------------------------------------------------------
// CApplicationManagementSession::Deliver2L()
// -------------------------------------------------------------------------------------------------------------------

void CApplicationManagementSession::Deliver2L(const RMessage2& aMessage) const
    {
    RDEBUG( "ApplicationManagementSession: Deliver2L" );

    TDeploymentComponentIPC comp;
    TPckg<TDeploymentComponentIPC> pcomp(comp);
    TInt read2(aMessage.Read(0, pcomp));

    HBufC8 *data = HBufC8::NewLC(aMessage.GetDesLength(1));
    TPtr8 pbuf(data->Des());
    TInt read(aMessage.Read(1, pbuf));

    CDeploymentComponent *compo = NULL;
    if (iTrustAdded)
        {
        compo = Server().Storage()->NewComponentL(EDCSDelivered,
                comp.iUserId, &iCertInfo);
        }
    else
        {
        compo
                = Server().Storage()->NewComponentL(EDCSDelivered,
                        comp.iUserId);
        }

    compo->Set(comp.iComponent);
    compo->Data().SetDataL(pbuf, comp.iMimeType);
    Server().Storage()->UpdateL(*compo);
    compo->CopyStateTo(comp.iComponent);

#ifdef _DEBUG
    RDEBUG_2( "Deliver2L: TDeploymentComponent is '%S'",
            ToStringLC( comp.iComponent ) );
    CleanupStack::PopAndDestroy();
#endif

    TPckg<TDeploymentComponent> pcomp2(comp.iComponent);
    aMessage.Write(0, pcomp2);
    CleanupStack::PopAndDestroy(data);
    TInt n(compo->InternalId());
    TPckg<TUint32> p(n);
    aMessage.WriteL(2, p);
    }

// -------------------------------------------------------------------------------------------------------------------
// CApplicationManagementSession::DeliverCompleteL()
// -------------------------------------------------------------------------------------------------------------------

void CApplicationManagementSession::DeliverCompleteL(
        const RMessage2& aMessage) const
    {
    RDEBUG( "ApplicationManagementSession: DeliverCompleteL" );
    TUint32 id;
    TPckg<TUint32> pid(id);

    TInt read(aMessage.Read(0, pid));
    RDEBUG_2( "ApplicationManagementSession: DeliverCompleteL id is %d", id );

    CDeploymentComponent &compo = Server().Storage()->ComponentL(id);

    HBufC8 *opts = HBufC8::NewLC(aMessage.GetDesLength(1));
    TPtr8 popts(opts->Des());
    TInt read1(aMessage.Read(1, popts));
    compo.SetInstallOptsL(*opts);

    HBufC8 *meta = HBufC8::NewLC(aMessage.GetDesLength(2));
    TPtr8 pmeta(meta->Des());
    TInt read2(aMessage.Read(2, pmeta));
    compo.SetMetaDataL(*meta, KNullDesC8);

    CleanupStack::PopAndDestroy(meta);
    CleanupStack::PopAndDestroy(opts);
    Server().Storage()->UpdateL(compo);
    }

// -------------------------------------------------------------------------------------------------------------------
// CApplicationManagementSession::GetComponentL()
// -------------------------------------------------------------------------------------------------------------------

void CApplicationManagementSession::GetComponentL(const RMessage2& aMessage) const
    {
    RDEBUG( "ApplicationManagementSession: GetComponentL" );
    TUint32 id;
    TPckg<TUint32> pid(id);
    TInt read(aMessage.Read(0, pid));

    RDEBUG_2( "ApplicationManagementSession: GetComponentL id is %d", id );
    CDeploymentComponent &compo = Server().Storage()->ComponentL(id);

    TDeploymentComponent comp;
    compo.CopyStateTo(comp);

    TPckg<TDeploymentComponent> pcomp(comp);
    aMessage.Write(1, pcomp);
    }

// -------------------------------------------------------------------------------------------------------------------
// CApplicationManagementSession::StartDownloadL()
// -------------------------------------------------------------------------------------------------------------------

void CApplicationManagementSession::StartDownloadL(const RMessage2& aMessage) const
    {
    RDEBUG( "ApplicationManagementSession: StartDownloadL" );
    TUint32 id = 0;
    TPckg<TUint32> pid(id);
    TInt read(aMessage.Read(0, pid));

    TPckgBuf<TDownloadTarget> target;
    read = aMessage.Read(1, target);
    RDEBUG_2( "ApplicationManagementSession: StartDownloadL id is %d", id );

    CDeploymentComponent &compo = Server().Storage()->ComponentL(id);
    compo.SetDownloadTarget(target());
    Server().AddDownloadL(&compo);
    }

// -------------------------------------------------------------------------------------------------------------------
// CApplicationManagementSession::DeactivateL()
// -------------------------------------------------------------------------------------------------------------------

void CApplicationManagementSession::DeactivateL(const RMessage2& aMessage) const
    {
    RDEBUG( "ApplicationManagementSession: DeactivateL" );
    TUint32 id = 0;
    TPckg<TUint32> pid(id);
    TInt read(aMessage.Read(0, pid));

    RDEBUG_2( "ApplicationManagementSession: DeactivateL id is %d", id );
    CDeploymentComponent &compo = Server().Storage()->ComponentL(id);
    Server().Storage()->DeactivateL(compo);
    }

// -------------------------------------------------------------------------------------------------------------------
// CApplicationManagementSession::ActivateL()
// -------------------------------------------------------------------------------------------------------------------

void CApplicationManagementSession::ActivateL(const RMessage2& aMessage) const
    {
    RDEBUG( "ApplicationManagementSession: ActivateL" );
    TUint32 id;
    TPckg<TUint32> pid(id);
    TInt read(aMessage.Read(0, pid));

    RDEBUG_2( "ApplicationManagementSession: ActivateL id is %d", id );
    CDeploymentComponent &compo = Server().Storage()->ComponentL(id);
    Server().Storage()->ActivateL(compo);
    }

// -------------------------------------------------------------------------------------------------------------------
// CApplicationManagementSession::GetUserIdL()
// -------------------------------------------------------------------------------------------------------------------

void CApplicationManagementSession::GetUserIdL(const RMessage2& aMessage) const
    {
    RDEBUG( "ApplicationManagementSession: GetUserIdL" );
    TUint32 id = 0;
    TPckg<TUint32> pid(id);
    TInt read(aMessage.Read(0, pid));

    RDEBUG_2( "ApplicationManagementSession: GetUserIdL id is %d", id );
    CDeploymentComponent &compo = Server().Storage()->ComponentL(id);

    aMessage.Write(1, compo.UserId());
    }

// -------------------------------------------------------------------------------------------------------------------
// CApplicationManagementSession::GetTemporaryInstFileL()
// -------------------------------------------------------------------------------------------------------------------

void CApplicationManagementSession::GetTemporaryInstFileL(
        const RMessage2& aMessage) const
    {
    // TIpcArgs( &id, &itemid, &filehandle )
    RDEBUG( "ApplicationManagementSession: GetTemporaryInstFileL" );
    TUint32 id;
    TPckg<TUint32> pid(id);
    TInt read(aMessage.Read(0, pid));

    TDeplCompAttrType item;
    TPckg<TDeplCompAttrType> itemid(item);
    read = aMessage.Read(1, itemid);

    RDEBUG_2( "ApplicationManagementSession: GetTemporaryInstFileL id is %d", id );
    CDeploymentComponent &compo = Server().Storage()->ComponentL(id);

    TFileName fileName;

    if (item == EDCData)
        {
        fileName.Copy(compo.Data().DataFileName());
        }
    else if (item == EDCMetaData)
        {
        fileName.Copy(compo.MetaData().DataFileName());
        }
    else
        {
        RDEBUG_2( "ApplicationManagementSession: GetTemporaryInstFileL: Not supported type: (%d)", item);
        User::Leave(KErrNotSupported);
        }

    RDEBUG_2( "ApplicationManagementSession: GetTemporaryInstFileL: (%S)", &fileName);

    RFs fs;
    User::LeaveIfError(fs.Connect());
    User::LeaveIfError(fs.ShareProtected());
    CleanupClosePushL(fs);
    RFile file;
    User::LeaveIfError(file.Open(fs, fileName, EFileWrite));
    CleanupClosePushL(file);

    // Takes care of completing with session (RFs), 
    // SendReceieve returns session handle at client side
    User::LeaveIfError(file.TransferToClient(aMessage, 2));
    CleanupStack::PopAndDestroy(2, &fs); // fs , file	
    }

// -------------------------------------------------------------------------------------------------------------------
// CApplicationManagementSession::PackageSizeL()
// -------------------------------------------------------------------------------------------------------------------

TInt CApplicationManagementSession::PackageSizeL(
        Swi::RSisRegistrySession& aSession, Swi::RSisRegistryEntry& aEntry)
    {
    RPointerArray<Swi::CSisRegistryPackage> listPackages;
    TInt size(0);
    TInt err(0);
    CleanupClosePushL(listPackages);

    aEntry.AugmentationsL(listPackages);

    for (TInt index(0); index < listPackages.Count(); index++)
        {
        TRAP(err, AugmentationSizeL( aSession, *listPackages[index]))
        if (err != KErrNone)
            {
            break;
            }
        }

    if (err != KErrNone)
        {
        size = 0;
        }
    else
        {
        TRAP(err, size += aEntry.SizeL() );
        }

    listPackages.ResetAndDestroy();
    CleanupStack::PopAndDestroy(&listPackages);

    if (err != KErrNone)
        {
        //We leave if there was an error, 
        // but we have to clean up the listPackages before the leave.
        User::Leave(err);
        }

    return size;
    }

// ------------------------------------------------------------------------------------------------
// CAppMngrSisxInfo::AugmentationSizeL()
// Returns the Size of the package
// (other items were commented in a header).
// ------------------------------------------------------------------------------------------------
//  
TInt CApplicationManagementSession::AugmentationSizeL(
        Swi::RSisRegistrySession& aSession,
        const Swi::CSisRegistryPackage& aPackage)
    {
    TInt size(0);
    Swi::RSisRegistryEntry regEntry;
    CleanupClosePushL(regEntry);

    regEntry.OpenL(aSession, aPackage);
    size += PackageSizeL(aSession, regEntry);

    CleanupStack::PopAndDestroy(&regEntry);

    return size;
    }

// -------------------------------------------------------------------------------------------------------------------
// CApplicationManagementSession::LookupSisRegistryL()
// -------------------------------------------------------------------------------------------------------------------

void CApplicationManagementSession::LookupSisRegistryL()
    {
    Swi::RSisRegistrySession sisses;
    TInt r(sisses.Connect());
    RPointerArray<Swi::CSisRegistryPackage> pkgs(12);
    sisses.InstalledPackagesL(pkgs);

    TInt count(pkgs.Count());
    for (TInt i(0); i < count; i++)
        {
        Swi::CSisRegistryPackage* p = pkgs[i];

        RDEBUG_5(" Found package: %d, '%S', uid: 0x%X, size: %d" ,
                i, &p->Name(), p->Uid().iUid, AugmentationSizeL( sisses, *p ) );
        }

    sisses.Close();
    pkgs.ResetAndDestroy();

    }

// -------------------------------------------------------------------------------------------------------------------
// CApplicationManagementSession::InstallL()
// -------------------------------------------------------------------------------------------------------------------

void CApplicationManagementSession::InstallL(const RMessage2& aMessage) const
    {
    RDEBUG( "ApplicationManagementSession: InstallL" );
    TUint32 id;
    TPckg<TUint32> pid(id);
    TInt read(aMessage.Read(0, pid));

    TDeploymentComponentState targetst;
    TPckg<TDeploymentComponentState> pk(targetst);

    read = aMessage.Read(1, pk);

    RDEBUG_3( "ApplicationManagementSession: InstallL id is %d, targetstate %d",
            id, targetst );

    CDeploymentComponent &compo = Server().Storage()->ComponentL(id);
    if (compo.State() == EDCSDelivered)
        {
        Server().DoTheInstallL(compo);
        }
    else
        {
        User::Leave(KErrArgument);
        }
    RDEBUG_2( "ApplicationManagementSession: InstallL success. New compo state (%d)", compo.State());
    }

// -------------------------------------------------------------------------------------------------------------------
// CApplicationManagementSession::UpdateDataL()
// -------------------------------------------------------------------------------------------------------------------

void CApplicationManagementSession::UpdateDataL(const RMessage2& aMessage) const
    {
    RDEBUG( "ApplicationManagementSession: UpdateDataL" );

    TDeplCompDataIPC ipc;
    TPckg<TDeplCompDataIPC> pipc(ipc);
    TInt read(aMessage.Read(0, pipc));
    RDEBUG_2( "ApplicationManagementSession: UpdateDataL id is %d", ipc.iInternalId );

    HBufC8 *buf = HBufC8::NewLC(aMessage.GetDesLength(1));
    TPtr8 pbuf(buf->Des());
    TInt read2(aMessage.Read(1, pbuf));

    CDeploymentComponent &compo = Server().Storage()->ComponentL(
            ipc.iInternalId);

    if (Server().CheckB64Encode(pbuf))
        {
        RDEBUG( "ApplicationManagementSession: UpdateDataL data is base64 encoded");
        // b64 encoded data, decode it 
        HBufC8* tmp;
        tmp = Server().DecodeB64DataLC(pbuf);
        pbuf.Copy(*tmp);
        pbuf = buf->Des();
        CleanupStack::PopAndDestroy(tmp);
        }

    if (compo.State() == EDCSDelivered)
        {
        if (ipc.iAttrType == EDCData)
            {
            compo.SetDataL(pbuf, ipc.iDataMime);
            }
        else if (ipc.iAttrType == EDCMetaData)
            {
            compo.SetMetaDataL(pbuf, ipc.iDataMime);
            }
        else
            {
            RDEBUG_2( "ApplicationManagementSession::UpdateDataL -  ERROR UpdateDataL called for illegal item: %d!", ipc.iAttrType );
            User::Leave(KErrArgument);
            }

        Server().Storage()->UpdateL(compo);
        }
    else
        {
        RDEBUG( "ApplicationManagementSession::UpdateDataL -  ERROR UpdateDataL called for illegal state message!" );
        User::Leave(KErrArgument);
        }
    CleanupStack::PopAndDestroy(buf);
    }

// -------------------------------------------------------------------------------------------------------------------
// CApplicationManagementSession::UpdateStreamedDataL()
// -------------------------------------------------------------------------------------------------------------------

void CApplicationManagementSession::UpdateStreamedDataL(
        const RMessage2& aMessage) const
    {
    RDEBUG( "ApplicationManagementSession: UpdateStreamedDataL() Start");
    TDeplCompDataIPC ipc;
    TPckg<TDeplCompDataIPC> pipc(ipc);
    TInt read(aMessage.Read(0, pipc));
    RDEBUG_2( "ApplicationManagementSession: UpdateStreamedDataL id is %d", ipc.iInternalId );

    CDeploymentComponent &compo = Server().Storage()->ComponentL(
            ipc.iInternalId);

    if (compo.State() == EDCSDelivered)
        {
        RFs fs;
        User::LeaveIfError(fs.Connect());
        CleanupClosePushL(fs);
        RFile file;
        TFileName dataFileName;
        TInt length(0);

        if (ipc.iAttrType == EDCData)
            {
            dataFileName.Copy(compo.Data().DataFileName());
            }
        else if (ipc.iAttrType == EDCMetaData)
            {
            dataFileName.Copy(compo.MetaData().DataFileName());
            }
        else
            {
            RDEBUG_2( "ApplicationManagementSession::UpdateStreamedDataL -  ERROR called for illegal item: %d!", ipc.iAttrType );
            User::Leave(KErrArgument);
            }

        RDEBUG_2( "ApplicationManagementSession::UpdateStreamedDataL(): using dataFile: (%S)", &dataFileName );

        // EFileWrite So we can delete the original decoded data file
        User::LeaveIfError(file.Open(fs, dataFileName, EFileWrite));
        CleanupClosePushL(file);
        if (Server().IsDataFileB64EncodedL(file, length))
            {
            RDEBUG( "CApplicationManagementServer::UpdateStreamedDataL(): data is decoded" );

            TFileName tempFileName;
            TBuf<30> privatePath;
            fs.PrivatePath(privatePath);
            RFile tempFile;
            TInt err(KErrNone);
            err = tempFile.Temp(fs, privatePath, tempFileName, EFileWrite);
            if (err == KErrNone)
                {
                RDEBUG_2( "ApplicationManagementSession::UpdateStreamedDataL(): created tempFile: (%S)", &tempFileName );
                }
            else
                {
                User::Leave(err);
                }
            CleanupClosePushL(tempFile);

            TRAP( err, Server().DecodeB64DataFileL( file, tempFile ) );
            if (err != KErrNone)
                {
                RDEBUG_2( "ApplicationManagementSession::UpdateStreamedDataL -  failed to decode datafile: %d", err );
                CleanupStack::PopAndDestroy(2, &file);
                compo.ResetDataL(fs); // Remove both files if exists
                User::Leave(err);
                }
            else
                {
                // close both files, delete decode file and rename temp file to correct one
                CleanupStack::PopAndDestroy(2, &file);
                fs.Delete(dataFileName);
                err = fs.Rename(tempFileName, dataFileName);
                RDEBUG_2( "ApplicationManagementSession::UpdateStreamedDataL -  successfully decoded datafile: %d", err );
                }
            CleanupStack::PopAndDestroy(&fs);
            }
        else
            {
            RDEBUG( "ApplicationManagementSession::UpdateStreamedDataL(): data is NOT decoded" );
            CleanupStack::PopAndDestroy(2, &fs);
            }

        if (ipc.iAttrType == EDCData)
            {
            compo.SetDataL(ipc.iDataMime);
            }
        else
            {
            compo.SetMetaDataL(ipc.iDataMime);
            }

        Server().Storage()->UpdateL(compo);
        }
    else
        {
        RDEBUG( "ApplicationManagementSession::UpdateStreamedDataL -  ERROR called for illegal state message!" );
        User::Leave(KErrArgument);
        }
    RDEBUG( "ApplicationManagementSession: UpdateStreamedDataL() End");
    }

// -------------------------------------------------------------------------------------------------------------------
// CApplicationManagementSession::UpdateL()
// -------------------------------------------------------------------------------------------------------------------

void CApplicationManagementSession::UpdateL(const RMessage2& aMessage) const
    {
    RDEBUG( "ApplicationManagementSession: UpdateL" );

    TUint32 id;
    TPckg<TUint32> pid(id);
    TInt read(aMessage.Read(0, pid));
    RDEBUG_2("ApplicationManagementSession: UpdateL id is %d", id );

    TDeplCompAttrType item;
    TPckg<TDeplCompAttrType> itemid(item);
    TInt read1(aMessage.Read(1, itemid));
    HBufC8 *buf = HBufC8::NewLC(aMessage.GetDesLength(2));
    TPtr8 pbuf(buf->Des());
    TInt read2(aMessage.Read(2, pbuf));

    CDeploymentComponent &c = Server().Storage()->ComponentL(id);
    TDeploymentComponentState st(c.State());
    if (st == EDCSDelivered || st == EDCSDownload)
        {
        switch (item)
            {
            case EDCId:
                {
                c.SetIdL(pbuf);
                break;
                }
            case EDCName:
                {
                c.SetNameL(pbuf);
                break;
                }
            case EDCVersion:
                {
                c.SetVersionL(pbuf);
                break;
                }
            case EDCDescriptionRef:
                {
                c.SetDescriptionL(pbuf);
                break;
                }
            case EDCInstallOptions:
                {
                c.SetInstallOptsL(pbuf);
                break;
                }
            case EDCMetaData:
                {
                c.SetMetaDataL(pbuf, KNullDesC8);
                break;
                }
            case EDCPkgID:
                {
                c.SetPkgIDL(pbuf);
                break;
                }
            case EDCPkgType:
                {
                c.SetPkgTypeL(pbuf);
                break;
                }

            default:
                {
                if (st == EDCSDelivered)
                    {
                    switch (item)
                        {
                        case EDCData:
                            {
                            c.SetDataL(pbuf, KNullDesC8);
                            if (c.State() == EDCSDownload)
                                {
                                //	Server().StartDownload( *compo );
                                }
                            break;
                            }
                        default:
                            {
                            RDEBUG_2("ApplicationManagementSession: ERROR UpdateL called for illegal item type!!!! %d", item );
                            User::Leave(KErrArgument);
                            }
                        }
                    }
                else if (st == EDCSDownload)
                    {
                    switch (item)
                        {
                        case EDCId:
                            {
                            c.SetIdL(pbuf);
                            break;
                            }
                        case EDCName:
                            {
                            c.SetNameL(pbuf);
                            break;
                            }
                        case EDCVersion:
                            {
                            c.SetVersionL(pbuf);
                            break;
                            }
                        case EDCDownloadURI:
                            {
                            c.SetDownloadURIL(pbuf);
                            break;
                            }
                        case EDCConRef:
                            {
                            TPckgBuf<TInt> aInt;
                            aInt.Copy(pbuf);
                            c.SetIAPL(aInt());
                            break;
                            }
                        default:
                            {
                            RDEBUG_2( "ApplicationManagementSession: ERROR UpdateL called for illegal item type!!!! %d", item );
                            User::Leave(KErrArgument);
                            break;
                            }
                        }
                    }
                break;
                }
            }
        Server().Storage()->UpdateL(c);
        }
    else if (st == EDCSActive || st == EDCSInactive)
        {
        switch (item)
            {
            case EDCName:
                {
                c.SetNameL(pbuf);
                break;
                }
            case EDCVersion:
                {
                c.SetVersionL(pbuf);
                break;
                }
            case EDCDescriptionRef:
                {
                c.SetDescriptionL(pbuf);
                break;
                }
            default:
                {
                RDEBUG_2( "ApplicationManagementSession: ERROR UpdateL called for illegal item type!!!! %d", item );
                User::Leave(KErrArgument);
                break;
                }
            }
        Server().Storage()->UpdateL(c);
        }
    else
        {
        RDEBUG( "ApplicationManagementSession: ERROR UpdateL called for illegal state message!" );
        User::Leave(KErrArgument);
        }
    CleanupStack::PopAndDestroy(buf);
    }

// -------------------------------------------------------------------------------------------------------------------
// CApplicationManagementSession::GetDataLengthL()
// -------------------------------------------------------------------------------------------------------------------

void CApplicationManagementSession::GetDataLengthL(const RMessage2& aMessage) const
    {
    RDEBUG( "ApplicationManagementSession: GetDataLengthL" );
    TUint32 id;
    TPckg<TUint32> pid(id);

    TInt read(aMessage.Read(0, pid));

    TInt length(0);

    TDeplCompAttrType dt;
    TPckg<TDeplCompAttrType> pdt(dt);
    read = aMessage.Read(1, pdt);
    CDeploymentComponent &compo = Server().Storage()->ComponentL(id);
    if (compo.State() == EDCSDelivered)
        {
        if (dt == EDCInstallOptions)
            {
            if (compo.InstallOptsSet())
                {
                length = sizeof(TAMInstallOptions);
                }
            }
        else if (dt == EDCData)
            {
            length = compo.Data().DataLengthL();
            }
        else if (dt == EDCDataMime)
            {
            length = compo.Data().MimeType().Length();
            }
        else if (dt == EDCMetaData)
            {
            length = compo.MetaData().DataLengthL();
            }
        else if (dt == EDCMetaDataMime)
            {
            length = compo.MetaData().MimeType().Length();
            }
        else
            {
            RDEBUG_2( "ApplicationManagementSession: GetDataLengthL ERROR called with illegal type %d", dt );
            User::Leave(KErrArgument);
            }
        }
    else if (compo.State() == EDCSDownload)
        {
        if (dt == EDCDownloadURI)
            {
            length = compo.DownloadURI().Length();
            }
        else if (dt == EDCInstallOptions)
            {
            if (compo.InstallOptsSet())
                {
                length = sizeof(TAMInstallOptions);
                }
            }
        else
            {
            RDEBUG_2( "ApplicationManagementSession: GetDataLengthL ERROR Download / called with illegal state component id %d",
                    compo.State() );
            User::Leave(KErrArgument);
            }
        }
    else
        {
        RDEBUG_2( "ApplicationManagementSession: GetDataLengthL ERROR called with illegal state component id %d",
                compo.State() );
        User::Leave(KErrArgument);
        }

    TPckg<TInt> pcomp(length);
    aMessage.Write(2, pcomp);
    }

// -------------------------------------------------------------------------------------------------------------------
// CApplicationManagementSession::GetDataL()
// -------------------------------------------------------------------------------------------------------------------

void CApplicationManagementSession::GetDataL(const RMessage2& aMessage) const
    {
    RDEBUG( "ApplicationManagementSession: GetDataL" );
    TUint32 id;
    TPckg<TUint32> pid(id);

    TInt read(aMessage.Read(0, pid));

    TDeplCompAttrType dt;
    TPckg<TDeplCompAttrType> pdt(dt);
    read = aMessage.Read(1, pdt);

    RDEBUG_3( "ApplicationManagementSession: GetDataL id is %d, and attribute %d", id, dt );
    CDeploymentComponent &compo = Server().Storage()->ComponentL(id);
    if (dt == EDCStatus)
        {
        TPckg<TInt> p(compo.Status());
        aMessage.Write(2, p);
        }
    else if (compo.State() == EDCSDelivered)
        {
        if (dt == EDCInstallOptions)
            {
            if (compo.InstallOptsSet())
                {
                TAMInstallOptionsPckgBuf pckg(compo.InstallOpts());
                aMessage.Write(2, pckg);
                }
            else
                {
                aMessage.Write(2, KNullDesC8);
                }
            }
        else if (dt == EDCData)
            {
            aMessage.Write(2, compo.Data().Data());
            }
        else if (dt == EDCDataMime)
            {
            aMessage.Write(2, compo.Data().MimeType());
            }
        else if (dt == EDCMetaData)
            {
            aMessage.Write(2, compo.MetaData().Data());
            }
        else if (dt == EDCMetaDataMime)
            {
            aMessage.Write(2, compo.MetaData().MimeType());
            }
        else
            {
            RDEBUG_2( "ApplicationManagementSession: GetDataL ERROR called with illegal type %d", dt );
            User::Leave(KErrArgument);
            }
        }
    else if (compo.State() == EDCSDownload)
        {

        if (dt == EDCDownloadURI)
            {
            aMessage.Write(2, compo.DownloadURI());
            }

        else if (dt == EDCInstallOptions)
            {
            if (compo.InstallOptsSet())
                {
                TAMInstallOptionsPckgBuf pckg(compo.InstallOpts());
                aMessage.Write(2, pckg);
                }
            else
                {
                aMessage.Write(2, KNullDesC8);
                }
            }
        else if (dt == EDCConRef)
            {
            TPckg<TInt> p(compo.GetIAP());
            aMessage.Write(2, p);
            }
        else
            {
            RDEBUG_2( "ApplicationManagementSession:GetDataL ERROR called with illegal type %d", dt );
            User::Leave(KErrArgument);
            }
        }
    else
        {
        RDEBUG_2("ApplicationManagementSession: GetDataL ERROR called with illegal state component id %d", compo.State() );
        User::Leave(KErrArgument);
        }
    }

// -------------------------------------------------------------------------------------------------------------------
// CApplicationManagementSession::RemoveL()
// -------------------------------------------------------------------------------------------------------------------

void CApplicationManagementSession::RemoveL(const RMessage2& aMessage) const
    {
    RDEBUG( "ApplicationManagementSession: RemoveL" );
    TUint32 id;
    TPckg<TUint32> pid(id);
    TInt read(aMessage.Read(0, pid));
    RDEBUG_2( "ApplicationManagementSession: RemoveL id is %d", id );
    RemoveInternalL(id);
    }

// -------------------------------------------------------------------------------------------------------------------
// CApplicationManagementSession::RemoveInternalL()
// -------------------------------------------------------------------------------------------------------------------

void CApplicationManagementSession::RemoveInternalL(const TUint32 &aId) const
    {
    CDeploymentComponent &compo = Server().Storage()->ComponentL(aId);

    Server().RemoveInternalL(compo);
    }

// -------------------------------------------------------------------------------------------------------------------
// CApplicationManagementSession::DownloadL()
// -------------------------------------------------------------------------------------------------------------------

void CApplicationManagementSession::DownloadL(const RMessage2& aMessage) const
    {
    RDEBUG( "ApplicationManagementSession: DownloadL" );

    TDCUserId userId;
    aMessage.ReadL(1, userId);
    CDeploymentComponent *compo = NULL;

    if (iTrustAdded)
        {
        compo = Server().Storage()->NewComponentL(EDCSDownload, userId,
                &iCertInfo);
        }
    else
        {
        compo = Server().Storage()->NewComponentL(EDCSDownload, userId);
        }
    TUint32 i(compo->InternalId());
    TPckg<TUint32> p(i);
    aMessage.WriteL(0, p);
    RDEBUG( "ApplicationManagementSession: DownloadL end" );
    }

// -------------------------------------------------------------------------------------------------------------------
// CApplicationManagementSession::FullUpdateL()
// -------------------------------------------------------------------------------------------------------------------

void CApplicationManagementSession::FullUpdateL(const RMessage2& aMessage) const
    {
    RDEBUG( "ApplicationManagementSession: FullUpdateL" );

    TUint32 sid;

    TPckg<TUint32> psid(sid);
    TInt read(aMessage.Read(0, psid));
    TUint32 tid;
    TPckg<TUint32> ptid(tid);
    TInt read2(aMessage.Read(1, ptid));

    RDEBUG_3( "ApplicationManagementSession: FullUpdateL sid is %d, tid is %d", sid, tid );
    CApplicationManagementServer &server = Server();
    server.FullUpdateL(sid, tid);
    }

// -------------------------------------------------------------------------------------------------------------------
// CApplicationManagementSession::ServiceError()
// -------------------------------------------------------------------------------------------------------------------

void CApplicationManagementSession::ServiceError(const RMessage2& aMessage,
        TInt aError)
    {
    RDEBUG_2( "ApplicationManagementSession: CApplicationManagementSession::ServiceError %d", aError);
    CSession2::ServiceError(aMessage, aError);
    }

// -------------------------------------------------------------------------------------------------------------------
// CApplicationManagementSession::AddTrustL()
// -------------------------------------------------------------------------------------------------------------------

void CApplicationManagementSession::AddTrustL(const RMessage2 &aMessage)
    {
    TPckg<TCertInfo> pkg(iCertInfo);
    aMessage.ReadL(0, pkg);
    iTrustAdded = ETrue;
    }

// -------------------------------------------------------------------------------------------------------------------
// CApplicationManagementSession::StateChangeComponentIdsL()
// -------------------------------------------------------------------------------------------------------------------

void CApplicationManagementSession::StateChangeComponentIdsL(
        const RMessage2& aMessage) const
    {
    RDEBUG( "CApplicationManagementSession: StateChangeComponentIdsL" );
    RComponentIdArray arr;
    Server().Storage()->GetStateChangeComponentIdsL(arr);
    TInt buflen(arr.Count());
    TUint32* buf = new (ELeave) TUint32[buflen];
    CleanupArrayDeletePushL(buf);
    for (TInt i(0); i < buflen; i++)
        {
        RDEBUG_2( "ApplicationManagementSession:ComponentIdsL Adding %d!!", arr[i]);
        buf[i] = arr[i];
        }
    TPtrC8 p((TUint8*) buf, buflen * sizeof(TUint32));
    aMessage.WriteL(0, p);
    arr.Close();
    CleanupStack::PopAndDestroy(buf);
    RDEBUG( "CApplicationManagementSession: StateChangeComponentIdsL end" );

    }

// -------------------------------------------------------------------------------------------------------------------
// CApplicationManagementSession::StateChangeComponentIdsCountL()
// -------------------------------------------------------------------------------------------------------------------

void CApplicationManagementSession::StateChangeComponentIdsCountL(
        const RMessage2& aMessage) const
    {
    RDEBUG( "CApplicationManagementSession: StateChangeComponentIdsCountL" );
    RPointerArray<TPreInstalledAppParams> preInstalledAppParams;
    CAMPreInstallApp* preInstallApp = CAMPreInstallApp::NewL();
    preInstallApp->GetPreInstalledAppsL(preInstalledAppParams);
    TInt count = 0;
    for (count = 0; count < preInstalledAppParams.Count(); count++)
        {
        RDEBUG8_2("CApplicationManagementSession::DownloadL: Installed App Name is: %S",&(preInstalledAppParams[count]->iPreInstalledAppame));
        RDEBUG8_2("CApplicationManagementSession::DownloadL: Installed App Vendor is: %S",&(preInstalledAppParams[count]->iPreInstalledAppVendorName));
        RDEBUG_2("CApplicationManagementSession::DownloadL: Installed App UID is : '0x%X'",preInstalledAppParams[count]->iPreInstalledAppUid);

        TBool found = EFalse;
        const RComponentIdArray &arrt = Server().Storage()->GetComponentIds();
        TInt countval(arrt.Count());
        for (TInt i(0); i < countval; i++)
            {
            CDeploymentComponent &compo = Server().Storage()->ComponentL(
                    arrt[i]);
            if (compo.Uid()
                    == preInstalledAppParams[count]->iPreInstalledAppUid)
                {
                RDEBUG( "CApplicationManagementSession: ActiveComponentsL found= TRUE" );
                found = ETrue;
                }
            }
        if (!found)
            {
            RDEBUG( "CApplicationManagementSession: Adding Pre-installed app" );
            TDCUserId preInstalledAppName;
            preInstalledAppName.Copy(
                    preInstalledAppParams[count]->iPreInstalledAppame);
            CDeploymentComponent *preInstallCompo = NULL;
            RDEBUG8_2("CApplicationManagementSession: Installed App Name is: %S",&preInstalledAppName);
            preInstallCompo = Server().Storage()->NewComponentL(EDCSActive,
                    preInstalledAppName);
            preInstallCompo->SetUid(
                    preInstalledAppParams[count]->iPreInstalledAppUid);
            preInstallCompo->SetNameL(preInstalledAppName);
            preInstallCompo->SetVersionL(
                    preInstalledAppParams[count]->iVersion);
            preInstallCompo->SetMimeTypeL(
                    preInstalledAppParams[count]->iMimeType);
            preInstallCompo->SetAppRemovableStatus(ETrue);

            Server().Storage()->UpdateL(*preInstallCompo);
            Server().Storage()->CheckForDuplicateNodesInDeployedL(
                    *preInstallCompo);
            }
        }
    delete preInstallApp;
    RComponentIdArray arr;
    Server().Storage()->GetStateChangeComponentIdsL(arr);
    TPckgBuf<TInt> buflen(arr.Count());
    aMessage.WriteL(0, buflen);
    arr.Close();
    RDEBUG( "CApplicationManagementSession: StateChangeComponentIdsCountL end" );
    }

// -------------------------------------------------------------------------------------------------------------------
// CApplicationManagementSession::StateChangeCompleteL()
// -------------------------------------------------------------------------------------------------------------------

void CApplicationManagementSession::StateChangeCompleteL(
        const RMessage2& aMessage) const
    {
    RDEBUG( "CApplicationManagementSession: StateChangeCompleteL" );
    TUint32 tid;
    TPckg<TUint32> ptid(tid);
    TInt read2(aMessage.Read(0, ptid));
    Server().Storage()->StateChangedL(tid);
    }
void CApplicationManagementSession::CheckStatusNodesValuesL()
    {
    const RComponentIdArray &arrt = Server().Storage()->GetComponentIds();
    TInt cont(arrt.Count());
    TBool isUpdateRequired = ETrue;
    for (TInt i(0); i < cont; i++)
        {
        CDeploymentComponent &compo = Server().Storage()->ComponentL(arrt[i]);
        isUpdateRequired = EFalse;
        if (compo.Status() == EDownload_DownloadProgress)
            {
            compo.SetStatusNode(EDownload_DownloadFailed);
            isUpdateRequired = ETrue;
            }
        else if (compo.Status() == EDelivered_InstallProgress)
            {
            compo.SetStatusNode(EDelivered_InstalledFailedWithData);
            isUpdateRequired = ETrue;
            }
        else if (compo.Status() == EDelivered_RemoveProgress)
            {
            compo.SetStatusNode(EDelivered_RemoveFailed);
            isUpdateRequired = ETrue;
            }
        else if (compo.Status() == EDeployed_RemoveProgress)
            {
            compo.SetStatusNode(EDeployed_RemoveFailed);
            isUpdateRequired = ETrue;
            }
        else if (compo.Status() == EDeployed_ActivateProgress)
            {
            compo.SetStatusNode(EDeployed_ActivateFailed);
            isUpdateRequired = ETrue;
            }
        else if (compo.Status() == EDeployed_DeactivateProgress)
            {
            compo.SetStatusNode(EDeployed_DeactivateFailed);
            isUpdateRequired = ETrue;
            }
        if (isUpdateRequired)
            Server().Storage()->UpdateL(compo);
        }

    }

void CApplicationManagementServer::SetSisAppVersionAndDriveL(
        CDeploymentComponent &aCompo)
    {
    RDEBUG( "CApplicationManagementSession: SetSisAppVersionAndDriveL: Begin" );

    Swi::RSisRegistrySession sisSession;
    User::LeaveIfError(sisSession.Connect());
    CleanupClosePushL(sisSession);

    Swi::RSisRegistryEntry sisEntry;

    //Opens the base package entry by specifying a UID. 
    User::LeaveIfError(sisEntry.Open(sisSession, aCompo.Uid()));
    CleanupClosePushL(sisEntry);

    TUint drivesMask = sisEntry.InstalledDrivesL();

    TInt drive = EDriveA;
    TDriveNumber installedDrive = EDriveC;

    if (drivesMask)
        {
        // Select the highest drive as location drive. That's the case when 
        // all installation is not in same drive

        while (drivesMask >>= 1)
            {
            drive++;
            }

        RDEBUG_2( "drive -> SetSisAppVersionAndDriveL %d", drive);

        installedDrive = (TDriveNumber) drive;
        }

    TBool status = EFalse;

    //Since "C" drive is not removable

    if (installedDrive != EDriveC)
        {
        status = IsInstalledAppRemovableL(installedDrive);
        }
    if (status)
        {
        RDEBUG( "IsInstalledAppRemovableL(SetSisAppVersionAndDriveL); True");
        }
    else
        {
        RDEBUG( "IsInstalledAppRemovableL(SetSisAppVersionAndDriveL); False");
        }

    //check for status drive
    aCompo.SetAppRemovableStatus(status);

    TVersion version = sisEntry.VersionL();

    if (aCompo.ComponentVersion() == KNullDesC8)
        {
        TBuf8<KVersionLength> pkgDes;
        pkgDes.AppendNum(version.iMajor);
        pkgDes.Append(KLiteralPeriod);
        pkgDes.AppendNum(version.iMinor);
        aCompo.SetVersionL(pkgDes);

        }
    CleanupStack::PopAndDestroy(2, &sisSession);

    RDEBUG( "CApplicationManagementSession: SetSisAppVersionAndDriveL: End" );
    }

TBool CApplicationManagementServer::IsInstalledAppRemovableL(
        TDriveNumber &iDrive)
    {
    RDEBUG( "IsInstalledAppRemovableL: Step1" );
    TBool removable = EFalse;
    RFs rfs;
    User::LeaveIfError(rfs.Connect());
    CleanupClosePushL(rfs);
    TDriveList driveList;
    TInt driveCount;

    //Get all removeable drive, both physically and logically
    User::LeaveIfError(DriveInfo::GetUserVisibleDrives(rfs, driveList,
            driveCount, KDriveAttRemovable));

    TInt max(driveList.Length());

    RDEBUG_2( "driveList.Length() %d", max);

    RDEBUG_2( "driveCount %d", driveCount);

    TBuf<26> buf;
    buf.Copy(driveList);
    RDEBUG_2( "Drive Info (%S)", &buf);

    for (TInt i = 0; i < max; ++i)
        {
        if (driveList[i])
            {
            TUint status;
            RDEBUG( "IsInstalledAppRemovableL: Step3" );
            DriveInfo::GetDriveStatus(rfs, i, status);

            RDEBUG( "IsInstalledAppRemovableL: Step4" );
            //To make sure the drive is physically removeable not logically removeable  
            if (status & DriveInfo::EDriveRemovable)
                {
                RDEBUG( "IsInstalledAppRemovableL: Drive removable" );
                RDEBUG_2( "IsInstalledAppRemovableL: Drive removable Drive ID %d", i);
                if (i == iDrive)
                    {
                    RDEBUG( "IsInstalledAppRemovableL: Step5" );
                    removable = ETrue;
                    break;
                    }
                }
            }
        }

    CleanupStack::PopAndDestroy(&rfs);

    return removable;
    }

// -----------------------------------------------------------------------------
// CShutDown::CShutdown()
// ------------------------------------------------------------------------------------------------

inline CShutdown::CShutdown() :
    CTimer(-1)
    {
    CActiveScheduler::Add(this);
    }

// ------------------------------------------------------------------------------------------------
// CShutDown::ConstructL()
// ------------------------------------------------------------------------------------------------

inline void CShutdown::ConstructL()
    {
    CTimer::ConstructL();
    }

// ------------------------------------------------------------------------------------------------
// CShutDown::Start()
// ------------------------------------------------------------------------------------------------

inline void CShutdown::Start()
    {
    RDEBUG( "ApplicationManagementServer: starting shutdown timeout" );
    After(KApplicationManagementShutdownDelay);
    }

// ------------------------------------------------------------------------------------------------
// CShutDown::RunL()
// ------------------------------------------------------------------------------------------------

void CShutdown::RunL()
    {
    RDEBUG( "ApplicationManagementServer timeout ... closing" );
    HbApplication::exit(0);
    }

// ------------------------------------------------------------------------------------------------
// CShutDown::Start()
// ------------------------------------------------------------------------------------------------
inline void CShutdown::Stop()
    {
    RDEBUG( "ApplicationManagementServer: Stop" );
    HbApplication::exit(0);
    }

//  End of File
