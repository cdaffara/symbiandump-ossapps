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

#include "emailtrace.h"

#include <qservicemanager.h>
#include <qserviceinterfacedescriptor.h>
#include <hbicon.h>
#include <HbApplication>
#include <QTranslator>
#include "nmicons.h"
#include "nmmailboxregisterinterface.h" 
#include "nmmailboxregisterinterface_p.h" 

//mandatory keys 
const QString hsItemName("item:name");
const QString hsItemLocName("item:locname");
const QString hsitemLaunchUri("item:launchuri");
const QString hsitemPublisherId("item:publisherId");
//not mandatory 
const QString hsItemWidgetUri("widget:uri");
const QString hsItemDescription("item:description");
const QString hsItemLocDescription("item:locdescription");
//for icon - not mandatory 
const QString hsIconFileName("icon:filename");//to display specific icon from file 
const QString hsIconName("icon:name");// HbIcon 
const QString hsIconApplicationId("icon:applicationid"); // icon from associated application 

//Custom for nmHsWidgets
//NmHsWidgetParamAccountId is used to identify what email account will be monitored by 
//the registered hs widget
const QString NmHsWidgetParamAccountId("widgetparam:accountId");
//same as above but for pushing the widget to homescreen
const QString NmHsAccountId("accountId");
//NmHsWidgetParamAccountIconName is used to identify mailbox icon in application library
const QString NmHsWidgetParamAccountIconName("widgetparam:accountIconName");
//same as above but for pushing the widget to homescreen
const QString NmHsAccountIconName("accountIconName");
//This is used to identify nmhswidget in homescreen
const QString NmHsWidget("nmhswidget");

//parameter values
const QString NmPublisherName("NmMailboxRegisterInterface");
const QString NmLaunchUri("appto://0x200255BA?activityname=EmailInboxView");
//URI for the defined service
const QLatin1String interfaceUri("com.nokia.symbian.IEmailRegisterAccount");

/** Localization file name*/
const QString KNmRegisterLocFileName = "nmregister_";
/** Localization file location*/
const QString KNmRegisterLocLocation = "/translations/";

/*!
 Constructor
 */
NmMailboxRegisterInterface::NmMailboxRegisterInterface(QObject *parent) :
    QObject(parent), m_d(new NmMailboxRegisterInterfacePrivate(this))
{
    NM_FUNCTION;
}

/*!
 Destructor
 */
NmMailboxRegisterInterface::~NmMailboxRegisterInterface()
{
    NM_FUNCTION;
    
    delete m_d;
}

/*!
 registerNewMailbox
 */
bool NmMailboxRegisterInterface::registerNewMailbox(
    quint64 accountId,
    QString accountName,
    QString accountIconName)
{
    NM_FUNCTION;
    
    return m_d->registerNewMailbox(accountId, accountName, accountIconName);
}

/*!
 updateMailboxName
 */
bool NmMailboxRegisterInterface::updateMailboxName(quint64 accountId, QString newName)
{
    NM_FUNCTION;
    
    return m_d->updateMailboxName(accountId, newName);
}

/*!
 unregisterMailbox
 */
bool NmMailboxRegisterInterface::unregisterMailbox(quint64 accountId)
{
    NM_FUNCTION;
    
    return m_d->unregisterMailbox(accountId);
}

/*!
 Constructor
 */
NmMailboxRegisterInterfacePrivate::NmMailboxRegisterInterfacePrivate(QObject *parent) :
    XQServiceProvider(interfaceUri, parent), 
    mManager(new QServiceManager()), 
    mService(0),
    mTranslator(new QTranslator())
{
    NM_FUNCTION;
}

/*!
 loadMenuClientService()
 Loads the Menu client service to mService. 
 \return true if mService exists or is succesfully loaded, otherwise false.
 */
bool NmMailboxRegisterInterfacePrivate::loadMenuClientService()
{
    if (mService){ 
        return true; //service is already loaded
    }
    
    QServiceFilter filter("com.nokia.symbian.IMenuClient");
    QList<QServiceInterfaceDescriptor> interfaces = mManager->findInterfaces(filter);
    if(interfaces.isEmpty()){
        return false; //no maching interfaces available
    }
    
    mService = mManager->loadInterface(interfaces.first());
    if(!mService){
        return false; //loading interface failed
    }
    
    return true; //succesfully loaded the service
}

/*!
 Destructor
 */
NmMailboxRegisterInterfacePrivate::~NmMailboxRegisterInterfacePrivate()
{
    NM_FUNCTION;

    delete mTranslator;
    mTranslator = NULL;

    delete mService;
    mService = NULL;

    delete mManager;
    mManager = NULL;
}

