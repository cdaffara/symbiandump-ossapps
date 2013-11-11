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

#ifndef MOSTVISITEDSNIPPET_H
#define MOSTVISITEDSNIPPET_H

#include "ChromeSnippet.h"
#include "ChromeWidget.h"

namespace GVA {

class MostVisitedSnippet : public ChromeSnippet
{
    Q_OBJECT
public:
    MostVisitedSnippet(const QString & elementId, ChromeWidget * chrome, QGraphicsWidget * widget, const QWebElement & element);
    static MostVisitedSnippet * instance(const QString& elementId, ChromeWidget * chrome, const QWebElement & element);
    void toggleVisibility(bool animate);
    void hide(bool animate);
    void updateMVGeometry();
    void setChromeWidget(QGraphicsWidget * widget);
    virtual ~MostVisitedSnippet();
    void close();
Q_SIGNALS:
    void mostVisitedSnippetCloseComplete();
private slots :
    void displayModeChanged(int newMode);
private:
    ChromeWidget *m_chrome;
};
}

#endif // MOSTVISITEDSNIPPET_H
