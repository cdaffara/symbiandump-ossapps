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

#ifndef CNTABSTRACTENGINE_H_
#define CNTABSTRACTENGINE_H_

#include <qcontactmanager.h>

class CntAbstractViewManager;
class CntThumbnailManager;
class CntExtensionManager;

QTM_USE_NAMESPACE

/*!
 * Contact application engine. Contains access to different shared
 * classes.
 */
class CntAbstractEngine
{
public:
    virtual ~CntAbstractEngine(){}
    
    virtual QContactManager& contactManager( const QString& aType ) = 0;
    virtual CntAbstractViewManager& viewManager() = 0;
    virtual CntExtensionManager& extensionManager() = 0;
    virtual CntThumbnailManager& thumbnailManager() = 0;
};

#endif /* CNTABSTRACTENGINE_H_ */
