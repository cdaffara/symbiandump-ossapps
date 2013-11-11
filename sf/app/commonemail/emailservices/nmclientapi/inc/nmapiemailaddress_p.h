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
#ifndef NMAPIEMAILADDRESS_P_H
#define NMAPIEMAILADDRESS_P_H

#include <QSharedData>

#include <QString>

namespace EmailClientApi
{

class NmApiEmailAddressPrivate : public QSharedData
{
public:
    NmApiEmailAddressPrivate()
    {
    }
    
    virtual ~NmApiEmailAddressPrivate()
    {
    }
    
    QString displayName;
    QString address;
};
}
#endif /* NMAPIEMAILADDRESS_P_H_ */
