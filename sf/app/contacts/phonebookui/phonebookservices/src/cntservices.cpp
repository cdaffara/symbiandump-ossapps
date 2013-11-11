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

#include "cntservices.h"
#include "cntglobal.h"
#include "cntdebug.h"
#include <cntabstractviewmanager.h>
#include "cntserviceviewmanager.h"
#include "cntimageutility.h"
#include "cntserviceviewparams.h"
#include <cntservicescontact.h>

#include <hbview.h>
#include <hblistview.h>
#include <qversitreader.h>
#include <qversitcontactimporter.h>
#include <qversitdocument.h>
#include <xqserviceprovider.h>
#include <cntabstractengine.h>

#include <QApplication>
#include <QPixmap>
#include <QFile>
#include <QUrl>
#include <QTextCodec>

const int Latin1CharSetMIB = 4;

CntServices::CntServices() :
mViewManager(NULL),
mCurrentProvider(NULL)
{
    CNT_ENTRY

    // Definition names supported in symbian back-end
    m_definitionNames << QContactName::DefinitionName;
    m_definitionNames << QContactNickname::DefinitionName;
    m_definitionNames << QContactPhoneNumber::DefinitionName;
    m_definitionNames << QContactEmailAddress::DefinitionName;
    m_definitionNames << QContactAddress::DefinitionName;
    m_definitionNames << QContactUrl::DefinitionName;
    m_definitionNames << QContactBirthday::DefinitionName;
    m_definitionNames << QContactOrganization::DefinitionName;
    m_definitionNames << QContactSyncTarget::DefinitionName;
    m_definitionNames << QContactNote::DefinitionName;
    m_definitionNames << QContactFamily::DefinitionName;
    m_definitionNames << QContactAvatar::DefinitionName;
    m_definitionNames << QContactAnniversary::DefinitionName;
    m_definitionNames << QContactGeoLocation::DefinitionName;
    m_definitionNames << QContactGender::DefinitionName;
    m_definitionNames << QContactOnlineAccount::DefinitionName;
    
    CNT_EXIT
}


CntServices::~CntServices()
{
    CNT_LOG
}


void CntServices::setEngine( CntAbstractEngine& aEngine )
{
    CNT_LOG
    mEngine = &aEngine;
    mViewManager = &mEngine->viewManager();
    CntServiceViewManager* srvMng = static_cast<CntServiceViewManager*>(mViewManager);
    connect(srvMng, SIGNAL(applicationClosed()), this, SLOT(terminateService()));
}

void CntServices::singleFetch(
    const QString &title, const QString &action,
    CntAbstractServiceProvider& aServiceProvider )
{
    CNT_ENTRY
    mCurrentProvider = &aServiceProvider;

    CntViewParameters params;
    params.insert(EViewId, serviceContactFetchView);
    params.insert(ESelectedAction, action);
    params.insert(KCntServiceViewParamTitle, title);
    // SINGLE fetch
    params.insert(ESelectionMode, HbAbstractItemView::SingleSelection);

    mViewManager->changeView(params);
    CNT_EXIT
}


void CntServices::multiFetch(
    const QString &title, const QString &action,
    CntAbstractServiceProvider& aServiceProvider )
{
    CNT_ENTRY
    mCurrentProvider = &aServiceProvider;

    CntViewParameters params;
    params.insert(EViewId, serviceContactFetchView);
    params.insert(ESelectedAction, action);
    params.insert(KCntServiceViewParamTitle, title);
    // MULTI fetch
    params.insert(ESelectionMode, HbAbstractItemView::MultiSelection);
    mViewManager->changeView(params);
    CNT_EXIT
}

void CntServices::editCreateNew(const QString &definitionName, const QString &value,
    CntAbstractServiceProvider& aServiceProvider, bool defaultForOnlineAccountIsImpp )
{
    CNT_ENTRY
    editCreateNew( definitionName, value, QString(), aServiceProvider, defaultForOnlineAccountIsImpp);
    CNT_EXIT
}

void CntServices::editCreateNew(const QString &definitionName, const QString &value, const QString& subType,
    CntAbstractServiceProvider& aServiceProvider, bool defaultForOnlineAccountIsImpp )
{
    CNT_ENTRY
    mCurrentProvider = &aServiceProvider;

    QContact contact;
    
    if (definitionName == QContactPhoneNumber::DefinitionName)
    {
        QContactPhoneNumber phoneNumber;
        phoneNumber.setNumber(value);
        if (!subType.isEmpty())
        {
            phoneNumber.setSubTypes(subType);
        }
        else
        {
            phoneNumber.setSubTypes(QContactPhoneNumber::SubTypeMobile); // Mobile is the default if subtype is not given.
        }
         contact.saveDetail(&phoneNumber);
    }
    else if (definitionName == QContactEmailAddress::DefinitionName)
    {
        QContactEmailAddress email;
        email.setEmailAddress(value);
        // Email addresses can't have subtypes.
        contact.saveDetail(&email);
    }
    else if (definitionName == QContactOnlineAccount::DefinitionName)
    {
        QContactOnlineAccount account;
        fillOnlineAccount( account, value, subType, defaultForOnlineAccountIsImpp);
        contact.saveDetail(&account);
    }    

    // Launch editor now
    CntViewParameters params;
    params.insert(EViewId, serviceEditView);
    QVariant var;
    var.setValue(contact);
    params.insert(ESelectedContact, var);
    mViewManager->changeView(params);
    CNT_EXIT
}


