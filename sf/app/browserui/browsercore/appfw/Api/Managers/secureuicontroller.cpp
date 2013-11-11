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

#include <QSslError>

#include "secureuicontroller_p.h"
#include "secureuicontroller.h"
#include "webpagecontroller.h"

namespace WRT {

SecureUIControllerPrivate::SecureUIControllerPrivate(SecureUIController* qq) :
    q(qq),
    m_widgetParent(0),
    m_internalState(NOLOAD),
    m_secureState(SecureUIController::unsecureLoadFinished)
{
    m_widgetParent = new QWidget();
}

SecureUIControllerPrivate::~SecureUIControllerPrivate()
{
    delete(m_widgetParent);
}



/*!
 * \class SecureUIController
 *
 * \brief Manages Secure UI
 *
 * This class is responsible for managing secure UI.
 * All secure UI operations go through this class, such as checking the secure connection,
 * mixed content, prompt certificate dialog etc.
 *
 * Each QWebpage (which is equivalent to a window, where loads can happen
 * continoussly,  should have an instance of its own to maintain the state correctly.
 *
 * This class implements a secure UI state machine using 4-bit opration:
 *
 * -# TOPLEVELLOADED (0b0001) is set when urlChanged signal is received
 * -# TOPLEVELSECURE (0b0010 is set when the top level url request is https
 * -# TOPLEVELUNTRUSTED (0b0100) is set when the top level response is from a site with bad certificate, but user ignores it
 * -# SUBLOADUNSECURE (0b1000) is set when any of subloads like image, css is http
 */

/*!
  Basic SecureUIController constructor requires a parent QObject
*/
SecureUIController::SecureUIController ( QObject* parent ) :
    QObject ( parent ),
    d(new SecureUIControllerPrivate(this))
{
}

/*!
  SecureUIController destructor destoys private data
*/
SecureUIController::~SecureUIController()
{
    delete d;
}

/* public function */
/*!
  get the saved trusted host list
*/
QList<QString> * SecureUIController::sslTrustedHostList()
{
    return &(d->m_sslTrustedHostList);
}

/*!
  get the current secure state
*/
int SecureUIController::secureState()
{
    //qDebug()<< __func__ <<  "secureState = "<< d->m_secureState;
    return d->m_secureState;
}


/*!
 set the saved trusted host list
*/
void SecureUIController::setSecureState(int state)
{
    d->m_internalState = state;
}

/*!
  get the saved QNetworkReply pointer
*/
QNetworkReply* SecureUIController::getReply()
{
    return d->m_reply;
}

/*!
  set the saved QNetworkReply
*/
void SecureUIController::setReply(QNetworkReply* reply)
{
    d->m_reply = reply;
}

/*!
 * Public slot to set secureState TOPLEVELLOADED bit AND
 * set secureState TOPLEVELSECURE bit if top level scheme is https,
 * otherwise emit unsecureLoadFinished signal
 *
 * Should be called when the top level response comes
 */
void SecureUIController::setTopLevelScheme(const QUrl & url)
{
    //qDebug()<<"setTopLevelScheme secureState = "<<d->m_internalState<<url;
    d->m_internalState |= TOPLEVELLOADED;
   
    if (url.scheme().toLower() == "https") {
        d->m_internalState |= TOPLEVELSECURE;
    }

    /* Save the current secure state */
    determineSecureState();

    /* to display secure icon earlier */
    emit pageSecureState(d->m_secureState );

}

/*!
 * Public slot to set secureState SUBLOADUNSECURE bit if subload scheme is http
 *
 * Should be called for every load response
 */

void SecureUIController::secureCheck(QWebFrame* frame, QNetworkRequest* request)
{
    //qDebug()<<"secureCheck secureState = "<<d->m_internalState<<frame<<request->url();
    if (!(d->m_internalState & TOPLEVELLOADED)) {
        return;
    }
    if (request->url().scheme().toLower() == "http") {
            d->m_internalState |= SUBLOADUNSECURE;

            /* Save the current secure state */
            determineSecureState();
    }
}   

/*!
 * Public slot to determine final secure state, emit the right signals then reset the scure state
 *
 * emit unsecureLoadedFinished for the following cases:
 * -# Top Level Loaded, UnSecure, no certificate error, no unsecure subload
 * -# Top Level Loaded, UnSecure, no certificate error, with unsecure subload
 *
 * emit secureLoadFinished for the following case:
 * -# Top Level Loaded, Secure, no certificate error, no unsecure subload
 *
 * emit untrustedLoadFinished for the following case:
 * -# Top Level Loaded, Secure, with certificate error, no unsecure subload
 *
 * emit mixedLoadFinished for the following case:
 *-# Top Level Loaded, Secure, no certificate error, with unsecure subload
 *
 * emit untrustedMixedLoadFinished for the following case:
 * -# Top Level Loaded, Secure, with certificate error, with unsecure subload
 *
 * This function should be called when the final load ended.
 */
void SecureUIController::endSecureCheck(bool loadFinished)
{
    //qDebug()<<"endSecureCheck Internal state = "<<d->m_internalState;
    
    if (!loadFinished)
        setTopLevelScheme(WebPageController::getSingleton()->currentDocUrl());

    /* Save the current secure state */
    determineSecureState();

    emit pageSecureState(d->m_secureState);

    d->m_internalState = NOLOAD;
    //qDebug()<<"endSecureCheck secure state = "<< d->m_secureState << "Secure Controller internal state" << d->m_internalState;
}

/*!
 * Public slot to handle sslErrors, prepare the data for message box
 *
 */
void SecureUIController::onSslErrors(QNetworkReply* reply,const QList<QSslError> & error)
{
        //temp change
        reply->ignoreSslErrors();
        d->m_internalState |= TOPLEVELUNTRUSTED;

        //Save the current secure state 
        determineSecureState();

        return;
        
/*
    qDebug()<<"enter onSSLerrors d->m_internalState = "<<d->m_internalState;
    if (d->m_internalState & TOPLEVELLOADED) {
        // ignore sslerror for subloads
        return;
    }
    // check if SSL certificate has been trusted already
    QString replyHost = reply->url().host() + ":" + reply->url().port();

    if(! d->m_sslTrustedHostList.contains(replyHost)) {
        QStringList errorStrings;
        for (int i = 0; i < error.count(); ++i)
            errorStrings += error.at(i).errorString();
        QString errors = errorStrings.join(QLatin1String("\n"));
        qDebug()<<errors;
        
        m_text = tr("Secure Page Warning:");
        m_informativeText = tr("Do you want to ignore?");
        //m_detailedText = tr("%1\n%2").arg(reply->url().toString()).arg(errors);
        m_detailedText = tr("SSL certificate is not valid.");
        m_buttons = QMessageBox::Yes | QMessageBox::No;
        m_defaultButton = QMessageBox::Yes;
        m_icon = QMessageBox::Warning;

        setReply(reply);
        emit showMessageBox(this);
    }
    else {
        reply->ignoreSslErrors();
        d->m_internalState |= TOPLEVELUNTRUSTED;

        //Save the current secure state 
        determineSecureState();
    }
*/
}    

/*!
 * Public function to handle sslError message box response
 */
void SecureUIController::onMessageBoxResponse(int retValue)
{   
	  
    if (retValue == QMessageBox::Yes) {
        QNetworkReply* reply = getReply();
        if (reply) {
            QString replyHost = reply->url().host() + ":" + reply->url().port();
            reply->ignoreSslErrors();
            d->m_sslTrustedHostList.append(replyHost);
            d->m_internalState |= TOPLEVELUNTRUSTED;

            /* Save the current secure state */
            determineSecureState();
        }
    }
    else {
        emit loadStopped();
    }
}

/*!
 * Determine secure state
 */

void SecureUIController::determineSecureState() {

    //qDebug() << __func__ << "Internal State " << d->m_internalState ;
    switch (d->m_internalState) {
    case NOLOAD: 
    case TOPLEVELLOADED: //0b0001: Top Level Loaded, UnSecure, no certificate error, no unsecure subload
    case TOPLEVELLOADED | SUBLOADUNSECURE: //0b1001: Top Level Loaded, UnSecure, no certificate error, with unsecure subload
        d->m_secureState =  unsecureLoadFinished;
        break;
        
    case TOPLEVELLOADED | TOPLEVELSECURE: //0b0011: Top Level Loaded, Secure, no certificate error, no unsecure subload
        d->m_secureState = secureLoadFinished;
        break;
        
    case TOPLEVELLOADED | TOPLEVELSECURE | TOPLEVELUNTRUSTED: //0b0111: Top Level Loaded, Secure, with certificate error, no unsecure subload
        d->m_secureState = untrustedLoadFinished;
        break;
       
    case TOPLEVELLOADED | TOPLEVELSECURE | SUBLOADUNSECURE: //0b1011: Top Level Loaded, Secure, no certificate error, with unsecure subload
        d->m_secureState = mixedLoadFinished;
        break;
        
    case TOPLEVELLOADED | TOPLEVELSECURE | TOPLEVELUNTRUSTED |SUBLOADUNSECURE: //0b1111: Top Level Loaded, Secure, with certificate error, with unsecure subload
        d->m_secureState = untrustedMixedLoadFinished;
        break;
        
    default:    
        break;
    }
    //qDebug() << __func__ << "Secure State " << d->m_secureState ;
}

/*!
  \fn void SecureUIController::loadStopped();
  emitted when user select to stop the load when seeing a security certificate error
*/

/*!
  \fn void SecureUIController::mixedLoadFinished();
  emitted when secure page load finished with unsecure items
*/

/*!
  \fn void SecureUIController::secureLoadFinished();
  emitted when secure page load finished
*/

/*!
  \fn void SecureUIController::unsecureLoadFinished();
  emitted when unsecure page load finished
*/

/*!
  \fn void SecureUIController::untrustedLoadFinished();
  emitted when untrusted page load finished after user chooses to ignore the security certificate errors
*/

/*!
  \fn void SecureUIController::untrustedMixedLoadFinished();
  emitted when untrusted page load finished after user chooses to ignore the security certificate errors, and with unsecure subloads
*/

/*!
  \fn void SecureUIController::showMessageBox(SecureUIController::MessageBoxData*);
  emitted when MessageBox need to be showed
*/
} // namespace WRT
