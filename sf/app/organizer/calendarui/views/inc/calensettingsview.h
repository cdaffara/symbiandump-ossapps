/*
* Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Header file for the class CalenSettingsView
*
*/


#ifndef DTSETTINGVIEW_H
#define DTSETTINGVIEW_H

#include <QObject>
#include <hbview.h>

class QGraphicsItem;
class HbDataForm;
class HbAction;
class CalenSettings;
class MCalenServices;

#ifdef  CALENVIEWS_DLL
#define CALENSETTINGSVIEW_EXPORT Q_DECL_EXPORT
#else
#define CALENSETTINGSVIEW_EXPORT Q_DECL_IMPORT
#endif

class CALENSETTINGSVIEW_EXPORT CalenSettingsView : public HbView
{
	Q_OBJECT

public:
    CalenSettingsView(MCalenServices& services, QGraphicsItem *parent = 0);
	~CalenSettingsView();

public:
    void initializeForm();
    void refreshView();
    
private slots:
	void launchPreviousView();
	
private:
	MCalenServices  &mServices;
	HbDataForm *mCalenSettingsForm;
	CalenSettings *mCalenSettings;
	/**
   * @var mSoftKeyAction
   * @brief Holder for soft key action
   */
  HbAction *mSoftKeyAction;
};

#endif // DTSETTINGVIEW_H

// End of file
