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

#include <nsmldmconst.h>
#include <downloadmanager.h>
#include <download.h>
#include <serviceipc.h>
#include <hbdialog.h>
#include <hblabel.h>
#include <hbmessagebox.h>
#include <hbapplication.h>
#include <hbdevicenotificationdialog.h>
#include <hbaction.h>
#include <apgtask.h>
#include <coemain.h>
#include <e32base.h>
#include <eikenv.h>
#include <driveinfo.h>
#include <e32property.h>
#include "ApplicationManagementUtility.h"
#include "debug.h"
#include "dmcommon.h"
#include "amstorage.h"
#include "appmgmtnotifier.h"
#include "appmgmtdownloadmgr.h"
#include "ServicePluginLogger.h"

#include <cmmanager.h>
#include <cmconnectionmethod.h>
using namespace NApplicationManagement;

// ------------------------------------------------------------------------------------------------
// appmgmtdownloadmgr::appmgmtdownloadmgr
// ------------------------------------------------------------------------------------------------ 
appmgmtdownloadmgr::appmgmtdownloadmgr(QWidget* parent,
        MDownloadMgrQTCallback& callback,TRequestStatus& aStatus) :
    QWidget(parent), m_callback(callback), iStatus(aStatus)
    {
    iProgressNote = NULL;
    iSetFinalValue = 0;
    iProgressCancelled = 0;
    iDialogAccepted = 0;
    mEndReported = 0;
    mNetworkLoss = 0;
    }

// ------------------------------------------------------------------------------------------------
// appmgmtdownloadmgr::~appmgmtdownloadmgr
// ------------------------------------------------------------------------------------------------ 
appmgmtdownloadmgr::~appmgmtdownloadmgr()
    {
    if (iProgressNote)
        {
        iProgressNote->closeAMProgDialog();
        delete iProgressNote;
        iProgressNote = 0;
        }
    if(mNote)
        {
        delete mNote;
        mNote=0;
        }
    
    delete iFileName;
    
    delete iContentType;

    delete iURI;
    delete iComp;
    }

// ------------------------------------------------------------------------------------------------
// appmgmtdownloadmgr::startDownload
// ------------------------------------------------------------------------------------------------ 
void appmgmtdownloadmgr::startDownload(CDeploymentComponent *aComponent, int aIapid)
    {

    RDEBUG( "appmgmtdownloadmgr::startDownload Start" );

    QString m_appName = "ApplicationManagementServer";

    iComp = aComponent;
    iDownloadManager = new DownloadManager(m_appName);
    // initialise the downloads of the last session

    connect(iDownloadManager,
            SIGNAL(downloadManagerEvent(DownloadManagerEvent *)), this,
            SLOT(downloadMgrEventRecieved(DownloadManagerEvent *)));

    iDownloadManager->initialize();

    DownloadType m_type = Parallel;

    DownloadPriority m_priority = High;
    #ifdef __WINS__
        iDownloadManager->setProxy("bswebproxy01.americas.nokia.com", 8080);
    #endif
    TBuf<KMaxUrlLength> buf;
    buf.Copy(aComponent->DownloadURI());

    QString url((QChar*) buf.Ptr(), buf.Length());
    
    TInt silentsession = 0;
    RProperty::Get(KPSUidNSmlSOSServerKey,KNSmlDMSilentJob,silentsession);
    
    CApplicationManagementUtility::iSilentSession = silentsession;
    
    QString name = GetIapNameWithIdL(aIapid);
    
    int ret = iDownloadManager->setAttribute(AccessPoint, name);

    iDl = iDownloadManager->createDownload(url, m_type);

    bool t = connect(iDl, SIGNAL(downloadEvent(DownloadEvent *)), this,
            SLOT(downloadEventRecieved(DownloadEvent *)));

    if (!iDl)
        return;

    QString path("c:\\system\\appmgr\\");

    iDl->setAttribute(Priority, m_priority);
    iDl->setAttribute(DestinationPath, path);
    iDl->start();

    iOperationType = aComponent->DownloadTarget();

    RDEBUG( "appmgmtdownloadmgr::startDownload End" );
    }

