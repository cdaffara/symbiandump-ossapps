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

#include "cntappservicehandler.h"

CntAppServiceHandler::CntAppServiceHandler(QObject *parent):
    XQServiceProvider(QLatin1String("com.nokia.services.phonebookappservices.Launch"), parent)
{
    publishAll();
}

CntAppServiceHandler::~CntAppServiceHandler()
{
}

/*
Empty implementation as highway takes care of app launching/bringing to foreground
*/
void CntAppServiceHandler::launch()
{
}
