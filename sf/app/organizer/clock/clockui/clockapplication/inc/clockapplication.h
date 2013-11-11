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
* Description: Header file for class ClockApplication.
*
*/
#ifndef CLOCKAPPLICATION_H
#define CLOCKAPPLICATION_H

// System includes
#include <QPointer>
#include <HbApplication>

// Forward Declaratios
class ClockAppController;

class ClockApplication : public HbApplication
{
	Q_OBJECT

public:
	ClockApplication(int &argc, char *argv[]);
	virtual ~ClockApplication();

signals:
	void applicationReady();

public:
	void createController();
	bool execution();
	
private slots:
	void handleAppReady();
	
private:
	QPointer<ClockAppController> mController;
};

#endif // CLOCKAPPLICATION_H

// End of file	--Don't remove this.
