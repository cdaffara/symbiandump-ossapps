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

#ifndef CNTACTIONPOPUP_P_H_
#define CNTACTIONPOPUP_P_H_

#include <QObject>
#include <qtcontactsglobal.h>
#include <hbselectiondialog.h>
#include <qcontactdetail.h>
#include "cntglobal.h"
#include "cntactionpopup.h"

class QStandardItemModel;
class QStandardItem;
class HbListView;
class QModelIndex;
QTM_BEGIN_NAMESPACE
class QContact;
class QContactDetail;
QTM_END_NAMESPACE

QTM_USE_NAMESPACE

/*
* Class for showing popup for Call, Message and Email actions if preffered number is not set
*/

class CntActionPopupPrivate : public HbSelectionDialog
    {
    Q_OBJECT
    Q_DECLARE_PUBLIC(CntActionPopup)

    enum ActionPopupPosition
    { 
            EInternalServices = 0,
            ECallMobile,
            ECallMobileHome,
            ECallMobileWork,
            ECallPhone,
            ECallPhoneHome,
            ECallPhoneWork,
            ECallFax,
            ECallFaxHome,
            ECallFaxWork,
            ECallPager,
            ECallAssistant,
            ECallCar,
            EEmail,
            EEmailHome,
            EEmailWork,
            ENotSupported
    }; 
public:
    CntActionPopupPrivate( QContact *contact, QGraphicsItem *parent = 0 );
    ~CntActionPopupPrivate();
    
    //returns true if the action is supported else false
    bool showActionPopup(QString action);
    
signals:
    //Emit signal for the perticular action with the detail selected from the popup
    void executeContactAction( QContact& aContact, QContactDetail contactDetail, QString actionName );
    void actionPopupCancelPressed();

private slots:

    void listItemSelected( QModelIndex index );
    void handleUserResponse(HbAction* action);
    void onLongPressed (HbAbstractViewItem *item, const QPointF &coords);
    
private:

    //Populate the call numbers for popup
    void buildCallActionPopup();
    //Populate the emails for popup
    void buildEmailActionPopup();
    //Populate the message numbers for popup
    void buildMessageActionPopup();
    
    void showPopup();
    
    void sortDataItems();
    
    int getPosition(const QString& aId, const QString& aContext);
    
    bool supportsDetail(const QString &actionName, const QContactDetail &contactDetail);

public:
    CntActionPopup* q_ptr;
    
private:
    QContact* mContact;
    QStandardItemModel  *mListModel;
    HbListView          *mListView;
    QList<QStandardItem*>  mDataItemList;
    HbAction *mCancelAction;
    
    friend class T_CntActionPopupTest;
    };
#endif /* CNTACTIONPOPUP_P_H_ */
