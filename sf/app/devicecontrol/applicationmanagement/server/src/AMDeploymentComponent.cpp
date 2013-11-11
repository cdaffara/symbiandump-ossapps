/*
 * Copyright (c) 2005 Nokia Corporation and/or its subsidiary(-ies). 
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

#include <e32debug.h>
#include <s32mem.h>
#include <featmgr.h>
#include <ssl.h>
#include <apgcli.h>
#include <apmrec.h>
#include <apmstd.h>
#ifdef __SERIES60_30__
#include <CMenuClient.h>
#endif
#include "amsmlhelper.h"
#include "amprivateCRKeys.h"
#include "AMDeploymentComponent.h"
#include "debug.h"


using namespace NApplicationManagement;

_LIT8( KAMDCDataSuffix, "_D.txt");
_LIT8( KAMDCMetaDataSuffix, "_M.txt");


/***************************************************
 // CDeploymentComponent
 ***************************************************/

CDeploymentComponent* CDeploymentComponent::NewLC(TInt aInternalId,
        const TDCUserId &aUserId)
    {
    CDeploymentComponent *self = new ( ELeave ) CDeploymentComponent( aInternalId );
    CleanupStack::PushL(self) ;
    self->ConstructL(aUserId) ;
    return self;
    }

CDeploymentComponent* CDeploymentComponent::NewL(TInt aInternalId,
        const TDCUserId &aUserId)
    {
    CDeploymentComponent *self = NewLC(aInternalId, aUserId);
    CleanupStack::Pop(self) ;
    return self;
    }

CDeploymentComponent* CDeploymentComponent::LoadLC(TInt aInternalId,
        const TDesC8 &aBuffer)
    {
    CDeploymentComponent *self = new ( ELeave ) CDeploymentComponent(aInternalId);
    CleanupStack::PushL(self) ;
    self->ConstructLoadL(aBuffer) ;
    return self;
    }

CDeploymentComponent* CDeploymentComponent::LoadL(TInt aInternalId,
        const TDesC8 &aBuffer)
    {
    CDeploymentComponent *self = LoadLC(aInternalId, aBuffer);
    CleanupStack::Pop(self) ;
    return self;
    }

CDeploymentComponent::CDeploymentComponent(TInt aInternalId) :
iInternalId(aInternalId), iPkgVersion( 3), iOwner( -1),
iOwnerProfile( -1)
        {
        iUninstallOpts.iKillApp = SwiUI::EPolicyAllowed;
        SwiUI::TInstallOptions &opts = iInstallOpts.iOptions;
        opts.iUpgrade = SwiUI::EPolicyNotAllowed;
        opts.iOptionalItems = SwiUI::EPolicyNotAllowed;
        opts.iOCSP = SwiUI::EPolicyNotAllowed;
        opts.iIgnoreOCSPWarnings = SwiUI::EPolicyNotAllowed;
        //    iIgnoreOtherWarnings = SwiUI::EPolicyNotAllowed;
        opts.iUntrusted = SwiUI::EPolicyNotAllowed;
        opts.iPackageInfo = SwiUI::EPolicyNotAllowed;
        opts.iCapabilities = SwiUI::EPolicyNotAllowed;
        opts.iKillApp = SwiUI::EPolicyNotAllowed;
        opts.iOverwrite = SwiUI::EPolicyNotAllowed;
        opts.iDownload = SwiUI::EPolicyNotAllowed;
        opts.iDrive = SwiUI::EPolicyNotAllowed;
        opts.iLang = ELangNone;
        opts.iUsePhoneLang = EFalse;
        }

CDeploymentComponent::~CDeploymentComponent()
    {
    RDEBUG8_3( "CDeploymentComponent::~CDeploymentComponent 0x%X - 0x%X",
            reinterpret_cast<TUint>(this), reinterpret_cast<TUint>(this)+sizeof( CDeploymentComponent ) );
    delete iData;
    iData = NULL;

    delete iMetaData;
    iMetaData = NULL;
    }

