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
* Description: Dialog class to display some useful message.
*
*/

#ifndef DIALOGMSG_H_
#define DIALOGMSG_H_

#include <qdialog.h>

class QPushButton;
class QLabel;
class QWidget;

class DialogMsg : public QDialog
    {
    Q_OBJECT

public:
    DialogMsg(QWidget* parent = 0);
    virtual ~DialogMsg();
    
public slots:
    void showMsg(const QString);
    
private:
    void createControls();
    void setLayout();
    void setupEventHandler();
    
private:
    QPushButton*    btnOk;
    QLabel*         lableMsg;
    };

#endif /* DIALOGMSG_H_ */
