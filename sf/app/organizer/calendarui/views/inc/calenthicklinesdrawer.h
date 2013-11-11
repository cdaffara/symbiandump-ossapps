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
* Description:  CalenThickLinesDrawer class definition.
*
*/

#ifndef CALENTHICKLINESDRAWER_H
#define CALENTHICKLINESDRAWER_H
// System includes
#include <hbwidget.h>

// User includes
#include "calencommon.h"

#ifdef  CALENVIEWS_DLL
#define CALENTHICKLINESDRAWER_EXPORT Q_DECL_EXPORT
#else
#define CALENTHICKLINESDRAWER_EXPORT Q_DECL_IMPORT
#endif

class CALENTHICKLINESDRAWER_EXPORT CalenThickLinesDrawer : public HbWidget
{
	Q_OBJECT
	
public:
	CalenThickLinesDrawer( CalendarNamespace::WidgetType type, 
						   QGraphicsItem* parent = NULL );  
	virtual ~CalenThickLinesDrawer();

private slots:
	void handleThemeChange();
	
private:
	void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, 
	           QWidget* widget);

protected:
	void polish(HbStyleParameters &params);
	
private:
	QColor mGridBorderColor;
	CalendarNamespace::WidgetType typeOfWidget;
};
#endif /* CALENTHICKLINESDRAWER_H */

// End of file  --Don't remove this.
