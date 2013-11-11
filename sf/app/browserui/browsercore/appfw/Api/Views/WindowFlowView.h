/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef __WINDOWFLOWVIEW_H__
#define __WINDOWFLOWVIEW_H__

#include <QObject>
#include <QAction>
#include "WindowView.h"
#include "controllableview.h"

class QWebWindow;
class WebPageController;

namespace WRT {


    class BWF_EXPORT WindowFlowView : public WindowView
    {
        Q_OBJECT
    public:
        WindowFlowView(WebPageController * mgr, QWidget* parent,const QString& aType);
        WindowFlowView(WebPageController * mgr, QGraphicsWidget* parent,const QString& aType);
        virtual QString type() const { return m_type; }

        static const QString Type() { return "WindowView" ;}
        static ControllableView  *createNew(QWidget* parent);
        static ControllableViewBase  *createNew(QGraphicsWidget* parent);

    public slots:
        void activate();
    protected:
        QString m_type;
    };

}
#endif // __WINDOWFLOWVIEW_H__
