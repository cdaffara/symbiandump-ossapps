/*
 * Copyright (c) 2000 Nokia Corporation and/or its subsidiary(-ies). 
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
 * Description: Implementation of applicationmanagement components
 *
 */

#ifndef AMVIEW_H_
#define AMVIEW_H_

#include <hbview.h>
#include <QObject>
#include "ApplicationManagementUtility.h"

class AMView : public HbView
    {
    Q_OBJECT

public:
    AMView();
    ~AMView();
signals :
	/**
	 * applicationReady() - To emit the application ready signal for matti tool.
   */
   void applicationReady();
public:       
    //Handles the incoming events
    bool eventFilter(QObject *object, QEvent *event);

private:    
    TBool Initilized;
    };
#endif /* AMVIEW_H_ */