CDeploymentComponent &CDeploymentComponent::operator=( const CDeploymentComponent &aData )
    {
    if( &aData != this )
        {
        *iData = aData.Data();
        *iMetaData = aData.MetaData();
        iId = aData.ComponentId();
        iName = aData.ComponentName();
        iVersion = aData.iVersion;
        iState = aData.State();
        iInternalId = aData.InternalId();
        iUid = aData.Uid();
        iUserId = aData.iUserId;
        iOwner = aData.iOwner;
        iInstallOpts = aData.InstallOpts();
        iPkgID = aData.PkgId();
        iDescription = aData.Description();
        iPkgIDRef = aData.PkgIDRef();
        iPkgType = aData.PkgType();
        iRemovableApp = aData.AppRemovable();
        iDriveSelection = aData.DriveSelected();
        iComponentId = aData.iComponentId;
        iInstallOptsSet = aData.InstallOptsSet();
        iPkgVersion = aData.iPkgVersion;
        iDownloadStatus = aData.iDownloadStatus;
        iDownloadURI = aData.iDownloadURI;
        iUninstallOpts = aData.UninstallOpts();
        iOwnerProfile = aData.OwnerProfile();
        }
    return *this;
    }

void CDeploymentComponent::PersistL(CRepository &aRepository) const
    {
    CBufBase *buf = CBufFlat::NewL( 32);
    CleanupStack::PushL(buf);
    TInt length(SerializedFormLength() );
    buf->ExpandL(0, length+1);
    TPtr8 p3(buf->Ptr(0) );
    RDesWriteStream stream(p3);
    TInt len(SerializedFormL(stream) );
    TBuf8< NCentralRepositoryConstants::KMaxBinaryLength> tbuf;

    TInt err(aRepository.Get(iInternalId, tbuf) );
    if (err == KErrNotFound)
        {
        err = aRepository.Create(iInternalId, buf->Ptr(0) );
        RDEBUG_3( "CDeploymentComponent::PersistL:  Created %d to centrep with status %d", iInternalId, err );
        }
    else
        {
        User::LeaveIfError(err) ;
        err = aRepository.Set(iInternalId, buf->Ptr(0) );
        RDEBUG_3( "CDeploymentComponent::PersistL:  Persisted %d to centrep with status %d", iInternalId, err );
        }
    User::LeaveIfError(err) ;
    CleanupStack::PopAndDestroy(buf);
    RFs fs;
    User::LeaveIfError(fs.Connect() );
    CleanupClosePushL(fs);
    iData->PersistL(fs);
    iMetaData->PersistL(fs);
    CleanupStack::PopAndDestroy( &fs) ;
    }

void CDeploymentComponent::CopyStateTo(TDeploymentComponent &aInfo) const
    {
    aInfo.iId = iId;
    aInfo.iName = iName;
    aInfo.iVersion = iVersion;
    aInfo.iDescription = iDescription;
    aInfo.iPkgID = iPkgID;
    aInfo.iPkgIDRef = iPkgIDRef;
    aInfo.iPkgType = iPkgType;
    aInfo.iState = iState;
    aInfo.iOldState = iOldState;
    }

TBool CDeploymentComponent::IsJavaMimeL(const TDesC8& aMime)
    {

    FeatureManager::InitializeLibL();
    TBool supported = EFalse;

    if (FeatureManager::FeatureSupported(KFeatureIdJavaMIDP20) )
        {
        supported = ((aMime == SwiUI::KJarMIMEType || aMime
                == SwiUI::KJadMIMEType || aMime == SwiUI::KJarxMIMEType
                || aMime == SwiUI::KJavaMIMEType) ? ETrue : EFalse );
        }
    FeatureManager::UnInitializeLib();

    return supported;
    }

