/*
* Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
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
* Description: Dialog class to add test dll files.
*
*/

#ifndef DIALOGADDTESTS_H_
#define DIALOGADDTESTS_H_
#include <QDialog>

class QTextEdit;
class QPushButton;
class QWidget;

class DialogAddTests: public QDialog
    {
    Q_OBJECT
    
public:
    DialogAddTests(QWidget* parent = 0);
    virtual ~DialogAddTests();
    
public:
    void clear();
    
signals:
    void testsSaved(QStringList);
    
private slots:
    void saveTests();
    void changeText();
    
private:
    void load();
    void setLayout();

private:
    QTextEdit*      txtTests;
    QPushButton*    btnOk;
    QPushButton*    btnCancel;
    };

#endif /* DIALOGADDTESTS_H_ */
