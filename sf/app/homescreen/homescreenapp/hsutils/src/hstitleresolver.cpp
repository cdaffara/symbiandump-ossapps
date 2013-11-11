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

#include <QDebug>
#include <QGraphicsLinearLayout>
#include <HbTextItem>

#include "hstitleresolver.h"
#include "hsdeviceinfolistener.h"
#include <QDebug>

namespace
{
    //Offline text in home screen view title
    const char hsLocTextId_Title_Offline[] = "txt_homescreen_title_offline";
    
    //Text in home screen view title, when device is out of network coverage or no SIM card present
    const char hsLocTextId_Title_NoService[] = "txt_homescreen_title_no_service";
}


/*!
    \class HsTitleResolver
    \ingroup group_hsutils
    \brief 
*/

/*!

*/
HsTitleResolver::HsTitleResolver(QObject *parent)
    : QObject(parent),
      mCurrentStatus(HsDeviceInfoListener::NoService)
{
    mDeviceInfoListener = new HsDeviceInfoListener(this);
    mOperatorName = mDeviceInfoListener->operatorName();
    mOfflineText = hbTrId(hsLocTextId_Title_Offline);
    mNoServiceText = hbTrId(hsLocTextId_Title_NoService);
    
    connect(mDeviceInfoListener, SIGNAL(networkNameChanged(QString)), SLOT(onNetworkNameChanged(QString)));
    connect(mDeviceInfoListener, SIGNAL(statusChanged(HsDeviceInfoListener::HsDeviceInfoStatus)), SLOT(onStatusChanged(HsDeviceInfoListener::HsDeviceInfoStatus)));
    mCurrentStatus = mDeviceInfoListener->status();
}

/*!

*/
HsTitleResolver::~HsTitleResolver()
{
}

/*!

*/
QString HsTitleResolver::title()
{
    QString titleText;
    switch (mCurrentStatus) {
        case HsDeviceInfoListener::NoService:
            titleText = mNoServiceText;
            break;
        case HsDeviceInfoListener::OfflineProfile:
            titleText = mOfflineText;
            break;
        default:
            if(mOperatorName.isEmpty()) {
                //Sometimes devicelistener has not get operatorNameChanged signal when
                //profile is changed from Offline to another which has active connections.
                mOperatorName = mDeviceInfoListener->operatorName();
            }
            titleText = mOperatorName;
            break;
    };
    qDebug() << "HsTitleResolver::title() - " << titleText;    
    return titleText;
}

/*!

*/
void HsTitleResolver::onNetworkNameChanged(const QString& name)
{
    qDebug() << "HsTitleResolver::onNetworkNameChanged() - Entry";
    if (mOperatorName != name && (mCurrentStatus == HsDeviceInfoListener::ServiceAvailable)) {
        qDebug() << "HsTitleResolver::onNetworkNameChanged() - Emitting: " << name;
        mOperatorName = name;
        emit titleChanged(name);
    }
    qDebug() << "HsTitleResolver::onNetworkNameChanged() - Exit";
}

/*!

*/
void HsTitleResolver::onStatusChanged(HsDeviceInfoListener::HsDeviceInfoStatus status)
{   
    qDebug() << "HsTitleResolver::onStatusChanged() - Entry";
    if (mCurrentStatus != status) {
        mCurrentStatus = status;
        qDebug() << "HsTitleResolver::onStatusChanged() - Emitting: " << title();
        emit titleChanged(title());
    }
    qDebug() << "HsTitleResolver::onStatusChanged() - Exit";
}

