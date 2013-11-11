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

#ifndef TESTOBJECTSTORE_H
#define TESTOBJECTSTORE_H

#include <qobject.h>
#include <qpointer.h>
#include <qlist.h>

class TestObjectStore : public QObject
{
    Q_OBJECT

public:
    
    static TestObjectStore &instance();
    
    void addObject(QObject *object);
    
private slots:

    void objectDestroyed(QObject *obj);
    
private:
    
    TestObjectStore();

    ~TestObjectStore();
    
    QList<QObject *> mObjects;
    
};

#endif 
