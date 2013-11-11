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

// INCLUDES
#include <QTimer>
#include <HbTextItem>
#include <HbPushButton>
#include <HbAction>
#include <hbmessagebox.h>
#include <cntservicescontact.h>
#include <xqaiwrequest.h>
#include <xqappmgr.h>
#include <telconfigcrkeys.h>        // KCRUidTelephonyConfiguration
#include <centralrepository.h>
#include <HbNotificationDialog>
#include <HbEditorInterface>

// USER INCLUDES
#include "debugtraces.h"
#include "msgunieditoraddress.h"
#include "msgunieditorlineedit.h"
#include "msgunieditormonitor.h"
#include "UniEditorGenUtils.h"
#include "msgsendutil.h"

const QString PBK_ICON("qtg_mono_contacts");
const QString REPLACEMENT_STR("; ");
const QString COMMA_SEPERATOR(",");

// Constants
const int KDefaultGsmNumberMatchLength = 7;  //matching unique ph numbers

#define LOC_TITLE hbTrId("txt_messaging_title_messaging")
#define LOC_SMS_RECIPIENT_LIMIT_REACHED hbTrId("txt_messaging_dialog_number_of_recipients_exceeded")
#define LOC_MMS_RECIPIENT_LIMIT_REACHED hbTrId("txt_messaging_dpopinfo_unable_to_add_more_recipien")
#define LOC_DIALOG_OK hbTrId("txt_common_button_ok")
#define LOC_BUTTON_CANCEL hbTrId("txt_common_button_cancel")
#define LOC_INVALID_RECIPIENT hbTrId("txt_messaging_dialog_invalid_recipient_found")
#define LOC_INVALID_RECIPIENT_NOT_ADDED hbTrId("txt_messaging_dialog_invalid_recipient_not_added")
#define LOC_INVALID_RECIPIENTS_NOT_ADDED hbTrId("txt_messaging_dialog_invalid_recipients_not_added")





MsgUnifiedEditorAddress::MsgUnifiedEditorAddress( const QString& label,
                                                  QGraphicsItem *parent ) :
MsgUnifiedEditorBaseWidget(parent),
mSkipMaxRecipientQuery(false),
mAboutToExceedMaxSmsRecipients(false),
mAboutToExceedMaxMmsRecipients(false),
mExceedsMaxMmsRecipientsBy(0)
{
    this->setContentsMargins(0,0,0,0);

    mLaunchBtn = new HbPushButton(this);
    HbStyle::setItemName(mLaunchBtn,"launchBtn");
    connect(mLaunchBtn,SIGNAL(clicked()),this,SLOT(fetchContacts()));

    mLaunchBtn->setIcon(HbIcon(PBK_ICON));

    mAddressEdit = new MsgUnifiedEditorLineEdit(label,this);
    HbStyle::setItemName(mAddressEdit,"addressField");

    mAddressEdit->setMaxRows(40);
    connect(mAddressEdit, SIGNAL(contentsChanged(const QString&)),
            this, SLOT(onContentsChanged(const QString&)));

    mAddressEdit->setInputMethodHints(Qt::ImhPreferNumbers);
    
    //To allow only latin char(s) in address fields.
    HbEditorInterface editorInterface(mAddressEdit);
    editorInterface.setInputConstraints(HbEditorConstraintLatinAlphabetOnly);
}

MsgUnifiedEditorAddress::~MsgUnifiedEditorAddress()
{
	//TODO: Should remove this code depending on orbit's reply whether it is needed
	//to unregister the same plugin registered on two different widgets twice.
    //style()->unregisterPlugin(mPluginPath);
}

