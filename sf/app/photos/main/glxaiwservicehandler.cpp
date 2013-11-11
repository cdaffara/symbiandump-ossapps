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
* Description:   ?Description
*
*/

#include <hbapplication.h>
#include <hbview.h>
#include <hbmenu.h>
#include <hbaction.h>
#include <hbnotificationdialog.h>

#include <QtDebug>
#include <Qt>
#include <QDir>
#include <QStringList>

#include <xqserviceutil.h>
#include <xqaiwdeclplat.h>
#include <xqappmgr.h>
#include <xqaiwdecl.h>

#include <glxmodelroles.h>
#include <glxaiwservicehandler.h>
#include <glxstatemanager.h>
#include <glximageviewermanager.h>
#include <glxexternalutility.h>
#include "glxlocalisationstrings.h"

#define IMAGE_FETCHER_SERVICE_NAME QLatin1String("photos.com.nokia.symbian.IImageFetch")
#define IMAGE_FETCHER_SERVICE_DEPINTERFACE_NAME QLatin1String("photos.Image")
#define IMAGE_FETCHER_DEPSERVICE_DEPINTERFACE_NAME QLatin1String("com.nokia.services.media.Image")
#define FILE_VIEWER_SERVICE_NAME QLatin1String("photos.com.nokia.symbian.IFileView")
#define FILE_BROWSER_SERVICE_NAME QLatin1String("photos.com.nokia.symbian.IImageBrowse")
#define XQINFO_KEY_FILTER_TYPE QLatin1String("GlxFetcherFilter")

// ----------------------------------------------------------------------------
// GlxAiwServiceHandler()
// ----------------------------------------------------------------------------
//
GlxAiwServiceHandler::GlxAiwServiceHandler() :
    HbMainWindow(), 
    mStateMgr(NULL),
    mFetcherService(NULL),
    mNSDIService(NULL),
    mDSDIService(NULL),
    mImageViewerService(NULL),
	mImageBrowserService(NULL)
    {
    QString currentInterfaceName = XQServiceUtil::interfaceName();
    if( 0 == currentInterfaceName.compare(QLatin1String("com.nokia.symbian.IImageFetch")))
        {
        mFetcherService = new GlxGetImageService(this);
        }
    else if( 0 == currentInterfaceName.compare(QLatin1String("Image")))
        {
        mNSDIService = new GlxGetImageServiceNSDI(this);
        mDSDIService = new GlxGetImageServiceDSDI(this);
        }
    else if( 0 == currentInterfaceName.compare(QLatin1String("com.nokia.symbian.IFileView")))
        {
		viewport()->setAttribute(Qt::WA_AcceptTouchEvents);
		viewport()->grabGesture(Qt::PinchGesture);
        mImageViewerService = new GlxImageViewerService(this);
        }
    else if( 0 == currentInterfaceName.compare(QLatin1String("com.nokia.symbian.IImageBrowse")))
        {
        mImageBrowserService = new GlxBrowseImageService(this);
        }

#ifdef _DEBUG
	QString t;
	QStringList args = QApplication::arguments();
    foreach (QString arg, args)
        {
        t += "GlxAiwServiceHandler::cmdline arg=" + arg + "\n";
        }
	qDebug()<< t;
#endif
    }

// ----------------------------------------------------------------------------
// ~GlxAiwServiceHandler()
// ----------------------------------------------------------------------------
//
GlxAiwServiceHandler::~GlxAiwServiceHandler()
    {
    delete mStateMgr;
    delete mFetcherService;
    delete mNSDIService;
    delete mDSDIService;
    delete mImageViewerService;
	delete mImageBrowserService;
    }

void GlxAiwServiceHandler::handleClientDisconnect()
    {
    // Just quit application
    qApp->quit();
    }

