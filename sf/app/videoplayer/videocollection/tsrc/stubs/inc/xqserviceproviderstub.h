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
* Description:   stub
*
*/

#ifndef XQSERVICEABSTRACTSERVICESTUB_H
#define XQSERVICEABSTRACTSERVICESTUB_H

#define XQ_EXPORT_PLUGIN2(x,y)

#include <qobject.h>
#include <qstring.h>
#include <QVariant>

class XQServiceProvider : public QObject
{
    Q_OBJECT
    
public:
    XQServiceProvider( const QString& service, QObject *parent = 0 );
    ~XQServiceProvider();

    //    void SetPlugin(QObject* impl_plugin);

//Q_SIGNALS:
    //void returnValueDelivered() ;
    //void clientDisconnected() ;
       
private:
};

#endif
