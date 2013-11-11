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
* Header file for DateTimeSettingsPlugin class.
*
*/


#ifndef DATETIMESETTINGSPLUGIN_H_
#define DATETIMESETTINGSPLUGIN_H_

// System includes
#include <QObject>

// User includes.
#include <cpplugininterface.h>

// Forward class declartion.
class HbTranslator;
class DateTimeSettingsPlugin : public QObject, public CpPluginInterface
{
	Q_OBJECT
	Q_INTERFACES(CpPluginInterface)

public:
	DateTimeSettingsPlugin();
	virtual ~DateTimeSettingsPlugin();

	virtual int uid() const;
	QList<CpSettingFormItemData*> createSettingFormItemData(CpItemDataHelper &itemDataHelper) const;
	
private:
	HbTranslator *mTranslator;
};

#endif /* DATETIMESETTINGSPLUGIN_H_ */

// End of file	--Don't remove this