TBool CDeploymentComponent::IsSisMimeL(const TDesC8& aMime)
    {

    
    TBool supported = EFalse;

    supported = ((aMime == SwiUI::KSisxMimeType || aMime
                == SwiUI::KSisMimeType || aMime == SwiUI::KPipMimeType) ? ETrue : EFalse );
   
    return supported;
    }

void CDeploymentComponent::ConstructL(const TDCUserId &aUserId)
    {
    iUserId = aUserId;
    RFs fs;
    User::LeaveIfError(fs.Connect() );
    CleanupClosePushL(fs);
    TInt e(fs.CreatePrivatePath(KDefaultDrive) );
    User::LeaveIfError(e);
    TBuf<30> privatePath;
    fs.PrivatePath(privatePath);
    CleanupStack::PopAndDestroy( &fs);

    TInt privPathLength(privatePath.Length() );
    TBuf8<10> buf;
    buf.NumFixedWidth(iInternalId, EHex, 10);

    TBuf8 <30> path;
    path.Copy(privatePath);
    HBufC8 *afilename = HBufC8::NewLC(privPathLength + 10 + KAMDCDataSuffix().Length());
    *afilename = path;
    TPtr8 afn(afilename->Des() );
    afn.Append(buf);
    afn.Append(KAMDCDataSuffix) ;
    iData = CDeploymentComponentData::NewL(CDeploymentComponentData::EData,
            KNullDesC8, KNullDesC8, afn);

    afn.Replace(privPathLength + 10, KAMDCMetaDataSuffix().Length(), KAMDCMetaDataSuffix);
    iMetaData = CDeploymentComponentData::NewL(
            CDeploymentComponentData::EMetaData, KNullDesC8, KNullDesC8, afn);

    CleanupStack::PopAndDestroy(afilename);
    }

TInt CDeploymentComponent::SerializedFormLength() const
    {
    //   0			1		2		3		4		5			6			7	8			9			10		11
    // iUserId , iState, iPkgVer, iUid, iIdLen, iNameLen, iVersLen, iDownState, iOwner, iOldState ,iDownUriLen, iInstallOptsset
    return (20*4)+iUserId.Length() + iId.Length() + iName.Length()
            + iVersion.Length() +
            
    iMidletName.Length() + iMideltVendorName.Length()
            + iMidletVersion.Length()+ iDownloadURI.Length()
            + iDescription.Length()+ iPkgID.Length()+iPkgIDRef.Length()
            +iPkgType.Length()+ sizeof(iRemovableApp)+sizeof(iDriveSelection)+sizeof(iComponentId) + iData->SerializedFormLength()
            + iMetaData->SerializedFormLength() + sizeof(TAMInstallOptions);
    }

