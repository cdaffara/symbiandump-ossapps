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

#ifndef CNTSERVICECONTACTFETCHVIEW_H
#define CNTSERVICECONTACTFETCHVIEW_H

#include <QObject>

#include "cntbaseselectionview.h"
#include <cntservicescontact.h>
#include "cntabstractserviceprovider.h"

class CntServicesContact;

class CntServiceContactFetchView : public CntBaseSelectionView
{
    Q_OBJECT

public:
    CntServiceContactFetchView( CntAbstractServiceProvider& aServiceProvider );
    ~CntServiceContactFetchView();

    int viewId() const { return serviceContactFetchView; }
    
public slots:
    void aboutToOpenView(CntAbstractViewManager* aMgr, const CntViewParameters viewParameters);
    void closeFetchView();
    void cancelFetch();
    void popupContactAction(QContact& aContact,QContactDetail contactDetail, QString aAction);
    void actionPopupCancelSlot();
    
private:
    void serviceCallMessageAction(QContact& aContact, QString aAction);
    void serviceEmailAction(QContact& aContact);
    void serviceAllAction(QContact& aContact);
    void showPreviousView();

private:
    CntAbstractServiceProvider& mProvider;
    QString mAction;
    CntServicesContactList mServiceList;
    int mIndex;
};

#endif /* CNTSERVICECONTACTFETCHVIEW_H */
