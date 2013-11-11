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
* Stub version of epoc32\include\platform\mw\afactivation.h to be used in desktop build.
*
*/
#ifndef AFACTIVATION_H
#define AFACTIVATION_H


#include <QObject>
#include <QString>
#include <QStringList>
#include <QVariant>

#include "afactivities_global.h"

class AfActivation : public QObject
{
    Q_OBJECT

public:
    AfActivation(QObject *parent = 0) {}

signals:
    void activated(Af::ActivationReason reason, QString name, QVariantHash parameters);

public slots:
    QVariantHash parameters() const {return QVariantHash();}
    Af::ActivationReason reason() const {return Af::ActivationReasonNormal;}
    QString name() const {return "";}
};

#endif // AFACTIVATION_H
