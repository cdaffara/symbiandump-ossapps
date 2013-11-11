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

#ifndef NMAPIMAILBOX_P_H
#define NMAPIMAILBOX_P_H

#include <QSharedData>

namespace EmailClientApi
{

class NmApiMailboxPrivate : public QSharedData
{
public:
    NmApiMailboxPrivate()
    {

    }

    virtual ~NmApiMailboxPrivate()
    {

    }

    quint64 id;
    QString name;
    QString address;
};

}

#endif /* NMAPIMAILBOX_P_H */
