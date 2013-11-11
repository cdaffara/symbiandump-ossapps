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
* Description:  stub 
* 
*/

#ifndef XQSERVICEUTILSTUB_H
#define XQSERVICEUTILSTUB_H

#include <QObject>
#include <QString>

class XQServiceUtil
{
public:
    static void toBackground(bool value);
    static bool isEmbedded(); 
    static bool isService();

    static QString interfaceName();
    static QString operationName();    
};

#endif
