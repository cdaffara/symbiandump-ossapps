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
#ifndef __UNITTEST_MEDIAMODEL_H__
#define __UNITTEST_MEDIAMODEL_H__

#include <QtTest/QtTest>

class GlxMediaModel;

class TestGlxMediaModel : public QObject
{
    Q_OBJECT
    
signals:

private slots:
    /**
     * called before anything else when test starts
     */
    void initTestCase();

  /**
     * will be called before each testfunction is executed.
     *
     */
    void init(); 
    
    void rowCountTestCase();
    void columnCountTestCase();
    void parentTestCase();
    
    void itemImageDataInvalidIndexTestCase();
    void itemImageDataGridImagetestCase();
    void itemImageDataHdmiBitmapTestCase(); 
    
    void ItemPropertyViewTitleTestCase();    
    void ItemPropertyImageTitleTestCase();
    void ItemPropertyDateTestCase();
    void ItemPropertyCorruptImageTestCase();
    void ItemPropertyTimeTestCase();
    void ItemPropertySizeTestCase();
    void ItemPropertyDescTestCase();
    
    void indexTestCase();
    void setDataContextRoleTestCase();
    void setDataRemoveContextRoleTestCase();
    void setDataFocusIndexRoleTestCase();
    void setDataVisualIndexTestCase();
    void setDataSelectedIndexTestCase();
    void setDataSubStateTestCase();
    void setDataTempVisualIndexTestCase();    
    void GetGridIconItemTestCase();
    void GetFsIconItemTestCase();    
    void getFullScreenIconTestCase();
    void getFullScreenImagetestCase();
    void setContextModeTestCase();
    void removeContextModeTestCase();
    void setFocusIndexTestCase();    
    void getFocusIndexTestcase();
    void setSelectedIndexTestCase();
    void getCorruptDefaultIconTestCase();
    void thumbnailPopulatedCheckTestCase();    
    void itemUpdatedTestCase();
    void itemsAddedTestCase();
    void itemsRemovedTestCase();
    void itemCorruptedTestCase();
    
    void albumTitleUpdatedTestCase();
    void modelpopulatedTestCase();
    void updateDetailItemsTestCase();
    void updateItemIconTestCase();
    
    /**
     * will be called after every testfunction.
     *
     */
    void cleanup();
    
    /**
     * will be called after testing ends
     *
     */
    void cleanupTestCase();

private:
  
    /**
     * object under test
     */  
    GlxMediaModel *mModel;
};

#endif //__UNITTEST_MEDIAMODEL_H__
