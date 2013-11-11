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

#ifndef URISCHEMEHANDLERSERVICE_H
#define URISCHEMEHANDLERSERVICE_H

#include <QObject>
#include <QString>
#include <xqserviceprovider.h>

class MPECallControlIF;
class MPECallSettersIF;
class UriSchemeParser;

class UriSchemeHandlerService : public XQServiceProvider
{
    Q_OBJECT

public:
    UriSchemeHandlerService(
        MPECallControlIF &call, 
        MPECallSettersIF &parameters, 
        QObject *parent = 0);
    ~UriSchemeHandlerService();
    
public slots:
    /*!
        \fn view(const QString& uri)
        
        This method provides handling for "call" URIs like TEL and CTI.
        It is intended to be used via Qt Highway.
        
        Usage example:
        QUrl telUri("tel:+358 40 123 4567");
        QScopedPointer<XQAiwRequest> request(mAiwMgr.create(telUri));
        if (request.isNull()) {
            // no handlers for the URI
            return;
        }
        
        QList<QVariant> args;
        args << telUri.toString();
        request->setArguments(args);
        
        QVariant uriHandled(false);
        bool requestOk = request->send(uriHandled);
    */
    bool view(const QString &uri);

private:
    UriSchemeParser *findSchemeParser(const QString &uri);
    bool verifyClientCapabilities() const;
    
private:
    MPECallControlIF &m_callControlIf;
    MPECallSettersIF &m_callParameters;
    QList<UriSchemeParser*> m_parsers;
};

#endif // URISCHEMEHANDLERSERVICE_H
