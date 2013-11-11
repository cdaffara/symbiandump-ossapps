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
* Description:  API extensions for RHttpDownloadMgr Stub
 *
*/

// Version : %version: 5 %

#ifdef USE_S60_DOWNLOAD_MANAGER 

#ifndef __RHTTPDOWNLOADMGRAPIEXT_H__
#define __RHTTPDOWNLOADMGRAPIEXT_H__


#include <e32base.h>
#include <f32file.h>
#include <eikenv.h>
#include <httpdownloadmgrcommon.h>

#include "videohelixtest.h"

class RHttpDownloadMgr;

class CDlMgrTestClass : public CBase
{
    public:
        static CDlMgrTestClass* NewL();
        virtual ~CDlMgrTestClass();

        void AddStifObserver( MStifTestObserver* aStifObserver );
        void AttachToDownloadL( const TInt32 aDlId );

        void AddDownloadMgr( RHttpDownloadMgr* aDlMgr );

        void PauseDownload();
        void ResumeDownload();
        void CancelDownload();

    private:
        CDlMgrTestClass();
        void ConstructL();

    private:
        MStifTestObserver* iStifObserver;
        RHttpDownloadMgr*  iDlMgr;
};

class RHttpDownload : public CBase
{
    public:

        IMPORT_C TInt SetBoolAttribute( const TUint aAttribute, TBool aValue );
        IMPORT_C TInt GetBoolAttribute( const TUint aAttribute, TBool& aValue );

        IMPORT_C TInt GetStringAttribute( const TUint aAttribute, TDes16& aValue  );
        IMPORT_C TInt SetStringAttribute( const TUint aAttribute, const TDesC16& aValue );

        IMPORT_C TInt GetStringAttribute( const TUint aAttribute, TDes8& aValue  );
        IMPORT_C TInt SetStringAttribute( const TUint aAttribute, const TDesC8& aValue );

        IMPORT_C TInt GetIntAttribute( const TUint aAttribute, TInt32& aValue );
        IMPORT_C TInt SetIntAttribute( const TUint aAttribute, TInt32 aValue );

        IMPORT_C TInt GetFileHandleAttribute( RFile& aFile );
        IMPORT_C TInt SetFileHandleAttribute( RFile& aFile );

        IMPORT_C TInt Start();

        IMPORT_C TInt Pause();

        IMPORT_C TInt Delete();

        IMPORT_C TInt Move();

        IMPORT_C TInt Reset();

        RHttpDownload( RHttpDownloadMgr* aServer );

        ~RHttpDownload();

    public:

        TInt iLength;
        TInt iState;
        TInt iSize;
        TInt iErrorId;
        TInt iGlobalError;

        TBool iProgressive;

        HBufC* iLocalFile;
        HBufC* iDestFile;

        RHttpDownloadMgr*  iServer;
};

NONSHARABLE_CLASS( THttpDownloadEvent )
{
    public:
        THttpDownloadState iDownloadState;
        THttpProgressState iProgressState;
};


class MHttpDownloadMgrObserver
{
    public:
        virtual void HandleDMgrEventL( RHttpDownload& aDownload, THttpDownloadEvent aEvent ) = 0;
};


class MHttpDownloadMgrNextUriObserver
{
    public:
        virtual void NextUriL( RHttpDownload& aDownload, const TDesC8& aUri ) = 0;
};


typedef CArrayPtrFlat<RHttpDownload> CDownloadArray;
typedef CArrayPtrFlat<MHttpDownloadMgrObserver>  CDownloadMgrObserver;

class RHttpDownloadMgr : public CBase
{
    public:

        IMPORT_C RHttpDownloadMgr();
        IMPORT_C void ConnectL( TUid aAppUid, MHttpDownloadMgrObserver& aObserver, TBool aMaster );
        IMPORT_C TVersion Version() const;
        IMPORT_C void Close();
        IMPORT_C const CDownloadArray& CurrentDownloads() const;
        IMPORT_C RHttpDownload& CreateDownloadL( const TDesC8& aUrl, TBool& aResult );
        IMPORT_C RHttpDownload& CreateDownloadL( const TDesC8& aUrl );
        IMPORT_C RHttpDownload& CreateClientSideDownloadL( TInt aHttpTransaction,TBool& aResult );
        IMPORT_C RHttpDownload& CreateCodDownloadL( const TDesC8& aUrl, const TDesC8& aBuf, const TDesC8& aMimeType, CEikonEnv* aEikEnv, TBool& aResult );
        IMPORT_C RHttpDownload* FindDownload( const TDesC8& aUrl, const TDesC8& aMsgBody );
        IMPORT_C TInt PauseAll();
        IMPORT_C TInt StartAll();
        IMPORT_C TInt ResetAll();
        IMPORT_C TInt DeleteAll();
        IMPORT_C TInt Disconnect();
        IMPORT_C TInt GetIntAttribute( const TUint aAttribute, TInt32& aValue );
        IMPORT_C TInt GetBoolAttribute( const TUint aAttribute, TBool& aValue );
        IMPORT_C TInt GetStringAttribute( const TUint aAttribute, TDes16& aValue  );
        IMPORT_C TInt GetStringAttribute( const TUint aAttribute, TDes8& aValue  );
        IMPORT_C TInt SetIntAttribute( const TUint aAttribute, TInt32 aValue );
        IMPORT_C TInt SetBoolAttribute( const TUint aAttribute, TBool aValue );
        IMPORT_C TInt SetStringAttribute( const TUint aAttribute, const TDesC16& aValue );
        IMPORT_C TInt SetStringAttribute( const TUint aAttribute, const TDesC8& aValue );
        IMPORT_C TInt SetDefaultIntAttribute( const TUint aAttribute, TInt32 aValue );
        IMPORT_C TInt SetDefaultBoolAttribute( const TUint aAttribute, TBool aValue );
        IMPORT_C TInt SetDefaultStringAttribute( const TUint aAttribute, const TDesC16& aValue );
        IMPORT_C TInt SetDefaultStringAttribute( const TUint aAttribute, const TDesC8& aValue );
        IMPORT_C void AddObserverL( MHttpDownloadMgrObserver& aObserver );
        IMPORT_C void RemoveObserver( MHttpDownloadMgrObserver& aObserver );
        IMPORT_C void SetNextUriObserver( MHttpDownloadMgrNextUriObserver* aObserver );

        ~RHttpDownloadMgr();

        static TInt UpdateDownloadProgress( TAny* aPtr );
        void DoUpdateDownloadProgress();

    public:

        CDownloadArray*        iDownloadArray;
        CDownloadMgrObserver*  iObservers;
        CIdle*                 iUpdateSize;
        CDlMgrTestClass*       iDlMgrCallback;
};


class RHttpDownloadMgrApiExt : public RHttpDownloadMgr
{
    public:

        IMPORT_C RHttpDownloadMgrApiExt();
        IMPORT_C RHttpDownload& AttachToDownloadL( const TInt32 aDlId );
};

#endif // __RHTTPDOWNLOADMGRAPIEXT_H__

#endif // USE_S60_DOWNLOAD_MANAGER 
