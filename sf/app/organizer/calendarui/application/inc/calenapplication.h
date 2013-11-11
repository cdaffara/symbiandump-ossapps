/*
* Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
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
* Description: Header file for class CalenApplication.
*
*/

// system includes
#include <QPointer>
#include <HbApplication>

// Forward Declarations.
class CCalenController;
class CalenServiceProvider;

class CalenApplication : public HbApplication
{

Q_OBJECT

public:
	CalenApplication(int &argc, char *argv[]);
	virtual ~CalenApplication();

signals:
	void applicationReady();
	
public:
	void createController();
	bool execution();
	void releaseController();
	
private slots:
	void handleAppReady();
	
private:
	QPointer<CCalenController> mController;
	QPointer<CalenServiceProvider> mService;
};

// End of file	--Don't remove this.
