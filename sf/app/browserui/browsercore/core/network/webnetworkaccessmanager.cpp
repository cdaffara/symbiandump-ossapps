/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
*
* This program is free software: you can redistribute it and/or modify
* it under the terms of the GNU Lesser General Public License as published by
* the Free Software Foundation, version 2.1 of the License.
* 
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU Lesser General Public License for more details.
*
* You should have received a copy of the GNU Lesser General Public License
* along with this program.  If not, 
* see "http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html/".
*
* Description:
*
*/

#include <QNetworkRequest>
#include <QNetworkReply>
#include <QAuthenticator>
#include <QNetworkInterface>
#if QT_VERSION >= 0x040500
#include <QNetworkDiskCache>

#endif

#include "bedrockprovisioning.h"
#include "wrtbrowsercontainer.h"
#include "webcookiejar.h"
#include "webnetworkaccessmanager.h"

#include "WebDialogProvider.h"

#include "networkerrorreply.h"
#include "SchemeHandlerBr.h"
#include <QWebFrame>

namespace WRT {

WebNetworkAccessManager::WebNetworkAccessManager(WrtBrowserContainer* container, QObject* /*parent*/) : QNetworkAccessManager(container), m_browserContainer(container)
{
    m_cookieJar = new CookieJar();
    m_reply = NULL;
    this->setCookieJar(m_cookieJar);
    connect(this, SIGNAL(authenticationRequired(QNetworkReply *, QAuthenticator *)), m_browserContainer, SLOT(slotAuthenticationRequired(QNetworkReply *, QAuthenticator *)));
    connect(this, SIGNAL(proxyAuthenticationRequired(const QNetworkProxy & , QAuthenticator * )), m_browserContainer, SLOT(slotProxyAuthenticationRequired(const QNetworkProxy &, QAuthenticator *)));
    connect(this, SIGNAL(finished(QNetworkReply *)), this, SLOT(onfinished(QNetworkReply *)));
    setupCache();
    setupNetworkProxy();
}

int WebNetworkAccessManager::activeNetworkInterfaces()
{
  int count = 0;
  QList<QNetworkInterface> allNIFs = QNetworkInterface::allInterfaces();
  foreach (QNetworkInterface aNIF, allNIFs) {
    if (aNIF.isValid() && aNIF.flags() & QNetworkInterface::IsUp && !(aNIF.flags() & QNetworkInterface::IsLoopBack))
        count++;
  }
  return count;
}

void WebNetworkAccessManager::onfinished(QNetworkReply* reply)
{
    QNetworkReply::NetworkError networkError = reply->error();
    QString requestUrl = reply->request().url().toString(); 

    if (networkError != QNetworkReply::OperationCanceledError && 
        networkError != QNetworkReply::NoError)
    {
        QString errorMsg = reply->errorString();
        
        // check for previously flagged error from scheme handler
        if (m_schemeError == SchemeHandler::SchemeUnsupported) {
           // override error message from Qt network access manager
           errorMsg = qtTrId("txt_browser_error_dialog_link_not_associated");
        } else if (m_schemeError == SchemeHandler::LaunchFailed) {
           // override error message from Qt network access manager
           errorMsg = qtTrId("txt_browser_error_dialog_application_not_responding");
        } else if (activeNetworkInterfaces() == 0) {
            errorMsg = "Network not available";
        } else {
            int httpErrorCode = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
            if (httpErrorCode) {
                QString httpErrorStr = QString ("HTTP %1 ").arg(httpErrorCode);
                QString httpReasonStr = reply->attribute(QNetworkRequest::HttpReasonPhraseAttribute).toString();
                errorMsg = httpErrorStr + httpReasonStr;
	         }
        }

        emit networkErrorHappened(errorMsg); 
        emit networkErrorUrl(requestUrl);
    }
}

WebNetworkAccessManager::~WebNetworkAccessManager()
{
    delete m_cookieJar;
    setCache(NULL);
}

QNetworkReply* WebNetworkAccessManager::createRequest(Operation op, const QNetworkRequest &request, QIODevice *outgoingData)
{
    QNetworkRequest req = request;
    
    m_schemeError = SchemeHandler::NoError; // clear previous error
    req.setAttribute(QNetworkRequest::HttpPipeliningAllowedAttribute, true);
	// improve performance by
	// load from cache if available, otherwise load from network. 
	// Note that this can return possibly stale (but not expired) items from cache.
	// QNetworkRequest::PreferNetwork is default value
	req.setAttribute(QNetworkRequest::CacheLoadControlAttribute, QNetworkRequest::PreferCache);

    if (m_reply != NULL) {
        delete m_reply;
        m_reply = NULL;
    }
    QNetworkReply* reply = NULL;


    if(m_browserContainer->mainFrame()) {
        if(m_browserContainer->mainFrame()->url().scheme() == "https") {

            if (op == QNetworkAccessManager::PostOperation && req.url().scheme() == "http") {

                m_text = tr("Secure Page Warning:");
                m_informativeText = tr("Do you want to continue?");
                m_detailedText = tr("Submit your data over an unencrypted connection.");
                m_buttons = QMessageBox::Yes | QMessageBox::No;
                m_defaultButton = QMessageBox::Yes;
                m_icon = QMessageBox::Warning;

                m_req = &req;

                emit showMessageBox(this);
                reply =  m_reply;
            }
        }
    }
    if(request.url().scheme() == "qrc")
        { 
            reply = new NetworkErrorReply(QNetworkReply::ProtocolUnknownError, "Unknown scheme", request.url());
            QMetaObject::invokeMethod(reply, "finished", Qt::QueuedConnection);
        }
                
    if (reply == NULL) {
		reply = createRequestHelper(op, req, outgoingData);
    }

    return reply;
}

void WebNetworkAccessManager::onMessageBoxResponse(int retValue)
{

    if (retValue == QMessageBox::No) {
        m_reply = new NetworkErrorReply(QNetworkReply::ContentAccessDenied, "Not allowed", m_req->url());
        //a finished signal will be sent and the member is invoked as soon as the application enters the main event loop
        QMetaObject::invokeMethod(m_reply, "finished", Qt::QueuedConnection);
    }
}


QNetworkReply* WebNetworkAccessManager::createRequestHelper(Operation op, const QNetworkRequest &request, QIODevice *outgoingData)
{
    QNetworkRequest req = request;

    // Don't load the content if we are restoring the session 
    if (m_browserContainer->restoreSession()) {
        // handled in scheme handler - block network access
        QNetworkReply* reply = new NetworkErrorReply(QNetworkReply::OperationCanceledError, "Page Restored", req.url());
        QMetaObject::invokeMethod(reply, "finished", Qt::QueuedConnection);
        return reply;
    }
    
    // Some schemes such as tel are handled by scheme handler.
    m_schemeError = m_browserContainer->schemeHandler()->HandleSpecialScheme(req.url());
    if (m_schemeError == SchemeHandler::NoError) {
        // handled in scheme handler - block network access
        QNetworkReply* reply = new NetworkErrorReply(QNetworkReply::OperationCanceledError, "Scheme Handled", req.url());
        QMetaObject::invokeMethod(reply, "finished", Qt::QueuedConnection);
        return reply;
    }
    // else let Qt network access manager create request but if it fails 
    // m_schemeError may be used to determine an appropriate error message

    //Accept-Language header
    QLocale language;
    QString langCountryCode = language.name();
    //QLocale::name returns the language and country as a  string of the form "language_country", where
    //language is a lowercase, two-letter ISO 639 language code, and country is an uppercase, two-letter 
    //ISO 3166 country code. But the format is expected to be "language-country". Note that hyphen is expected 
    //instead of underscore.
    langCountryCode.replace(QString("_"), QString("-"));
    req.setRawHeader("Accept-Language", langCountryCode.toUtf8());

    return QNetworkAccessManager::createRequest(op, req, outgoingData);
}

void WebNetworkAccessManager::setupNetworkProxy()
{
   QNetworkProxy proxy;
	 
   QString proxyString = BEDROCK_PROVISIONING::BedrockProvisioning::createBedrockProvisioning()->valueAsString("NetworkProxy");
   QString portString = BEDROCK_PROVISIONING::BedrockProvisioning::createBedrockProvisioning()->valueAsString("NetworkPort");
   
   if (proxyString.isEmpty())
   	{
      proxy.setType(QNetworkProxy::NoProxy);
      proxy.setHostName("");
      proxy.setPort(0);
   	}
   	else
		{
      proxy.setType(QNetworkProxy::HttpProxy);
      proxy.setHostName(proxyString);
      proxy.setPort(portString.toInt());
 		}

   	setProxy(proxy);
}
// Setup cache
// Need to use WrtSettingsUI to setup Disk Cache Directory Path
void WebNetworkAccessManager::setupCache()
{

#if QT_VERSION >= 0x040500
    qDiskCache = new QNetworkDiskCache(this);

    
    if ( !BEDROCK_PROVISIONING::BedrockProvisioning::createBedrockProvisioning()->value("DiskCacheEnabled").toBool() ) 
		return;

    QString diskCacheDir = BEDROCK_PROVISIONING::BedrockProvisioning::createBedrockProvisioning()->value("DiskCacheDirectoryPath").toString();
    if(diskCacheDir.isEmpty()) return;
    // setup cache
    qDiskCache->setCacheDirectory(diskCacheDir);

    int cacheMaxSize = BEDROCK_PROVISIONING::BedrockProvisioning::createBedrockProvisioning()->value("DiskCacheMaxSize").toInt();
    qDiskCache->setMaximumCacheSize(cacheMaxSize);

    setCache(qDiskCache);

#endif
}

}
