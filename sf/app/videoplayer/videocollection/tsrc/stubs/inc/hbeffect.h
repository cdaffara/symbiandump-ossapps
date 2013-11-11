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
* Description:  hbdocumentloader stub
*
*/

#ifndef HBEFFECT_H
#define HBEFFECT_H

#include <qobject.h>
#include <qgraphicsitem.h>
#include <qvariant.h>
#include <qrect.h>
#include <qvariant.h>

class HbEffect : public QObject
{
    Q_OBJECT
    
    HbEffect() { }
    ~HbEffect() { }
    
public:

    enum EffectEvent
    {
        EffectFinished                = 1,
        EffectCancelled               = 2,
        EffectNotStarted              = 3
    };
    
    struct EffectStatus {
        QGraphicsItem *item;
        QString effectEvent;
        EffectEvent reason;
        QVariant userData;
    };
    
    static bool start(QGraphicsItem *item, 
                            const QString &itemType, 
                            const QString &effectEvent,
                            QObject *receiver = 0,
                            const char *member = 0,
                            const QVariant &userData = QVariant(),
                            const QRectF &extRect = QRectF())
    {
        Q_UNUSED(item);
        Q_UNUSED(itemType);
        Q_UNUSED(effectEvent);
        Q_UNUSED(receiver);
        Q_UNUSED(member);
        Q_UNUSED(userData);
        Q_UNUSED(extRect);
        
        return false;
    }

    static bool start(QGraphicsItem *item, 
                            const QString &effectEvent,
                            QObject *receiver = 0,
                            const char *member = 0,
                            const QVariant &userData = QVariant(),
                            const QRectF &extRect = QRectF())
    {
        mLastStartedEffectContainer = item;
        Q_UNUSED(effectEvent);
        Q_UNUSED(receiver);
        Q_UNUSED(member);
        Q_UNUSED(userData);
        Q_UNUSED(extRect);
        
        return false;
    }
    
    static bool start(const QList<QGraphicsItem *> &items, 
                            const QString &itemType, 
                            const QString &effectEvent,
                            QObject *receiver = 0,
                            const char *member = 0,
                            const QVariant &userData = QVariant())
    {
        Q_UNUSED(items);
        Q_UNUSED(itemType);
        Q_UNUSED(effectEvent);
        Q_UNUSED(receiver);
        Q_UNUSED(member);
        Q_UNUSED(userData);

        return false;
    }
    
    static bool effectRunning(QGraphicsItem */*item*/, const QString &/*effectEvent = QString()*/) {return false;}

    static bool cancel(
        QGraphicsItem */*item*/,
        const QString &/*effectEvent = QString()*/,
        bool /*hideEffect = false*/,
        bool /*sendCallback = true*/,
        bool /*itemIsValid = true*/) {return false;}

    static bool add(const QString &/*itemType*/, const QString &/*filePath*/, const QString &/*effectEvent = QString()*/) {return false;}
    static bool add(const QStringList &/*itemType*/, const QStringList &/*filePath*/, const QStringList &/*effectEvent = QStringList()*/) {return false;}

    static bool add(QGraphicsItem */*item*/, const QString &/*filePath*/, const QString &/*effectEvent = QString()*/) {return false;}
    static bool add(QGraphicsItem */*item*/, const QStringList &/*filePath*/, const QStringList &/*effectEvent = QStringList()*/) {return false;}
    
    static bool remove(const QString &/*itemType*/, const QString &/*filePath*/, const QString &/*effectEvent = QString()*/) {return false;}
    static bool remove(QGraphicsItem */*item*/, const QString &/*filePath*/, const QString &/*effectEvent = QString()*/) {return false;}
    static bool remove(QGraphicsItem */*item*/) {return false;}

    static void enable(QGraphicsItem */*item*/) {}
    static void disable(QGraphicsItem */*item*/) {}
    
    
    static QGraphicsItem* mLastStartedEffectContainer;
    
private:
    
};

#endif