// ----------------------------------------------------------------------------
// itemSelected()
// ----------------------------------------------------------------------------
//
void GlxAiwServiceHandler::itemSelected(const QModelIndex & index,QAbstractItemModel & model)
    {
    qDebug() << "GlxFetcher::itemSelected";
    if (mFetcherService && mFetcherService->isActive())
        {
        qDebug() << "GlxFetcher::itemSelected mFetcherService->isActive()";
        QVariant variant = model.data(index, GlxUriRole);
        if (variant.isValid())
            {
            QString itemPath = variant.value<QString> ();
            qDebug() << "GlxFetcher::itemSelected mFetcherService->isActive()::PATH = " << itemPath;
            QStringList list = (QStringList() << itemPath);
            mFetcherService->complete(list);
            }
        }
    
    else if (mNSDIService && mNSDIService->isActive())
        {
    qDebug() << "GlxFetcher::itemSelected mNSDIService->isActive()";
        QVariant variant = model.data(index, GlxUriRole);
        if (variant.isValid())
            {
            QString itemPath = variant.value<QString> ();
            QStringList list = (QStringList() << itemPath);
            mNSDIService->complete(list);
            }
        }
    
    else if (mDSDIService && mDSDIService->isActive())
        {
    qDebug() << "GlxFetcher::itemSelected mDSDIService->isActive()";
        QVariant variant = model.data(index, GlxUriRole);
        if (variant.isValid())
            {
            QString itemPath = variant.value<QString> ();
            QStringList list = (QStringList() << itemPath);
            mDSDIService->complete(list);
            }
        }
    }	


void GlxAiwServiceHandler::launchFetcher(QString viewTitle,int fetcherFilterType)
    {
    qDebug() << "GlxAiwServiceHandler::launchFetcher START";
    qApp->setApplicationName(viewTitle);

    mUtil = GlxExternalUtility::instance();
    mUtil->setMainWindow(this);

    mStateMgr = new GlxStateManager();
	int err = connect(mStateMgr, SIGNAL(gridItemSelected(const QModelIndex &,QAbstractItemModel &)), this,
            SLOT( itemSelected(const QModelIndex &,QAbstractItemModel &)));
	qDebug() << "GlxAiwServiceHandler::launchFetcher err = "<< err;
	
    mStateMgr->launchFetcher(fetcherFilterType);
    return;
    }

void GlxAiwServiceHandler::launchImageViewer(QString viewTitle)
    {
    qApp->setApplicationName(viewTitle);
    mUtil = GlxExternalUtility::instance();
    mUtil->setMainWindow(this);

    mStateMgr = new GlxStateManager();
    mStateMgr->launchFromExternal();
    if (mImageViewerService && mImageViewerService->asyncRequest())
        {
        connect(mImageViewerService, SIGNAL(returnValueDelivered()), this,
                SLOT(handleAnswerDelivered()));
        mImageViewerService->complete(true);
        }
    return;
    }

void GlxAiwServiceHandler::launchImageBrowser(QString viewTitle)
    {
    qApp->setApplicationName(viewTitle);
    mStateMgr = new GlxStateManager();
	mStateMgr->launchFetcher(0);
    if (mImageBrowserService && mImageBrowserService->asyncRequest())
        {
        connect(mImageBrowserService, SIGNAL(returnValueDelivered()), this,
                SLOT(handleAnswerDelivered()));
        mImageBrowserService->complete(true);
        }
    return;
    }
	
// ----------------------------------------------------------------------------
// GlxGetImageService()
// ----------------------------------------------------------------------------
//
GlxGetImageService::GlxGetImageService(GlxAiwServiceHandler* parent) :
            XQServiceProvider(IMAGE_FETCHER_SERVICE_NAME, parent),
            mServiceApp(parent)
    {
    mImageRequestIndex = -1;
    publishAll();
    connect(this, SIGNAL(clientDisconnected()), mServiceApp,
            SLOT(handleClientDisconnect()));
    }

// ----------------------------------------------------------------------------
// ~GlxGetImageService()
// ----------------------------------------------------------------------------
//
GlxGetImageService::~GlxGetImageService()
    {
    qDebug() << "GlxGetImageService::~GlxGetImageService()";
    }

// ----------------------------------------------------------------------------
// fetchFailed()
// ----------------------------------------------------------------------------
//
void GlxGetImageService::fetchFailed(int errorCode)
    {
    QStringList filesList;
    filesList.insert(0, QString::number(errorCode));//result
    doComplete(filesList);
    }

