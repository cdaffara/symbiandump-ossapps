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
* Description:   TestVideoSettingsPlugin class definition
* 
*/

#ifndef TESTVIDEOSETTINGSPLUGIN_H
#define TESTVIDEOSETTINGSPLUGIN_H


// INCLUDES
#include <qobject.h>

class CpItemDataHelper;
class VideoSettingsPlugin;

class TestVideoSettingsPlugin : public QObject
	{
    
    Q_OBJECT

private slots:
    
    /**
     * called at the very beginning of the test
     */
    void initTestCase();
    
    /**
     * called at the very end of the test
     */
    void cleanupTestCase();
    
    /**
     * called at the start of every test.
     */
    void init();
    
    /**
     * called at the end of every test.
     */
    void cleanup();
    
    /**
     * test createSettingFormItemData
     */
    void testCreateSettingFormItemData();

private:
    
    CpItemDataHelper* mItemHelper;
    
    VideoSettingsPlugin* mPlugin;
    
    };

#endif  // TESTVIDEOSETTINGSPLUGIN_H

// End of File
