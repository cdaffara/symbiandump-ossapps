/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  API extensions for RHttpDownloadMgr
*
*/

// Version : %version: 4 %

#include "stifutilities.h"
#include "dlmgr_stub.h"
#include "mpxvideo_debug.h"

CDlMgrTestClass*
CDlMgrTestClass::NewL()
{
    MPX_ENTER_EXIT(_L("CDlMgrTestClass::NewL()"));

    StifUtilities* utility = reinterpret_cast<StifUtilities*>( Dll::Tls() );

    if ( ! utility->iDlMgrTester )
    {
        utility->iDlMgrTester = new CDlMgrTestClass();
    }

    return utility->iDlMgrTester;
}

CDlMgrTestClass::CDlMgrTestClass()
{
}

CDlMgrTestClass::~CDlMgrTestClass()
{
}

void
CDlMgrTestClass::AddStifObserver( MStifTestObserver* aStifObserver )
{
    MPX_ENTER_EXIT(_L("CDlMgrTestClass::AddStifObserver()"));

    iStifObserver = aStifObserver;
}

void
CDlMgrTestClass::AttachToDownloadL( const TInt32 aDlId )
{
    MPX_ENTER_EXIT(_L("CDlMgrTestClass::AttachToDownloadL()"));

    if ( iStifObserver )
    {
        TCallbackEvent* event = new TCallbackEvent;
        event->iEvent = EConnectedToDownload;
        event->iData  = aDlId;
        event->iError = KErrNone;
        iStifObserver->HandleUtilityEvent( event );
    }
}

void
CDlMgrTestClass::AddDownloadMgr( RHttpDownloadMgr* aDlMgr )
{
    iDlMgr = aDlMgr;
}

void
CDlMgrTestClass::PauseDownload()
{
    iDlMgr->PauseAll();
}

void
CDlMgrTestClass::ResumeDownload()
{
    iDlMgr->StartAll();
}

void
CDlMgrTestClass::CancelDownload()
{
    iDlMgr->DeleteAll();
}

EXPORT_C TInt
RHttpDownload::SetBoolAttribute( const TUint aAttribute, TBool aValue )
{
    MPX_DEBUG(_L("RHttpDownload::SetBoolAttribute(%d, %d)"), aAttribute, aValue);

    if ( aAttribute == EDlAttrProgressive )
    {
        iProgressive = aValue;
    }

    return KErrNone;
}

EXPORT_C TInt
RHttpDownload::GetBoolAttribute( const TUint aAttribute, TBool& aValue )
{
    if ( aAttribute == EDlAttrProgressive )
    {
        aValue = iProgressive;
    }

    MPX_DEBUG(_L("RHttpDownload::GetBoolAttribute(%d, %d)"), aAttribute, aValue);

    return KErrNone;
}

EXPORT_C TInt
RHttpDownload::GetStringAttribute( const TUint /*aAttribute*/, TDes16& /*aValue*/ )
{
    return KErrNone;
}

EXPORT_C TInt
RHttpDownload::SetStringAttribute( const TUint aAttribute, const TDesC16& aValue)
{
    MPX_DEBUG(_L("RHttpDownload::SetStringAttribute(%d, %S)"), aAttribute, &aValue);

    if ( aAttribute == EDlAttrLocalFileName )
    {
        iLocalFile = aValue.Alloc();
    }
    else if ( aAttribute == EDlAttrDestFilename )
    {
        iDestFile = aValue.Alloc();
    }

    return KErrNone;
}

EXPORT_C TInt
RHttpDownload::GetStringAttribute( const TUint /*aAttribute*/, TDes8& /*aValue*/ )
{
    return KErrNone;
}

EXPORT_C TInt
RHttpDownload::SetStringAttribute( const TUint /*aAttribute*/, const TDesC8& /*aValue*/ )
{
    return KErrNone;
}

