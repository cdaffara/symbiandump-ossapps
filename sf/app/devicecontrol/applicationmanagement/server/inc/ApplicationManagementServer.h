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

#ifndef __APPLICATIONMANAGEMENTSERVER_H__
#define __APPLICATIONMANAGEMENTSERVER_H__

// INCLUDES
#include <SyncMLClient.h>
#include <e32base.h>
#include <centralrepository.h>
#include <SWInstApi.h>
#include <ssl.h>
#include <swi/launcher.h>
#include <swi/sisregistrysession.h>
#include <swi/sisregistrypackage.h>
#include <swi/sisregistryentry.h>
#include <hbprogressdialog.h>
#include <hbmainwindow.h>
#include <hbaction.h>
#include <usif/sif/sif.h>
#include <SWInstApi.h>
#include <QGraphicsLinearLayout>
#include "ApplicationManagementClientServer.h"
#include "PlatformSecurityPolicies.h"
#include "AMDeploymentComponent.h"
#include "amstorage.h"
#include "ApplicationManagementUtility.h"
#include "AMDownloaddb.h"
#include "amwaitprogdialog.h"
#include "AMDownloadStore.h"

// CONSTANTS
const TUid NProperty = {0x200267FB};
const TUint32 NInteger = 0x00000001;
const TInt KBase64BufSize = 131072;
const QString path = "z:/resource/qt/translations/";
class CAppMgmtSrvApp;
_LIT(KIndicatorParamDownloading, "Downloading");
namespace NApplicationManagement
    {

    // -----------------------------------------------------------
    // ApplicationManagement server panic codes
    // -----------------------------------------------------------
    enum TApplicationManagementPanic
        {
        EPanicGeneral,
        EBadSubsessionHandle,
        EPanicIllegalFunction,
        EBadDescriptor
        };
    // -----------------------------------------------------------
    // Wait dialog types
    // -----------------------------------------------------------
    enum TDialogType
        {
        EInstallWaitDlg,
        EUninstallWaitDlg
        };
    // FUNCTION PROTOTYPES

    void PanicClient(const RMessagePtr2& aMessage,
            TApplicationManagementPanic aPanic);

    // CLASS DECLARATION     
    class CShutdown : public CTimer
        {
        enum
            {
            KApplicationManagementShutdownDelay = 0x3000000 // approx 30s
            };
public:
        inline CShutdown();
        inline void ConstructL();
        inline void Start();
	inline void Stop();
private:
        void RunL();
        };

    /**
     *  CApplicationManagementServer 
     *  Description.
     */

    class CApplicationManagementServer : public CPolicyServer,
        public MDownloadMngrObserver,public MInstallerCallBack
        {
public:
        static CServer2* NewL(HbMainWindow *mainWindow);
        ~CApplicationManagementServer();

        void Panic(TInt aPanicCode);
        void AddSession();
        void DropSession();
        
        CPolicyServer::TCustomResult CreateServiceSecurityCheckL(
                TUid aServiceType, const RMessage2& aMsg, TInt& aAction,
                TSecurityInfo& aMissing);
private:

        CApplicationManagementServer();
        virtual void ConstructL(HbMainWindow *mainWindow);
        CSession2* NewSessionL(const TVersion& aVersion, 
         const RMessage2& aMessage) const;

        //From CPolicyServer
        CPolicyServer::TCustomResult
                CustomSecurityCheckL(const RMessage2& aMsg, TInt& aAction,
                        TSecurityInfo& aMissing);

        // browses sis registry
        void LookupSisRegistryL();

        //New methods	
        void DownloadCompleteL(CDeploymentComponent *aComponent, TInt iapid,
                HBufC8* aServerId);

        void InstallCompleteL(const CDeploymentComponent &aCompo,
                const TBool &aInstallSuccess=ETrue);
                
        TInt GetErrorStatus(TInt aErrorDownload, TInt aErrorInstall);
        
        void EnableDMNetworkMonL(TInt iapid, HBufC8 *aServerId);

public:

        /**
         * 	Storage Returns the storage
         * @return The storage pointer
         */
        inline CDeliveryComponentStorage *Storage()
            {
            return iStorage;
            }

        /**
         * UninstallL Uninstalls the given deployment component.
         * @param aComponent The component to be uninstalled
         * @param aStatus TRequestStatus of the call
         */
        void UninstallL(const CDeploymentComponent &aComponent,
                TRequestStatus &aStatus);

        void AddDownloadL(CDeploymentComponent *aComponent);

        void PerformRFSL();
        void RemoveInternalL(const CDeploymentComponent &aCompo,
                TBool aDoUninstall = ETrue);

        TBool CheckCertL(const TCertInfo &aInfo) const;
        void DoTheInstallL(CDeploymentComponent &aCompo);

        void GetAMServerDownloadDBL(TUint32 internalid, TInt& finalresult,
                TDes8& atargetURI);
                
        TBool IsOMASCOMOEnabledL();

        /**
         * JavaUidsL Returns java uids.
         * @param aUids the array to hold uids
         */
        void JavaUidsL(RArray<TUid> &aUids) const;

        /**
         * FullUpdateL updates target with source
         * @param sid Source deployment component id
         * @param tid Target deployment component id
         */
        void FullUpdateL(const TUint32 sid, const TUint32 tid);

        /**
         * SetDataL sets data to deployment component and checks possible uid overlaps
         * @param aCompo deployment component where data is set
         * @param aData new data to be set
         * @param aMime mimetype of the new data to be set
         */
        void SetDataL(CDeploymentComponent &aCompo, const TDesC8 &aData,
                const TDesC8 &aMime);
        /**
         * Checks of data in file is Base64 decoded.
         * @param aFile file to be checked if data is encoded
         * @param aLength data length in file
         * @return ETrue if given data in file is Base64 encoded
         */
        TBool IsDataFileB64EncodedL(RFile &aFile, TInt& aLength);

        /**
         * Checks of data is Base64 decoded.
         * @param aData data to be checked for encoding
         * @return ETrue if given data is Base64 encoded
         */
        TBool CheckB64Encode(const TDesC8& aData);

        /**
         * B64 Decodes data in aData and return decoded data
         * @param aSourceFile file containing decoded data
         * @param aDestinationFile file where to decode data
         */
        void DecodeB64DataFileL(RFile& aSourceFile, RFile& aDestinationFile);

        /**
         * B64 Decodes data in aData and return decoded data
         * @param aData decoded data
         * @return Decoded data
         */
        HBufC8* DecodeB64DataLC(const TDesC8 &aData);

        /**
         * B64 Encodes data in aData to aTarget
         * @param aData data which is encoded
         * @param aTarget encoded data buffer
         */
        void EncodeDataL(const TDesC8& aData, CBufBase &aTarget);
        void HandleAllClientsClosed();

        void DeleteGenericAlertForIDL(TUint32 internalid);
        void CheckforDuplicateMidletsL(TMidletParameters& amidletParameters);

public:
        // from MDownloadMngrObserver

        /**
         * 
         * @param aComponent 
         * @param aStatus 
         */
        void ComponentDownloadComplete(CDeploymentComponent *aComponent,
                TInt iapid, HBufC8 *aServerId);

        /**
         * Looks all deployments components and sis & java registries
         * to find out whether given uid is already used...
         * @param aUid The uid to be looked for
         * @return ETrue if uid is found, EFalse otherwise
         */
        TBool HasUidL(const TUid &aUid, CDeploymentComponent *& aCompo,
                CDeploymentComponent *aIgnored);

        void ComponentDownloadFailed(CDeploymentComponent *aComponent,
                TInt aReason);

        void StartShutDownTimerL();

        TBool IsInstalledAppRemovableL(TDriveNumber &iDrive);

	  void InstallationCancelled();
private:

        /**
         * FullUpdateL updates target with source
         * @param sid Source deployment component 
         * @param tid Target deployment component 
         */
        void FullUpdateL(CDeploymentComponent &scompo,
                const CDeploymentComponent &tcompo);

        /**
         * InstallL Installs the given deployment component. Async version.
         * @param aComponent The component to be installed
         * @param aStatus TRequestStatus of the call
         */
        void InstallL(const CDeploymentComponent &aComponent,
                TRequestStatus &aStatus);
        /**
         * InstallL Installs the given deployment component. Syncronized version.
         * @param aComponent The component to be installed
         */
        void InstallL(const CDeploymentComponent &aComponent);

        /**
         * PrepareInstallL Prepares install of given component.
         * @param aComponent The component to be installed
         * @param aFileName	If call is successful, will contain 
         * the name of file ready to be installed
         * @return ETrue if preparation is successful, EFalse otherwise
         */
        TBool PrepareInstallL(const CDeploymentComponent &aComponent,
                TFileName &aFileName);

        /**
         * Finds whether the given uid is found in sis or java registry.
         * Searches first sis registry and uses FindJavaUidL if not found
         */
        TBool FindInstalledSisUidL(const TUid &aUid) const;

        /**
         * Finds whether the given uid is found in java registry
         */
        TBool FindInstalledJavaUidL(const TUid &aUid) const;
        void SetSisAppVersionAndDriveL(CDeploymentComponent &aCompo);
        void JavaInstallL(CDeploymentComponent &aCompo);
        void SisInstallL(const CDeploymentComponent &aCompo);
        TUid FindNewUid(const RArray<TUid> &aUidsOrig,
                const RArray<TUid> &aUidsNew) const;

        /**
         * Reads Jar file name from Jad file
         * @param aFile file handle for Jad file where to find Jar filename
         * @return aFileName	If call is successful, will contain the name of file ready to be installed
         */
        TFileName ReadJarFileNameL(RFile& aFile) const;

        /**
         * Reads line of data from file.
         * @param aStream stream handle where to read data
         * @param aLineBuffer buffer where function appends read data for line. Return a full line at time.
         */
        void ReadLineL(RReadStream& aStream, TDes8& aLineBuffer) const;

        /**
         * DoDataUpdateCheck checks whether there's dc having same data uid
         * @return The conflicting DC or NULL
         */
        CDeploymentComponent * DoDataUpdateCheckL(
                CDeploymentComponent &aCompo);

        // Functions used to generate temporary files Java installation use case
        // where both Jad and Jar files are delivered to phone and Jad file is used to install 
        // the Java application from local Jar file.
        void GenerateTempJarFileNameL(RFs &aFs, const TDesC& aMetaSource,
                TFileName &aFileName) const;

        void GenerateTempFileNameL(const RFs &aFs, const TDesC& aSource,
                TFileName &aFileName) const;

        void GenerateTempFileNameL(RFs &aFs,
                const CDeploymentComponent& aCompo, TFileName& aSource,
                TFileName &aMetaSource, TFileName &aFileName,
                TFileName &aMetaFileName) const;

        void GenerateTempFileNameL(RFs &aFs,
                const CDeploymentComponent& aCompo, TFileName &aFileName,
                TFileName &aMetaFileName) const;

        CApaAppServiceBase* CreateServiceL(TUid aServiceType) const;
public:
        CAppMgmtSrvApp* iParentApp;
private:
        TInt iSessionCount;
        CShutdown iShutdown;
        CApplicationManagementUtility* iUtility;
        CDeliveryComponentStorage *iStorage;
        TBool iInstallInProgress;

        Usif::RSoftwareInstall iInstaller;
        Usif::COpaqueNamedParams *iArgu;
        Usif::COpaqueNamedParams *iResults;
        SwiUI::TInstallOptionsPckg iInstallOptions;
        SwiUI::TUninstallOptionsPckg iUninstallOptions;
        SwiUI::TInstallReqPckg iInstallReq;
        TFileName iAppname;
        RFs iInstallRFs;
        RFile iInstallFile;
        CAMDownloadStore* iDownloadMngr;
        CAMDownloaddb* iAMServerDB;
        TBool iOMASCOMOEnabled;
        
        appmgmtdownloadmgr* m_DownloadMngr;
        QString m_appName;
        AMWaitProgDialog* m_Dlg;
        CDeploymentComponent* iComp;
        HbMainWindow* m_Window;
        int mUserCancelled;
        };

    namespace NPolicyConstants
        {

        // "CSF" + CA + SerialNumber + Fingerprint) ;
        //_LIT( KSubjectFormat, "CSF%S%S%S" );
        namespace NActions
            {
            _LIT8( KUse, "Use" );
            }
        _LIT8( KResourceValue, "ApplicationManagement" );
        }

    class CApplicationManagementSession : public CSession2
        {
public:
        inline CApplicationManagementSession();
        inline CApplicationManagementSession(const TCertInfo &aInfo);
        void CreateL();
private:
        ~CApplicationManagementSession();
        inline CApplicationManagementServer& Server() const;

        void ServiceL(const RMessage2& aMessage);
        TBool PreDispatchMessageL(const RMessage2 &aMessage);
        void DispatchMessageL(const RMessage2& aMessage);
        void ServiceError(const RMessage2& aMessage, TInt aError);

        void ComponentCountL(const RMessage2& aMessage);
        void ComponentIdsL(const RMessage2& aMessage) const;
        void Deliver2L(const RMessage2& aMessage) const;
        void DeliverCompleteL(const RMessage2& aMessage) const;
        void GetComponentL(const RMessage2& aMessage) const;
        void InstallL(const RMessage2 &aMessage) const;
        void UpdateL(const RMessage2 &aMessage) const;
        void GetDataL(const RMessage2 &aMessage) const;
        void GetDataLengthL(const RMessage2 &aMessage) const;
        void RemoveL(const RMessage2 &aMessage) const;
        void UpdateDataL(const RMessage2 &aMessage) const;
        void UpdateStreamedDataL(const RMessage2& aMessage) const;
        void DownloadL(const RMessage2 &aMessage) const;
        void AddTrustL(const RMessage2 &aMessage);
        void FullUpdateL(const RMessage2 &aMessage) const;
        void GetUserIdL(const RMessage2 &aMessage) const;
        void StartDownloadL(const RMessage2& aMessage) const;
        void StateChangeComponentIdsL(const RMessage2& aMessage) const;
        void StateChangeComponentIdsCountL(const RMessage2& aMessage) const;
        void StateChangeCompleteL(const RMessage2& aMessage) const;
        void ActivateL(const RMessage2& aMessage) const;
        void DeactivateL(const RMessage2& aMessage) const;
        void GetTemporaryInstFileL(const RMessage2 &aMessage) const;
        void GetFinalResultForGAL(const RMessage2 &aMessage);

        TInt PackageSizeL(Swi::RSisRegistrySession& aSession,
                Swi::RSisRegistryEntry& aEntry);
        TInt AugmentationSizeL(Swi::RSisRegistrySession& aSession,
                const Swi::CSisRegistryPackage& aPackage);

        void RemoveInternalL(const TUint32 &aId) const;

        /**
         * Collects active components in aArrt and places them in aArr
         */
        void ActiveComponentsL(const RComponentIdArray &aArrt,
                RComponentIdArray &aArr,TDeploymentComponentState &aState) const;

        void GenericAlertSentForIdL(const RMessage2& aMessage) const;

        void CheckStatusNodesValuesL();

private:
        void LookupSisRegistryL();

        TCertInfo iCertInfo;
        TBool iTrustAdded;
        
        };

    }
#endif //__APPLICATIONMANAGEMENTSERVER_H__
//  End of File