// ------------------------------------------------------------------------------------------------
// appmgmtdownloadmgr::convertSize
// ------------------------------------------------------------------------------------------------ 
QString appmgmtdownloadmgr::convertSize()
    {
    RDEBUG( "appmgmtdownloadmgr::convertSize Start" );
    TBuf<64> size;
    TReal filesize;
    TRealFormat realFormat;
    TInt64 fileKBytes;
    realFormat.iWidth = 6;
    
    //converting size to Kb
    fileKBytes = m_totalSize / KSizeDivisor;
    filesize = (TReal) m_totalSize / (KSizeDivisor);
    size.Num(fileKBytes);
    
    //get the length of the value in Kb before the decimal point.
    //Accordingly convert it further.
    int len = size.Length();
    QString text;
    if (len >= 7)
        {
        //in gb
        TReal mul = (KSizeDivisor * KSizeDivisor);
        TReal temp = filesize / mul;
        size.Num(temp, realFormat);
        mSize = QString::fromUtf16(size.Ptr(), size.Length());
        text = hbTrId("txt_deviceupdate_info_size_1_Gb").arg(mSize);
        mSizeFormat = "gb";
        }
    else if (len >= 4)
        {
        //in mb
        TReal temp = filesize / KSizeDivisor;
        size.Num(temp, realFormat);
        mSize = QString::fromUtf16(size.Ptr(), size.Length());
        text = hbTrId("txt_deviceupdate_info_size_1_mb").arg(mSize);
        mSizeFormat = "mb";
        }
    else
        {
        mSize = QString::fromUtf16(size.Ptr(), size.Length());
        text = hbTrId("txt_deviceupdate_info_size_1_kb").arg(mSize);
        mSizeFormat = "kb";
        }
    RDEBUG( "appmgmtdownloadmgr::convertSize End" );
    return text;
    }

// ------------------------------------------------------------------------------------------------
// appmgmtdownloadmgr::getAvailableDrives()
// ------------------------------------------------------------------------------------------------ 
void appmgmtdownloadmgr::getAvailableDrives()
    {
    RDEBUG( "appmgmtdownloadmgr::getAvailableDrives start" );
    RFs iFs;
    qt_symbian_throwIfError(iFs.Connect());
    TDriveList driveList;
    TInt driveCount = 0;
    TBuf<20> buf;
    int i = 0;
    QString val;
    QStringList list;
    TDriveInfo info;
    TVolumeInfo volumeInfo;
    qt_symbian_throwIfError(DriveInfo::GetUserVisibleDrives(iFs, driveList,
            driveCount));
    TUint driveStatus = 0;
    for (TInt index(0); index < KMaxDrives; index++)
        {
        if (driveList[index])
            {
            TRealFormat realFormat;
            qt_symbian_throwIfError(DriveInfo::GetDriveStatus(iFs, index,
                    driveStatus));
            TFileName path;
            TDriveUnit driveUnit(index);
            if (!(driveStatus & DriveInfo::EDriveRemote))
                {
                TBuf<64> freeSpace;
                TReal free;
                TInt64 freeKBytes;
                realFormat.iWidth = 6;
                iFs.Drive(info, index);
                if(iFs.Volume(volumeInfo, index)==KErrNone)
                    {
                RDEBUG_2("free space: (%d)", volumeInfo.iFree  );
                freeKBytes = 0;
                freeKBytes = volumeInfo.iFree / KSizeDivisor;
                free = volumeInfo.iFree / KSizeDivisor;
                freeSpace.Num(freeKBytes);
                TInt sizeUnit = EKb;

                //converting the drive freespace
                int len = freeSpace.Length();
                if (len >= 7)
                    {
                    TReal nul = (KSizeDivisor * KSizeDivisor);
                    TReal temp = free / nul;
                    freeSpace.Num(temp, realFormat);
                    sizeUnit = EGb;
                    }
                else
                    {
                    TReal temp = free / KSizeDivisor;
                    freeSpace.Num(temp, realFormat);
                    sizeUnit = EMb;
                    }
                //Getting the drive name in QString
                path.Zero();
                path.Append(driveUnit.Name());
                val = QString::fromUtf16(path.Ptr(), path.Length());
                drilist.Append(path);
                //Getting the drive free space in QString
                path.Zero();
                path.Append(freeSpace);
                QString str1 = QString::fromUtf16(path.Ptr(), path.Length());
                QString driv;
                if (sizeUnit == EKb)
                    {
                    driv
                            = hbTrId(
                                    "txt_deviceupdate_setlabel_install_to_val_1_2_kb").arg(
                                    val, str1);
                    }
                else if (sizeUnit == EMb)
                    {
                    driv
                            = hbTrId(
                                    "txt_deviceupdate_setlabel_install_to_val_1_2_Mb").arg(
                                    val, str1);
                    }
                else
                    {
                    driv
                            = hbTrId(
                                    "txt_deviceupdate_setlabel_install_to_val_1_2_Gb").arg(
                                    val, str1);
                    }
                    mDriveBox->insertItem(i++, driv);
                    }
                }
            }
        }
    RDEBUG( "appmgmtdownloadmgr::getAvailableDrives End" );
    }

