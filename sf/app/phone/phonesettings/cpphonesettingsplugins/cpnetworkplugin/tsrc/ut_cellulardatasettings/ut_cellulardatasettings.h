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

#ifndef UT_CELLULARDATASETTINGS_H
#define UT_CELLULARDATASETTINGS_H

#include <QtTest/QtTest>
#include <mockservice.h>

class CellularDataSettings;

class UT_CellularDataSettings : public QObject, MockService
{
    Q_OBJECT

public:    
    UT_CellularDataSettings();
    ~UT_CellularDataSettings();
    
private slots:

    void init();
    
    void t_memleak();
    void t_dataUseHome();
    void t_setDataUseHome();
    void t_dataUseRoam();
    void t_setDataUseRoam();
    void cleanup();
    
private:
    CellularDataSettings *m_cellularDataSettings;

};

#endif // UT_CELLULARDATASETTINGS_H
