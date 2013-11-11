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
#include "cntactionpopup_p.h"
#include <qcontact.h>
#include <qcontactdetail.h>
#include <qcontactphonenumber.h>
#include <hbaction.h>
#include <QDebug>
#include "cntstringmapper.h"
#include <hblistview.h>
#include <hblistviewitem.h>
#include <QStandardItemModel>
#include <hblabel.h>
//#include <QtAlgorithms.h>



CntActionPopupPrivate::CntActionPopupPrivate( QContact* aContact, QGraphicsItem *parent ) :
HbSelectionDialog(parent),
mContact( NULL ),
mListModel(NULL),
mListView(NULL),
mCancelAction(NULL)
{
    mContact = new QContact( *aContact );
    mListView = new HbListView(this);
    mListModel = new QStandardItemModel(this);
    setAttribute(Qt::WA_DeleteOnClose, true);
    mDataItemList.clear();
}

CntActionPopupPrivate::~CntActionPopupPrivate()
{
    delete mContact;
    mContact = NULL;
}
    
void CntActionPopupPrivate::buildCallActionPopup()
{
    // Create call action
    QList<QContactPhoneNumber> details = mContact->details<QContactPhoneNumber>();
    CntStringMapper map;
    for (int index = 0; index < details.count(); index++)
    {
       if(supportsDetail("call", details[index]))
       {
           QString context = details[index].contexts().isEmpty() ? QString() : details[index].contexts().first();
           QString subtype = details[index].subTypes().isEmpty() ? details[index].definitionName() : details[index].subTypes().first();
           
           int position = getPosition(subtype, context);
           
           QString titleText = map.getContactCardListLocString(subtype , context);
           QString valueText = details[index].number();
           
           QStandardItem *labelItem = new QStandardItem();
           QStringList textList;
           textList << titleText << valueText;
           labelItem->setData(textList, Qt::DisplayRole);
           labelItem->setData(HbIcon(map.getContactCardIconString(subtype, context)), Qt::DecorationRole);
           labelItem->setData(index, Qt::UserRole);
           labelItem->setData("call", Qt::UserRole+1);
           labelItem->setData(position, Qt::UserRole+2);
           mDataItemList.append(labelItem);
       }
    }
}

void CntActionPopupPrivate::buildEmailActionPopup()
{
    // Create email action
    QList<QContactEmailAddress> details = mContact->details<QContactEmailAddress>();
    CntStringMapper map;
    for (int index = 0; index < details.count(); index++)
    {
       if(supportsDetail("email", details[index]))
       {
           QString context = details[index].contexts().isEmpty() ? QString() : details[index].contexts().first();
           int position = getPosition(details[index].definitionName(), context);
           
           QString titleText = map.getContactCardListLocString(details[index].definitionName() , context);
           QString valueText = details[index].emailAddress();
           
           QStandardItem *labelItem = new QStandardItem();
           QStringList textList;
           textList << titleText << valueText;
           labelItem->setData(textList, Qt::DisplayRole);
           labelItem->setData(HbIcon(map.getContactEditorIconString(details[index].definitionName(), context)), Qt::DecorationRole);
           labelItem->setData(index, Qt::UserRole);
           labelItem->setData("email", Qt::UserRole+1);
           labelItem->setData(position, Qt::UserRole+2);
           mDataItemList.append(labelItem);
       }
    }
}