EXPORT_C TInt
RHttpDownload::GetIntAttribute( const TUint aAttribute, TInt32& aValue )
{
    switch ( aAttribute )
    {
        case EDlAttrLength:
        {
            aValue = iLength;
            break;
        }
        case EDlAttrDownloadedSize:
        {
            aValue = iSize;
            break;
        }
        case EDlAttrState:
        {
            aValue = iState;
            break;
        }
        case EDlAttrErrorId:
        {
            aValue = iErrorId;
            break;
        }
        case EDlAttrGlobalErrorId:
        {
            aValue = iGlobalError;
        }
    }

    MPX_DEBUG(_L("RHttpDownload::GetIntAttribute(%d, %d)"), aAttribute, aValue);

    return KErrNone;
}

EXPORT_C TInt
RHttpDownload::SetIntAttribute( const TUint /*aAttribute*/, TInt32 /*aValue*/ )
{
    return KErrNone;
}

EXPORT_C TInt
RHttpDownload::GetFileHandleAttribute( RFile& /*aFile*/ )
{
    return KErrNone;
}

EXPORT_C TInt
RHttpDownload::SetFileHandleAttribute( RFile& /*aFile*/ )
{
    return KErrNone;
}

EXPORT_C TInt
RHttpDownload::Start()
{
    MPX_DEBUG(_L("RHttpDownload::Start()"));

    iState = EHttpDlInprogress;

    RFile file;
    _LIT(KFileName, "\\vhpp_dlmgr.txt");

    RFs fs;
    fs.Connect();

    file.Open( fs, KFileName, EFileRead | EFileShareAny );

    TInt dlSize;
    HBufC8* fBuf = HBufC8::NewL( 16 );
    TPtr8 fileBuf = fBuf->Des();

    file.Read( fileBuf, 16 );

    TLex8 number( fileBuf );
    number.Val( dlSize );

    iSize = dlSize;

    file.Close();
    fs.Close();

    iServer->iUpdateSize->Start(
        TCallBack( RHttpDownloadMgr::UpdateDownloadProgress, iServer ) );

    return KErrNone;
}

EXPORT_C TInt
RHttpDownload::Pause()
{
    MPX_DEBUG(_L("RHttpDownload::Pause()"));

    iState = EHttpDlPaused;

    iServer->iUpdateSize->Start(
        TCallBack( RHttpDownloadMgr::UpdateDownloadProgress, iServer ) );

    return KErrNone;
}

EXPORT_C TInt
RHttpDownload::Delete()
{
    MPX_DEBUG(_L("RHttpDownload::Delete()"));

    iState = EHttpDlFailed;

    iServer->iUpdateSize->Start(
        TCallBack( RHttpDownloadMgr::UpdateDownloadProgress, iServer ) );

    return KErrNone;
}

EXPORT_C TInt
RHttpDownload::Move()
{
    MPX_DEBUG(_L("RHttpDownload::Move()"));
    return KErrNone;
}

EXPORT_C TInt
RHttpDownload::Reset()
{
    MPX_DEBUG(_L("RHttpDownload::Reset()"));
    return KErrNone;
}

RHttpDownload::RHttpDownload( RHttpDownloadMgr* aServer )
{
    iServer = aServer;
}

RHttpDownload::~RHttpDownload()
{
    if ( iLocalFile )
    {
        delete iLocalFile;
    }

    if ( iDestFile )
    {
        delete iDestFile;
    }
}

EXPORT_C
RHttpDownloadMgr::RHttpDownloadMgr()
{
    MPX_DEBUG(_L("RHttpDownloadMgr::RHttpDownloadMgr()"));

    iDownloadArray = new (ELeave) CArrayPtrFlat< RHttpDownload >( 1 );

    iObservers = new (ELeave) CDownloadMgrObserver( 1 );

    iUpdateSize = CIdle::NewL( CActive::EPriorityLow );

    iDlMgrCallback = CDlMgrTestClass::NewL();

    iDlMgrCallback->AddDownloadMgr( this );
}

RHttpDownloadMgr::~RHttpDownloadMgr()
{
    delete iDownloadArray;
    delete iObservers;

    if ( iUpdateSize->IsActive() )
    {
        iUpdateSize->Cancel();
    }

    delete iUpdateSize;

    delete iDlMgrCallback;
}