// ------------------------------------------------------------------------------------------------
// appmgmtdownloadmgr::showInstalltoDialog()
// ------------------------------------------------------------------------------------------------ 
void appmgmtdownloadmgr::showInstalltoDialog()
    {
    RDEBUG( "appmgmtdownloadmgr::showIntalltoDialog start" );
    HbDocumentLoader loader;
    QString val;
    bool ok = false;
    HbLabel* label;
        
    loader.load(":/xml/sample.docml", &ok);
    if (!ok)
        {
        return;
        }
    Q_ASSERT_X(ok, "Device Manager", "Invalid docml file");
    mDialog = qobject_cast<HbDialog*> (loader.findWidget("dialog"));

    //set title for the dialog
    label = qobject_cast<HbLabel*> (loader.findWidget(
            "qtl_dialog_pri_heading"));
    if (iOperationType == EInstallAndActivate)
        {
        val = hbTrId("txt_device_update_title_download_and_install");
        label->setPlainText(val);
        }
    else
        {
        val = hbTrId("txt_device_update_title_install");
        label->setPlainText(val);
        }

    //set the app name
    label = qobject_cast<HbLabel*> (loader.findWidget("label"));
    val = hbTrId("txt_deviceupdate_info_file_1_2").arg(m_appName);
    label->setPlainText(val);

    //size
    label = qobject_cast<HbLabel*> (loader.findWidget("label_1"));
    if (m_totalSize != 0)
        {
        m_sizeStr = convertSize();
        label->setPlainText(m_sizeStr);
        }
    else
        {
        val = hbTrId("txt_device_update_info_details_not_available");
        label->setPlainText(val);
        }

    //icon
    label = qobject_cast<HbLabel*> (loader.findWidget("icon"));
    HbIcon* icon = new HbIcon(":/icon/qgn_prop_sml_http.svg");
    label->setIcon(*icon);

    //"install to" label
    label = qobject_cast<HbLabel*> (loader.findWidget("qtl_dialog_pri5"));
    val = hbTrId("txt_device_update_setlabel_install_to");
    label->setPlainText(val);

    //getting the available free space in all drives and adding to combobox

    mDriveBox = qobject_cast<HbComboBox*> (loader.findWidget("combobox"));
    int err(KErrNone);
    TRAP(err,QT_TRYCATCH_LEAVING(getAvailableDrives()));
    
    if(err!=KErrNone)
        {
        mDriveBox->setVisible(false);
        }
    iComp->SetDriveSelectionStatus(true);
    CDeliveryComponentStorage* iStorage;
    QT_TRAP_THROWING(iStorage = CDeliveryComponentStorage::NewL());
    TRAP( err, QT_TRYCATCH_LEAVING(iStorage->UpdateL(*iComp)));
    delete iStorage;
    iStorage = NULL;
    
    connect(mDriveBox, SIGNAL(currentIndexChanged(int)), this,
            SLOT(defaultDriveChanged(int )));
    //setting the actions
    mOkButton = qobject_cast<HbAction*> (loader.findObject(
            "qtl_dialog_softkey_2_left"));
    mCancel = qobject_cast<HbAction*> (loader.findObject(
            "qtl_dialog_softkey_2_right"));
    val = hbTrId("txt_common_button_ok");
    mOkButton->setText(val);
    val = hbTrId("txt_common_button_cancel");
    mCancel->setText(val);
    RDEBUG( "appmgmtdownloadmgr::showIntalltoDialog End" );
    }

