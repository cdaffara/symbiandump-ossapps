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
* Description:  Entry point for shortcut widget 
*
*/

#ifndef HSSHORTCUTSERVICE_H
#define HSSHORTCUTSERVICE_H

#include <QObject>
#include <QIcon>
#include <QScopedPointer>
#include <QMetaType>

#include "hsdomainmodel_global.h"
#include "hstest_global.h"

HOMESCREEN_TEST_CLASS(TestHsDomainModel)

class QStateMachine;
class HsShortcutServicePrivate;

class HSDOMAINMODEL_EXPORT HsShortcutService : public QObject
{
	Q_OBJECT

public:
    static HsShortcutService *instance(QStateMachine *stateMachine = 0);
    ~HsShortcutService();
		
	void executeCollectionAction(int shortcutId, const QString& collectionType);
    bool isItemShortcutWidget(int itemId);

private:
    HsShortcutService(QStateMachine *stateMachine, QObject *parent = 0);
	Q_DISABLE_COPY(HsShortcutService)

private:
	QScopedPointer<HsShortcutServicePrivate> mD; 	
    static HsShortcutService *mInstance;    
    
    HOMESCREEN_TEST_FRIEND_CLASS(TestHsDomainModel)
};

Q_DECLARE_METATYPE(HsShortcutService*)

#endif // HSSHORTCUTSERVICE_H
