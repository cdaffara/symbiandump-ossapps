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
* Description:
*
*/

#ifndef CNTSERVICEASSIGNCONTACTCARDVIEW_H
#define CNTSERVICEASSIGNCONTACTCARDVIEW_H

#include <QObject>
#include <qmobilityglobal.h>

#include "cntcontactcardview.h"

class CntServiceHandler;
class CntAbstractViewManager;
class CntAbstractServiceProvider;

QTM_BEGIN_NAMESPACE
class QContact;
class QContactDetail;
QTM_END_NAMESPACE

QTM_USE_NAMESPACE

class CntServiceAssignContactCardView : public CntContactCardView
{
    Q_OBJECT

public slots:
    void doCloseView(int value);
    void addToContacts();
    void saveNew();
    void updateExisting();
    
    void doViewActivated(CntAbstractViewManager* aMgr, const CntViewParameters aArgs);

public:
    CntServiceAssignContactCardView( CntAbstractServiceProvider& aServiceController );
    ~CntServiceAssignContactCardView();

    int viewId() const { return serviceAssignContactCardView; }

private:
    CntAbstractServiceProvider& mProvider;
    CntAbstractViewManager  *mViewManager;
    QContact                mContact;
    QContactDetail          mDetail;
   
};

#endif // CNTSERVICEASSIGNCONTACTCARDVIEW_H

// EOF
