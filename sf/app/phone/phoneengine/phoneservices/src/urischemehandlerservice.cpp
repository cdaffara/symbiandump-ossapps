/*!
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
* Description: Provides services to create phone calls via URIs.
*
*/

#include <HbMessageBox>
#include <HbDeviceMessageBox>
#include <e32capability.h>
#include "urischemehandlerservice.h"
#include "mpecallcontrolif.h"
#include "mpecallsettersif.h"
#include "ctiurischemeparser.h"
#include "telurischemeparser.h"
#include "qtphonelog.h"

/*!
  UriSchemeHandlerService::UriSchemeHandlerService.
 */
UriSchemeHandlerService::UriSchemeHandlerService(
    MPECallControlIF &call, MPECallSettersIF &parameters, QObject* parent)
    : 
    XQServiceProvider(
        QLatin1String("phoneui.com.nokia.symbian.IUriView"), parent),
    m_callControlIf(call),
    m_callParameters(parameters)
{
    PHONE_DEBUG("UriSchemeHandlerService::UriSchemeHandlerService");
    
    publishAll();
    
    QScopedPointer<UriSchemeParser> telUriParser( 
        new TelUriSchemeParser(this));
    m_parsers.append(telUriParser.data());
    telUriParser.take();
    
    QScopedPointer<UriSchemeParser> ctiUriParser(
        new CtiUriSchemeParser(this));
    m_parsers.append(ctiUriParser.data());
    ctiUriParser.take();
}


/*!
  UriSchemeHandlerService::~UriSchemeHandlerService.
 */
UriSchemeHandlerService::~UriSchemeHandlerService()
{
    PHONE_DEBUG("UriSchemeHandlerService::~UriSchemeHandlerService");
}


/*!
  UriSchemeHandlerService::view.
 */
bool UriSchemeHandlerService::view(const QString &uri)
{
    PHONE_DEBUG2("UriSchemeHandlerService::view URI:", uri);
    
    bool handled = false;
    
    UriSchemeParser* schemeParser = findSchemeParser(uri);
    if (schemeParser && verifyClientCapabilities()) {
        QString phoneNumber = "";
        QString dtmfString = "";
        if (schemeParser->parseUri(uri, phoneNumber, dtmfString)) {
            QString dialString = phoneNumber + dtmfString;
            
            HbDeviceMessageBox confirmationNote(
                hbTrId("txt_phone_info_call_l1").arg(dialString),
                HbMessageBox::MessageTypeQuestion, 
                NULL);
            confirmationNote.setStandardButtons(HbMessageBox::Yes | HbMessageBox::No);
            confirmationNote.setTimeout(0);
            
            const QAction *action = confirmationNote.exec();
            if (confirmationNote.isAcceptAction(action)) {
                m_callParameters.SetPhoneNumber(dialString.utf16());
                m_callParameters.SetCallTypeCommand(EPECallTypeCSVoice);
                
                handled = (KErrNone == m_callControlIf.HandleDialServiceCall());
                }
            }
        }
    
    return handled;
}


/*!
  UriSchemeHandlerService::findSchemeParser.
 */
UriSchemeParser* UriSchemeHandlerService::findSchemeParser(
    const QString &uri)
{
    PHONE_DEBUG("UriSchemeHandlerService::findSchemeParser");
    
    UriSchemeParser* handler = NULL;
    
    QList<UriSchemeParser*>::const_iterator it = m_parsers.constBegin();
    for (;(it != m_parsers.constEnd()) && (!handler); ++it) {
        if ((*it)->isSupportedUriScheme(uri)) {
            handler = (*it); 
        }
    }
    
    return handler;
}


/*!
  UriSchemeHandlerService::verifyClientCapabilities.
  Checks that client has enough capabilities for making a phone call.
 */
bool UriSchemeHandlerService::verifyClientCapabilities() const
{
    PHONE_DEBUG("UriSchemeHandlerService::verifyClientCapabilities");
    
    QSet<int> capabilities = requestInfo().clientCapabilities();
    return (capabilities.contains(ECapabilityNetworkServices) &&
            capabilities.contains(ECapabilityNetworkControl));
}
