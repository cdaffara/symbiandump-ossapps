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

#include "t_hsdomainmodel.h"
#include "hscontentservice.h"

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
void TestHsDomainModel::testCreateNDeleteWidget()
{   
    createSceneWithPages();

    HsContentService contentService;

    QVariantHash params;

    params["uri"]= "invalidmockwidgetplugin";
    QVERIFY(!contentService.createWidget(params));

    //this has to be uri1 because of mocked factory
    params["uri"]= "mockwidgetplugin";
    QVERIFY(contentService.createWidget(params));
  
    QVERIFY(!HsContentService::instance()->addWidget(QString(), QVariantHash()));

    destroySceneAndPages();
}

void TestHsDomainModel::testContentServiceStaticInterface()
{
    QVERIFY(HsContentService::instance());
}


void TestHsDomainModel::testContentServiceWidgetCount()
{
    createSceneWithPages();
    HsContentService *contentService = HsContentService::instance();
    QVERIFY(contentService);
    int count = 0;
    QVERIFY(contentService->widgets("testing", QVariantHash(), count));
    
    
}