// ------------------------------------------------------------------------------------------------
// appmgmtdownloadmgr::showDownloadDialog()
// ------------------------------------------------------------------------------------------------ 
void appmgmtdownloadmgr::showDownloadDialog()
    {
    RDEBUG( "appmgmtdownloadmgr::showDownloadDialog start" );
    HbDocumentLoader loader;
    QString val;
    bool ok = false;
    HbLabel* label;

    loader.load(":/xml/downloaddialog.docml", &ok);

    Q_ASSERT_X(ok, "Device Manager", "Invalid docml file");
    mDialog = qobject_cast<HbDialog*> (loader.findWidget("dialog"));

    //set title for the dialog
    label = qobject_cast<HbLabel*> (loader.findWidget(
            "qtl_dialog_pri_heading"));
    val = hbTrId("txt_device_update_title_download");
    label->setPlainText(val);

    //set icon
    label = qobject_cast<HbLabel*> (loader.findWidget("icon"));
    HbIcon* icon = new HbIcon(":/icon/qgn_prop_sml_http.svg");
    label->setIcon(*icon);

    label = qobject_cast<HbLabel*> (loader.findWidget("qtl_dialog_pri5"));

    //set appname
    val = hbTrId("txt_deviceupdate_info_file_1_2").arg(m_appName);
    label->setPlainText(val);

    //size
    label = qobject_cast<HbLabel*> (loader.findWidget("qtl_dialog_pri5_1"));

    //size
    if (m_totalSize != 0)
        {
        m_sizeStr = convertSize();
        label->setPlainText(m_sizeStr);
        }
    else
        {
        val = hbTrId("txt_device_update_info_details_not_available");
        label->setPlainText(val);
        }
    //setting the actions
    mOkButton = qobject_cast<HbAction*> (loader.findObject(
            "qtl_dialog_softkey_2_left"));
    mCancel = qobject_cast<HbAction*> (loader.findObject(
            "qtl_dialog_softkey_2_right"));
    val = hbTrId("txt_common_button_ok");
    mOkButton->setText(val);
    val = hbTrId("txt_common_button_cancel");
    mCancel->setText(val);
    RDEBUG( "appmgmtdownloadmgr::showDownloadDialog End" );
    }

// ------------------------------------------------------------------------------------------------
// appmgmtdownloadmgr::showDialog
// ------------------------------------------------------------------------------------------------ 
void appmgmtdownloadmgr::showDialog(CDeploymentComponent *aComponent)
    {
    CCoeEnv* coe = CCoeEnv::Static();
    TApaTaskList taskList(coe->WsSession());
    TApaTask task1(taskList.FindApp(TUid::Uid(KAppMgmtServerUid)));
    if (task1.Exists())
        {
        task1.BringToForeground();
        }

    

    //get the Size of the install package
    m_totalSize = iDl->attribute(TotalSize).toInt();

    //get the name of the install package
    TDeploymentComponentName name = aComponent->ComponentName();
    TBuf<KDeploymentComponentNameMaxLength> nameBuf;
    nameBuf.Copy(name);
    m_appName = QString::fromUtf16(nameBuf.Ptr(), nameBuf.Length());

    (iOperationType == EInstallAndActivate)?showInstalltoDialog():showDownloadDialog();

    mDialog->setTimeout(20000);
    mNotifierdata = m_appName;
    QString comma(",");
    mNotifierdata.append(comma);
    mNotifierdata.append(mSize);
    mNotifierdata.append(comma);
    mNotifierdata.append(mSizeFormat);
    
    mDialogData = m_appName;
    mDialogData.append(comma);
    mDialogData.append(m_sizeStr);
    
    CApplicationManagementUtility::mCurrDlg = mDialog;
    mDialog->setAttribute(Qt::WA_DeleteOnClose);
    mDialog->open(this,SLOT(dialogSlot(HbAction*)));
    }

// ------------------------------------------------------------------------------------------------
// appmgmtdownloadmgr::defaultDriveChanged
// ------------------------------------------------------------------------------------------------ 
void appmgmtdownloadmgr::defaultDriveChanged(int i)
    {
    TAMInstallOptions opts = iComp->InstallOpts();
    int err = KErrNone;
    TChar drive = drilist[i*2];
    drive.LowerCase();
    opts.iOptions.iDrive = drive;
    TPtrC8 buf;
    TAMInstallOptionsPckg optsb(opts);
    buf.Set(optsb);
    TRAP( err, QT_TRYCATCH_LEAVING(iComp->SetInstallOptsL(buf)));
    CDeliveryComponentStorage* iStorage;
    TRAP( err, QT_TRYCATCH_LEAVING(iStorage = CDeliveryComponentStorage::NewL()));
    TRAP( err, QT_TRYCATCH_LEAVING(iStorage->UpdateL(*iComp)));
    delete iStorage;
    iStorage = NULL;
    }

