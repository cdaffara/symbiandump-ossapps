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
* Description:  Stub for FM Radio home screen widget unit testing.
*
*/

#ifndef XQREQUESTINFO_H
#define XQREQUESTINFO_H

// System includes
#include <QObject>
#include <QVariant>

class XQRequestInfo : public QObject
{
    Q_OBJECT

public:
    XQRequestInfo();
    ~XQRequestInfo();
    
    void setForeground(bool on);
    void setBackground(bool on);
    void setInfo(const QString &key, const QVariant &value);

public slots:

signals:

};

#endif // XQREQUESTINFO_H