TInt CDeploymentComponent::SerializedFormL(RWriteStream &aBuffer) const
    {
#ifdef _DEBUG
    RDEBUG_2( "Offset before uint: %d", aBuffer.Sink()->TellL( MStreamBuf::EWrite ).Offset() );
#endif

    aBuffer.WriteUint32L(iPkgVersion);

#ifdef _DEBUG
    RDEBUG_2( "Offset after uint: %d" , aBuffer.Sink()->TellL( MStreamBuf::EWrite ).Offset() );
#endif

    aBuffer.WriteUint32L(iState);
    aBuffer.WriteUint32L(iStatus);
    aBuffer.WriteUint32L(iUid.iUid);
    aBuffer.WriteUint32L(iUserId.Length() );
    aBuffer.WriteL(iUserId);
    aBuffer.WriteUint32L(iId.Length() );
    aBuffer.WriteL(iId);
    aBuffer.WriteUint32L(iName.Length() );
    aBuffer.WriteL(iName);
    aBuffer.WriteUint32L(iVersion.Length() );
    aBuffer.WriteL(iVersion);
    aBuffer.WriteUint32L(iMidletName.Length() );
    aBuffer.WriteL(iMidletName);
    aBuffer.WriteUint32L(iMideltVendorName.Length() );
    aBuffer.WriteL(iMideltVendorName);
    aBuffer.WriteUint32L(iMidletVersion.Length() );
    aBuffer.WriteL(iMidletVersion);
    aBuffer.WriteInt32L(iDownloadStatus);
    aBuffer.WriteInt32L(iOwner);
    aBuffer.WriteUint32L(iOldState);
    aBuffer.WriteUint32L(iDownloadURI.Length() );
    aBuffer.WriteL(iDownloadURI);
    aBuffer.WriteUint32L(iDescription.Length() );
    aBuffer.WriteL(iDescription);
    aBuffer.WriteUint32L(iPkgID.Length() );
    aBuffer.WriteL(iPkgID);
    aBuffer.WriteUint32L(iPkgIDRef.Length() );
    aBuffer.WriteL(iPkgIDRef);
    aBuffer.WriteUint32L(iPkgType.Length() );
    aBuffer.WriteL(iPkgType);
    aBuffer.WriteUint32L(iRemovableApp);
    aBuffer.WriteUint32L(iDriveSelection);
    aBuffer.WriteInt32L(iComponentId);
    iData->SerializedFormL(aBuffer);
    iMetaData->SerializedFormL(aBuffer);
    aBuffer.WriteUint32L(iInstallOptsSet);
    TPckg<TAMInstallOptions> opts(iInstallOpts);
    aBuffer.WriteL(opts);
    return aBuffer.Sink()->TellL( MStreamBuf::EWrite ).Offset();
    }

void CDeploymentComponent::ConstructLoadL(const TDesC8 &aBuffer)
    {
    RDEBUG( "CDeploymentComponent::ConstructLoadL ");
    TDeploymentComponent aRes;
    TMimeType aType;

    RDesReadStream stream(aBuffer);
    iPkgVersion = stream.ReadUint32L();

    iState = static_cast<TDeploymentComponentState>(stream.ReadUint32L() );
    iStatus = static_cast<TSCOMOStatusNodeStates>(stream.ReadUint32L() );
    iUid.iUid = stream.ReadUint32L();

    TUint32 len(stream.ReadUint32L() );
    stream.ReadL(iUserId, len);

    len = stream.ReadUint32L();
    stream.ReadL(iId, len);

    len = stream.ReadUint32L();
    stream.ReadL(iName, len);

    len = stream.ReadUint32L();
    stream.ReadL(iVersion, len);

    len = stream.ReadUint32L();
    stream.ReadL(iMidletName, len);

    len = stream.ReadUint32L();
    stream.ReadL(iMideltVendorName, len);

    len = stream.ReadUint32L();
    stream.ReadL(iMidletVersion, len);

    iDownloadStatus = stream.ReadInt32L();

    if (iPkgVersion > 1)
        {
        iOwner = stream.ReadInt32L();
        }
    if (iPkgVersion > 2)
        {
        iOldState = TDeploymentComponentState(stream.ReadUint32L() );
        }

    len = stream.ReadUint32L() ;
    stream.ReadL(iDownloadURI, len);

    len = stream.ReadUint32L();
    stream.ReadL(iDescription, len);

    len = stream.ReadUint32L();
    stream.ReadL(iPkgID, len);

    len = stream.ReadUint32L();
    stream.ReadL(iPkgIDRef, len);

    len = stream.ReadUint32L();
    stream.ReadL(iPkgType, len);

    iRemovableApp = stream.ReadUint32L();
    iDriveSelection = stream.ReadUint32L();
    iComponentId = stream.ReadInt32L();
    iData = CDeploymentComponentData::LoadL(stream);
    iMetaData = CDeploymentComponentData::LoadL(stream);

    //using namespace SwiUI;
    iInstallOptsSet = stream.ReadUint32L();
    TAMInstallOptionsPckg opts(iInstallOpts);
    stream.ReadL(opts, opts.Length() );
    RDEBUG( "CDeploymentComponent::ConstructLoadL end");
    }

