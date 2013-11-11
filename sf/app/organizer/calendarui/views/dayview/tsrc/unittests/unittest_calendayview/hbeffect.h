/****************************************************************************
 **
 ** Copyright (C) 2008-2010 Nokia Corporation and/or its subsidiary(-ies).
 ** All rights reserved.
 ** Contact: Nokia Corporation (developer.feedback@nokia.com)
 **
 ** This file is part of the HbCore module of the UI Extensions for Mobile.
 **
 ** GNU Lesser General Public License Usage
 ** This file may be used under the terms of the GNU Lesser General Public
 ** License version 2.1 as published by the Free Software Foundation and
 ** appearing in the file LICENSE.LGPL included in the packaging of this file.
 ** Please review the following information to ensure the GNU Lesser General
 ** Public License version 2.1 requirements will be met:
 ** http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
 **
 ** In addition, as a special exception, Nokia gives you certain additional
 ** rights.  These rights are described in the Nokia Qt LGPL Exception
 ** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
 **
 ** If you have questions regarding the use of this file, please contact
 ** Nokia at developer.feedback@nokia.com.
 **
 ****************************************************************************/

#ifndef HBEFFECT_H
#define HBEFFECT_H

#include <hbglobal.h>
#include <hbnamespace.h>
#include <QRectF>
#include <QStringList>

QT_BEGIN_NAMESPACE
class QGraphicsItem;
class QVariant;
QT_END_NAMESPACE

class HbEffect
	{

public:

	struct EffectStatus
		{
		QGraphicsItem *item;
		QString effectEvent;
		Hb::EffectEvent reason;
		QVariant userData;
		};

	static bool start(QGraphicsItem *item, const QString &itemType,
			const QString &effectEvent, QObject *receiver = 0,
			const char *member = 0, const QVariant &userData = QVariant(),
			const QRectF &extRect = QRectF())
		{
		Q_UNUSED(item);
		Q_UNUSED(itemType);
		Q_UNUSED(effectEvent);
		Q_UNUSED(receiver);
		Q_UNUSED(member);
		Q_UNUSED(userData);
		Q_UNUSED(extRect);
		return false;
		}

	static bool start(QGraphicsItem *item, const QString &effectEvent,
			QObject *receiver = 0, const char *member = 0,
			const QVariant &userData = QVariant(), const QRectF &extRect =
					QRectF())
		{
		Q_UNUSED(item);
		Q_UNUSED(effectEvent);
		Q_UNUSED(receiver);
		Q_UNUSED(member);
		Q_UNUSED(userData);
		Q_UNUSED(extRect);
		return false;
		}

	static bool start(const QList<QGraphicsItem *> &items,
			const QString &itemType, const QString &effectEvent,
			QObject *receiver = 0, const char *member = 0,
			const QVariant &userData = QVariant())
		{
		Q_UNUSED(items);
		Q_UNUSED(itemType);
		Q_UNUSED(effectEvent);
		Q_UNUSED(receiver);
		Q_UNUSED(member);
		Q_UNUSED(userData);
		return false;
		}

	static bool effectRunning(QGraphicsItem *item, const QString &effectEvent =
			QString())
		{
		Q_UNUSED(item);
		Q_UNUSED(effectEvent);
		return false;
		}

	static bool cancel(QGraphicsItem *item, const QString &effectEvent =
			QString(), bool clearEffect = false, bool sendCallback = true,
			bool itemIsValid = true)
		{
		Q_UNUSED(item);
		Q_UNUSED(effectEvent);
		Q_UNUSED(clearEffect);
		Q_UNUSED(sendCallback);
		Q_UNUSED(itemIsValid);
		return false;
		}

	static bool add(const QString &itemType, const QString &filePath,
			const QString &effectEvent = QString())
		{
		Q_UNUSED(itemType);
		Q_UNUSED(filePath);
		Q_UNUSED(effectEvent);
		return false;
		}
	static bool add(const QStringList &itemType, const QStringList &filePath,
			const QStringList &effectEvent = QStringList())
		{
		Q_UNUSED(itemType);
		Q_UNUSED(filePath);
		Q_UNUSED(effectEvent);
		return false;
		}

	static bool add(QGraphicsItem *item, const QString &filePath,
			const QString &effectEvent = QString())
		{
		Q_UNUSED(item);
		Q_UNUSED(filePath);
		Q_UNUSED(effectEvent);
		return false;
		}
	static bool add(QGraphicsItem *item, const QStringList &filePath,
			const QStringList &effectEvent = QStringList())
		{
		Q_UNUSED(item);
		Q_UNUSED(filePath);
		Q_UNUSED(effectEvent);
		return false;
		}

	static bool remove(const QString &itemType, const QString &filePath,
			const QString &effectEvent = QString())
		{
		Q_UNUSED(itemType);
		Q_UNUSED(filePath);
		Q_UNUSED(effectEvent);
		return false;
		}
	static bool remove(QGraphicsItem *item, const QString &filePath,
			const QString &effectEvent = QString())
		{
		Q_UNUSED(item);
		Q_UNUSED(filePath);
		Q_UNUSED(effectEvent);
		return false;
		}
	static bool remove(QGraphicsItem *item)
		{
		Q_UNUSED(item);
		return false;
		}

	static void enable(QGraphicsItem *item)
		{
		Q_UNUSED(item);
		}
	static void disable(QGraphicsItem *item)
		{
		Q_UNUSED(item);
		}
	};

Q_DECLARE_METATYPE(HbEffect::EffectStatus)

#endif // HBEFFECT_H