void CntActionPopupPrivate::buildMessageActionPopup()
{
    // Create message action
    QList<QContactPhoneNumber> details = mContact->details<QContactPhoneNumber>();
    CntStringMapper map;
    for (int index = 0; index < details.count(); index++)
    {
        if(supportsDetail("message", details[index]))
        {
            // Todo : Use loc string for this dialog
            QString context = details[index].contexts().isEmpty() ? QString() : details[index].contexts().first();
            QString subtype = details[index].subTypes().isEmpty() ? details[index].definitionName() : details[index].subTypes().first();
            
            int position = getPosition(subtype, context);
        
            QString icon;
            if (details[index].contexts().isEmpty())
            {
              icon = "qtg_large_message";
            }
            else if (details[index].contexts().first() == QContactDetail::ContextHome)
            {
              icon = "qtg_large_message_home";
            }
            else if (details[index].contexts().first() == QContactDetail::ContextWork)
            {
              icon = "qtg_large_message_work";
            }
            else
            {
              icon = "qtg_large_message";
            }
            
            QString titleText( hbTrId("txt_phob_dblist_send_message"));
            QString valueText = details[index].number();
            
            QStandardItem *labelItem = new QStandardItem();
            QStringList textList;
            textList << titleText << valueText;
            labelItem->setData(textList, Qt::DisplayRole);
            labelItem->setData(HbIcon(icon), Qt::DecorationRole);
            labelItem->setData(index, Qt::UserRole);
            labelItem->setData("message", Qt::UserRole+1);
            labelItem->setData(position, Qt::UserRole+2);
            mDataItemList.append(labelItem);
        }
    }
}

bool CntActionPopupPrivate::showActionPopup(QString action)
{
    if (action.compare("call", Qt::CaseInsensitive) == 0 )
    {
        buildCallActionPopup();
    }
    else if( action.compare("message", Qt::CaseInsensitive) == 0 )
    {       
        buildMessageActionPopup();
    }
    else if (action.compare("email", Qt::CaseInsensitive) == 0 )
    {
        buildEmailActionPopup();
    }
    
    if(mDataItemList.count())
    {
        showPopup();
        return true;
    }
    return false; 
    
}

void CntActionPopupPrivate::showPopup()
{
    for (int i= 0 ;i< mDataItemList.count() ;i++) 
        {
        mListModel->appendRow(mDataItemList.at(i));
        }
    mListModel->setSortRole(Qt::UserRole+2);
    mListModel->sort(0, Qt::AscendingOrder);

    clearActions(); 
    mListView->setModel(mListModel);
    mListView->setScrollingStyle(HbScrollArea::PanWithFollowOn);
    mListView->setFrictionEnabled(true);
    mListView->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Maximum );
    
    QString contactName = mContact->displayLabel();
    if (contactName.isEmpty())
    {
        contactName = hbTrId("txt_phob_list_unnamed");
    }
    HbLabel *label = new HbLabel();
    label->setPlainText(contactName);
    
    setHeadingWidget(label);
    setContentWidget(mListView);
    
    mCancelAction = new HbAction(hbTrId("txt_common_button_cancel"), this);
    addAction(mCancelAction);
    setTimeout(HbDialog::NoTimeout);
    setDismissPolicy(HbDialog::NoDismiss);
    setModal(true);
    
    connect(mListView, SIGNAL(activated(const QModelIndex&)), this, SLOT(listItemSelected(QModelIndex)));
    connect(mListView, SIGNAL(longPressed(HbAbstractViewItem*,const QPointF&)), 
            this, SLOT(onLongPressed(HbAbstractViewItem*,const QPointF&)) );
      
    
    //launch dialog asyncronously
    open(this, SLOT(handleUserResponse(HbAction*)));

}

void CntActionPopupPrivate::handleUserResponse(HbAction* action)
{
    if (action == mCancelAction )
    {
    emit actionPopupCancelPressed();
    }
    mCancelAction = 0;
    
}

void CntActionPopupPrivate::onLongPressed(HbAbstractViewItem *item, const QPointF &coords)
{
    Q_UNUSED(coords);
    QModelIndex index = item->modelIndex();
    listItemSelected(index);
}

void CntActionPopupPrivate::listItemSelected( QModelIndex index )
{   
    if (index.isValid())
    {
        int selectedDetail = mListModel->item(index.row())->data(Qt::UserRole).toInt();
        QString actionName = mListModel->item(index.row())->data(Qt::UserRole+1).toString();   
        if (actionName.compare("call", Qt::CaseInsensitive) == 0 
            || actionName.compare("message", Qt::CaseInsensitive) == 0 )
        {       
            QList<QContactPhoneNumber> details = mContact->details<QContactPhoneNumber>();
            emit executeContactAction( *mContact, details[selectedDetail], actionName);
        }
        else if (actionName.compare("email", Qt::CaseInsensitive) == 0 )
        {
            QList<QContactEmailAddress> details = mContact->details<QContactEmailAddress>();
            emit executeContactAction( *mContact, details[selectedDetail], actionName);
        }
    }
    close();
}