CDeploymentComponentData &CDeploymentComponent::Data()
    {
    return *iData;
    }

const CDeploymentComponentData &CDeploymentComponent::Data() const
    {
    return *iData;
    }

CDeploymentComponentData &CDeploymentComponent::MetaData()
    {
    return *iMetaData;
    }

const CDeploymentComponentData &CDeploymentComponent::MetaData() const
    {
    return *iMetaData;
    }

const TAMInstallOptions &CDeploymentComponent::InstallOpts() const
    {
    return iInstallOpts;
    }

const SwiUI::TUninstallOptions &CDeploymentComponent::UninstallOpts() const
    {
    return iUninstallOpts;
    }

TBool CDeploymentComponent::InstallOptsSet() const
    {
    return iInstallOptsSet;
    }

TBool CDeploymentComponent::AppRemovable() const
    {
    return iRemovableApp;
    }

TBool CDeploymentComponent::DriveSelected() const
        {
        return iDriveSelection;
        }

TInt CDeploymentComponent::GetComponentId() const
        {
        return iComponentId;
        }
void CDeploymentComponent::SetMetaDataL(const TDesC8 &aMimeType)
    {
    iMetaData->DataFileName();
    iMetaData->SetDataL(aMimeType);
    }

void CDeploymentComponent::SetMetaDataL(const TDesC8 &aMetaData,
        const TDesC8 &aMimeType)
    {
    iMetaData->SetDataL(aMetaData, aMimeType);
    }

void CDeploymentComponent::SetAppRemovableStatus(const TBool &aRemovable)
    {
    iRemovableApp = aRemovable;
    }

void CDeploymentComponent::SetDriveSelectionStatus(const TBool &aDriveSelection)
    {
    iDriveSelection = aDriveSelection;
    }

void CDeploymentComponent::SetInstallOptsL(const TDesC8 &aInstallOpts)
    {
    using namespace SwiUI;
    if (aInstallOpts != KNullDesC8)
        {
        RDEBUG8_2( "CDeploymentComponent::SetInstallOptsL: '%S'", &aInstallOpts );
        TInt tmpIAP(iInstallOpts.iIAP);
        TAMInstallOptionsPckg pckg(iInstallOpts);
        pckg.Copy(aInstallOpts);
        iInstallOptsSet= ETrue;

        if (iInstallOpts.iIAP == -1 && tmpIAP > -1)
            {
            RDEBUG8_2( "CDeploymentComponent::SetInstallOptsL: using IAPid %d", tmpIAP );
            iInstallOpts.iIAP = tmpIAP;
            }
        }

    }

void CDeploymentComponent::ResetDataL(RFs &aFs)
    {
    iData->ResetData(aFs);
    iMetaData->ResetData(aFs);
    }

void CDeploymentComponent::SetAttr(TDes8 &aAttr, const TDesC8 &aNewVal) const
    {
    if (aNewVal.Length() > aAttr.MaxLength() )
        {
        aAttr = aNewVal.Left(aAttr.MaxLength() );
        }
    else
        {
        aAttr = aNewVal;
        }
    }

void CDeploymentComponent::SetIdL(const TDesC8 &aId)
    {
    SetAttr(iId, aId);
    }

void CDeploymentComponent::SetNameL(const TDesC8 &aName)
    {
    if (iName != aName)
        {
        SetAttr(iName, aName);
        }
    }

void CDeploymentComponent::SetVersionL(const TDesC8 &aVersion)
    {
    SetAttr(iVersion, aVersion);
    }
void CDeploymentComponent::SetMimeTypeL( const TDesC8 &aMimeType )
{
	iData->SetMimeTypeL(aMimeType);
}

void CDeploymentComponent::SetDescriptionL(const TDesC8 &aDescription)
    {
    SetAttr(iDescription, aDescription);
    }

