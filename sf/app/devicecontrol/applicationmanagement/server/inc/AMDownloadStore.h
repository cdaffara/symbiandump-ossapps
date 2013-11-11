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
 * Description: Implementation of applicationmanagement components
 *
 */

#ifndef __CAMDOWNLOADSTORE_H__
#define __CAMDOWNLOADSTORE_H__

#include <e32base.h>
#include <qobject.h>
#include "AMDeploymentComponent.h"
#include "appmgmtdownloadmgr.h"


namespace NApplicationManagement
    {

    
    // FORWARD DECLARATIONS
    class CDeploymentComponent;
    class CAMDwnldQTWrap;
    
    
    class MDownloadMngrObserver
          {
    public:
          virtual void ComponentDownloadComplete(
                  class CDeploymentComponent *aComponent, TInt iapid,
                  HBufC8* aServerId) = 0;
          virtual TBool HasUidL(const TUid &aUid,
                  CDeploymentComponent *& aCompo,
                  CDeploymentComponent *aIgnored) = 0;
          };
    
    

    class CAMDownloadStore : public CActive
        {

    private:
        /**
         * C++ default constructor.
         */
        CAMDownloadStore(MDownloadMngrObserver& aObserver);

        /**
         * By default Symbian 2nd phase constructor is private.
         */
        void ConstructL();

    public:
        // Constructors and destructor

        /**
         * Two-phased constructor.
         */
        static CAMDownloadStore* NewL(MDownloadMngrObserver& aObserver);
        /**
         * Destructor.
         */
        virtual ~CAMDownloadStore();

    public:

        void AddDownloadL(CDeploymentComponent *aComponent);
        
        void RunL();
        
        void DoCancel();

        void StartDownload();
        
        TInt DownloadCount();

        RPointerArray<CAMDwnldQTWrap> iDwnldStore;
        MDownloadMngrObserver& iObserver;

        };

    class CAMDwnldQTWrap : public CBase, public MDownloadCallback, public MDownloadMgrQTCallback
        {
    private:
        /**
         * C++ default constructor.
         */
        CAMDwnldQTWrap(MDownloadMngrObserver& aObserver, CDeploymentComponent *aComponent);

        /**
         * By default Symbian 2nd phase constructor is private.
         */
        void ConstructL();

    public:
        // Constructors and destructor

        /**
         * Two-phased constructor.
         */
        static CAMDwnldQTWrap* NewL(MDownloadMngrObserver& aObserver, CDeploymentComponent *aComponent);
        /**
         * Destructor.
         */
        virtual ~CAMDwnldQTWrap();
        
    public:
        void StartDownload(CDeploymentComponent *aComponent, TRequestStatus& aStatus);
        
        TInt getPendingDownloads();


    public:
        void DownloadCompleteL(CDeploymentComponent *aComponent);
	  void DownloadCompleted();
	  void DownloadFailed(TInt aDownloaderr);
	  void SetStatus(TInt aStatus);
        
        
    public:
        void DownloadComplete(class CDeploymentComponent *aComponent,
                TInt aStatus);
        TBool UidExistsL(const TUid &aUid, CDeploymentComponent *& aCompo,
                CDeploymentComponent *aIgnored);
        
    public:
        appmgmtdownloadmgr* appdwnld;
        MDownloadMngrObserver& iObserver;
        CDeploymentComponent *iComponent;
        HBufC8* serverid;
	  TInt iap;
        };

    }

#endif