// ----------------------------------------------------------------------------
// complete()
// ----------------------------------------------------------------------------
//
void GlxGetImageService::complete(QStringList filesList)
    {
    doComplete(filesList);
    }

// ----------------------------------------------------------------------------
// doComplete()
// ----------------------------------------------------------------------------
//
void GlxGetImageService::doComplete(QStringList filesList)
    {
    if (isActive())
        {
        completeRequest(mImageRequestIndex, filesList);
        mImageRequestIndex = -1;
        connect(this, SIGNAL(returnValueDelivered()), qApp, SLOT(quit()));
        }
    }

// ----------------------------------------------------------------------------
// isActive()
// ----------------------------------------------------------------------------
//
bool GlxGetImageService::isActive()
    {
    return mImageRequestIndex > -1;
    }

// ----------------------------------------------------------------------------
// fetch()
// ----------------------------------------------------------------------------
//
void GlxGetImageService::fetch()
    {
    mImageRequestIndex = setCurrentRequestAsync();

    QString title = requestInfo().info(XQINFO_KEY_WINDOW_TITLE).toString();
    if(title.isNull()){
        title = GLX_TITLE;
    }

	int fetcherFilterType = 0; //no filter
    if(requestInfo().info(XQINFO_KEY_FILTER_TYPE).canConvert<int>()){
        fetcherFilterType = (requestInfo().info(XQINFO_KEY_FILTER_TYPE)).toInt();
        qDebug()<< "GlxGetImageService::fetch() fetcherFilterType = " << fetcherFilterType;
    }
    mServiceApp->launchFetcher(title,fetcherFilterType);
    }

// ----------------------------------------------------------------------------
// GlxGetImageService()
// ----------------------------------------------------------------------------
//
GlxGetImageServiceNSDI::GlxGetImageServiceNSDI(GlxAiwServiceHandler* parent) :
            XQServiceProvider(IMAGE_FETCHER_SERVICE_DEPINTERFACE_NAME, parent),
            mServiceApp(parent)
    {
    mImageRequestIndex = -1;
    publishAll();
    connect(this, SIGNAL(clientDisconnected()), mServiceApp,
            SLOT(handleClientDisconnect()));
    }

// ----------------------------------------------------------------------------
// ~GlxGetImageService()
// ----------------------------------------------------------------------------
//
GlxGetImageServiceNSDI::~GlxGetImageServiceNSDI()
    {
    }

// ----------------------------------------------------------------------------
// fetchFailed()
// ----------------------------------------------------------------------------
//
void GlxGetImageServiceNSDI::fetchFailed(int errorCode)
    {
    QStringList filesList;
    filesList.insert(0, QString::number(errorCode));//result
    doComplete(filesList);
    }

// ----------------------------------------------------------------------------
// complete()
// ----------------------------------------------------------------------------
//
void GlxGetImageServiceNSDI::complete(QStringList filesList)
    {
    doComplete(filesList);
    }

// ----------------------------------------------------------------------------
// doComplete()
// ----------------------------------------------------------------------------
//
void GlxGetImageServiceNSDI::doComplete(QStringList filesList)
    {
    if (isActive())
        {
        completeRequest(mImageRequestIndex, filesList);
        mImageRequestIndex = -1;
        connect(this, SIGNAL(returnValueDelivered()), qApp, SLOT(quit()));
        }
    }

// ----------------------------------------------------------------------------
// isActive()
// ----------------------------------------------------------------------------
//
bool GlxGetImageServiceNSDI::isActive()
    {
    return mImageRequestIndex > -1;
    }

// ----------------------------------------------------------------------------
// fetch()
// ----------------------------------------------------------------------------
//
void GlxGetImageServiceNSDI::fetch(QVariantMap filter, QVariant flag)
    {
    qDebug() << "GlxGetImageServiceNSDI::fetch WITH PARAMETER START";
    Q_UNUSED(filter)
    Q_UNUSED(flag)
    fetch();
    qDebug() << "GlxGetImageServiceNSDI::fetch WITH PARAMETER END";
    }