/*!
Returns position of specific item
*/
int CntActionPopupPrivate::getPosition(const QString& aId, const QString& aContext)
{
    int position = CntActionPopupPrivate::ENotSupported;
    if (aId == QContactPhoneNumber::SubTypeAssistant && aContext.isEmpty() )
    {
        position = CntActionPopupPrivate::ECallAssistant;
    }
    else if (aId == QContactPhoneNumber::SubTypeCar && aContext.isEmpty() )
    {
        position = CntActionPopupPrivate::ECallCar;
    }
    else if (aId == QContactPhoneNumber::SubTypeMobile && aContext.isEmpty() )
    {
        position = CntActionPopupPrivate::ECallMobile;
    }
    else if (aId == QContactPhoneNumber::SubTypeMobile && aContext == QContactDetail::ContextHome )
    {
        position = CntActionPopupPrivate::ECallMobileHome;
    }
    else if (aId == QContactPhoneNumber::SubTypeMobile && aContext == QContactDetail::ContextWork )
    {
        position = CntActionPopupPrivate::ECallMobileWork;
    }
    else if (aId == QContactPhoneNumber::SubTypeLandline && aContext.isEmpty() )
    {
        position = CntActionPopupPrivate::ECallPhone;
    }
    else if (aId == QContactPhoneNumber::SubTypeLandline && aContext == QContactDetail::ContextHome )
    {
        position = CntActionPopupPrivate::ECallPhoneHome;    
    }
    else if (aId == QContactPhoneNumber::SubTypeLandline && aContext == QContactDetail::ContextWork )
    {
        position = CntActionPopupPrivate::ECallPhoneWork;
    }
    else if (aId == QContactPhoneNumber::SubTypeFax && aContext.isEmpty() )
    {
        position = CntActionPopupPrivate::ECallFax;
    }
    else if (aId == QContactPhoneNumber::SubTypeFax && aContext == QContactDetail::ContextHome )
    {
        position = CntActionPopupPrivate::ECallFaxHome;
    }
    else if (aId == QContactPhoneNumber::SubTypeFax && aContext == QContactDetail::ContextWork )
    {
        position = CntActionPopupPrivate::ECallFaxWork;
    }
    else if (aId == QContactPhoneNumber::SubTypePager && aContext.isEmpty() )
    {
        position = CntActionPopupPrivate::ECallPager;
    }
    else if (aId == QContactEmailAddress::DefinitionName && aContext.isEmpty())
    {
            position = CntActionPopupPrivate::EEmail;
    }
    else if (aId == QContactEmailAddress::DefinitionName && aContext == QContactDetail::ContextHome)
    {
            position = CntActionPopupPrivate::EEmailHome;
    }
    else if (aId == QContactEmailAddress::DefinitionName && aContext == QContactDetail::ContextWork)
    {
            position = CntActionPopupPrivate::EEmailWork;
    }
    else 
    {
        position = CntActionPopupPrivate::ENotSupported;
    }
    
    return position;
}

/*!
Returns true if contactDetails contains spesific action.
*/
bool CntActionPopupPrivate::supportsDetail(const QString &actionName, const QContactDetail &contactDetail)
{    
    QList<QContactActionDescriptor> actionDescriptors = QContactAction::actionDescriptors(actionName, "symbian");
    if (actionDescriptors.isEmpty())
    {
        return false;
    }
    
    QContactAction* contactAction = QContactAction::action(actionDescriptors.first());
    bool isSupportDetail = contactAction->isDetailSupported(contactDetail);
    
    delete contactAction;
    
    return isSupportDetail;
}

// End of File
