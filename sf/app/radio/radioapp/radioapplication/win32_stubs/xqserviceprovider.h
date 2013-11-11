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

#ifndef XQSERVICEABSTRACTSERVICE_H
#define XQSERVICEABSTRACTSERVICE_H

// THIS IS A STUB CLASS FOR WIN32 ENVIRONMENT!
// It is only meant to compile, not to actually do anything

#include <QObject>
#include <QString>
#include <QVariant>
#include <QSet>
#include <QStringList>

class XQRequestInfo
{
public:

    bool isValid() const { return true; }

    void setEmbedded(bool) {}
    bool isEmbedded() const { return false; }
    void setBackground(bool) {}
    bool isBackground() const { return false; }
    bool isSynchronous() const { return false; }
    void setForeground(bool) {}
    bool isForeground() const { return true; }

    quint32 clientSecureId() const { return VID_DEFAULT; }
    quint32 clientVendorId() const { return VID_DEFAULT; }
    QSet<int> clientCapabilities() const { return QSet<int>(); }
    int id() const { return -1; }

    void setInfo(const QString&, const QVariant&) {}
    QVariant info(const QString&) const { return QVariant(); }
    QStringList infoKeys() const { return QStringList(); }

};

class XQServiceProvider : public QObject
{
    Q_OBJECT
public:
    explicit XQServiceProvider( const QString& service, QObject* parent = 0 );
    ~XQServiceProvider();

    void SetPlugin(QObject*) {}

Q_SIGNALS:
    void returnValueDelivered();
    void clientDisconnected();
    
protected:    
    void publishAll() {}
    int setCurrentRequestAsync() { return -1; }
    bool completeRequest(int, const QVariant&) { return true; }
    XQRequestInfo requestInfo() const { return XQRequestInfo(); }

    template <typename T> 
    inline bool completeRequest(int index, const T& retValue)
    {
        QVariant retValueVariant;        
        retValueVariant.setValue(retValue);
        return completeRequest(index, retValueVariant);
    }
    
};

#endif
