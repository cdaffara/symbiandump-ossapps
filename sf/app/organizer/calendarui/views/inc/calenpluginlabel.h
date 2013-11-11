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
* Description:  CalenPluginLabel class definition.
*
*/

#ifndef CALENPLUGINLABEL_H
#define CALENPLUGINLABEL_H

#ifdef  CALENVIEWS_DLL
#define CALENPLUGINLABEL_EXPORT Q_DECL_EXPORT
#else
#define CALENPLUGINLABEL_EXPORT Q_DECL_IMPORT
#endif

// System includes
#include <hblabel.h>

class MCalenServices;
class CalenPluginLabel : public HbLabel
{
	Q_OBJECT

public:
	CALENPLUGINLABEL_EXPORT CalenPluginLabel(MCalenServices &services, QGraphicsItem *parent = 0);
	CALENPLUGINLABEL_EXPORT ~CalenPluginLabel();

protected:
	void gestureEvent(QGestureEvent *event);

	
private:
	MCalenServices	&mServices;
};
	
#endif /* CALENPLUGINLABEL_H */

// End of file  --Don't remove this.
