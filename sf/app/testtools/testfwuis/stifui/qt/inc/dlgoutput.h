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
*              DlgOutput is a QT based dialog.
*              Used to display case execution output and control the test case (pause/resume/abort).
*
*/
#ifndef DLGOUTPUT_H_
#define DLGOUTPUT_H_

#include <QDialog>
#include <QHash>
#include "istfqtuicontroller.h"


QT_BEGIN_NAMESPACE
class QTabWidget;
class QGridLayout;
class QPlainTextEdit;
class QPushButton;
QT_END_NAMESPACE

class DlgOutput : public QDialog, public IStfEventListener {
    Q_OBJECT

public:
    DlgOutput(IStfQtUIController* ctl, QWidget *parent = 0);
    ~DlgOutput();
    
private:
    void CreateItem(QString index, QString item);
    void CloseItem(QString index);
    void ShowMessage(QString index, QString msg);
    
private://implement IStfEventListener
    void OnGetMessage(const QString& ){};
    void OnSetListChanged(){};
    void OnCaseOutputChanged(const IStfEventListener::CaseOutputCommand& cmd, const QString& index, const QString& msg);
    
private slots:
    void on_btnPause_clicked();
    void on_btnAbort_clicked();
    void on_btnClose_clicked();
    
    
private:
    IStfQtUIController* controller;
    QTabWidget* tabMain;
    QPushButton* btnPause;
    QPushButton* btnAbort;
    QPushButton* btnClose;
    QHash<QString , QPlainTextEdit*> tabList;
    
};


#endif /* DLGOUTPUT_H_ */

// End of File
