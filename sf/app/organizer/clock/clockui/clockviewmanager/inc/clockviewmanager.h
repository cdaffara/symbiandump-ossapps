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
* Header file for class ClockViewManager.
*
*/

#ifndef CLOCKVIEWMANAGER_H
#define CLOCKVIEWMANAGER_H

// System includes
#include <QObject>

// User includes
#include "clockviewmanagerdefines.h"
#include "clockdatatypes.h"

// Forward declarations
class ClockAppControllerIf;
class ClockMainView;
class ClockWorldView;

class ClockViewManager : public QObject
{
	Q_OBJECT

public:
	CLOCKVIEWMANAGER_EXPORT ClockViewManager(
			ClockAppControllerIf &controllerIf, QObject *parent = 0);
	CLOCKVIEWMANAGER_EXPORT ~ClockViewManager();

public:
	CLOCKVIEWMANAGER_EXPORT void showView(ClockViews view);

signals:
		void appReady();

private:
	void loadMainView();
	void loadWorldClockView();

private slots:
	void loadOtherViews();

private:
	ClockAppControllerIf &mAppControllerIf;
	ClockMainView *mMainView;
	ClockWorldView *mWorldClockView;

private:
	Q_DISABLE_COPY(ClockViewManager)
};

#endif // CLOCKVIEWMANAGER_H

// End of file	--Don't remove this.
