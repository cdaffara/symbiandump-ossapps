/*!
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
* Description:
*
*/

#include <hbaction.h>

QString hbTrId(const char *id, int n) { return QString(id); };

HbAction::HbAction(QObject *parent)
     : QAction(parent), d_ptr(0)
{
}

HbAction::HbAction(const QString &text, QObject *parent)
     : QAction(text, parent), d_ptr(0)
{
}

HbAction::~HbAction()
{
}

int HbAction::qt_metacall(enum QMetaObject::Call, int, void * *) {};
void* HbAction::qt_metacast(char const *) {};
struct QMetaObject const * HbAction::metaObject(void) const {};

#include <hbicon.h>

class HbIconPrivate
{
 public:
     HbIconPrivate(){};
     ~HbIconPrivate(){};
     
     QAtomicInt ref;
};

HbIcon::HbIcon() {};
HbIcon::HbIcon(const QString& string) {};
HbIcon::~HbIcon() {};
   
#include <qgraphicswidget.h>

void QGraphicsWidget::addAction(QAction *action)
{
}

void QGraphicsWidget::removeAction(QAction *action)
{
}

#include "phoneaction.h"

PhoneAction::PhoneAction () : 
    m_text (QString ()), m_command (-1), m_icon (0), m_disabled(false),
    m_role(PhoneAction::None)
{
}

PhoneAction::~PhoneAction ()
{
}

