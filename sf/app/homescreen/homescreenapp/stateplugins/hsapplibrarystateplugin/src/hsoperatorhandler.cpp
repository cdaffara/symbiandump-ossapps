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
 * Description: hsoperatorhandler.cpp
 *
 */

#include "hsoperatorhandler.h"

#include <HbAction>

#ifdef QT_EXTENSIONS
    #include "hsoperatorhandler_p.h"
#else
    #include <HbIcon>
#endif




/*!
    \class HsOperatorHandler
    \ingroup group_hsmenustateplugin

    \brief

    \sa
*/

/*!
 Constructor
 \param parent parent object
 */
HsOperatorHandler::HsOperatorHandler(QObject *parent):
    QObject(parent),
#ifdef QT_EXTENSIONS
    m_q(new HsOperatorHandlerPrivate(this))
#else
    m_q(0)
#endif
{
}

/*!
 Destructor
 */
HsOperatorHandler::~HsOperatorHandler()
{
}

/*!
 Returns Operators icon
 \retval icon
 */
HbIcon HsOperatorHandler::icon()
{
#ifdef QT_EXTENSIONS
    return m_q->icon();
#else
    return HbIcon();
#endif
}

/*!
 Returns Operators text
 \retval icon
 */
QString HsOperatorHandler::text()
{
#ifdef QT_EXTENSIONS
    return m_q->text();
#else
    return QString();
#endif
}


/*!
 Open Operator application or related url action
 */
void HsOperatorHandler::action()
{
#ifdef QT_EXTENSIONS
    m_q->action();
#endif
}

/*!
 Returns Operator Store presence value predefined in Central Repositoryicon
 \retval true if Operator Store should be present in toolbar
 */
bool HsOperatorHandler::operatorStorePresent()
{
#ifdef QT_EXTENSIONS
    return m_q->operatorStorePresent();
#else
    return false;
#endif
}

/*!
 Returns Ovi Store presence value predefined in Central Repositoryicon
 \retval true if Ovi Store should be present in toolbar
 */
bool HsOperatorHandler::oviStorePresent()
{
#ifdef QT_EXTENSIONS
    return m_q->oviStorePresent();
#else
    return false;
#endif
}

/*!
 Returns if Operator Store should first in toolbar
 \retval true if Operator Store should first in toolbar
 */
bool HsOperatorHandler::operatorStoreFirst()
{
#ifdef QT_EXTENSIONS
    return m_q->operatorStoreFirst();
#else
    return false;
#endif
}

/*!
 Prepares HbAction for Operator Store
 \param HbAction to prepare
 \retval HbAction prepared action
 */
HbAction *HsOperatorHandler::prepareOperatorStoreAction(
        HbAction *const operatorAction)
{
    operatorAction->setIcon(icon());
    connect(operatorAction, SIGNAL(triggered()), this, SLOT(action()));
    return operatorAction;
}

