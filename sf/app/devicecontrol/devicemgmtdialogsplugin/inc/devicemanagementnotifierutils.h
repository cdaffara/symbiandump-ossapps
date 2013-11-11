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
* Description: 
*
*/

#ifndef DEVICEMANAGEMENTNOTIFIERUTILS_H_
#define DEVICEMANAGEMENTNOTIFIERUTILS_H_

#include <hbpushbutton.h>
#include <qtimeline.h>
#include <hbcheckbox.h>
#include <hblabel.h>
#include <hblineedit.h>
#include <hbtextedit.h>
#include <QGraphicsWidget>
#include <hbdocumentloader.h>
#include <hbinputeditorinterface.h>
#include <hbview.h>
#include <hbprogressbar.h>
#include <hbdialog.h>
#include <hbmainwindow.h>
#include "syncmlnotifierprivate.h"
#include <hbprogressdialog.h>

class syncmlnotifier;

class devicemanagementnotifierwidget;
class syncmlConnectNotifier;
class devicemanagementnotifierutils :public HbDialog  {

Q_OBJECT

public:
devicemanagementnotifierutils(devicemanagementnotifierwidget* ptr,const QVariantMap &parameters);
virtual ~devicemanagementnotifierutils();
private:
syncmlnotifier* notifier;
syncmlConnectNotifier* connectDialog;
  
};

class syncmlnotifier:public QObject {

Q_OBJECT

public:
syncmlnotifier(devicemanagementnotifierwidget* ptr);
virtual ~syncmlnotifier();
public:
     void launchDialog(const QVariantMap &parameters);
     void createserverinitnotifier(int& profileid, int& uimode,QString& servername,
             bool& defaultprofile, int& timeout);
     void createserveralertinformative(const QString &string);
     void createserveralertconfirmative(const QString &string);
     void createscomonotifier(QString notifiertype, QString name=NULL,QString size=NULL, QString format=NULL);
public slots:
    void okSelected();
    void cancelSelected(); 
    void infoNoteOkSelected();
    void conirmNoteOkSelected();
    void conirmNoteCancelSelected();
    void noteTimedOut(HbAction* action);  

signals:
    void servertextmessage(const QString &string);
 
private:
  syncmlnotifierprivate* msymnotifier;
  devicemanagementnotifierwidget* iPtr;
  bool mDefaultServerPkgZero;

};
    
class syncmlConnectNotifier:public QObject {

Q_OBJECT

public:
syncmlConnectNotifier(devicemanagementnotifierwidget* ptr);
virtual ~syncmlConnectNotifier();
public:
     void launchDialog(const QVariantMap &parameters);
  
public slots:        
void buttonClicked(HbAction* action);

private:
HbProgressDialog *pDialog;      
devicemanagementnotifierwidget* iPtr;

};

#endif
