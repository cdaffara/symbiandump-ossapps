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
#ifndef __GINEBRA_SETTINGSTOOLBARSNIPPET_H
#define __GINEBRA_SETTINGSTOOLBARSNIPPET_H

#include <QtGui>
#include "DualButtonToolbarSnippet.h"

namespace GVA {

    /* \brief This class provides a snippet for the toolbar in SettingsView
     *
     * This class provides a snippet for the toolbar in SettingsView. It contains two
     * buttons - back and give feedback, placed at either ends of the toolbar. The actions are created here.
     * It inherits \sa DualButtonToolbar class
     */
    class SettingsToolbarSnippet : public DualButtonToolbarSnippet
    {
        Q_OBJECT
    public:
        SettingsToolbarSnippet(const QString& elementId, ChromeWidget * chrome, const QWebElement & element);
        virtual ~SettingsToolbarSnippet();
        static SettingsToolbarSnippet * instance(const QString& elementId, ChromeWidget * chrome, const QWebElement & element);

    private Q_SLOTS:
        // Back action trigger handler. Switches back to webview
        void handleBackButton();
        void handleFeedbackButton();
        void onAspectChanged( ); 

    private:
        /// Reimplemented
        void setAction(ChromeSnippet * s);
        void addChild(ChromeSnippet * child) ;

        // OWNED actions because this view is based on SuperPage
        QAction * m_action1;
        QAction * m_action2;
    };

} // end of namespace GVA

#endif // __GINEBRA_SETTINGSTOOLBARSNIPPET_H
