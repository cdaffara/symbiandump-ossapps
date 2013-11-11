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
* Description:   ?Description
*
*/

#ifndef MSGAPPLICATION_H_
#define MSGAPPLICATION_H_

#include <hbApplication.h>

class MsgApplication: public HbApplication
{

Q_OBJECT
	
public:
	
	/**
	 * Constructor
	 */
	MsgApplication( int &argc, char *argv[] );
	
	/**
	 * Destructor
	 */
	virtual ~MsgApplication();
	
	/**
	 * initMattiAutomation
	 * connects viewReady signal to handleAppReady slot
	 */
	void initViewReady();
	
private slots:
	void handleAppReady();

signals:
	void applicationReady();
};
    
#endif // MSGAPPLICATION_H_
