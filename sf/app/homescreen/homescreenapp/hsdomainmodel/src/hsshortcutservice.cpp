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

#include <QStateMachine>    
#include "hsshortcutservice.h"
#include "hsshortcutservice_p.h"
#include "hsdatabase.h"
#include "hsdomainmodeldatastructures.h"
#include "hsmenueventfactory.h"

namespace
{
    const char gShortcutId[]        = "caEntryId";
    const char gShortcutWidgetUri[] = "hsshortcutwidgetplugin";
}


/*!
    \class HsShortcutService
    \ingroup group_hsdomainmodel
    \brief 
*/

/*!

*/
HsShortcutServicePrivate::HsShortcutServicePrivate(QStateMachine *stateMachine, QObject *parent)
    : QObject(parent),
	  mStateMachine(stateMachine)
{
}

/*!

*/
HsShortcutServicePrivate::~HsShortcutServicePrivate()
{

}

/*!

*/
void HsShortcutServicePrivate::executeCollectionAction(
        int shortcutId, const QString& collectionType)
{
	QEvent *menuEvent = HsMenuEventFactory::
        createOpenCollectionEvent(shortcutId, collectionType);
	mStateMachine->postEvent(menuEvent);
}

/*!

*/
bool HsShortcutServicePrivate::isItemShortcutWidget(int itemId)
{
    HsDatabase *db = HsDatabase::instance();
    Q_ASSERT(db);

	QList<HsWidgetData> widgetDatas;
	if (db->widgets(gShortcutWidgetUri, widgetDatas)) {
        for (int i = 0; i < widgetDatas.count(); ++i) {
            QVariant id;
            if (db->widgetPreference(widgetDatas.at(i).id, gShortcutId, id) && 
                id.toInt() == itemId) {
                return true;
            }
        }
    }
	return false;
}

/*!

*/
HsShortcutService *HsShortcutService::instance(QStateMachine *stateMachine)
{
    if (!mInstance && stateMachine) {
        mInstance = new HsShortcutService(stateMachine);
    }
    return mInstance;
}

/*!

*/
HsShortcutService::~HsShortcutService()
{
    
}

/*!

*/
void HsShortcutService::executeCollectionAction(
        int shortcutId, const QString& collectionType)
{
    mD->executeCollectionAction(shortcutId, collectionType);
}

/*!

*/
bool HsShortcutService::isItemShortcutWidget(int itemId)
{
    return mD->isItemShortcutWidget(itemId);
}

/*!

*/
HsShortcutService::HsShortcutService(QStateMachine *stateMachine, QObject *parent)
    : QObject(parent)
{
    mD.reset(new HsShortcutServicePrivate(stateMachine));
}

HsShortcutService *HsShortcutService::mInstance(0);
