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
* Description:   helper class to go around a bug in QSignalSpy.
*/

#ifndef __METADATASIGNALSPY_H
#define __METADATASIGNALSPY_H

#include <qobject.h>
#include <qvariant.h>

class MetaDataSignalSpy : QObject 
{
    Q_OBJECT
    
public:
    MetaDataSignalSpy(QObject* testObject, const char *signal) 
    {
        count = 0;
        connect(testObject, signal, this, SLOT(detailsReady(QVariant&)));
    }
    
    virtual ~MetaDataSignalSpy() {}
    
    void clear()
    {
        arg = QVariant();
        count = 0;
    }
    
private slots:
    void detailsReady(QVariant& arg)
    {
        count++;
        this->arg = arg;
    }

public:    
    int count;
    QVariant arg;
};


#endif // __METADATASIGNALSPY_H