void MsgUnifiedEditorAddress::fetchContacts()
{
    mLaunchBtn->blockSignals(true);

    QString service("phonebookservices");
    QString interface("com.nokia.symbian.IContactsFetch");
    QString operation("multiFetch(QString,QString)");
    XQApplicationManager appManager;
    XQAiwRequest* request = appManager.create(service, interface, operation, true); // embedded
	
    if ( request == NULL )
        {
        	return;
        }
		
    request->setSynchronous(false);
	
    // Result handlers
    connect (request, SIGNAL(requestOk(const QVariant&)), this, SLOT(handleOk(const QVariant&)));
    connect (request, SIGNAL(requestError(int,const QString&)), this, SLOT(handleError(int,const QString&)));

    QList<QVariant> args;
    args << LOC_TITLE;
    args << KCntActionSms;
    args << KCntFilterDisplayAll;

    request->setArguments(args);
    request->send();
    
    //unblock click signal after some delay.
    QTimer::singleShot(250,this,SLOT(unblockSignals()));
}

void MsgUnifiedEditorAddress::handleOk(const QVariant& value)
{
    // delete the request
    XQAiwRequest* request = qobject_cast<XQAiwRequest*>(this->sender());
    if(request)
    {
        delete request;        
    }
	
   CntServicesContactList contactList =
           qVariantValue<CntServicesContactList>(value);
    int count = contactList.count();

    ConvergedMessageAddressList addrlist;
    for(int i = 0; i < count; i++ )
    {
        ConvergedMessageAddress* address =
                new ConvergedMessageAddress();
        if(!contactList[i].mPhoneNumber.isEmpty())
        {
            address->setAddress(contactList[i].mPhoneNumber);
        }
        else
        {
            address->setAddress(contactList[i].mEmailAddress);
        }
        address->setAlias(contactList[i].mDisplayName);
        addrlist << address;
    }
    setAddresses(addrlist);
    
}

void MsgUnifiedEditorAddress::handleError(int errorCode, const QString& errorMessage)
{
    Q_UNUSED(errorMessage)
    Q_UNUSED(errorCode)
	
    XQAiwRequest* request = qobject_cast<XQAiwRequest*>(this->sender());
    if(request)
    {
       delete request;      
    }
}

// ----------------------------------------------------------------------------
// MsgUnifiedEditorAddress::addresses
// @see header
// ----------------------------------------------------------------------------
ConvergedMessageAddressList MsgUnifiedEditorAddress::addresses(
        bool removeDuplicates)
{
#ifdef _DEBUG_TRACES_
    qCritical() << "MsgUnifiedEditorAddress::addresses start";
#endif
    // sync-up map to account for user-actions on edit-field
    syncDeletionsToMap();
    syncAdditionsToMap();

    ConvergedMessageAddressList addresses;
    if(removeDuplicates)
    {
        QStringList uniqueAddr;
        uniqueAddr = uniqueAddressList();
        foreach(QString addr, uniqueAddr)
        {
            ConvergedMessageAddress* address = new ConvergedMessageAddress;
            address->setAddress(addr);
            address->setAlias(mAddressMap.value(addr));
            addresses.append(address);
        }
    }
    else
    {
        QMap<QString, QString>::iterator i = mAddressMap.begin();
        while (i != mAddressMap.end())
        {
            ConvergedMessageAddress* address = new ConvergedMessageAddress;
            address->setAddress(i.key());
            address->setAlias(i.value());
            addresses.append(address);
            i++;
        }
    }
#ifdef _DEBUG_TRACES_
    qCritical() << "MsgUnifiedEditorAddress::addresses end";
#endif
    return addresses;
}

int MsgUnifiedEditorAddress::addressCount()
{
    return mAddressEdit->addresses().count();
}

