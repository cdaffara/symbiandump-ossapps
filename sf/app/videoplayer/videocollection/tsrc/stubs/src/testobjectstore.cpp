/*
* Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include "testobjectstore.h"

TestObjectStore::TestObjectStore()
{
    
}

TestObjectStore::~TestObjectStore()
{
    while(!mObjects.isEmpty())
    {
        QObject *obj = mObjects.takeFirst();
        delete obj;
    }
}

TestObjectStore &TestObjectStore::instance()
{
    static TestObjectStore _store;
    return _store;
}

void TestObjectStore::addObject(QObject *object)
{
    if(connect(object, SIGNAL(destroyed(QObject *)), this, SLOT(objectDestroyed(QObject *))))
    {
        mObjects.append(object);
    }
}

void TestObjectStore::objectDestroyed(QObject *obj)
{
    if(obj)
    {
        disconnect(obj, SIGNAL(destroyed(QObject *)), this, SLOT(objectDestroyed(QObject *)));
        mObjects.removeOne(obj);
    }
}