EXPORT_C void
RHttpDownloadMgr::ConnectL( TUid /*aAppUid*/, MHttpDownloadMgrObserver& aObserver, TBool /*aMaster*/ )
{
    MPX_DEBUG(_L("RHttpDownloadMgr::ConnectL()"));

    iObservers->AppendL( &aObserver );
}

EXPORT_C TVersion
RHttpDownloadMgr::Version() const
{
    TVersion temp;

    return temp;
}

EXPORT_C void
RHttpDownloadMgr::Close()
{
}

EXPORT_C const CDownloadArray&
RHttpDownloadMgr::CurrentDownloads() const
{
    return (*iDownloadArray);
}

EXPORT_C RHttpDownload&
RHttpDownloadMgr::CreateDownloadL( const TDesC8& /*aUrl*/, TBool& /*aResult*/ )
{
    RHttpDownload* dl = new RHttpDownload( this );

    iDownloadArray->AppendL( dl );

    return *(*iDownloadArray)[0];
}

EXPORT_C RHttpDownload&
RHttpDownloadMgr::CreateDownloadL( const TDesC8& /*aUrl*/ )
{
    RHttpDownload* dl = new RHttpDownload( this );

    iDownloadArray->AppendL( dl );

    return *(*iDownloadArray)[0];
}

EXPORT_C RHttpDownload&
RHttpDownloadMgr::CreateClientSideDownloadL( TInt /*aHttpTransaction*/, TBool& /*aResult*/ )
{
    return *(*iDownloadArray)[0];
}

EXPORT_C RHttpDownload&
RHttpDownloadMgr::CreateCodDownloadL( const TDesC8& /*aUrl*/, const TDesC8& /*aBuf*/, const TDesC8& /*aMimeType*/, CEikonEnv* /*aEikEnv*/, TBool& /*aResult*/ )
{
    return *(*iDownloadArray)[0];
}

EXPORT_C RHttpDownload*
RHttpDownloadMgr::FindDownload( const TDesC8& /*aUrl*/, const TDesC8& /*aMsgBody*/ )
{
    return (*iDownloadArray)[0];
}

EXPORT_C TInt
RHttpDownloadMgr::PauseAll()
{
    MPX_DEBUG(_L("RHttpDownloadMgr::PauseAll()"));

    (*iDownloadArray)[0]->Pause();

    return KErrNone;
}

EXPORT_C TInt
RHttpDownloadMgr::StartAll()
{
    MPX_DEBUG(_L("RHttpDownloadMgr::PauseAll()"));

    (*iDownloadArray)[0]->Start();

    return KErrNone;
}

EXPORT_C TInt
RHttpDownloadMgr::ResetAll()
{
    return KErrNone;
}

EXPORT_C TInt
RHttpDownloadMgr::DeleteAll()
{
    (*iDownloadArray)[0]->Delete();

    return KErrNone;
}

EXPORT_C TInt
RHttpDownloadMgr::Disconnect()
{
    return KErrNone;
}

EXPORT_C TInt
RHttpDownloadMgr::GetIntAttribute( const TUint /*aAttribute*/, TInt32& /*aValue*/ )
{
    return KErrNone;
}

EXPORT_C TInt
RHttpDownloadMgr::GetBoolAttribute( const TUint /*aAttribute*/, TBool& /*aValue*/ )
{
    return KErrNone;
}

EXPORT_C TInt
RHttpDownloadMgr::GetStringAttribute( const TUint /*aAttribute*/, TDes16& /*aValue*/  )
{
    return KErrNone;
}

EXPORT_C TInt
RHttpDownloadMgr::GetStringAttribute( const TUint /*aAttribute*/, TDes8& /*aValue*/ )
{
    return KErrNone;
}

EXPORT_C TInt
RHttpDownloadMgr::SetIntAttribute( const TUint /*aAttribute*/, TInt32 /*aValue*/ )
{
    return KErrNone;
}

