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

#include "cntservicecontactfetchview.h"

#include <cntservicescontact.h>
#include <cntlistmodel.h>
#include "cntserviceviewparams.h"

#include <hbmenu.h>
#include <hbview.h>
#include <hblistview.h>
#include <hbdocumentloader.h>
#include <hbaction.h>
#include "cntactionpopup.h"

#include <QCoreApplication>

CntServiceContactFetchView::CntServiceContactFetchView( CntAbstractServiceProvider& aServiceProvider ):
CntBaseSelectionView(),
mProvider( aServiceProvider )
    {
    HbAction* cancel = static_cast<HbAction*>( mDocument->findObject( "cnt:cancel" ) );
    mView->menu()->addAction( cancel );
    connect(cancel,  SIGNAL(triggered()), this, SLOT(cancelFetch()) );
    connect( this, SIGNAL(viewClosed()), this, SLOT(closeFetchView()) );
    connect( this, SIGNAL(viewOpened(CntAbstractViewManager*, const CntViewParameters)), this, SLOT(aboutToOpenView(CntAbstractViewManager*, const CntViewParameters)) );
}

CntServiceContactFetchView::~CntServiceContactFetchView()
{
}


void CntServiceContactFetchView::cancelFetch()
    {
    CntServicesContactList serviceList;
    QVariant variant;
    variant.setValue(serviceList);
    mProvider.CompleteServiceAndCloseApp(variant);
    }



void CntServiceContactFetchView::closeFetchView()
{
    QModelIndexList temp = mListView->selectionModel()->selection().indexes();
    mIndex = 0;
    for(int i = 0; i < temp.count(); i++ )
    {
        QContact contact = mListModel->contact(temp.at(i));
        if (mAction == KCntActionCall)
        {
            serviceCallMessageAction(contact, KCntActionCall);
        }
        else if (mAction == KCntActionSms )
        {
            serviceCallMessageAction(contact, "message");
        }
        else if (mAction == KCntActionEmail)
        {
            serviceEmailAction(contact);
        }
        else
        {
            serviceAllAction(contact);
        }
    }
    showPreviousView();
}

void CntServiceContactFetchView::serviceCallMessageAction(QContact& aContact, QString aAction)
{
    CntServicesContact servicesContact;
    //get the phonenumber
    QList<QContactPhoneNumber> phonenumbers = aContact.details<QContactPhoneNumber>();
    
    Q_ASSERT_X( phonenumbers.count() > 0, "serviceCallMessageAction", "Number count is zero");
    
    QContactDetail detail = aContact.preferredDetail(aAction);
    if (!detail.isEmpty())
    {
        //Set preferred detail for services
        QContactPhoneNumber number = detail;
        servicesContact.mDisplayName = aContact.displayLabel();
        servicesContact.mPhoneNumber = number.number();
        servicesContact.mEmailAddress = "";
        servicesContact.mContactId = aContact.localId();
        mServiceList.append(servicesContact);
    }
    else 
    {
        // show popup for call numbers
        CntActionPopup *actionPopup = new CntActionPopup(&aContact);
        bool popup = actionPopup->showActionPopup(aAction);
        if(popup)
        {
            //if call numbers are present wait for signals
            mIndex++;
            connect( actionPopup, SIGNAL(executeContactAction(QContact&,QContactDetail, QString)), this, 
                    SLOT(popupContactAction(QContact&, QContactDetail, QString)));
            connect( actionPopup, SIGNAL(actionPopupCancelPressed()), this, 
                                SLOT(actionPopupCancelSlot()));
        }
        else 
        {
            //else delete popup if no call number present
            delete actionPopup;
        }
    }
}

void CntServiceContactFetchView::serviceEmailAction(QContact& aContact)
{
    CntServicesContact servicesContact;
    //get the email address
    QList<QContactEmailAddress> emailAddresses = aContact.details<QContactEmailAddress>();
    
    Q_ASSERT_X( emailAddresses.count() > 0, "serviceEmailAction", "email address count is zero");
    
    QContactDetail detail = aContact.preferredDetail("email");
    if (!detail.isEmpty())
    {
        //Set preferred detail for services
        QContactEmailAddress email = detail;
        servicesContact.mDisplayName = aContact.displayLabel();
        servicesContact.mPhoneNumber = "";
        servicesContact.mEmailAddress = email.emailAddress();
        servicesContact.mContactId = aContact.localId();
        mServiceList.append(servicesContact);
        
    }
    else 
    {
        // show popup for email id's 
        CntActionPopup *actionPopup = new CntActionPopup(&aContact);
        bool popup = actionPopup->showActionPopup("email");
        if(popup)
        {
            //if email id's are present wait for signals
            mIndex++;
            connect( actionPopup, SIGNAL(executeContactAction(QContact&,QContactDetail, QString)), this, 
                    SLOT(popupContactAction(QContact&, QContactDetail, QString)));
            connect( actionPopup, SIGNAL(actionPopupCancelPressed()), this, 
                                SLOT(actionPopupCancelSlot()));
        }
        else 
        {
            //else delete popup if no email present
            delete actionPopup;
        }
    }
}

