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
*              DlgSetSelector is a QT based dialog.
*              Used to display available test set for select to add cases into.
*
*/
#ifndef DLGSETSELECTOR_H_
#define DLGSETSELECTOR_H_

#include <QDialog>
#include <QList>

QT_BEGIN_NAMESPACE
class QGridLayout;
class QPushButton;
class QRadioButton;
class QListWidget;
QT_END_NAMESPACE

class DlgSetSelector : public QDialog {
    Q_OBJECT

public:
    DlgSetSelector(QList<QString> setList, QWidget *parent = 0);
    
public:
    inline QString SelectName(){return selectName;}
    
private:
    void SetupUI();
    
private slots:
    void on_btnOk_clicked();
    void on_btnCancel_clicked();
    void on_radio1Selection_Changed(bool checked);
    void on_radio2Selection_Changed(bool checked);
    
private:
    QString selectName;
    
    QRadioButton* rdoNewSet;
    QRadioButton* rdoOldSet;
    QListWidget* lstSet;
    QPushButton* btnOk;
    QPushButton* btnCancel;
    QList<QString> setList;
    
    
};


#endif /* DLGSETSELECTOR_H_ */

// End of File
