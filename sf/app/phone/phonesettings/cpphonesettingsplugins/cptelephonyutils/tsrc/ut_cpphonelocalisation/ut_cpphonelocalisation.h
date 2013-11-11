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

#ifndef UT_CPPHONELOCALISATION_H
#define UT_CPPHONELOCALISATION_H

#include <QtTest/QtTest>
#include <mockservice.h>

class PhoneLocalisation; 

class UT_cpphonelocalisation : public QObject, MockService
{
    Q_OBJECT

public:    
    UT_cpphonelocalisation();
    ~UT_cpphonelocalisation();
    
private slots:
    void init();
    void cleanup();
    void t_memleak();
    void t_installTranslator();
    void t_removeTranslators(); 
 
private: 
    QScopedPointer<PhoneLocalisation> m_phoneLocalisation; 
    
};

#endif