void CntServices::editCreateNewFromVCard(const QString &fileName,
    CntAbstractServiceProvider& aServiceProvider )
{
    CNT_ENTRY
    mCurrentProvider = &aServiceProvider;

    CntImageUtility imageUtility;
    QContact contact;
    QVersitReader reader;
    QFile inputFile(fileName);
    if (!inputFile.open(QIODevice::ReadOnly))
        return;
    
    // Versit parser default codec is UTF-8
    // Check if decoding text to unicode is possible, else use Latin-1 text codec
    QByteArray ba = inputFile.readAll();
    if(!ba.isEmpty())
        {
        QTextCodec *c = QTextCodec::codecForUtfText(ba);
        // Text codec returned is Latin-1, set default to Latin-1
        if(c->mibEnum()==Latin1CharSetMIB)
            reader.setDefaultCodec(QTextCodec::codecForName("ISO 8859-1"));
        }
    inputFile.close();
    
    QFile vCardFile(fileName);
    if (!vCardFile.open(QIODevice::ReadOnly))
        return;
    reader.setDevice(&vCardFile);
    reader.startReading();
    reader.waitForFinished();
    // Use the resulting document(s)...
    QVersitContactImporter importer;
    QList<QVersitDocument> versitDocuments = reader.results();
    if(versitDocuments.count() > 0 
       && importer.importDocuments(versitDocuments))
        {
        QList<QContact> contacts = importer.contacts();
        if(contacts.count() > 0)
            contact = contacts.first();
        }
    vCardFile.close();
    
    // check if the contact is in db already.
    updateLocalId( contact );
    
    // Save thumbnail images
    QList<QContactThumbnail> details = contact.details<QContactThumbnail>();
    for (int i = 0;i < details.count();i++)
        {
        if (!details.at(i).thumbnail().isNull())
            {
            QString imagepath;
            QPixmap pixmap = QPixmap::fromImage(details.at(i).thumbnail());
            if(imageUtility.createImage(pixmap,imagepath))
                {
                // Update contact detail
                QContactAvatar detail=details.at(i);
                detail.setImageUrl(imagepath);
                contact.saveDetail(&detail);
                }
            }
        }
    
    removeNotSupportedDetails(contact);
    removeNotSupportedFields(contact);

    // Launch editor now
    CntViewParameters params;
    params.insert(EViewId, serviceEditView);
    QVariant var;
    var.setValue(contact);
    params.insert(ESelectedContact, var);
    mViewManager->changeView(params);
    CNT_EXIT
}

void CntServices::editUpdateExisting(const QString &definitionName, const QString &value,
    CntAbstractServiceProvider& aServiceProvider, bool defaultForOnlineAccountIsImpp )
{
    CNT_ENTRY
    editUpdateExisting( definitionName, value, QString(), aServiceProvider, defaultForOnlineAccountIsImpp );
    CNT_EXIT
}

void CntServices::editUpdateExisting(const QString &definitionName, const QString &value, const QString& subType,
    CntAbstractServiceProvider& aServiceProvider, bool defaultForOnlineAccountIsImpp )
{
    CNT_ENTRY
    mCurrentProvider = &aServiceProvider;

    QContactDetail detail;
    
    if (definitionName == QContactPhoneNumber::DefinitionName)
    {
        QContactPhoneNumber phoneNumber;
        phoneNumber.setNumber(value);
        if (!subType.isEmpty())
        {        
            phoneNumber.setSubTypes(subType);
        }
        else
        {
            phoneNumber.setSubTypes(QContactPhoneNumber::SubTypeMobile);    
        }
        detail = phoneNumber;
    }
    else if (definitionName == QContactEmailAddress::DefinitionName)
    {
        QContactEmailAddress email;
        email.setEmailAddress(value);
        // Email addresses can't have subtypes.
        detail = email;
    }
    else if (definitionName == QContactOnlineAccount::DefinitionName)
    {
        QContactOnlineAccount account;
        fillOnlineAccount( account, value, subType, defaultForOnlineAccountIsImpp);
        detail = account;
    }

    CntViewParameters params;
    params.insert(EViewId, serviceContactSelectionView);
    QVariant var;
    var.setValue(detail);
    params.insert(ESelectedDetail, var);
    mViewManager->changeView(params);
    CNT_EXIT
}