void CDeploymentComponent::SetPkgIDL(const TDesC8 &aPkgID)
    {
    // PkgID and PkgIDRef should be same and Get done on PkgID
    // when state is in Download or Delivered and PkgIDRef is
    // returned when state is in Deployed state
    SetAttr(iPkgID, aPkgID);
    SetAttr(iPkgIDRef, aPkgID);
    }

void CDeploymentComponent::SetPkgTypeL(const TDesC8 &aPkgType)
    {

    SetAttr(iPkgType, aPkgType);

    }
void CDeploymentComponent::SetMidletNameL(const TDesC8 &aMidletName)
    {
    SetAttr(iMidletName, aMidletName);
    }

void CDeploymentComponent::SetMidletVendorL(const TDesC8 &aMidletVendor)
    {
    SetAttr(iMideltVendorName, aMidletVendor);
    }

void CDeploymentComponent::SetMidletVersionL(const TDesC8 &aMidletVersion)
    {
    SetAttr(iMidletVersion, aMidletVersion);
    }

void CDeploymentComponent::SetUid(const TUid aUid)
    {
    RDEBUG8_2("CDeploymentComponent::SetUid 0x%x", aUid.iUid );
    iUid = aUid;
    iId.Zero();
    if (iUid != TUid::Null() )
        {
        iId.AppendNumFixedWidth(iUid.iUid, EHex, 8);
        }
    }

void CDeploymentComponent::SetOwnerProfile(TSmlProfileId aOwnerProfile)
    {
    iOwnerProfile = aOwnerProfile;
    }

TInt CDeploymentComponent::SetState(const TDeploymentComponentState &aState)
    {
    RDEBUG_2( "CDeploymentComponent::SetState - set to state (%d)", aState );

    TInt ret(KErrNone);
    iOldState = iState;
    iState = aState;
#ifdef __SERIES60_30__
    if( iState != iOldState && iState != EDCSDownload && iState != EDCSDelivered )
        {
        if ( iUid != TUid::Null() )
            {
            RMenuServSession ses;
            TBool visible( EFalse );
            if ( iState == EDCSActive )
                {
                visible = ETrue;
                }
            TRAP( ret, ses.SetApplicationVisibilityL( iUid.iUid, visible ) );
            if (ret != KErrNone )
                {
                RDEBUG_2( "CDeploymentComponent::SetState - failed to set visiblity %d", ret );
                }
            else
                {
                RDEBUG_2( "CDeploymentComponent::SetState - Set visiblity to %d", visible );
                }
            }
        else
            {
            RDEBUG( "CDeploymentComponent::SetState - No need to set visibility to null uid" );
            }

        }
    else
        {
        RDEBUG( "CDeploymentComponent::SetState - No need to change visibility " );
        }
#endif	
    return ret;
    }

void CDeploymentComponent::SetStatusNode(
        const TSCOMOStatusNodeStates &aStatus) const
    {
    iStatus = aStatus;
    CRepository *repository=NULL;

    TRAPD( erx, repository = CRepository::NewL ( KCRUidPrivateApplicationManagementKeys ) )
    ;
    if (erx!=KErrNone)
        return;

    TRAPD(err, PersistL(*repository))
    ;

    delete repository;

    if (err!=KErrNone)
        return;

    }

void CDeploymentComponent::Set(const TDeploymentComponent &aInfo)
    {
    iId = aInfo.iId;
    iName = aInfo.iId;
    iVersion = aInfo.iVersion;
    iDescription = aInfo.iDescription;
    iPkgID = aInfo.iPkgID;
    iPkgIDRef = aInfo.iPkgIDRef;
    iPkgType = aInfo.iPkgType;
    }

void CDeploymentComponent::SetDownloadURIL(const TDesC8 &aURI)
    {
    if (iState == EDCSDownload)
        {
        iDownloadURI = aURI;
        }
    else
        {
        RDEBUG_2( "CDeploymentComponent::SetDownloadURIL: ERROR Illegal state: %d", iState );
        User::Leave(KErrNotSupported) ;
        }
    }

