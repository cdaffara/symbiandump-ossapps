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
#include <QtGui>
#include <QtTest/QtTest>
#include <QDebug>
#include <hbapplication.h>
#include <hbglobal_p.h>
#include "hbphonemainwindow.h"


class U_PhoneMainWindow: public QObject
{
Q_OBJECT

public:
	U_PhoneMainWindow();
	~U_PhoneMainWindow();

private slots:
	void initTestCase();
    void cleanupTestCase();
	void testShow();
	
private:
	
    HbPhoneMainWindow* m_Window; // widget under test

};

U_PhoneMainWindow::U_PhoneMainWindow()
{
}

U_PhoneMainWindow::~U_PhoneMainWindow()
{
}

void U_PhoneMainWindow::initTestCase()
{

    m_Window = new HbPhoneMainWindow();
	
}

void U_PhoneMainWindow::cleanupTestCase()
{
	delete m_Window;
}

void U_PhoneMainWindow::testShow()
{
    m_Window->show();
    Q_ASSERT(m_Window->isVisible());
	
}

HB_TEST_MAIN(U_PhoneMainWindow)
#include "ut_phonemainwindow.moc"
