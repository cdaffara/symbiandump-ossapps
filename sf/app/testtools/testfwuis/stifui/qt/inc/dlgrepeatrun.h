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

#ifndef DLGREPEATRUN_H_
#define DLGREPEATRUN_H_
#include <QDialog>
#include <QLineEdit>

QT_BEGIN_NAMESPACE
class QTabWidget;
class QGridLayout;
class QRadioButton;
class QPushButton;
QT_END_NAMESPACE

class DlgRepeatRun : public QDialog {    
    Q_OBJECT
    
public:
    DlgRepeatRun(QWidget *parent = 0);
    inline bool isRepeatInfinitely() {return isInfinite;};
    inline int  GetLoopTimes() {return loopTimes;};
    
    
private:
    void SetupUI();
    
private:
    QPushButton* btnOk;
    QPushButton* btnCancel;
    QLineEdit* lineEdit;
    QRadioButton* rdoLoopInfinitely;
    QRadioButton* rdoGiveLoopTimes;
    
    int loopTimes;
    bool isInfinite;
    
private slots:
    //on button clicked
    void on_btnOk_clicked();
    void on_btnCancel_clicked();
    
    //on check box state changed
    void on_rdoLoopInfinitely_stateChanged(bool checked);
    void on_rdoGiveLoopTimes_stateChanged(bool checked);
    
    };

#endif /* DLGREPEATRUN_H_ */

// End of File
