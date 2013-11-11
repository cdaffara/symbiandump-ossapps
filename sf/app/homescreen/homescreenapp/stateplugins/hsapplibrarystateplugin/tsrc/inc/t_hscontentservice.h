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
* Description:
*
*/

#ifndef T_HSCONTENTSERVICE_H
#define T_HSCONTENTSERVICE_H

#include <QObject>
#include <QMetaType>
#include <QVariant>

class HsWidgetHost;

class HsContentService : public QObject
{
    Q_OBJECT
public:
    HsContentService(QObject *parent = 0);
    ~HsContentService();

    bool widgets(const QString &uri, const QVariantHash &preferences, int &count);
    static HsContentService *instance();
signals:
    bool widgetAdded(const QString &, const QVariantHash &);
    bool widgetRemoved(const QString &, const QVariantHash &);
private:
    Q_DISABLE_COPY(HsContentService)

};

Q_DECLARE_METATYPE(HsContentService *)

#endif // T_HSCONTENTSERVICE_H
