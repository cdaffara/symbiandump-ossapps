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
#ifndef IRMEMORYCOLLECTOR_H
#define IRMEMORYCOLLECTOR_H

#include <QObject>

class IRApplication;

class IRMemoryCollector : public QObject
{
    Q_OBJECT
public:
    IRMemoryCollector(IRApplication* aApplication);
    
private slots:
    void aboutToQuit();
    
private:
    IRApplication *iApplication;
};

#endif