void MsgUnifiedEditorAddress::setAddresses(ConvergedMessageAddressList addrlist,bool aSkipCheck)
{
    // avoid processing if no info available
    if(addrlist.count() == 0)
    {
        return;
    }

    // ensure flags are reset before starting the addr addition
    mAboutToExceedMaxSmsRecipients = false;
    mAboutToExceedMaxMmsRecipients = false;
    mExceedsMaxMmsRecipientsBy = 0;

    // first, we check if MMS max-recipient count will exceed
    int count = addrlist.count();
	int futureCount = count + MsgUnifiedEditorMonitor::msgAddressCount();
	if(futureCount > MsgUnifiedEditorMonitor::maxMmsRecipients())
	{
	    mAboutToExceedMaxMmsRecipients = true;
	    mExceedsMaxMmsRecipientsBy =
	            futureCount - MsgUnifiedEditorMonitor::maxMmsRecipients();
	}
	// else, check if SMS max-recipient count will exceed
	else if(!mSkipMaxRecipientQuery)
	{
	    futureCount = count + addressCount();
	    if( (addressCount() <= MsgUnifiedEditorMonitor::maxSmsRecipients()) &&
	        (futureCount > MsgUnifiedEditorMonitor::maxSmsRecipients()) )
	    {
	        mAboutToExceedMaxSmsRecipients = true;
	    }
	}

	int  invalidCount= 0;
    QString invalidContacts;
    for(int i = 0; i < count; i++ )
        {
        bool isValid = false;
        if(!aSkipCheck)
        {
            QScopedPointer<MsgSendUtil> sendUtil(new MsgSendUtil());
            isValid = sendUtil->isValidAddress(addrlist.at(i)->address());
        }
        else
        {
            // no need to validate, assume correct
            isValid = true;
        }
        if(!isValid)
           {
            invalidCount ++;
            // append the comma till last but one contact.
            // add the invalid ocntacts to the " , " seperated string.
            if(invalidCount > 1)
                {
                invalidContacts.append(COMMA_SEPERATOR);
                }
            if(addrlist[i]->alias().isEmpty())
                invalidContacts.append(addrlist.at(i)->address());
            else
                invalidContacts.append(addrlist.at(i)->alias());
           }
       else
           {
           mAddressMap.insertMulti(addrlist[i]->address(), addrlist[i]->alias());
           if(!addrlist[i]->alias().isEmpty())
              {
              mAddressEdit->setText(addrlist[i]->alias());
              }
           else
              {
              mAddressEdit->setText(addrlist[i]->address(), false);
              }
           }
       }
    if(invalidCount)
        {
        QString invalidStr;
        (invalidCount == 1)?(invalidStr = QString(LOC_INVALID_RECIPIENT_NOT_ADDED.arg(invalidContacts))) :(invalidStr = QString(LOC_INVALID_RECIPIENTS_NOT_ADDED.arg(invalidContacts)));
		HbMessageBox::information(invalidStr, 0, 0, HbMessageBox::Ok);
    }
    

    // addition operation complete, reset flags
    mAboutToExceedMaxSmsRecipients = false;
    mAboutToExceedMaxMmsRecipients = false;
    mExceedsMaxMmsRecipientsBy = 0;
}

int MsgUnifiedEditorAddress::contactMatchDigits()
{
    // Read the amount of digits to be used in contact matching
    // The key is owned by PhoneApp
    int matchDigitCount = 0;
    TRAP_IGNORE(
        CRepository* repository = CRepository::NewLC(KCRUidTelConfiguration);
        if ( repository->Get(KTelMatchDigits, matchDigitCount) == KErrNone )
        {
            // Min is 7
            matchDigitCount = Max(matchDigitCount, KDefaultGsmNumberMatchLength);
        }
        CleanupStack::PopAndDestroy(); // repository
    );
    return matchDigitCount;
}

void MsgUnifiedEditorAddress::onContentsChanged(const QString& text)
{
    // Max MMS recipient count check
    if( mAboutToExceedMaxMmsRecipients ||
        (MsgUnifiedEditorMonitor::msgAddressCount() >= MsgUnifiedEditorMonitor::maxMmsRecipients()) )
    {
        if(mAboutToExceedMaxMmsRecipients)
        {// show discreet note only once
            --mExceedsMaxMmsRecipientsBy;
            if(!mExceedsMaxMmsRecipientsBy)
            {
                HbMessageBox::information(LOC_MMS_RECIPIENT_LIMIT_REACHED, 0, 0, HbMessageBox::Ok);
            }
            resetToPrevious();
        }
        else
        {
            // update monitor data
            emit contentChanged();
            if(MsgUnifiedEditorMonitor::msgAddressCount() > MsgUnifiedEditorMonitor::maxMmsRecipients())
            {
                HbMessageBox::information(LOC_MMS_RECIPIENT_LIMIT_REACHED, 0, 0, HbMessageBox::Ok);
                resetToPrevious();
                // reset monitor data
                emit contentChanged();
            }
            else
            {
                mPrevBuffer = text;
            }
        }
        return;
    }

    // Max SMS recipient count check
    if( !mSkipMaxRecipientQuery &&
        (MsgUnifiedEditorMonitor::messageType() == ConvergedMessage::Sms) &&
        (mAddressEdit->addresses().count() > MsgUnifiedEditorMonitor::maxSmsRecipients()) )
    {
        // when we show this dialog, we don't want the intermediate states
        // to be signalled to us
        disconnect(mAddressEdit, SIGNAL(contentsChanged(const QString&)),
                this, SLOT(onContentsChanged(const QString&)));
        QTimer::singleShot(50, this, SLOT(handleRecipientLimitReached()));
    }
    else
    {
        if(!mAboutToExceedMaxSmsRecipients)
        {// remember addresses before the block insertion started
            mPrevBuffer = text;
        }
        emit contentChanged();
    }
}

