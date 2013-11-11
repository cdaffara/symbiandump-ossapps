/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0""
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description: HbValidator stub
*
*/

#ifndef HBVALIDATOR_H
#define HBVALIDATOR_H

#include <hbglobal.h>
#include <QObject>

#include <QValidator>

class HbValidator : public QObject
{
public:
    HbValidator(QObject *parent=0);
    virtual ~HbValidator();

    void addField(QValidator *validator, const QString &defaultValue);

public:
    
    QList<QValidator*> mValidators;
    QList<QString> mDefaultValues;
    
};

#endif // HBVALIDATOR_H