bool NmMailboxRegisterInterfacePrivate::registerNewMailbox(
    quint64 accountId,
    QString accountName,
    QString accountIconName)
{
    NM_FUNCTION;
	
    if(!loadMenuClientService()){
        NM_ERROR(1,"loadService() failed!");
        return false; 
    }
    
    //localization
    QString lang = QLocale::system().name();
    bool loadSucceed = mTranslator->load(KNmRegisterLocFileName + lang, KNmRegisterLocLocation);
    QCoreApplication::installTranslator(mTranslator);
    
    //check that accountIconName is valid otherwise pass default mailbox icon
    HbIcon mailBoxIcon(accountIconName);
    if (mailBoxIcon.pixmap().height() == 0) {
        mailBoxIcon = NmIcons::getIcon(NmIcons::NmIconDefaultMailbox);
        accountIconName = mailBoxIcon.iconName();
    }

    QMap<QString, QVariant> map;
    //------------------------------
    map[hsItemLocName] = "loc://" + KNmRegisterLocFileName + "/txt_mailwdgt_dblist_mail";
    map[hsitemLaunchUri] = formLaunchUri(accountId);
    map[hsitemPublisherId] = NmPublisherName;
    map[hsItemWidgetUri] = NmHsWidget;
    map[hsItemDescription] = accountName;
    map[hsIconFileName] = accountIconName;
    // to add widget params that are mapped to widgets properties
    map[NmHsWidgetParamAccountId] = QString::number(accountId);
    map[NmHsWidgetParamAccountIconName] = accountIconName;
    //------------------------------

    bool retVal(false);
    bool ret(false);
    ret = QMetaObject::invokeMethod( mService,"add",
        Qt::DirectConnection,
        Q_RETURN_ARG(bool, retVal),
        Q_ARG(QVariantMap, map));

    //Push the registered widget also to homescreen right away
    bool pushRetVal = pushWidgetToHomescreen(accountId, accountIconName);

    return (retVal && ret && pushRetVal);
}

bool NmMailboxRegisterInterfacePrivate::updateMailboxName(quint64 accountId, QString newName)
{
    NM_FUNCTION;

    if(!loadMenuClientService()){
    	NM_ERROR(1,"loadService() failed!");
        return false; 
    }
    
    QMap<QString, QVariant> map;
    //------------------------------
    map[hsitemLaunchUri] = formLaunchUri(accountId);
    map[hsitemPublisherId] = NmPublisherName;
    map[hsItemWidgetUri] = NmHsWidget;
    map[NmHsWidgetParamAccountId] = QString::number(accountId);
    //------------------------------

    QList<QVariantMap> list; //list of items that mach the query
    bool retVal(false);
    bool ret(false);
    ret = QMetaObject::invokeMethod(mService, "getList", 
        Qt::DirectConnection, 
        Q_RETURN_ARG(QList<QVariantMap> , list),
        Q_ARG(QVariantMap, map));

    //Normally there should be only one instance, but just in case
    while (!list.isEmpty()) {
        //update the account name
        list.first()[hsItemDescription] = newName;
        //commit changes
        ret = QMetaObject::invokeMethod( mService,"add",
            Qt::DirectConnection,
            Q_RETURN_ARG(bool, retVal),
            Q_ARG(QVariantMap, list.first()));
        list.removeFirst();
    }
    //if either invoke or "getList" or "add" return failure
    return (retVal && ret);
}

bool NmMailboxRegisterInterfacePrivate::unregisterMailbox(quint64 accountId)
{
    NM_FUNCTION;

    if(!loadMenuClientService()){
    	NM_ERROR(1,"loadService() failed!");
        return false; 
    }
    
    QMap<QString, QVariant> map;
    //------------------------------
    map[hsitemLaunchUri] = formLaunchUri(accountId);
    map[hsitemPublisherId] = NmPublisherName;
    map[hsItemWidgetUri] = NmHsWidget;
    map[NmHsWidgetParamAccountId] = QString::number(accountId);
    //------------------------------

    QList<QVariantMap> list; //list of items that mach the query
    bool retVal(false);
    bool ret(false);
    ret = QMetaObject::invokeMethod(mService, "getList", 
        Qt::DirectConnection, 
        Q_RETURN_ARG(QList<QVariantMap> , list), 
        Q_ARG(QVariantMap, map));

    //Normally there should be only one instance, but just in case
    //Otherwise there will be ghost instances in the application list
    while (!list.isEmpty()) {

        //commit changes
        ret = QMetaObject::invokeMethod( mService,"remove",
            Qt::DirectConnection,
            Q_RETURN_ARG(bool, retVal),
            Q_ARG(QVariantMap, list.first()));
        list.removeFirst();
    }

    return (retVal && ret);
}

bool NmMailboxRegisterInterfacePrivate::pushWidgetToHomescreen(
    quint64 accountId,
    QString accountIconName)
{
    NM_FUNCTION;
    
    // load plugin
    QServiceManager manager;
    QServiceFilter filter("com.nokia.symbian.IHomeScreenClient");
    filter.setServiceName("hshomescreenclientplugin");
    QList<QServiceInterfaceDescriptor> interfaces = manager.findInterfaces(filter);

    if (interfaces.isEmpty()) {
        return false;
    }
    QObject* service = manager.loadInterface(interfaces.first());
    if (!service) {
        NM_ERROR(1,"service is null ");
        return false;
    }

    //--------------------------------
    QVariantHash map;
    map[NmHsAccountId] = QString::number(accountId);
    map[NmHsAccountIconName] = accountIconName;
    //--------------------------------

    // invoke function synchronously
    bool retVal(false);
    bool ret(false);
    ret = QMetaObject::invokeMethod( service, "addWidget",
        Qt::DirectConnection,
        Q_RETURN_ARG(bool, retVal),
        Q_ARG(QString, NmHsWidget),
        Q_ARG(QVariantHash, map));

    delete service;
    service = NULL;

    return (retVal && ret);
}

QString NmMailboxRegisterInterfacePrivate::formLaunchUri(quint64 accountId) const
{
    return NmLaunchUri + QString::number(accountId);
}
