/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description: QT C++ based Class.
*/

#ifndef DLGSETTING_H_
#define DLGSETTING_H_
#include <QDialog>
#include "uisetting.h"


QT_BEGIN_NAMESPACE
class QTabWidget;
class QGridLayout;
class QCheckBox;
class QPushButton;
class QLabel;
class QLineEdit;
QT_END_NAMESPACE

class DlgSetting : public QDialog {    
    Q_OBJECT
    
public:
    DlgSetting(UiSetting* settingObj, QWidget *parent = 0);
    
private:
    void SetupUI();
    
private:
    QPushButton* btnOk;
    QPushButton* btnCancel;
    QCheckBox* chkShowoutput;
    QLabel* lblFilter;
    QLineEdit* ledFilter;
    QCheckBox* chkFilterCaseSens;
    
private slots:
    void on_btnOk_clicked();
    void on_btnCancel_clicked();
    
private:
    UiSetting* setting;
    
    };

#endif /* DLGSETTING_H_ */

// End of File
