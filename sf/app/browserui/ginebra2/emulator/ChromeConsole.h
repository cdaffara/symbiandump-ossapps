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

#ifndef CHROMECONSOLE_H
#define CHROMECONSOLE_H

#include <QWidget>
#include <QtGui>
#include "../ChromeWidget.h"


/**
 * \brief Javascript console window for development and testing
 *
 * The ChromeConsole class displays a simple dialog that allows the user to enter javascript
 * statements into a text field and then execute them in the chrome's javascript engine.
 * Not used in S60 builds.
 */
class ChromeConsole : public QDialog
{
    Q_OBJECT
public:
    ChromeConsole(GVA::ChromeWidget *chromeWidget);

    QVariant evaluateExpression(const QString &expression);

public slots:
    void evaluate();
    void reloadChrome();
    void dump();
    void exit() { QApplication::exit(0); }
    virtual void accept();

protected:
    virtual void keyPressEvent(QKeyEvent *event);

protected:
    QTextEdit m_textEdit;
    GVA::ChromeWidget *m_chromeWidget;
    class Ui_ConsoleDialog *m_ui;
    QStringList m_expressionHistory;
    int m_historyIndex;
};


#endif // CHROMECONSOLE_H