void CDeploymentComponent::SetDownloadTarget(
        const TDownloadTarget& aDownloadTarget)
    {
    RDEBUG_2("CDeploymentComponent::SetDownloadTarget: (%d)", aDownloadTarget);
    iDownloadTarget = aDownloadTarget;
    }

void CDeploymentComponent::SetDataL(const TDesC8 &aData,
        const TDesC8 &aMimeType)
    {
    iUid = iData->SetDataL(aData, aMimeType);
    iId.Zero();
    if (iUid != TUid::Null() )
        {
        iId.AppendNumFixedWidth(iUid.iUid, EHex, 8);
        }
    }

void CDeploymentComponent::SetDataL(const TDesC &aFileName,
        const TDesC8 &aMimeType)
    {
    SetUid(iData->SetDataL(aFileName, aMimeType) );
    }

void CDeploymentComponent::SetDataL(const TDesC8 &aMimeType)
    {
    SetUid(iData->SetDataL(aMimeType) );
    }

// ----------------------------------------------------------------------------------------
// CDeploymentComponent::StatusUpdateL
// ----------------------------------------------------------------------------------------	
void CDeploymentComponent::StatusUpdateL(TInt aNewStatus)
    {
    RDEBUG( "CDeploymentComponent::StatusUpdateL : begin");
    RDEBUG_2( "CDeploymentComponent::StatusUpdateL %d", aNewStatus );

    if (State() == EDCSDownload)
        {
        iDownloadStatus = aNewStatus;
        RDEBUG_2( "CDeploymentComponent::StatusUpdate: Set status to: %d", iDownloadStatus );

        if (DownloadFailed() )
            {
            SetStatusNode(EDownload_DownloadFailed);
            RDEBUG8_2( "CDeploymentComponent::StatusUpdate: ERROR Failed to download URI: '%S' ", &iDownloadURI );
            }

        if (iCallback)
            {
            iCallback->DownloadComplete( this, iDownloadStatus);
            }
        }
    else
        {
        RDEBUG_2( "CDeploymentComponent::StatusUpdate: ERROR Illegal state: %d", State() );
        }
    RDEBUG( "CDeploymentComponent::StatusUpdateL : end");
    }

