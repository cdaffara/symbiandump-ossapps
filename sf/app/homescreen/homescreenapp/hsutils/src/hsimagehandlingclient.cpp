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
* Description:  Photos image handling client.
*
*/


#include "hsimagehandlingclient.h"

#include <QDir>
#include <QDebug>
#include <QVariant>
#ifdef Q_OS_SYMBIAN
#include <xqaiwinterfacedescriptor.h>
#include <xqaiwdecl.h>
#endif // Q_OS_SYMBIAN
#ifdef FF_IMAGE_EDITOR
#include <photoeditor_highway.hrh>
#endif //FF_IMAGE_EDITOR

#ifdef COVERAGE_MEASUREMENT
#pragma CTC SKIP
#endif //COVERAGE_MEASUREMENT


/*!
    \class HsImageHandlingClient
    \ingroup group_hsutils
    \brief 
*/

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
#ifdef Q_OS_SYMBIAN
HsImageHandlingClient::HsImageHandlingClient(QObject *parent)
  : QObject(parent),
    mReq(0)
{
}
#else
HsImageHandlingClient::HsImageHandlingClient(QObject *parent)
  : QObject(parent)
{
}
#endif // Q_OS_SYMBIAN

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
HsImageHandlingClient::~HsImageHandlingClient()
{
#ifdef Q_OS_SYMBIAN
    delete mReq;
#endif // Q_OS_SYMBIAN
}

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
void HsImageHandlingClient::fetch()
{
#ifdef Q_OS_SYMBIAN
    qDebug() << "HsImageHandlingClient::fetch START";

    if (mReq) {
        delete mReq;
        mReq = 0;
    }

    mReq = mAppMgr.create(XQI_IMAGE_FETCH, XQOP_IMAGE_FETCH, true);
    if (mReq) {        
        connect(mReq, SIGNAL(requestOk(const QVariant&)), this, SLOT(handleOk(const QVariant&)));
        connect(mReq, SIGNAL(requestError(int,const QString&)), this, SLOT(handleError(int,const QString&)));
        mReq->setSynchronous(false);
    } else {
        qCritical() << "HsImageHandlingClient::fetch -> Create request failed";
        return;
    }

    if (!(mReq)->send()) {
        qCritical() << "HsImageHandlingClient::fetch -> Send failed" << mReq->lastErrorMessage();;
    }

    qDebug() << "HsImageHandlingClient::fetch END";
#endif // Q_OS_SYMBIAN
}

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
void HsImageHandlingClient::edit(const QString &imagePath)
{
#ifdef Q_OS_SYMBIAN
    qDebug() << "HsImageHandlingClient::edit START";
    
    const QString service = QLatin1String("PhotoEditor");
    const QString interface = QLatin1String("com.nokia.symbian.imageeditor");
    const QString operation = QLatin1String("view(QString,int)");

   //Connect to service provider
    if (mReq) {
        delete mReq;
        mReq = 0;
    }

   //Connect to service provider
   mReq = mAppMgr.create(service, interface, operation, true);

   if (mReq) {        
        connect(mReq, SIGNAL(requestOk(const QVariant&)), this, SLOT(handleEditorOk(const QVariant&)));
        connect(mReq, SIGNAL(requestError(int,const QString&)), this, SLOT(handleEditorError(int,const QString&)));
        mReq->setSynchronous(false);

    } else {
        qCritical() << "HsImageHandlingClient::edit -> Create request failed";
        return;
    }
    QList<QVariant> args;
    args << imagePath;
    args << EEditorHighwayWallpaperCrop;
    mReq->setArguments(args);
    if (!mReq->send()) {
        qCritical() << "HsImageHandlingClient::edit -> Send failed" << mReq->lastErrorMessage();;
    }

    qDebug() << "HsImageHandlingClient::edit END";
#endif // Q_OS_SYMBIAN
}

// Aiw request responses
void HsImageHandlingClient::handleOk(const QVariant &result)
{
#ifdef Q_OS_SYMBIAN
    // disconnect error signal as it will give -4999 (EConnectionClosed) error afterwards even though image
    // is fetched successfully
    disconnect(mReq, SIGNAL(requestError(int,const QString&)), this, SLOT(handleError(int,const QString&)));

    XQAiwRequest *request = static_cast<XQAiwRequest *>(sender());
    int implementationId=-1;
    implementationId = (request->descriptor().property(XQAiwInterfaceDescriptor::ImplementationId)).toInt();

    if (result.canConvert<QString>()) {
        qDebug("HsImageHandlingClient::%x:handleOk result=%s,%s",
               implementationId,
               result.typeName(),
               qPrintable(result.value<QString>()));

        emit fetchCompleted(result.value<QString>());
    } else if (result.canConvert<QStringList>()) {
        QStringList resultList = result.value<QStringList>();
	    if (resultList.size() > 0) {
	        //for debug
	        QString msg = QString("HsImageHandlingClient::handleOk, size=")+QString::number( resultList.size() )+QString("\n");
	        for (int i(0), size(resultList.size()); i < size; ++i) {
                msg += QString::number(i) + QString(":") + resultList[i] + QString("\n");
	        }
	        qDebug()<<msg;
		}
        emit fetchCompleted(resultList[0]);
	} else {
        qCritical("AppMgrClient::%x:handleOk result=%s. Not displayable",
            implementationId,
            result.typeName());
        emit fetchFailed(-1, QString());
    }
#else
	Q_UNUSED(result)
#endif // Q_OS_SYMBIAN
}

void HsImageHandlingClient::handleError(int errorCode, const QString &errorMessage)
{
#ifdef Q_OS_SYMBIAN
    XQAiwRequest *request = static_cast<XQAiwRequest *>(sender());
    int implementationId = -1;
    implementationId = (request->descriptor().property(XQAiwInterfaceDescriptor::ImplementationId)).toInt();

    qCritical("HsImageHandlingClient::%x:handleError code=%d, errorMessage:%s",
        implementationId, errorCode, qPrintable(errorMessage));
    emit fetchFailed(errorCode, errorMessage);
#else
	Q_UNUSED(errorCode)
	Q_UNUSED(errorMessage)
#endif // Q_OS_SYMBIAN
}

void HsImageHandlingClient::handleEditorOk(const QVariant& result)
{
    Q_UNUSED(result)
    emit editorCompleted();
}

void HsImageHandlingClient::handleEditorError(int errorCode,
                                             const QString& errorMessage)
{
    emit editorFailed(errorCode, errorMessage);
}
#ifdef COVERAGE_MEASUREMENT
#pragma CTC ENDSKIP
#endif //COVERAGE_MEASUREMENT