void MsgUnifiedEditorAddress::handleRecipientLimitReached()
{
    HbMessageBox* dlg = new HbMessageBox(HbMessageBox::MessageTypeQuestion);
    dlg->setAttribute(Qt::WA_DeleteOnClose);
    dlg->setFocusPolicy(Qt::NoFocus);
    dlg->setTimeout(HbPopup::NoTimeout);

    dlg->setText(LOC_SMS_RECIPIENT_LIMIT_REACHED);
    
    dlg->clearActions();
    HbAction* okAction = new HbAction(LOC_DIALOG_OK,dlg);
    dlg->addAction(okAction);

    HbAction* cancelAction = new HbAction(LOC_BUTTON_CANCEL,dlg);
    dlg->addAction(cancelAction);

    dlg->open(this,SLOT(onMaxRecipientsReached(HbAction*)));
    // reconnect to get back updates
    connect(mAddressEdit, SIGNAL(contentsChanged(const QString&)),
            this, SLOT(onContentsChanged(const QString&)));
}

void MsgUnifiedEditorAddress::syncDeletionsToMap()
{
    // get address list from edit-field
    QStringList addrList = mAddressEdit->addresses();

    QMap<QString, QString>::iterator i = mAddressMap.begin();
    while (i != mAddressMap.end())
    {
        // check if either key or value is present in the list
        if( !(addrList.contains(i.value(), Qt::CaseSensitive) ||
              addrList.contains(i.key(), Qt::CaseSensitive)) )
        {// if none are present, then delete entry from map
            i = mAddressMap.erase(i);
        }
        else
        {
            // ensure that the matched contact is removed from the
            // address's list
            int matchedIndex = addrList.indexOf(i.value());
            if(matchedIndex == -1)
            {
                matchedIndex = addrList.indexOf(i.key());
            }
            if(matchedIndex != -1)
            {
                addrList.removeAt(matchedIndex);
            }
            // now go to next index in map
            ++i;
        }
    }
}

void MsgUnifiedEditorAddress::syncAdditionsToMap()
{
    // remove already mapped addresses from edit-field
    QStringList userInputAddrList(mAddressEdit->addresses());
    QStringList mapAddrList = mAddressMap.values();
    mapAddrList << mAddressMap.keys();
    foreach(QString addr, mapAddrList)
    {
        userInputAddrList.removeOne(addr);
    }

    // add the unmapped addresses to address-map
    foreach(QString addr, userInputAddrList)
    {
        mAddressMap.insertMulti(addr, QString());
    }
}

QStringList MsgUnifiedEditorAddress::uniqueAddressList()
{
    int matchDigitCount = MsgUnifiedEditorAddress::contactMatchDigits();
    QStringList mapAddrList = mAddressMap.keys();
    for(int j = 0;j<mapAddrList.count()-1;j++)
    {
        for(int i =j+1;i<mapAddrList.count();i++)
        {
            if(0 == mapAddrList[j].right(matchDigitCount).compare(mapAddrList[i].right(matchDigitCount)))
            {
               mapAddrList.removeAt(i);
               i--;
            }
        }
    }
    return mapAddrList;
}

