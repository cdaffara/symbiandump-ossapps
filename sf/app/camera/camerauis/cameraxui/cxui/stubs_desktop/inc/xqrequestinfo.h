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
* Stub version of epoc32\include\mw\xqrequestinfo.h to be used in desktop build.
*
*/

#ifndef XQREQUESTINFO_H
#define XQREQUESTINFO_H

#include <QVariant>
#include <QStringList>
#include <QSet>


class XQRequestInfo
{
    public:
        XQRequestInfo() {}
        virtual ~XQRequestInfo() {}

    public:

        bool isValid() const {return true;}

        void setEmbedded(bool on) {}
        bool isEmbedded() const {return false;}
        void setBackground(bool on) {}
        bool isBackground() const {return false;}
        bool isSynchronous() const {return false;}
        void setForeground(bool on) {}
        bool isForeground() const {return false;}

        quint32 clientSecureId() const {return 0;}
        quint32 clientVendorId() const {return 0;}
        QSet<int> clientCapabilities() const {return QSet<int>();}
        int id() const {return 0;}

        void setInfo(const QString &key, const QVariant &value) {}
        QVariant info(const QString &key) const {return QVariant();}
        QStringList infoKeys() const {return QStringList();}

};

#endif
