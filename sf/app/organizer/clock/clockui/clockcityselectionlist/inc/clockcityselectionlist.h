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
* The header file for class ClockCitySelectionList.
*
*/

#ifndef CLOCKCITYSELECTIONLIST_H
#define CLOCKCITYSELECTIONLIST_H

// System includes
#include <QObject>

// User includes
#include "clockcityselectionlistdefines.h"
#include "clockdatatypes.h"

// Forward declarations
class HbTranslator;
class ClockCitySelectionListPrivate;
class TimezoneClient;

class CLOCKCITYSELECTIONLIST_EXPORT ClockCitySelectionList : public QObject
{
	Q_OBJECT

public:
	explicit ClockCitySelectionList(TimezoneClient *client, QObject *parent = 0);
	explicit ClockCitySelectionList(QObject *parent = 0);
	virtual ~ClockCitySelectionList(); 

signals:
	void citySelected(LocationInfo info);

public slots:
	void showCityList();
	void closeCityList();

private:
	ClockCitySelectionListPrivate *d_ptr;
	Q_DECLARE_PRIVATE_D(d_ptr, ClockCitySelectionList)
	Q_DISABLE_COPY(ClockCitySelectionList)
	HbTranslator *mTranslator;
};

#endif // CLOCKCITYSELECTIONLIST_H

// End of file	--Don't remove this.