// ----------------------------------------------------------------------------
// MsgUnifiedEditorAddress::skipMaxRecipientQuery
// @see header
// ----------------------------------------------------------------------------
void MsgUnifiedEditorAddress::skipMaxRecipientQuery(bool skip)
{
    mSkipMaxRecipientQuery = skip;
}

void MsgUnifiedEditorAddress::setFocus()
{
    mAddressEdit->setFocus(Qt::MouseFocusReason);
}

// ----------------------------------------------------------------------------
// MsgUnifiedEditorAddress::resetToPrevious
// @see header
// ----------------------------------------------------------------------------
void MsgUnifiedEditorAddress::resetToPrevious()
{
    // when we do this reset operation, we don't want the intermediate states
    // to be signalled to us
    disconnect(mAddressEdit, SIGNAL(contentsChanged(const QString&)),
            this, SLOT(onContentsChanged(const QString&)));

    mAddressEdit->clearContent();
    QStringList list = mPrevBuffer.split(REPLACEMENT_STR,
            QString::SkipEmptyParts);
    int count = list.count();
    QStringList valList = mAddressMap.values();
    for(int i=0; i<count; i++)
    {
        QString addr = list.at(i);
        if(valList.contains(addr))
        {
            mAddressEdit->setText(addr);
        }
        else
        {
            mAddressEdit->setText(addr, false);
        }
    }
    syncDeletionsToMap();
    connect(mAddressEdit, SIGNAL(contentsChanged(const QString&)),
            this, SLOT(onContentsChanged(const QString&)));
}

// ----------------------------------------------------------------------------
// MsgUnifiedEditorAddress::onMaxRecipientsReached
// @see header
// ----------------------------------------------------------------------------
void MsgUnifiedEditorAddress::onMaxRecipientsReached(HbAction* action)
{
    HbMessageBox *dlg = qobject_cast<HbMessageBox*> (sender());
    if (action == dlg->actions().at(0)) {
        // accept new content, update prev-buffer
        emit contentChanged();
        mPrevBuffer = mAddressEdit->content();
    }
    else {
        // reject the new content, keep the old
        resetToPrevious();
    }
}

// ----------------------------------------------------------------------------
// MsgUnifiedEditorAddress::validateContacts
// @see header
// ----------------------------------------------------------------------------
bool MsgUnifiedEditorAddress::validateContacts()
{

    // sync-up map to account for user-actions on address-field
    syncDeletionsToMap();
    syncAdditionsToMap();

    // get the list of contacts in address-field
    QStringList fieldAddresses(mAddressEdit->addresses());

    bool isValid = true;
    foreach(QString addr, fieldAddresses)
    {
        // run address validation only if address is unmapped
        // (i.e. user-inserted)
        if(mAddressMap.contains(addr))
        {
            QScopedPointer<MsgSendUtil> sendUtil(new MsgSendUtil());
            isValid = sendUtil->isValidAddress(addr);
            if(!isValid)
            {
                mAddressEdit->highlightInvalidString(addr);
                QString invalidAddrStr =
                        QString(LOC_INVALID_RECIPIENT).arg(addr);
                HbMessageBox* dlg = new HbMessageBox(invalidAddrStr,
                        HbMessageBox::MessageTypeInformation);
                dlg->setDismissPolicy(HbPopup::TapInside);
                dlg->setTimeout(HbPopup::NoTimeout);
                dlg->setAttribute(Qt::WA_DeleteOnClose, true);
                dlg->open(this, SLOT(handleInvalidContactDialog(HbAction*)));
                break;
            }
        }
    }

    return isValid;
}

void MsgUnifiedEditorAddress::handleInvalidContactDialog(
        HbAction* act)
{
    Q_UNUSED(act);
    QTimer::singleShot(250, this, SLOT(setFocus()));
}

void MsgUnifiedEditorAddress::unblockSignals()
{
    mLaunchBtn->blockSignals(false);
}

Q_IMPLEMENT_USER_METATYPE(CntServicesContact)
Q_IMPLEMENT_USER_METATYPE_NO_OPERATORS(CntServicesContactList)

//EOF
