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
* Stub version of epoc32\include\platform\mw\afactivitystorage.h to be used in desktop build.
*
*/
#ifndef AFACTIVITYSTORAGE_H
#define AFACTIVITYSTORAGE_H


#include <QString>
#include <QStringList>
#include <QVariant>
#include <QScopedPointer>

#include "afactivities_global.h"

class AfActivityStorage : public QObject
{
    Q_OBJECT

public:
    AfActivityStorage(QObject *parent = 0) {}
    virtual ~AfActivityStorage() {}

public slots:
    bool saveActivity(const QString &activityId, const QVariant &activityData, const QVariantHash &metadata) {return true;}
    bool removeActivity(const QString &activityId) {return true;}

    QStringList allActivities() const {return QStringList();}

    QVariant activityData(const QString &activityId) const {return QVariant();}
    QVariantHash activityMetaData(const QString &activityId) const {return QVariantHash();}

private:
    Q_DISABLE_COPY(AfActivityStorage)

};

#endif // AFACTIVITYSTORAGE_H
