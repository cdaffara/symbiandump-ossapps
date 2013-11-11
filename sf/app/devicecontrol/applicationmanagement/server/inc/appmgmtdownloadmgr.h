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

#include <QObject>
#include <qtimeline.h>
#include <QTimer>
#include <QWidget>
#include <hbcombobox.h>
#include <downloadevent.h>
#include <downloadmanager.h>
#include <hbdocumentloader.h>
#include "appmgmtprogdialog.h"
#include "AMDeploymentComponent.h"
#include "appmgmtnotifier.h"
#define  KOmaDMAppUid   0x101F6DE5
const TReal KSizeDivisor = 1024.0;

const TUint32 KDownloadActive = 0x0000008;
enum TDriveSize
    {
    EKb = -1,
    EMb = 0,
    EGb = 1
    };

using namespace WRT;

namespace NApplicationManagement
    {

    class MDownloadMgrQTCallback
        {
    public:
        virtual void DownloadCompleted()=0;
        virtual void DownloadFailed(TInt aDownloaderr)=0;
        virtual void SetStatus(TInt aStatus)=0;
        };
    
    class CDialogWait : public CActive
        {
    public:
        static CDialogWait* NewL();
        CDialogWait();
        void ConstructL();
        ~CDialogWait();
        void StartWait();
        void Stop();
    protected:
        void DoCancel();
        void RunL();
        TInt RunError(TInt aError);
    private:
        CActiveSchedulerWait* iWait;
        };
    
    class appmgmtdownloadmgr : public QWidget
        {

    Q_OBJECT

    public:
        appmgmtdownloadmgr(QWidget* parent, MDownloadMgrQTCallback& wrapper, TRequestStatus& aStatus);
        ~appmgmtdownloadmgr();
        
        void startDownload(CDeploymentComponent *aComponent, int aIapid);
        void showDialog(CDeploymentComponent *aComponent);
        void showUninstallDialog(const CDeploymentComponent &aCompo, TRequestStatus &s);
        void updateProgress(TInt32 aDownloadedSize, TInt32 aContentSize);
        void closeProgress();
        void setUri();
        void getUri();
        void setState();
        void getState();
        void MimeType(QString& aMimeType);
        void FileName(QString& afilename);
	  void SetMimeType();
        void sendSrvToBg();
        QString convertSize();
        int currDwnldStatForGenericAlert(int dwnldstatus);
        QString GetIapNameWithIdL(TInt aIapId);
        
    public slots:
        bool downloadMgrEventRecieved(DownloadManagerEvent *event);
        bool downloadEventRecieved(DownloadEvent *event);
        void dialogSlot(HbAction*);
        void defaultDriveChanged(int);
    private:
        void getAvailableDrives();
        void showInstalltoDialog();
        void showDownloadDialog();
    private:
        QStringList m_drivList;
        QString m_appName;
        HbAction* mOkButton;
        HbAction* mCancel;
        QString mSize;
        QString mSizeFormat;
        QString mNotifierdata, mDialogData;
        QString m_sizeStr;
        QString m_mimetype;
        HbComboBox* mDriveBox;
        HbDialog* mDialog;
    public:
        // Functions from base classes
        TBuf<256> drilist;
        DownloadManager* iDownloadManager;
        Download* iDl;
        int m_inProgress;
        int m_currentDownloadedSize;
        int m_totalSize;
        CDialogWait* iWait;
        TUint32 iProgStarted;
        TUint32 iSetFinalValue;
		int m_downloadStatus;
        AppMgmtProgDialog *iProgressNote;
        TDownloadTarget iOperationType;
    private:
        // Data
        MDownloadMgrQTCallback& m_callback;
        CDeploymentComponent* iComp;
        HBufC *iFileName;
        HBufC8 *iContentType;
        HBufC8 *iURI;
        TBool iProgressCancelled;
        TBool iDialogAccepted;
        TRequestStatus& iStatus;
        AppMgmtNotifier* mNote;
        int mEndReported;
        int mNetworkLoss;
        };

    }
