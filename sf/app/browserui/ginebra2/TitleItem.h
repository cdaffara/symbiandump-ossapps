/*
 * Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
 * All rights reserved.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, version 2.1 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program.  If not,
 * see "http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html/".
 *
 * Description:
 *
 */

#ifndef __TITLE_ITEM_H__
#define __TITLE_ITEM_H__


#include <QGraphicsTextItem>
#include <QtGui>

namespace GVA {


 
class GTitleItem : public QGraphicsTextItem
{
    Q_OBJECT

    public:
        GTitleItem(QGraphicsItem * parent);
        virtual ~GTitleItem();

        void setMaxTextLength(int length);
        qreal textWidth();

      protected:
        virtual void mousePressEvent(QGraphicsSceneMouseEvent * event);

      public Q_SLOTS:
        void setText(const QString & text);

      Q_SIGNALS:
        void tapped(QPointF& pos);

      private:
        QTextLine m_textLine;
        int m_maxTextLength;
};


} // namespace GVA

#endif // __TITLE_ITEM_H__
