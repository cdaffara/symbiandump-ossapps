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
* Description: This is the header file for ClockAppController class.
*
*/

#ifndef CLOCKAPPCONTROLLER_H
#define CLOCKAPPCONTROLLER_H

// System includes
#include <QObject>

// User inlcudes
#include "clockappcontrollerdefines.h"

// Forward declarations
class ClockViewManager;
class ClockAppControllerIfImpl;

class ClockAppController : public QObject
{
	Q_OBJECT
	
public:
	CLOCKAPPCONTROLLER_EXPORT explicit ClockAppController(QObject *parent = 0);
	CLOCKAPPCONTROLLER_EXPORT virtual ~ClockAppController();

signals:
	void appReady();

private slots:
	void handleAppReady();
	
private:
	ClockViewManager *mViewManager;
	ClockAppControllerIfImpl *mIfImpl;

	friend class ClockAppControllerIfImpl;
};

#endif // CLOCKAPPCONTROLLER_H

// End of file	--Don't remove this.
