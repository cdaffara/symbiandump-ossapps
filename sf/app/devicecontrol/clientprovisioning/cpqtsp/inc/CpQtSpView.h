/*
* Copyright (c) 2002 Nokia Corporation and/or its subsidiary(-ies). 
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
*
*/

#ifndef CPQTSPVIEW_H
#define CPQTSPVIEW_H

#include <hbview.h>
#include <xqserviceprovider.h>
#include <QGraphicsLinearLayout>
#include <CWPNameValue.h>
#include <msvapi.h>
#include <hbscrollarea.h>
#include <CpQtSpMainWindow.h>
#include <HbGroupBox>
#include <hbtoolbar.h>

class CWPBioControl;

class CObserver : public CBase, public MMsvSessionObserver
{
public:
        void HandleSessionEventL(TMsvSessionEvent, TAny*, TAny*, TAny*) {};
};


class CpQtSp;
class XQServiceRequest;

class CpQtSpView : public HbView
{
    Q_OBJECT

public:
    CpQtSpView(CpQtSp* service,CpQtSpMainWindow* ptr, QGraphicsItem *parent = 0);
    ~CpQtSpView();
    void constructView();
    void orientationChanges();
    void setViewStatus();
    bool getViewStatus();
    bool buildView(QGraphicsLinearLayout* layout);
    bool getOrientationFlag();
    
public slots:
    void requestCompleted(const QVariant& value);
    void saveProvisoningMessage();
    void deleteMessage();
    void expand();
    void setHorizontal();
    void setVertical();
    void corruptMessageDialog(HbAction *action);
private:
    CpQtSp* iServiceProvider; 
    QGraphicsLinearLayout *layout;
    HbFontSpec *iPrimaryFont;
    HbFontSpec *iSecondaryFont;
    RPointerArray<CWPNameValue>* iArray;
    CWPBioControl* iBio;   
    CpQtSpMainWindow* window;
    HbGroupBox* group;
    HbToolBar* toolBar1;
    bool iViewStatus;
    RPointerArray<CWPNameValue>* array;
    bool iOrientationFlag;
};

class CpQtSp : public XQServiceProvider
{
    Q_OBJECT
public:
    CpQtSp( CpQtSpMainWindow* window, QObject *parent = 0 );
    ~CpQtSp();
    void complete();    
	TMsvId returnId();
	CMsvSession* returnSession();
	CWPBioControl* returnBio();
	void authenticationStatus();
Q_SIGNALS:	
    void showView();
    
public slots:
    QString ProcessMessage(const QString& number);
    
private:
    TMsvId iId;
    CObserver* iObserver;
    CMsvSession* iSession;   
    CWPBioControl* iBioCtrl;   
    CpQtSpMainWindow* window;
};

#endif // CPQTSPVIEW_H