void CntServices::editExisting(int contactId,
CntAbstractServiceProvider& aServiceProvider)
{
    CNT_ENTRY
    mCurrentProvider = &aServiceProvider;

    // Give parameters for launching editor.
    CntViewParameters params;
    params.insert(EViewId, serviceEditView);

    QContact contact = contactManager()->contact(contactId);
    QVariant varContact;
    varContact.setValue(contact);
    params.insert(ESelectedContact, varContact);
    mViewManager->changeView(params);
    CNT_EXIT
}


void CntServices::launchContactCard(int aContactId,
    CntAbstractServiceProvider& aServiceProvider )
{
    CNT_ENTRY
    mCurrentProvider = &aServiceProvider;

    QContact contact = contactManager()->contact(aContactId);

    // Launch Contact Card view
    CntViewParameters params;
    params.insert(EViewId, serviceContactCardView);
    QVariant var;
    var.setValue(contact);
    params.insert(ESelectedContact, var);
    mViewManager->changeView( params );
    CNT_EXIT
}


void CntServices::launchTemporaryContactCard(const QString &definitionName, const QString &value,
    CntAbstractServiceProvider& aServiceProvider )
{
    CNT_ENTRY
    mCurrentProvider = &aServiceProvider;

    QContact contact;
    QContactDetail detail;
    
    QContactName name;
    name.setFirstName(hbTrId("txt_phob_list_unknown"));
    contact.saveDetail(&name);
    
    if (definitionName == QContactPhoneNumber::DefinitionName)
    {
        QContactPhoneNumber phoneNumber;
        phoneNumber.setNumber(value);
        phoneNumber.setSubTypes(QContactPhoneNumber::SubTypeMobile);
        contact.saveDetail(&phoneNumber);
        detail = phoneNumber;
    }
    else if (definitionName == QContactEmailAddress::DefinitionName)
    {
        QContactEmailAddress email;
        email.setEmailAddress(value);
        contact.saveDetail(&email);
        detail = email;
    }
    else if (definitionName == QContactOnlineAccount::DefinitionName)
    {
        QContactOnlineAccount account;
        account.setAccountUri(value);
        account.setSubTypes(QContactOnlineAccount::SubTypeSipVoip);
        contact.saveDetail(&account);
        detail = account;
    }

    // Launch the view
    CntViewParameters params;
    params.insert(EViewId, serviceAssignContactCardView);
    QVariant var;
    var.setValue(contact);
    params.insert(ESelectedContact, var);
    QVariant varDetail;
    varDetail.setValue(detail);
    params.insert(ESelectedDetail, varDetail);
    mViewManager->changeView(params);
    CNT_EXIT
}

void CntServices::launchGroupMemberView(int aContactId,
    CntAbstractServiceProvider& aServiceProvider )
{
    CNT_ENTRY
    mCurrentProvider = &aServiceProvider;

    QContact contact = contactManager()->contact(aContactId);

    // Launch Group member view
    CntViewParameters params;
    params.insert(EViewId, serviceGroupMemberView);
    QVariant var;
    var.setValue(contact);
    params.insert(ESelectedGroupContact, var);
    mViewManager->changeView( params );
    CNT_EXIT
}

void CntServices::quitApp()
{
    CNT_ENTRY   
    
    qApp->quit();
    
    CNT_EXIT
}

void CntServices::removeNotSupportedDetails(QContact& contact)
{
    CNT_ENTRY
    // Check definition map, remove details not supported
    for (int i=0; i < contact.details().count(); i++)
    {
        if (!m_definitionNames.contains(contact.details().at(i).definitionName()))
        {
            QContactDetail detail= contact.details().at(i);
            contact.removeDetail(&detail);
        }
    }
    CNT_EXIT
}

