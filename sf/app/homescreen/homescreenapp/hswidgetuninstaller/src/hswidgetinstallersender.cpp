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

#include "hswidgetinstallersender.h"
#include "hswidgetcomponentdescriptor.h"

#include <QDebug>
#include <QFile>
#include <QVariantHash>
#ifdef HOMESCREEN_TEST
#include "xqrequestinfo_mock.h"
#include "xqservicerequest_mock.h"
#else
#include <xqrequestinfo.h>
#include <xqservicerequest.h>
#endif

const char INTERFACE_NAME[] = "com.nokia.symbian.IHomeScreenClient";

/*!
    \class HsWidgetInstallerSender
    \ingroup group_hswidgetuninstaller
    \brief Sends qt highway function calls to com.nokia.services.hsapplication.IHomeScreenClient service.
*/

/*!
    Constructor, \a parent.
*/
HsWidgetInstallerSender::HsWidgetInstallerSender(QObject *parent):QObject(parent)
{
}
    
/*!
    Destructor.
*/
HsWidgetInstallerSender::~HsWidgetInstallerSender()
{

}

/*!
    Sends gt highway command to com.nokia.services.hsapplication.IHomeScreenClient service. \a functionSignature function 
    name, \a widgetDescriptor parsed widget descriptor.
*/
void HsWidgetInstallerSender::widgetChanged(QString functionSignature, HsWidgetComponentDescriptor &widgetDescriptor)
{
    QVariantHash widgetDescriptorHash;
    widgetDescriptorHash["uri"] = widgetDescriptor.uri(); 
    widgetDescriptorHash["title"] = widgetDescriptor.title(); 
    widgetDescriptorHash["description"] = widgetDescriptor.description(); 
    widgetDescriptorHash["iconUri"] = widgetDescriptor.iconUri(); 
    widgetDescriptorHash["hidden"] = widgetDescriptor.hidden(); 
    widgetDescriptorHash["serviceXml"] = widgetDescriptor.serviceXml(); 
    widgetDescriptorHash["version"] = widgetDescriptor.version();
    widgetDescriptorHash["installationPath"] = widgetDescriptor.installationPath(); 
    widgetDescriptorHash["Hs::translationFilename"] = widgetDescriptor.translationFilename();
    
    XQServiceRequest snd(INTERFACE_NAME, functionSignature, true);
    XQRequestInfo requestInfo = snd.info();
    requestInfo.setBackground(true);
    snd.setInfo(requestInfo);
    snd << widgetDescriptorHash;
    bool res=snd.send();
    if (!res) {
        qDebug() << "error send: " << snd.latestError();
    }

}

