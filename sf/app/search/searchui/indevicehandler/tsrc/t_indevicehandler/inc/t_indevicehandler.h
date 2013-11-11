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
 * Description:  Tests for SearchStateProvider class.
 *
 */

#include <QtTest/QtTest>
#include "indevicehandler.h"

/**
 * @test Test class for stateproviders modules.
 */
class SearchInDeviceHandlerTest : public QObject
    {
Q_OBJECT

public:
    SearchInDeviceHandlerTest();
    ~SearchInDeviceHandlerTest();

private slots:
    void init();
    void initTestCase();
    void cleanupTestCase();
    void testCreateAndDestructIndeviceHandler();
    void testhandleAsyncSearchResult();
    void testhandleDocument();
    void testgetSearchResult();
    void testgetDocumentAsync();
    void testgetDocumentAsyncAtIndex();
    void testsearchAsync();
    void testcancelLastSearch();
    void testgetSearchResultCount();
    void testsetCategory();
    void testisPrepared();
    };

