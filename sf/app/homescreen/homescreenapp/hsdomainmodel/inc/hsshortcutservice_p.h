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
* Description:  Private implementation for HsShortcutService 
*
*/

#ifndef HSSHORTCUTSERVICE_P_H
#define HSSHORTCUTSERVICE_P_H

#include <QObject>

class QStateMachine;

class HsShortcutServicePrivate : public QObject
{
	Q_OBJECT

public:
	HsShortcutServicePrivate(QStateMachine *stateMachine, QObject *parent = 0);
	~HsShortcutServicePrivate();

	void executeCollectionAction(int shortcutId, const QString& collectionType);
    bool isItemShortcutWidget(int itemId);

public:
	QStateMachine *mStateMachine;
};

#endif // HSSHORTCUTSERVICE_P_H
