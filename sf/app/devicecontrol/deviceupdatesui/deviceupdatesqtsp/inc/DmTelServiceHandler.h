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
 
#ifndef DMTELSERVICEHANDLER_H
#define DMTELSERVICEHANDLER_H

#include <xqserviceprovider.h>
#include <xqservicerequest.h>
#include <xqserviceutil.h>

_LIT( KDMUIProcess, "\\deviceupdates.exe" );

class DmTelServiceHandler : public XQServiceProvider
{
    Q_OBJECT
public:
    DmTelServiceHandler(QObject *parent = 0);
    ~DmTelServiceHandler();

public slots:
    void showVersionNumber();

};

#endif /* DMTELSERVICEHANDLER_H */
