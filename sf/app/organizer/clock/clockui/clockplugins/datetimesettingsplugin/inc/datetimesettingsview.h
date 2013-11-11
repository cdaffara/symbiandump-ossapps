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
* Header file for DateTimeSettingsView class.
*
*/

#ifndef	DATETIMESETTINGSVIEW_H_
#define	DATETIMESETTINGSVIEW_H_

// User includes
#include <cpsettingformentryitemdata.h>

// Forward declarations
class QTimer;
class TimezoneClient;
class HbTranslator;

class DateTimeSettingsView : public CpSettingFormEntryItemData
{
	Q_OBJECT
public:
	explicit DateTimeSettingsView(
			CpItemDataHelper &itemDataHelper, const QString &text = QString(),
			const QString &description = QString(),
			const HbIcon &icon = HbIcon(),
			const HbDataFormModelItem *parent = 0,
			HbTranslator *translator = 0);
	virtual ~DateTimeSettingsView();

private slots:
	void onLaunchView();
	void updateDisplayTime();
	
private:
	virtual CpBaseSettingView *createSettingView() const;

private:
	QTimer *mTickTimer;
	TimezoneClient *mTimezoneClient;
	HbTranslator *mTranslator;

};

#endif // DATETIMESETTINGSVIEW_H_

// End of file	--Don't remove this
