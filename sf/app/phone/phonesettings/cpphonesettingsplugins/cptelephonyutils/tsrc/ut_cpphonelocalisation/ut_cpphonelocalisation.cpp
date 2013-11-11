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

#include "ut_cpphonelocalisation.h"
#include "qtestmains60.h"
#include <smcmockclassincludes.h>
#define private public
#include "cpphonelocalisation.h"

void QCoreApplication::installTranslator(QTranslator * messageFile)
{
    SMC_MOCK_METHOD1( void, QTranslator *, messageFile)
}

/*!
  UT_cpphonelocalisation::UT_cpphonelocalisation
 */
UT_cpphonelocalisation::UT_cpphonelocalisation() 
{
    
}

/*!
  UT_cpphonelocalisation::~UT_cpphonelocalisation
 */
UT_cpphonelocalisation::~UT_cpphonelocalisation()
{
    
}

/*!
  UT_cpphonelocalisation::init
 */
void UT_cpphonelocalisation::init()
{
    initialize();
    
    m_phoneLocalisation.reset(new PhoneLocalisation); 
}

/*!
  UT_cpphonelocalisation::cleanup
 */
void UT_cpphonelocalisation::cleanup()
{
    reset();
    delete m_phoneLocalisation.take(); 
}

/*!
  UT_cpphonelocalisation::t_memleak
 */
void UT_cpphonelocalisation::t_memleak()
{
    
}

/*!
  UT_cpphonelocalisation::t_installTranslator
 */
void UT_cpphonelocalisation::t_installTranslator()
{
    
    // Test: successfull load  
    expect("QTranslator::load").
        returns(true);    
    expect("QCoreApplication::installTranslator").times(1);
    
    m_phoneLocalisation->installTranslator(
            PhoneLocalisation::TranslationFileCommon);
    QVERIFY(m_phoneLocalisation->m_translators.count() == 1);
    QVERIFY(verify());
    
    // Test: failing load  
    expect("QTranslator::load").
        returns(false);
    expect("QCoreApplication::installTranslator").times(0);
    m_phoneLocalisation->installTranslator(
            PhoneLocalisation::TranslationFileCommon);
    QVERIFY(m_phoneLocalisation->m_translators.count() == 1);
    
    // Test: Unknown enum value
    QVERIFY(verify());
    expect("QCoreApplication::installTranslator").times(0);
    m_phoneLocalisation->installTranslator(
            (PhoneLocalisation::TranslationFileId)2);
    QVERIFY(m_phoneLocalisation->m_translators.count() == 1);
    QVERIFY(verify());
    
    // Test: telephone_cp translator load 
    expect("QTranslator::load").
        returns(false);
    expect("QCoreApplication::installTranslator").times(0);
    m_phoneLocalisation->installTranslator(
            PhoneLocalisation::TranslationFileTelephoneCp);
    QVERIFY(verify());
 
}

/*!
  UT_cpphonelocalisation::t_removeTranslators
 */
void UT_cpphonelocalisation::t_removeTranslators()
{
    expect("QTranslator::load").
        returns(true);
    
    m_phoneLocalisation->installTranslator(
            PhoneLocalisation::TranslationFileCommon); 
    m_phoneLocalisation->installTranslator(
            PhoneLocalisation::TranslationFileTelephoneCp);
    
    m_phoneLocalisation->removeTranslators(); 
    QVERIFY(m_phoneLocalisation->m_translators.count() == 0);
    QVERIFY(verify());
}




QTEST_MAIN_S60(UT_cpphonelocalisation)
