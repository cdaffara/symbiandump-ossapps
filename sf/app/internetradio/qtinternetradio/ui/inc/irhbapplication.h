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

#ifndef IRHBAPPLICATION_H_
#define IRHBAPPLICATION_H_

#include <HbApplication>

class IRHbApplication : public HbApplication
{
    Q_OBJECT
public:
    IRHbApplication(int &argc, char *argv[], Hb::ApplicationFlags flags = Hb::DefaultApplicationFlags);
    void initMattiAutomation();
    
private slots:
    void handleAppReady();
    
signals:
    void applicationReady();
};

#endif /* IRHBAPPLICATION_H_ */
