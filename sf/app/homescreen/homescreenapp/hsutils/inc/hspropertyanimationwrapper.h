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

#ifndef HSPROPERTYANIMATIONWRAPPER_H
#define HSPROPERTYANIMATIONWRAPPER_H

#include <QScopedPointer>

#include "hsutils_global.h"
#include "hstest_global.h"

HOMESCREEN_TEST_CLASS(t_hsUtils)

class QEasingCurve;

struct HsPropertyAnimationWrapperImpl;
class HSUTILS_EXPORT HsPropertyAnimationWrapper : public QObject
{
    Q_OBJECT

public:
    HsPropertyAnimationWrapper(QObject *parent=0);
    ~HsPropertyAnimationWrapper();
    
    void setTargetObject(QObject *target);
    void setPropertyName(const QByteArray &propertyName);
    bool isRunning();
    void setEndValue(const QVariant &value);
    void setDuration(int msecs);
    void setForward();
    void setBackward();
    bool isForward() const;
    void setEasingCurve(const QEasingCurve &curve);

signals:
    void finished();

public slots:
    void start();
    void stop();

private:
    Q_DISABLE_COPY(HsPropertyAnimationWrapper)

 
private:
    QScopedPointer<HsPropertyAnimationWrapperImpl> mImpl;
    HOMESCREEN_TEST_FRIEND_CLASS(t_hsUtils)
};

#endif // HSPAGEINDICATOR_H
