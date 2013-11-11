/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0""
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:  
*       test the functions in cppluginconfigreader class 
*/

#ifndef UT_CPPLUGINCONFIGREADER_H
#define UT_CPPLUGINCONFIGREADER_H


#include "cppluginconfig.h"

#include <QObject>
#include <QList>

class TestCppluginConfigReader :public QObject
{
    Q_OBJECT
private slots:
    
    void initTestCase();
    void testConstructor();
    void testReadValidConfigFile(); // test with a valid config file.
    void testReadEmptyConfigFilePath(); // test testReadEmptyConfigFilePath() with empty path.
    void testReadEmptyConfigFile(); // test with empty config file.
    void testReaderWithWrongChildPluginTag(); // test with wrong child plugin tags.
    void testReaderWithEmptyChildElement(); // test with empty child plugin element.
    void testReaderWithWrongPluginTag(); // test with wrong plugin tag.
    void testReaderWithoutAttrs(); // test without any plugin attrs.
    void testReaderWithWrongDescTag(); // test with wrong desc tag.
    void cleanupTestCase();    
private:
    QList<CpPluginConfig> mCorrectConfig;
};
#endif /* UT_CPPLUGINCONFIGREADER_H */
