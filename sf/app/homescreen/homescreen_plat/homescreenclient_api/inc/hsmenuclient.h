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
 * Description: hsmenuclient.h
 *
 */

#ifndef HSMENUCLIENT_H
#define HSMENUCLIENT_H

#include <QObject>
#include <QVariant>
#include <qservicemanager.h>

QTM_USE_NAMESPACE

//mandatory keys
const QString hsItemName("item:name");
// or for localized name
const QString hsItemLocName("item:locname");

const QString hsitemLaunchUri("item:launchuri");
const QString hsitemPublisherId("item:publisherId");

//not mandatory
const QString hsItemWidgetUri("widget:uri");
const QString hsItemDescription("item:description");
// or for localized name
const QString hsItemLocDescription("item:locdescription");

//for icon - not mandatory
const QString hsIconFileName("icon:filename");//to display specific icon from file
const QString hsIconName("icon:name");// HbIcon
const QString hsIconApplicationId("icon:applicationid"); // icon from associated application


class HsMenuClientPrivate;

class HsMenuClient : public QObject
{
    Q_OBJECT

public:
    HsMenuClient(QObject *parent = 0);
    ~HsMenuClient();
public slots:
   bool add(const QVariantMap &entryPreference = QVariantMap());
   bool remove(const QVariantMap &queryPreference = QVariantMap()) const;
   
   QList<QVariantMap > getList(const QVariantMap &queryPreference = QVariantMap());
  
private:
   Q_DISABLE_COPY(HsMenuClient)
    /**
     * Pointer to a private implementation.
     */
   HsMenuClientPrivate * const m_d;
};



#endif // HSMENUCLIENT_H
