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

#include <QObject>
#include <QMap>
#include "phoneaction.h"

#ifndef PHONERESOURCEADAPTER_H
#define PHONERESOURCEADAPTER_H

class PhoneResourceAdapter : public QObject
{
    Q_OBJECT
public:
    static PhoneResourceAdapter* Instance(QObject *parent = 0);
    QMap<PhoneAction::ActionType, PhoneAction *> convert (int symbianResourceId, ...);
};

#endif
