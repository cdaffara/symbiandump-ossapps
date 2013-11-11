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
* Description: 
*
*/
#include <xqserviceprovider.h>
#include <xqserviceipcmarshal.h>

class CCalenController;

class CalenServiceProvider : public XQServiceProvider
{

Q_OBJECT

public:
	enum CalenView{
			MonthView = 0,
			AgendaView
		};
	//Q_DECLARE_USER_METATYPE_ENUM(enum);
    CalenServiceProvider(CCalenController *controller, QObject* parent = 0);
    ~CalenServiceProvider();

public slots:
    void launchCalendarApp(const QDateTime& date, int viewId);
	
private:
    CCalenController *mController;
};

// End of file	--Don't remove this.
