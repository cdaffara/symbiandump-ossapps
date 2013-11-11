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

#ifndef __gva_actionbutton_h__
#define __gva_actionbutton_h__

#include "NativeChromeItem.h"
#include "ChromeSnippet.h"
#include <QIcon>
#include <QImage>

namespace GVA {

class ActionButton : public NativeChromeItem
    {
        Q_OBJECT

    public:
        ActionButton( ChromeSnippet * snippet, QGraphicsItem * parent = 0 );
        virtual ~ActionButton() {};
        void paint( QPainter * painter, const QStyleOptionGraphicsItem * opt, QWidget * widget );
        void addIcon( const QString & icon, QIcon::Mode mode = QIcon::Normal );
        QIcon icon() {return m_icon;}
        void setAction ( QAction * action, bool triggerOnDown = false,  bool triggerOnUp = true);
        void setEnabled(bool);
        void setActive(bool);
        void setActiveOnPress(bool);
        QAction * defaultAction();
        bool isChecked() {return m_internalAction->isChecked();}

    public Q_SLOTS:
        void onActionChanged();
        void onHidden();
        void onShown();

   Q_SIGNALS:
        void activated();

    protected:
        void mousePressEvent( QGraphicsSceneMouseEvent * ev );
        void mouseReleaseEvent( QGraphicsSceneMouseEvent * ev );
    private:
        QAction * m_internalAction;
        QIcon m_icon;
        bool m_triggerOnUp;
        bool m_triggerOnDown;
        QIcon::Mode m_state;
        bool m_active;
        bool m_activeOnPress;
    };
} // end of namespace GVA

#endif // ACTIONBUTTON_H