// ------------------------------------------------------------------------------------------------
// appmgmtdownloadmgr::dialogSlot
// ------------------------------------------------------------------------------------------------ 
void appmgmtdownloadmgr::dialogSlot(HbAction* recAction)
    {
    RDEBUG("dialogslot");
    TRequestStatus* stat = &iWait->iStatus;
    User::RequestComplete(stat,KErrNone);
    if(recAction == mCancel)
        {
    mEndReported = 1;
        sendSrvToBg();
        iWait->Stop();
        iDl->cancel();
        }
    else
        {
        //visited if ok is selected or after timeout
        iDialogAccepted = 1;      
        int err = RProperty::Set(TUid::Uid(KOmaDMAppUid), KDownloadActive, iDialogAccepted);
        RDEBUG_2("RProperty set (%d)", err  );
        }  
    CApplicationManagementUtility::mCurrDlg = 0;
    mOkButton=0;
    mCancel = 0;
    RDEBUG("dialogslot end");
    }

// ------------------------------------------------------------------------------------------------
// appmgmtdownloadmgr::downloadMgrEventRecieved(
// ------------------------------------------------------------------------------------------------ 
bool appmgmtdownloadmgr::downloadMgrEventRecieved(DownloadManagerEvent *event)
    {
    //DEventType type = (DEventType)event->type();
    //bool eventHandled = false;
    switch (event->type())

        {
        case DownloadManagerEvent::Created:
            {
            RDEBUG("DownloadCreated");
            return true;
            }
        case DownloadManagerEvent::Removed:
            {
            RDEBUG("Removed");
            return true;
            }

        default:
            {
            //QWidget::event(event);
            return false;
            }
        }
    }

// ------------------------------------------------------------------------------------------------
// appmgmtdownloadmgr::FileName
// ------------------------------------------------------------------------------------------------ 
void appmgmtdownloadmgr::FileName(QString& filename)
    {
    if (iDl)
        filename = iDl->attribute(WRT::FileName).toString();
    }

// ------------------------------------------------------------------------------------------------
// appmgmtdownloadmgr::MimeType
// ------------------------------------------------------------------------------------------------ 
void appmgmtdownloadmgr::MimeType(QString& mimetype)
    {
    	mimetype=m_mimetype;
    	TBuf<1024> buffer(mimetype.utf16());
	RDEBUG_2( "appmgmtdownloadmgr::MimeType '%S'", &buffer );
    }
void appmgmtdownloadmgr::SetMimeType()
   {
    if (iDl)
      m_mimetype  = iDl->attribute(ContentType).toString();
	TBuf<1024> buffer(m_mimetype.utf16());
	RDEBUG_2( "appmgmtdownloadmgr::MimeType SetMimeType '%S'", &buffer );
    }