// ----------------------------------------------------------------------------
// fetch()
// ----------------------------------------------------------------------------
//
void GlxGetImageServiceNSDI::fetch()
    {
    qDebug() << "GlxGetImageServiceNSDI::fetch START";
    mImageRequestIndex = setCurrentRequestAsync();

    QString title = requestInfo().info(XQINFO_KEY_WINDOW_TITLE).toString();
    if(title.isNull()){
        title = GLX_TITLE;
    }

    mServiceApp->launchFetcher(title);
    qDebug() << "GlxGetImageServiceNSDI::fetch END";
    }

//==============================================================================//
// ----------------------------------------------------------------------------
// GlxGetImageService()
// ----------------------------------------------------------------------------
//
GlxGetImageServiceDSDI::GlxGetImageServiceDSDI(GlxAiwServiceHandler* parent) :
            XQServiceProvider(IMAGE_FETCHER_DEPSERVICE_DEPINTERFACE_NAME, parent),
            mServiceApp(parent)
    {
    mImageRequestIndex = -1;
    publishAll();
    connect(this, SIGNAL(clientDisconnected()), mServiceApp,
            SLOT(handleClientDisconnect()));
    }

// ----------------------------------------------------------------------------
// ~GlxGetImageService()
// ----------------------------------------------------------------------------
//
GlxGetImageServiceDSDI::~GlxGetImageServiceDSDI()
    {
    }

// ----------------------------------------------------------------------------
// fetchFailed()
// ----------------------------------------------------------------------------
//
void GlxGetImageServiceDSDI::fetchFailed(int errorCode)
    {
    QStringList filesList;
    filesList.insert(0, QString::number(errorCode));//result
    doComplete(filesList);
    }

// ----------------------------------------------------------------------------
// complete()
// ----------------------------------------------------------------------------
//
void GlxGetImageServiceDSDI::complete(QStringList filesList)
    {
    doComplete(filesList);
    }

// ----------------------------------------------------------------------------
// doComplete()
// ----------------------------------------------------------------------------
//
void GlxGetImageServiceDSDI::doComplete(QStringList filesList)
    {
    if (isActive())
        {
        completeRequest(mImageRequestIndex, filesList);
        mImageRequestIndex = -1;
        connect(this, SIGNAL(returnValueDelivered()), qApp, SLOT(quit()));
        }
    }

// ----------------------------------------------------------------------------
// isActive()
// ----------------------------------------------------------------------------
//
bool GlxGetImageServiceDSDI::isActive()
    {
    return mImageRequestIndex > -1;
    }

// ----------------------------------------------------------------------------
// fetch()
// ----------------------------------------------------------------------------
//
void GlxGetImageServiceDSDI::fetch(QVariantMap filter, QVariant flag)
    {
    Q_UNUSED(filter)
    Q_UNUSED(flag)
    mImageRequestIndex = setCurrentRequestAsync();
    QString title = requestInfo().info(XQINFO_KEY_WINDOW_TITLE).toString();
    if(title.isNull()){
        title = GLX_TITLE;
    }
    
    mServiceApp->launchFetcher(title);
    }

// ----------GlxImageViewerService---------------

GlxImageViewerService::GlxImageViewerService(GlxAiwServiceHandler* parent) :
    XQServiceProvider(FILE_VIEWER_SERVICE_NAME, parent),
            mServiceApp(parent), mAsyncReqId(-1), mAsyncRequest(false),
            mImageViewerInstance(NULL)

    {
    publishAll();
    connect(this, SIGNAL(clientDisconnected()), mServiceApp,
            SLOT(handleClientDisconnect()));
    }

GlxImageViewerService::~GlxImageViewerService()
    {
    if (mImageViewerInstance)
        {
        mImageViewerInstance->Close();
        }
    }

void GlxImageViewerService::complete(bool ok)
    {
    if (mAsyncReqId == -1)
        return;
    completeRequest(mAsyncReqId, QVariant(ok));
    }

