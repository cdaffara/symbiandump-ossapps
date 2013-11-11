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
* Description:  Calendar Launcher
*
*/
#include <QObject>
#include <qdatetime.h>

#ifdef CALENLAUNCHER_LIB
#   define CALENLAUNCHER_EXPORT Q_DECL_EXPORT
#else
#   define CALENLAUNCHER_EXPORT Q_DECL_IMPORT
#endif
class CALENLAUNCHER_EXPORT CalenLauncher : public QObject
{
Q_OBJECT

public:
	enum CalenView{
			MonthView = 0,
			AgendaView,
			DayView
		};
    CalenLauncher(QObject* parent = 0);
    ~CalenLauncher();
    
    void launchCalendarApp(CalenView view, QDateTime& dateTime, 
                           bool synchronousLaunch = true);
    
signals:
	void calendarLaunchFailed(int error);
	
public slots:
	void handleError(int error);
};

// End of file	--Don't remove this.
