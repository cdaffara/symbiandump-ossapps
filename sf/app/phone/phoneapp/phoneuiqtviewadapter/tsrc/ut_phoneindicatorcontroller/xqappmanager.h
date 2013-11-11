/*!
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
*/

#ifndef XQAPPMANAGER_H_
#define XQAPPMANAGER_H_

#include<QObject>
#include<QDebug>
#include "xqaiwrequest.h"
#include "qservicefilter.h"
#include "qserviceinterfacedescriptor.h"
 
 /*!
 * Mock
  */
class XQApplicationManager: public QObject
{
Q_OBJECT
public:
    XQApplicationManager();
     virtual ~XQApplicationManager();
 
    XQAiwRequest* create(const QString& /*interface*/, const QString& /*operation*/, bool embedded = true);
    XQAiwRequest* create(const QString& /*service*/, const QString& /*interface*/, const QString& /*operation*/, bool embedded = true);
};

#endif /* XQAPPMANAGER_H_ */