bool GlxImageViewerService::view(QString file)
    {
    XQApplicationManager appmgr;
    QFile tempfile(file);
    QVariantList attrValues;
	QList<int> attrNames;
	attrNames.append(XQApplicationManager::IsProtected);
    bool ok = appmgr.getDrmAttributes(tempfile, attrNames, attrValues);
    if(attrValues.at(0).toBool()){
		HbNotificationDialog::launchDialog("NOT SUPPORTED");
        connect(this, SIGNAL(returnValueDelivered()), mServiceApp,
                SLOT(handleAnswerDelivered()));
		complete(true);
		return false;
    }
	
    XQRequestInfo info = requestInfo();
    mAsyncRequest = !info.isSynchronous();
    if (!mImageViewerInstance)
        {
        mImageViewerInstance = CGlxImageViewerManager::InstanceL();
        }
    QString filepath(QDir::toNativeSeparators(file));
    TPtrC16 str(reinterpret_cast<const TUint16*> (filepath.utf16()));
    HBufC* uri = str.Alloc();

    mImageViewerInstance->SetImageUriL(*uri);
    if (mAsyncRequest)
        {
        mAsyncReqId = setCurrentRequestAsync();
        }

    QString title = requestInfo().info(XQINFO_KEY_WINDOW_TITLE).toString();
    if(title.isNull()){
        title =GLX_IMAGE_VIEWER;
    }
    
    mServiceApp->launchImageViewer(title);
    return true;
    }

bool GlxImageViewerService::view(XQSharableFile sf)
    {
    XQApplicationManager appmgr;
    QFile tempfile(sf.fileName());
    QVariantList attrValues;
    QList<int> attrNames;
    attrNames.append(XQApplicationManager::IsProtected);
    bool ok1 = appmgr.getDrmAttributes(tempfile, attrNames, attrValues);
    if(ok1 && attrValues.at(0).toBool()){
        HbNotificationDialog::launchDialog("NOT SUPPORTED");
        connect(this, SIGNAL(returnValueDelivered()), mServiceApp,
                SLOT(handleAnswerDelivered()));
        complete(true);
        return false;
    }

    if (!mImageViewerInstance)
        {
        mImageViewerInstance = CGlxImageViewerManager::InstanceL();
        }
    RFile file;
    bool ok = sf.getHandle(file);
    if (ok)
        {
        mImageViewerInstance->SetImageFileHandleL(file);
        sf.close();
        }

    QString title = requestInfo().info(XQINFO_KEY_WINDOW_TITLE).toString();
    if(title.isNull()){
        title = GLX_IMAGE_VIEWER;
    }
    
    mServiceApp->launchImageViewer(title);
    mAsyncRequest = !XQServiceUtil::isEmbedded();

    if (mAsyncRequest)
        {
        mAsyncReqId = setCurrentRequestAsync();
        connect(this, SIGNAL(clientDisconnected()), this,
                SLOT(handleClientDisconnect()));
        }
    return true;
    }

// ----------GlxBrowseImageService---------------

GlxBrowseImageService::GlxBrowseImageService(GlxAiwServiceHandler* parent) :
    XQServiceProvider(FILE_BROWSER_SERVICE_NAME, parent),
            mServiceApp(parent), mAsyncReqId(-1), mAsyncRequest(false)

    {
    publishAll();
    connect(this, SIGNAL(clientDisconnected()), mServiceApp,
            SLOT(handleClientDisconnect()));
    }

GlxBrowseImageService::~GlxBrowseImageService()
    {
    }

void GlxBrowseImageService::complete(bool ok)
    {
    if (mAsyncReqId == -1)
        return;
    completeRequest(mAsyncReqId, QVariant(ok));
    }

void GlxBrowseImageService::browse()
    {
	mServiceApp->launchImageBrowser(GLX_TITLE);
    XQRequestInfo info = requestInfo();
    mAsyncRequest = !info.isSynchronous();
	
    if (mAsyncRequest)
        {
        mAsyncReqId = setCurrentRequestAsync();
        connect(this, SIGNAL(clientDisconnected()), this,
                SLOT(handleClientDisconnect()));
        }
    }

	
