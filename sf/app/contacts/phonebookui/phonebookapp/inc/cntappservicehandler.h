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

#ifndef CNTAPPSERVICEHANDLER_H
#define CNTAPPSERVICEHANDLER_H

#include <xqserviceprovider.h>

class CntAppServiceHandler : public XQServiceProvider
{
    Q_OBJECT
public:
    CntAppServiceHandler(QObject *parent = 0);
    ~CntAppServiceHandler();

public slots:
    void launch();

};

#endif /* CNTAPPSERVICEHANDLER_H */