void CntServices::removeNotSupportedFields(QContact& contact)
{
    CNT_ENTRY
    QList<QContactDetail>   removeList;
    
    // Not all fields are supported in symbian back-end
    // Remove not supported fields
    for (int i=0; i < contact.details().count(); i++)
    {
        QContactDetail detail = contact.details().at(i);
        if(detail.definitionName() == QContactAddress::DefinitionName)
        {
            QContactAddress addrDetail = static_cast<QContactAddress>(detail);
            
            // Sub-types not supported
            if(detail.removeValue(QContactAddress::FieldSubTypes))
                contact.saveDetail(&detail);
        }
        else if(detail.definitionName() == QContactAnniversary::DefinitionName)
        {
            QContactAnniversary annivDetail = static_cast<QContactAnniversary>(detail);
            
            // Sub-types not supported
            if(detail.removeValue(QContactAnniversary::FieldSubType))
                contact.saveDetail(&detail);
            
            // Context not supported
            if(detail.removeValue(QContactDetail::FieldContext))
                contact.saveDetail(&detail);
        }
        else if(detail.definitionName() == QContactPhoneNumber::DefinitionName)
        {
            QContactPhoneNumber phoneDetail = static_cast<QContactPhoneNumber>(detail);
            
            if(phoneDetail.subTypes().size() > 0 )
            {
                // Sub-types not supported
                if(phoneDetail.subTypes().contains(QContactPhoneNumber::SubTypeVoice)
                   || phoneDetail.subTypes().contains(QContactPhoneNumber::SubTypeMessagingCapable))
                {
                    // Default to subtype mobile
                    QStringList l;
                    l << QLatin1String(QContactPhoneNumber::SubTypeMobile);
                    detail.setValue(QContactPhoneNumber::FieldSubTypes, l);
                    contact.saveDetail(&detail);
                }
            }
        }
        else if(detail.definitionName() == QContactUrl::DefinitionName)
        {
            QContactUrl urlDetail = static_cast<QContactUrl>(detail);
            
            QString stype=QContactUrl::SubTypeHomePage;
            if(urlDetail.subType().compare(stype) != 0)
            {
                detail.removeValue(QContactUrl::FieldSubType);
                contact.saveDetail(&detail);
            }
        }
    }
    CNT_EXIT
}

void CntServices::fillOnlineAccount( QContactOnlineAccount& account, const QString& value,
    const QString& subType, bool defaultForOnlineAccountIsImpp )
    {
        // The value should normally consist of two parts:
        //    <service provider>:<user ID>
        // for eg. "serviceprovider:jack@serviceprovider.com"
        QStringList accountDetails = value.split(":");
        if (accountDetails.count() == 1)
        {
            // For some reason it only had one part, so we're assuming it's the user ID.
            account.setAccountUri(accountDetails.at(0));
        }
        else if (accountDetails.count() >= 2)
        {
            account.setServiceProvider(accountDetails.at(0));        
            account.setAccountUri(accountDetails.at(1)); // the user ID
        }

        if (!subType.isEmpty())
        {        
            account.setSubTypes(subType);
        }
        else
        {
            if (defaultForOnlineAccountIsImpp)
            {
                account.setSubTypes( QContactOnlineAccount::SubTypeImpp );
            }
            else
            {
                account.setSubTypes( QContactOnlineAccount::SubTypeSipVoip );
            }
        }
    }

void CntServices::updateLocalId( QContact& aContact )
{
    CNT_ENTRY
    if ( aContact.localId() == 0 )
        {
        QContactGuid guid = aContact.detail<QContactGuid>();
        QString guidString = guid.guid();
            
        QContactDetailFilter filter;
        filter.setDetailDefinitionName( QContactGuid::DefinitionName, QContactGuid::FieldGuid );
        filter.setValue( guid.guid() );
            
        QContactManager& cm = mEngine->contactManager( SYMBIAN_BACKEND );
        QList<QContactLocalId> idList = cm.contactIds( filter, QList<QContactSortOrder>() );
        int count = idList.count();
        
        if ( !idList.isEmpty() )
            {
            QContactId id;
            id.setLocalId( idList.first() );
            id.setManagerUri( cm.managerUri() );
            
            aContact.setId( id );
            }
        }
    CNT_EXIT
}
// This method is inherited from CntAbstractServiceProvider
void CntServices::CompleteServiceAndCloseApp(const QVariant& retValue)
{
    CNT_ENTRY
    if (  mCurrentProvider )
    {
        mCurrentProvider->CompleteServiceAndCloseApp( retValue );
    }
    CNT_EXIT
}

// This method is inherited from CntAbstractServiceProvider
void CntServices::overrideReturnValue(const QVariant& retValue)
{
    CNT_ENTRY
    if (  mCurrentProvider )
    {
        mCurrentProvider->overrideReturnValue( retValue );
    }
    CNT_EXIT
}

// This method is inherited from CntAbstractServiceProvider
bool CntServices::allowSubViewsExit()
{
    bool allow = true;
    if (  mCurrentProvider )
    {
        allow = mCurrentProvider->allowSubViewsExit();
    }
    
    return allow;
}

void CntServices::terminateService()
{
    CNT_ENTRY
    if (  mCurrentProvider )
    {
        // Complete the service with KCntServicesTerminated return value
        QVariant var(KCntServicesTerminated);
        mCurrentProvider->CompleteServiceAndCloseApp( var );
    }
    CNT_EXIT
}

QContactManager* CntServices::contactManager()
{
    if ( mEngine )
        return &mEngine->contactManager(SYMBIAN_BACKEND);
    return NULL;
}

Q_IMPLEMENT_USER_METATYPE(CntServicesContact)
Q_IMPLEMENT_USER_METATYPE_NO_OPERATORS(CntServicesContactList)

