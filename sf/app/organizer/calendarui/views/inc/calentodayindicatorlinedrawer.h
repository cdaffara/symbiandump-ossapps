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
* Description:  CalenTodayIndicatorLineDrawer class definition.
*
*/

#ifndef CALENTODAYINDICATORLINEDRAWER_H
#define CALENTODAYINDICATORLINEDRAWER_H

// System includes
#include <hbwidget.h>

class CalenTodayIndicatorLineDrawer : public HbWidgetBase
{
	Q_OBJECT
	
public:
	CalenTodayIndicatorLineDrawer( QGraphicsItem* parent = NULL );  
	virtual ~CalenTodayIndicatorLineDrawer();
		
private:
	void paint(
			QPainter* painter, const QStyleOptionGraphicsItem* option,
			QWidget* widget);
	
private slots:
	void handleThemeChange();
		
};
#endif /* CALENTODAYINDICATORLINEDRAWER_H */

// End of file  --Don't remove this.