// ------------------------------------------------------------------------------------------------
// appmgmtdownloadmgr::downloadEventRecieved
// ------------------------------------------------------------------------------------------------ 
bool appmgmtdownloadmgr::downloadEventRecieved(DownloadEvent *event)
    {
    //DEventType type = (DEventType)event->type();
    bool eventHandled = true;
    switch (event->type())
        {
        case DownloadEvent::Started:
            {
            RDEBUG("Started");
            m_inProgress = false;
            break;
            }

        case DownloadEvent::HeadersReceived:
            {
            RDEBUG("HeaderReceived");
            m_inProgress = false;
		SetMimeType();
            break;
            }

        case DownloadEvent::InProgress:
            {
            RDEBUG("Progress");

            //HandleAlreadyRunningL(downloadedsize, contentsize, *download, aEvent.iProgressState);
            if (!iDialogAccepted)
                {
                if (!CApplicationManagementUtility::iSilentSession)
                    {
                    showDialog(iComp);
                    int err = KErrNone;
                    TRAP( err, QT_TRYCATCH_LEAVING(iWait = CDialogWait::NewL()));
                    iWait->StartWait();                  
                    }
                }
            else
                {
                m_totalSize = iDl->attribute(TotalSize).toInt();
                RDEBUG_2("total size = %d",m_totalSize );
                m_currentDownloadedSize
                        = iDl->attribute(DownloadedSize).toInt();
                RDEBUG_2("m_currentDownloadedSize = %d",m_currentDownloadedSize );
                if (!CApplicationManagementUtility::iSilentSession)
                    updateProgress(m_currentDownloadedSize, m_totalSize);
                }
            break;
            }
        case DownloadEvent::Paused:
            {
            RDEBUG("Paused");
            break;
            }
        case DownloadEvent::Completed:
            {
            RDEBUG("Completed");
            m_inProgress = false;
            closeProgress();
            QApplication::processEvents();
            if (iOperationType == EDeliver && !CApplicationManagementUtility::iSilentSession)
                {
                mNote = new AppMgmtNotifier(m_appName);
				if(mNote)
				    mNote->showDownloadSuccessNote();
                delete mNote;
                mNote=NULL;
                }
            m_callback.DownloadCompleted();
            iStatus = KErrNone;
            break;
            }

        case DownloadEvent::Failed:
            {
            RDEBUG("Failed");
            m_inProgress = false;
            closeProgress();
            if(!CApplicationManagementUtility::iSilentSession)
                {
            mNote = new AppMgmtNotifier(m_appName);
			if(mNote)
			{
            bool ret = mNote->showDownloadFailedNote(mNotifierdata);
            RDEBUG_2( "device dialog : %d", ret );
            delete mNote;
            mNote=NULL;
                    }
			}

            break;
            }
        case DownloadEvent::Cancelled:
            {
            RDEBUG("Cancelled");
            m_inProgress = false;

            closeProgress();
            if(!mNetworkLoss)
                {
                m_callback.DownloadFailed(KStatusUserCancelled);
                }
            else
                {
                m_callback.DownloadFailed(KStatusDownloadFailed);
                }
            iStatus = KErrNone;
            break;
            }
        case DownloadEvent::ConnectionNeeded:
        case DownloadEvent::ConnectionDisconnected:
        case DownloadEvent::NetworkLoss:
            {
            RDEBUG("NetworkLoss");
            m_inProgress = false;
            int errornum;
            int statusdwnld;
			closeProgress();
            if(!mEndReported)
                {
                mNetworkLoss = 1;
                mEndReported = 1;
                if (!CApplicationManagementUtility::iSilentSession)
                    {
                mNote = new AppMgmtNotifier(m_appName);
                if(mNote)
                    mNote->showDownloadFailedNote(mNotifierdata);
                delete mNote;
                mNote=NULL;
                    }
                iDl->cancel();
                }
            break;
            }

        case DownloadEvent::Error:
            {
            RDEBUG("Error");
            m_inProgress = false;

            int errornum;
            int statusdwnld;
            
            Download::State state =
                    (Download::State) iDl->attribute(State).toInt();

            errornum = iDl->attribute(LastError).toInt();
            
            statusdwnld = currDwnldStatForGenericAlert(errornum);
			closeProgress();
            if(!mEndReported)
                {
                if (!CApplicationManagementUtility::iSilentSession)
                    {
                mNote = new AppMgmtNotifier(m_appName);
                if(mNote)
                   mNote->showDownloadFailedNote(mNotifierdata);
                delete mNote;
                mNote=NULL;
                    }
                m_callback.DownloadFailed(statusdwnld);
                mEndReported = 1;
                }
            iStatus = KErrNone;
            break;
            }

        default:
            {
            eventHandled = false;
            }
        }
    return eventHandled;
    }

// ------------------------------------------------------------------------------------------------
// appmgmtdownloadmgr::updateProgress
// ------------------------------------------------------------------------------------------------ 
void appmgmtdownloadmgr::updateProgress(TInt32 aDownloadedSize,
        TInt32 aContentSize)
    {       
    if (!iProgressNote)
        {
        iProgressNote = new AppMgmtProgDialog(mDialogData,*iDl,mEndReported); 
        iProgressNote->startDialog(aContentSize, aDownloadedSize);
        }

    //handles the progress dialog
    //creating progress dialog and set final val and update
    if (aContentSize >= 0)
        {
        iProgressNote->updateProgress(aDownloadedSize);
        }
    }

// ------------------------------------------------------------------------------------------------
// appmgmtdownloadmgr::currDwnldStatForGenericAlert
// ------------------------------------------------------------------------------------------------ 
int appmgmtdownloadmgr::currDwnldStatForGenericAlert(int dwnldstatus)
    {
     int status;
        if(dwnldstatus == DiskFull )
            status = KStatusDowloadFailedOOM;
        else if(dwnldstatus == TransactionFailed ||dwnldstatus== HostNotFoundError || dwnldstatus== TimeoutError || dwnldstatus== OperationCanceledError)
            status = KStatusAltDowldUnavailable;
        else if (dwnldstatus == AuthenticationRequiredError )
            status = KStatusAlternateDownldAuthFail;
        else
            status = KStatusDownloadFailed;
        
        return status;
            
    }

