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

#ifndef APPMGMTNOTIFIERS_H_
#define APPMGMTNOTIFIERS_H_

#include <QObject>
#include <hbdevicenotificationdialog.h>
#include <hbdocumentloader.h>
#include <hbdialog.h>
#include <hblabel.h>
#include <hbaction.h>
#include <hbcombobox.h>
#include "AMDeploymentComponent.h"

using namespace NApplicationManagement;

class AppMgmtNotifier : public QWidget
    {
    Q_OBJECT
    
public:
    AppMgmtNotifier(QString aAppName = 0);
    ~AppMgmtNotifier();
    
    void getAvailableDrives();
    void showInstallSuccessNote();
    void showInstallFailedNote();
    void showUninstallDialog(const CDeploymentComponent &aCompo, TRequestStatus &s);
    void showInstallDialog(CDeploymentComponent *aCompo, TRequestStatus &s);
    void showUnInstallSuccessNote();
    void showUnInstallFailedNote();
    bool showDownloadFailedNote(QString aNotifierdata);
    void showDownloadSuccessNote();
    void sendServerToBackground();
    void bringServerToForeground();
    void registerStatus(TRequestStatus &stat);
public slots:
    void dlgSlot(HbAction* action);
    void dialogUnSlot(HbAction* action);
    void successSlot(HbAction* reaction);
    void defaultDriveChanged(int i);
private:
    CDeploymentComponent* iComp;
    TRequestStatus *iStat;
    
    QString m_appname; 
    HbAction* m_OkButton;
    HbAction* m_Cancel;
    HbDialog* m_Dialog;  
    TBuf<256> m_Drilist;
    TRequestStatus* m_Stat;
    HbComboBox* m_DriveBox;   
    };
//}
#endif /* APPMGMTNOTIFIERS_H_ */