void CntServiceContactFetchView::serviceAllAction(QContact& aContact)
{
    CntServicesContact servicesContact;
    servicesContact.mDisplayName = aContact.displayLabel();
    QContactDetail callDetail = aContact.preferredDetail("call");
    //Check for preferredDetail from call
    // if not present check for preferredDetail for message
    // if not present pick first phone number
    if (!callDetail.isEmpty())
    {
        QContactPhoneNumber number = callDetail;
        servicesContact.mPhoneNumber = number.number();
    }
    else
    {
        QContactDetail smsDetail = aContact.preferredDetail("message");
        if (!callDetail.isEmpty())
        {
            QContactPhoneNumber number = smsDetail;
            servicesContact.mPhoneNumber = number.number();
        }
        else 
        {
            QList<QContactPhoneNumber> phonenumbers = aContact.details<QContactPhoneNumber>();
            if(phonenumbers.count() > 0)
            {
            servicesContact.mPhoneNumber = phonenumbers.first().number();
            }
            else
            {
            servicesContact.mPhoneNumber = "";
            }
        }
    }
    
    QContactDetail emailDetail = aContact.preferredDetail("email");
    if (!emailDetail.isEmpty())
    {
        QContactEmailAddress email = emailDetail;
        servicesContact.mEmailAddress = email.emailAddress();
    }
    else
    {
        //get first email address
        QList<QContactEmailAddress> emailAddresses = aContact.details<QContactEmailAddress>();
        if(emailAddresses.count() > 0)
        {
            servicesContact.mEmailAddress = emailAddresses.first().emailAddress();
        }
        else 
        {
            servicesContact.mEmailAddress = "";
        }
    }
    servicesContact.mContactId = aContact.localId();
    mServiceList.append(servicesContact);
}

void CntServiceContactFetchView::aboutToOpenView(CntAbstractViewManager* aMgr, const CntViewParameters aArgs)
{
    mMgr = aMgr;
    
    // Set title of the view.
    QString title = aArgs.value(KCntServiceViewParamTitle).toString();
    mView->setTitle(title);
    
    // Set action filter
    mAction = aArgs.value(ESelectedAction).toString();
    if (mAction == KCntActionSms)
        {
            QContactActionFilter actionFilter;
            actionFilter.setActionName("message");
            mListModel->setFilter(actionFilter);
        }
        else if (mAction == KCntActionCall)
        {
            QContactActionFilter actionFilter;
            actionFilter.setActionName("call");
            mListModel->setFilter(actionFilter);
        }
        else if (mAction == KCntActionEmail)
        {
            QContactDetailFilter emailFilter;
            emailFilter.setMatchFlags( QContactDetailFilter::MatchContains );
            emailFilter.setDetailDefinitionName( 
                    QContactEmailAddress::DefinitionName, 
                    QContactEmailAddress::FieldEmailAddress );
            // we need to give something to filter for the email filter
            emailFilter.setValue( "@" );
            mListModel->setFilter( emailFilter );
            
        }
        else
        {
            QContactDetailFilter filter;
            filter.setDetailDefinitionName(QContactType::DefinitionName, QContactType::FieldType);
            QString typeContact = QContactType::TypeContact;
            filter.setValue(typeContact);
            mListModel->setFilter(filter);
        }

        // hide my card if it's not set
        if ( mListModel->myCardId() == 0 )
        {
            mListModel->showMyCard( false );
        }
}

void CntServiceContactFetchView::popupContactAction(QContact& aContact,QContactDetail contactDetail, QString aAction)
{
    if (aAction.compare("call", Qt::CaseInsensitive) == 0 
           || aAction.compare("message", Qt::CaseInsensitive) == 0 )
       {       
           CntServicesContact servicesContact;
           QContactPhoneNumber number = static_cast<QContactPhoneNumber>(contactDetail);
           QString name = aContact.displayLabel();
           servicesContact.mDisplayName = aContact.displayLabel();
           servicesContact.mPhoneNumber = number.number();
           servicesContact.mEmailAddress = "";
           servicesContact.mContactId = aContact.localId();
           mServiceList.append(servicesContact);
           mIndex--;
       }
       else if (aAction.compare("email", Qt::CaseInsensitive) == 0 )
       {
           CntServicesContact servicesContact;
           QContactEmailAddress email = static_cast<QContactEmailAddress>(contactDetail);
           servicesContact.mDisplayName = aContact.displayLabel();
           servicesContact.mPhoneNumber = "";
           servicesContact.mEmailAddress = email.emailAddress();
           servicesContact.mContactId = aContact.localId();
           mServiceList.append(servicesContact);
           mIndex--;
       }
    
    if (aContact.preferredDetail(aAction).isEmpty() && (aAction == "call" || aAction == "message" || aAction == "email"))
    {
        aContact.setPreferredDetail(aAction, contactDetail);
        //return value will be ignored because we cannot do anything if it fails.
        mEngine->contactManager(SYMBIAN_BACKEND).saveContact(&aContact);
    }
    
    showPreviousView();
}

void CntServiceContactFetchView::actionPopupCancelSlot()
{
    mIndex--;
    showPreviousView();
}

void CntServiceContactFetchView::showPreviousView()
{
    if(mIndex==0)
    {
        QVariant variant;
        variant.setValue(mServiceList);
        mProvider.CompleteServiceAndCloseApp(variant);
        
        CntViewParameters args;
        mMgr->back( args );
    }
}
// EOF