EXPORT_C TInt
RHttpDownloadMgr::SetBoolAttribute( const TUint /*aAttribute*/, TBool /*aValue*/ )
{
    return KErrNone;
}

EXPORT_C TInt
RHttpDownloadMgr::SetStringAttribute( const TUint /*aAttribute*/, const TDesC16& /*aValue*/ )
{
    return KErrNone;
}

EXPORT_C TInt
RHttpDownloadMgr::SetStringAttribute( const TUint /*aAttribute*/, const TDesC8& /*aValue*/ )
{
    return KErrNone;
}

EXPORT_C TInt
RHttpDownloadMgr::SetDefaultIntAttribute( const TUint /*aAttribute*/, TInt32 /*aValue*/ )
{
    return KErrNone;
}

EXPORT_C TInt
RHttpDownloadMgr::SetDefaultBoolAttribute( const TUint /*aAttribute*/, TBool /*aValue*/ )
{
    return KErrNone;
}

EXPORT_C TInt
RHttpDownloadMgr::SetDefaultStringAttribute( const TUint /*aAttribute*/, const TDesC16& /*aValue*/ )
{
    return KErrNone;
}

EXPORT_C TInt
RHttpDownloadMgr::SetDefaultStringAttribute( const TUint /*aAttribute*/, const TDesC8& /*aValue*/ )
{
    return KErrNone;
}

EXPORT_C void
RHttpDownloadMgr::AddObserverL( MHttpDownloadMgrObserver& /*aObserver*/ )
{
}

EXPORT_C void
RHttpDownloadMgr::RemoveObserver( MHttpDownloadMgrObserver& /*aObserver*/ )
{
}

EXPORT_C void
RHttpDownloadMgr::SetNextUriObserver( MHttpDownloadMgrNextUriObserver* /*aObserver*/ )
{
}

void
RHttpDownloadMgr::DoUpdateDownloadProgress()
{
    MPX_ENTER_EXIT(_L("RHttpDownloadMgr::DoUpdateDownloadProgress()"));

    RHttpDownload* dl = (*iDownloadArray)[0];

    THttpDownloadEvent event;

    event.iDownloadState = (THttpDownloadState)dl->iState;
    event.iProgressState = EHttpProgResponseBodyReceived;

    MPX_DEBUG(_L("RHttpDownloadMgr::DoUpdateDownloadProgress() dl->iSize=%d"), dl->iSize );

    if ( dl->iState == EHttpDlInprogress )
    {
        if ( dl->iSize >= dl->iLength )
        {
            dl->iState = EHttpDlCompleted;

            event.iDownloadState = (THttpDownloadState)dl->iState;
            event.iProgressState = EHttpProgNone;
        }
    }

    (*iObservers)[0]->HandleDMgrEventL( *dl, event );
}

TInt
RHttpDownloadMgr::UpdateDownloadProgress( TAny* aPtr )
{
    MPX_ENTER_EXIT(_L("RHttpDownloadMgr::UpdateDownloadProgress()"));

    static_cast<RHttpDownloadMgr*>(aPtr)->DoUpdateDownloadProgress();

    return KErrNone;
}

EXPORT_C
RHttpDownloadMgrApiExt::RHttpDownloadMgrApiExt()
{
    MPX_DEBUG(_L("RHttpDownloadMgrApiExt::RHttpDownloadMgrApiExt()"));
}

EXPORT_C RHttpDownload&
RHttpDownloadMgrApiExt::AttachToDownloadL( const TInt32 aDlId )
{
    MPX_DEBUG(_L("RHttpDownloadMgrApiExt::AttachToDownloadL(%d)"), aDlId);

    RHttpDownload* dl = new RHttpDownload( this );

    dl->iErrorId = KErrNone;
    dl->iGlobalError = KErrNone;

    iDlMgrCallback->AttachToDownloadL( aDlId );

    dl->iState   = EHttpDlInprogress;
    dl->iSize    = 25;
    dl->iLength  = 100;

    iDownloadArray->AppendL( dl );

    return *(*iDownloadArray)[0];
}