// ----------------------------------------------------------------------------------------
// CDeploymentComponent::SuccessStatusUpdateL
// ----------------------------------------------------------------------------------------	
void CDeploymentComponent::SuccessStatusUpdateL(const TDesC &aDlFileName,
        const TDesC8& aDlMimeType)
    {
    RDEBUG( "CDeploymentComponent::SuccessStatusUpdateL : start");
    
   
    
    TBuf<256> FileType;
    TBuf8<256> FileType8;
    
    RApaLsSession RSession;      
    if(RSession.Connect() == KErrNone)        
        {  
        TUid uid;
        TDataType datatype;
        RSession.AppForDocument(aDlFileName,uid, datatype );
         
        FileType.Copy(datatype.Des()); 
        RDEBUG_2( "CDeploymentComponent::Pkg Id: %d ", uid.iUid);  
        RDEBUG_2( "CDeploymentComponent::Pkg MIME: '%S' ", &FileType);  
        }
    RSession.Close();
    
    FileType8.Copy(FileType);
    
    SetDataL(aDlFileName, FileType8);
    
    
    
    //SetDataL(FileType8);

    RDEBUG8_2( "CDeploymentComponent::Pkg MIME: '%S' ", &FileType8);

    
    // Set PkgID same as MiME type of downloaded content
    SetPkgTypeL(FileType8);

    CDeploymentComponent* conflict( NULL);
    TBool dlOk(ETrue);
    if (iUid != TUid::Null() )
        {
        __ASSERT_DEBUG( iCallback, User::Invariant() );
        if (iCallback->UidExistsL(iUid, conflict, this) )
            {
            }
        }
    else
        {
        RDEBUG( "CDeploymentComponent::StatusUpdateL - WARNING cannot detect uid" );
        if (IsJavaMimeL(FileType8) ||IsSisMimeL(FileType8) )
            {
            RDEBUG( "CDeploymentComponent::StatusUpdateL - Ignoring prev warning because it's java" );
            }
        else
            {
            dlOk = EFalse;
            //The Operation has been rejected because the device does not support this MIME type.
            iDownloadStatus = KStatusUnSupportedEnvironment;
            }
        }

    if (dlOk)
        {
        /*for Predefined static node, append static node name with Internal ID before 
         * moving to delivered state to avoid duplicates
         */
        TBuf8<256> staticnode;
        TInt staticnodeenabled = 0;
        CRepository* cenrep = CRepository::NewLC( KCRUidPrivateApplicationManagementKeys );
        cenrep->Get( KAMDownloadPredefinedNode, staticnode ); 
        cenrep->Get( KAMStaticNodeEnabled, staticnodeenabled );
        CleanupStack::PopAndDestroy(); //cenrep
        if (iUserId.Compare(staticnode) == 0 && staticnodeenabled)
            {
            iUserId.Append(*IntToDes8LC(iInternalId));
            CleanupStack::PopAndDestroy(); // for IntToDes8LC()
            }

        //Reset the value of DownloadStatus for success update
        iDownloadStatus = 0;
                
        SetState(EDCSDelivered);
        SetStatusNode(EIdle);
        }

    if (iCallback)
        {
        iCallback->DownloadComplete( this, iDownloadStatus);
        }

    RDEBUG( "CDeploymentComponent::SuccessStatusUpdateL : end");
    }

const TSmlProfileId& CDeploymentComponent::OwnerProfile() const
    {
    return iOwnerProfile;
    }

const TDesC8& CDeploymentComponent::MimeType() const
    {
    if (iMetaData->MimeType().Length() )
        {
        return iMetaData->MimeType();
        }
    return iData->MimeType();
    }

TBool CDeploymentComponent::DownloadFailed() const // http or symbian error
    {
    return (iDownloadStatus >= 400 || iDownloadStatus < 0) ? ETrue : EFalse;
    }

TInt CDeploymentComponent::DownloadStatus() const
    {
    return iDownloadStatus;
    }

void CDeploymentComponent::DestroyL(CRepository &aRepository)
    {
    TDeploymentComponentState st(State() );
    if ( !(st == EDCSActive || st == EDCSInactive))
        {
        RFs fs;
        User::LeaveIfError(fs.Connect() );
        CleanupClosePushL(fs);
        iMetaData->DestroyL(fs);
        iData->DestroyL(fs);
        CleanupStack::PopAndDestroy( &fs) ;// fs
        }

    TInt err(aRepository.Delete(iInternalId) );
    RDEBUG_3( "CDeploymentComponent::DestroyL:  Deleting %d in centrep resulted with status %d", iInternalId, err );
    }

void CDeploymentComponent::SetOwner(const TInt &aInfo)
    {
    iOwner = aInfo;
    }

void CDeploymentComponent::SetComponentId(const TInt &aComponentId)
    {
    iComponentId = aComponentId;
    }

void CDeploymentComponent::SetIAPL(const TInt aIap)
    {
    RDEBUG8_2( "CDeploymentComponent::SetIAPL: %d ", aIap );
    iInstallOpts.iIAP = aIap;
    }

TInt CDeploymentComponent::GetIAP() const
    {
    return iInstallOpts.iIAP;
    }
HBufC8* CDeploymentComponent::IntToDes8LC(const TInt aLuid)
    {
    HBufC8* buf = HBufC8::NewLC( 10); //10 = max length of 32bit integer
    TPtr8 ptrBuf = buf->Des();
    ptrBuf.Num(aLuid);
    return buf;
    }

//  End of File