// ------------------------------------------------------------------------------------------------
// appmgmtdownloadmgr::closeProgress
// ------------------------------------------------------------------------------------------------ 
void appmgmtdownloadmgr::closeProgress()
    {
    if(iProgressNote)
        {
        iProgressNote->closeAMProgDialog();
        }
    }

// ------------------------------------------------------------------------------------------------
// appmgmtdownloadmgr::sendSrvToBg
// ------------------------------------------------------------------------------------------------ 
void appmgmtdownloadmgr::sendSrvToBg()
{
    {
    CCoeEnv* coe = CCoeEnv::Static();
    TApaTaskList taskList(coe->WsSession());
    TApaTask task=taskList.FindApp(TUid::Uid(KAppMgmtServerUid));
    if(task.Exists())
        {
        task.SendToBackground();
        }
    }
}

// --------------------------------------------------------------------------
//  Gets the access point name for a given access point id.
// --------------------------------------------------------------------------
QString appmgmtdownloadmgr::GetIapNameWithIdL(TInt aIapId)
    {
    QString name(NULL);

    RCmManager cmManager;
    cmManager.OpenL();

    RCmConnectionMethod conn;
    conn = cmManager.ConnectionMethodL(aIapId);

    HBufC* temp = conn.GetStringAttributeL(CMManager::ECmName);
    name = QString::fromUtf16(temp->Ptr(), temp->Length());
    delete temp;
    temp = NULL;

    conn.Close();

    cmManager.Close();

    return name;
    }

//---------------------------------------------------------------------------------
// CDialogWait::CDialogWait
//---------------------------------------------------------------------------------
CDialogWait::CDialogWait()
: CActive(0)
    {
    CActiveScheduler::Add(this);
    }

//---------------------------------------------------------------------------------
// CDialogWait::ConstructL
//---------------------------------------------------------------------------------
void CDialogWait::ConstructL()
    {
    iWait = new( ELeave ) CActiveSchedulerWait;
    }

//---------------------------------------------------------------------------------
// CDialogWait::NewL
//---------------------------------------------------------------------------------
CDialogWait* CDialogWait::NewL()
    {
    CDialogWait* self = new( ELeave ) CDialogWait();                                                       
        CleanupStack::PushL( self );
        self->ConstructL();
        CleanupStack::Pop();
        return self;
    }

//---------------------------------------------------------------------------------
// CDialogWait::~CDialogWait
// Destructor
//---------------------------------------------------------------------------------
CDialogWait::~CDialogWait()
    {
    Cancel();
    delete iWait;
    }

//---------------------------------------------------------------------------------
// CDialogWait::Stop
//---------------------------------------------------------------------------------
void CDialogWait::Stop()
    {
    if (iWait && iWait->IsStarted() && iWait->CanStopNow())
        {
        iWait->AsyncStop();
        }
    }

//---------------------------------------------------------------------------------
// CDialogWait::WaitOnDialog
//---------------------------------------------------------------------------------
void CDialogWait::StartWait()
    {    
    RDEBUG("startwait start");
    if (!IsActive() )
	{
		if(iWait)
		{
		  if(!iWait->IsStarted())
		      {
              iStatus = KRequestPending;
              SetActive();
              iWait->Start();
		      }
		}
	}
    RDEBUG("startwait end");
    }

// --------------------------------------------------------------------------
// CDialogWait::DoCancel()
// --------------------------------------------------------------------------
//
void CDialogWait::DoCancel()
    {
     if (iWait && iWait->IsStarted() && iWait->CanStopNow())
         {
         iWait->AsyncStop();
         }
    }    

// --------------------------------------------------------------------------
// CDialogWait::RunL()
// --------------------------------------------------------------------------
//
void CDialogWait::RunL()
    {
    if (iWait && iWait->IsStarted() && iWait->CanStopNow())
        {
        iWait->AsyncStop();
        }
    }

// --------------------------------------------------------------------------
// CDialogWait::RunError()
// --------------------------------------------------------------------------
//
TInt CDialogWait::RunError(TInt aError)
    {
    RDEBUG_2("CDialogWait::RunError %d",aError );
    return KErrNone;
    }
