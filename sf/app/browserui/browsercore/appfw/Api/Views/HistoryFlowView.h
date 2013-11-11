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

#ifndef __HISTORYFLOWVIEW_H__
#define __HISTORYFLOWVIEW_H__

#include <QObject>
#include <QAction>
#include "HistoryView.h"
#include "controllableview.h"

class QWebHistory;
class WebPageController;

namespace WRT {

    class BWF_EXPORT HistoryFlowView : public HistoryView
    {
        Q_OBJECT
    public:
        HistoryFlowView(WebPageController * mgr, QWidget* parent,const QString& aType);
        HistoryFlowView(WebPageController * mgr, QGraphicsWidget* parent,const QString& aType);
        virtual QString type() const { return m_type; }
        
        static const QString Type() { return "HistoryView"; }
        static ControllableView *createNew(QWidget *parent);
        static ControllableViewBase  *createNew(QGraphicsWidget* parent);

        QImage getCurrentSlide();
    public slots:
        void activate();
        void setCenterIndex(int i);
    protected:
        QString m_type;
    };

}
#endif // __HISTORYFLOWVIEW_H__
