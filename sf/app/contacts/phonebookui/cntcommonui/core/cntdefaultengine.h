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

#ifndef CNTDEFAULTENGINE_H_
#define CNTDEFAULTENGINE_H_

#include <QObject>
#include <QList>
#include <cntabstractengine.h>

class CntDefaultEngine : public QObject, public CntAbstractEngine
{
    Q_OBJECT
public:
    CntDefaultEngine( CntAbstractViewManager& aManager );
    ~CntDefaultEngine();
    
public:
    QContactManager& contactManager( const QString& aType );
    CntAbstractViewManager& viewManager();
    CntExtensionManager& extensionManager();
    CntThumbnailManager& thumbnailManager();
    
private:
    QList<QContactManager*> mBackends;
    CntAbstractViewManager& mViewManager;
    CntExtensionManager* mExtManager;
    CntThumbnailManager* mThumbnail;
};

#endif /* CNTDEFAULTENGINE_H_ */
