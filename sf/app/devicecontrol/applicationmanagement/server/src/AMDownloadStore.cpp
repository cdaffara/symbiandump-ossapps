/*
 * Copyright (c) 2002-2004 Nokia Corporation and/or its subsidiary(-ies). 
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
 * Description:  Implementation of applicationmanagement components
 *
 */

#include "AMDownloadStore.h"
#include "debug.h"
#include "ApplicationManagementUtility.h"
#include "amsmlhelper.h"

using namespace NApplicationManagement;

CAMDownloadStore::CAMDownloadStore(MDownloadMngrObserver& aObserver) :
    CActive(CActive::EPriorityStandard),iObserver(aObserver)
    {

    }

void CAMDownloadStore::ConstructL()
    {
    CActiveScheduler::Add(this);
    }

CAMDownloadStore* CAMDownloadStore::NewL(MDownloadMngrObserver& aObserver)
    {
    CAMDownloadStore* self = new (ELeave) CAMDownloadStore(aObserver);

    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop();

    return self;
    }

CAMDownloadStore::~CAMDownloadStore()
    {
    iDwnldStore.ResetAndDestroy();
    }

void CAMDownloadStore::AddDownloadL(CDeploymentComponent *aComponent)
    {
    CAMDwnldQTWrap *download = CAMDwnldQTWrap::NewL(iObserver, aComponent);
    iDwnldStore.AppendL(download);
    StartDownload();
    }

void CAMDownloadStore::StartDownload()
    {
    if (!IsActive())
        {

        iStatus = KRequestPending;

        TInt downldcount = iDwnldStore.Count();
        CAMDwnldQTWrap *download = iDwnldStore[downldcount - 1];

        download->StartDownload(download->iComponent, iStatus);

        SetActive();
        }
    }

TInt CAMDownloadStore::DownloadCount()
    {
    return iDwnldStore.Count();
    }

void CAMDownloadStore::RunL()
    {
    TInt downldcount = iDwnldStore.Count();

    iDwnldStore.Remove(downldcount - 1);

    if (iDwnldStore.Count() > 0)
        StartDownload();
    }

void CAMDownloadStore::DoCancel()
    {

    }

CAMDwnldQTWrap::CAMDwnldQTWrap(MDownloadMngrObserver& aObserver,
        CDeploymentComponent *aComponent) :
    iObserver(aObserver), iComponent(aComponent)
    {

    }

void CAMDwnldQTWrap::ConstructL()
    {

    serverid = SmlHelper::GetCurrentServerIDL();

    iap = -1;
    TRAPD( err, SmlHelper::GetDefaultIAPFromDMProfileL( iap ) );
    if (err != KErrNone)
        {
        iap = -1;
        RDEBUG_2("CAMDwnldQTWrap::ConstructL iapid to : %d", err );
        }

    }

CAMDwnldQTWrap* CAMDwnldQTWrap::NewL(MDownloadMngrObserver& aObserver,
        CDeploymentComponent *aComponent)
    {
    RDEBUG( "CAMDwnldQTWrap::NewL Start" );

    CAMDwnldQTWrap* self = new (ELeave) CAMDwnldQTWrap(aObserver, aComponent);

    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop();

    RDEBUG( "CAMDwnldQTWrap::NewL End" );

    return self;
    }

CAMDwnldQTWrap::~CAMDwnldQTWrap()
    {
    if (serverid)
        delete serverid;
    }

void CAMDwnldQTWrap::StartDownload(CDeploymentComponent *aComponent,
        TRequestStatus& aStatus)
    {
    RDEBUG( "CAMDwnldQTWrap::StartDownload Start" );

    aComponent->SetCallback(this);
    appdwnld = new appmgmtdownloadmgr(0, *this, aStatus);
    appdwnld->startDownload(aComponent, iap);

    RDEBUG( "CAMDwnldQTWrap::StartDownload End" );

    }

void CAMDwnldQTWrap::DownloadComplete(CDeploymentComponent *aComponent,
        TInt aStatus)
    {

    TInt err(KErrNone);
    TRAP( err, DownloadCompleteL( aComponent ));

    }

// ----------------------------------------------------------------------------------------
// CAMDownloadManager::DownloadCompleteL
// ---------------------------------------------------------------------------------------- 
void CAMDwnldQTWrap::DownloadCompleteL(CDeploymentComponent *aComponent)
    {
    iObserver.ComponentDownloadComplete(aComponent, iap, serverid);
    }

// ----------------------------------------------------------------------------------------
// CAMDownloadManager::UidExistsL
// ---------------------------------------------------------------------------------------- 
TBool CAMDwnldQTWrap::UidExistsL(const TUid &aUid,
        CDeploymentComponent *& aCompo, CDeploymentComponent *aIgnored)
    {
    TBool uidExist(EFalse);
    uidExist = iObserver.HasUidL(aUid, aCompo, aIgnored);
    return uidExist;
    }

// ---------------------------------------------------------------------------
// DownloadCompleted
// ---------------------------------------------------------------------------
void CAMDwnldQTWrap::DownloadCompleted()
    {

    TInt err(KErrNone);
    QString filename;
    QString mimetype;

    QString path("c:\\system\\appmgr\\");

    QString filefullpath(path);

    appdwnld->FileName(filename);
    appdwnld->MimeType(mimetype);

    filefullpath.append(filename);

    TPtrC filenamefullpathptr(
            reinterpret_cast<const TUint16*> (filefullpath.utf16()));
    TPtrC8 mimetypeptr(
            reinterpret_cast<const TUint8*> (mimetype.toUtf8().constData()));

    CApplicationManagementUtility::SetFileName(filenamefullpathptr);

    TRAP( err, iComponent->SuccessStatusUpdateL( filenamefullpathptr, mimetypeptr) );

    if (err != KErrNone)
        {
        RDEBUG_2(" ->iComponent::StatusUpdateL: %d", err );
        }
    RDEBUG_2( "CAMDownload::DownloadCompleted: end Download delete ERROR (%d)", err );
    }

void CAMDwnldQTWrap::DownloadFailed(TInt aDwnldStatus)
    {
    RDEBUG( "CAMDownload::DownloadFailed: start");
    //TInt err(KErrNone);
    //TInt32 errorCode = iDownloader->DownloadStatusCodeL(aDownloadState);
    TRAPD( err, iComponent->StatusUpdateL( aDwnldStatus ) );

    if (err != KErrNone)
        {
        RDEBUG_2(" ->iComponent::StatusUpdateL: %d", err );
        }
    RDEBUG_2( "CAMDownload::DownloadFailed: end Download delete ERROR (%d)", err );
    }

void CAMDwnldQTWrap::SetStatus(TInt aStatus)
    {
    TRAPD( err, iComponent->StatusUpdateL( aStatus ) );
    if (err != KErrNone)
        {
        RDEBUG_2(" ->iComponent::StatusUpdateL: %d", err );
        }
    RDEBUG_2( "CAMDownload::DownloadFailed: end Download delete ERROR (%d)", err );
    }
