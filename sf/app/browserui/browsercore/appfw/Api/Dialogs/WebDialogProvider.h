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

#ifndef WEBDIALOGPROVIDER_H
#define WEBDIALOGPROVIDER_H

#include <QKeyEvent>
#include <QLineEdit>
#include <QSpinBox>
#include <QString>
#include <QValidator>

#include "BaseDialog.h"
#include "BWFGlobal.h"

// forward decarations
class QAbstractButton;
class QDialogButtonBox;
class QLabel;

// This internal class adds extra validation to a QSpinBox by emitting textChanged(bool)
// after events that may potentially change the visible text. Return or Enter key presses
// are not propagated if the visible text is invalid. Instead, the visible text is modified
// to the last valid value.
class WebDialogProviderValidatedSpinBox : public QSpinBox
{
    Q_OBJECT
	
public:
    WebDialogProviderValidatedSpinBox(int minValue, int maxValue, int step, int value) :
        QSpinBox(0)
		{
			setRange(minValue, maxValue);
			setSingleStep(step);
			setValue(value);
			selectAll();
			validator = new QIntValidator(minValue, maxValue, this);
			connect(lineEdit(), SIGNAL(textChanged(const QString &)),
					this, SLOT(notifyTextChanged()));
			connect(this, SIGNAL(editingFinished()),
					this, SLOT(notifyTextChanged()));
		}

private:
    QIntValidator *validator;
    void keyPressEvent(QKeyEvent *event)
		{
			if ((event->key() == Qt::Key_Return || event->key() == Qt::Key_Enter) &&
                !textValid())
			{
#ifndef QT_NO_PROPERTIES
				setProperty("value", property("value"));
#endif
			}
			else
			{
				QSpinBox::keyPressEvent(event);
			}
			notifyTextChanged();
		}

    void mousePressEvent(QMouseEvent *event)
		{
			QSpinBox::mousePressEvent(event);
			notifyTextChanged();
		}

    bool textValid() const
		{
			QString t = text();
			int pos = 0;
			return validate(t, pos) == QValidator::Acceptable;
		}

private slots:
    void notifyTextChanged()
		{
			emit textChanged(textValid());
		}

signals:
    void textChanged(bool);
};

// This internal class adds extra validation to a QDoubleSpinBox by emitting textChanged(bool)
// after events that may potentially change the visible text. Return or Enter key presses
// are not propagated if the visible text is invalid. Instead, the visible text is modified
// to the last valid value.
class WebDialogProviderValidatedDoubleSpinBox : public QDoubleSpinBox
{
    Q_OBJECT

public:
    WebDialogProviderValidatedDoubleSpinBox(
        double minValue, double maxValue, int decimals, double value) :
        QDoubleSpinBox(0)
		{
			setDecimals(decimals);
			setRange(minValue, maxValue);
			setValue(value);
			selectAll();
			validator = new QDoubleValidator(minValue, maxValue, decimals, this);
			connect(lineEdit(), SIGNAL(textChanged(const QString &)),
					this, SLOT(notifyTextChanged()));
			connect(this, SIGNAL(editingFinished()),
					this, SLOT(notifyTextChanged()));
		}
		
private:
    QDoubleValidator *validator;
    void keyPressEvent(QKeyEvent *event)
		{
			if ((event->key() == Qt::Key_Return || event->key()	== Qt::Key_Enter) &&
                !textValid())
			{
#ifndef QT_NO_PROPERTIES
				setProperty("value", property("value"));
#endif
			}
			else
			{
				QDoubleSpinBox::keyPressEvent(event);
			}
			notifyTextChanged();
		}

    void mousePressEvent(QMouseEvent *event)
		{
			QDoubleSpinBox::mousePressEvent(event);
			notifyTextChanged();
		}

    bool textValid() const
		{
			QString t = text();
			int pos = 0;
			return validate(t, pos) == QValidator::Acceptable;
		}

private slots:
    void notifyTextChanged()
		{
			emit textChanged(textValid());
		}

signals:
    void textChanged(bool);
};

class BWF_EXPORT WebDialogProvider : public BaseDialog
{
    Q_OBJECT

private:
    // ### Qt 5: remove
    enum Type
    {
        LineEdit, SpinBox, DoubleSpinBox, ComboBox, EditableComboBox
    };

    // ### Qt 5: remove
    WebDialogProvider(const QString &label, QWidget* parent,
                      Type type, Qt::WindowFlags flags);
    WebDialogProvider(const QString &title, const QString &label,
                      QWidget *parent, QWidget *input, Qt::WindowFlags flags);
    WebDialogProvider(const QString &label, QWidget* parent,
                      const QStringList &buttons, int *buttonIndex);
    WebDialogProvider(const QString &label, QWidget* parent);
    WebDialogProvider(QWidget* parent, QWidget* inputWidget);

    ~WebDialogProvider();

public slots:
    void clicked(QAbstractButton *button);

public:
    static QString getText(
        QWidget *parent,
        const QString &title,
        const QString &label,
        QLineEdit::EchoMode echo = QLineEdit::Normal, 
        const QString &text = QString(),
        bool *ok = 0, 
        Qt::WindowFlags flags = 0);

    static int getInteger(
        QWidget *parent, 
        const QString &title,
        const QString &label, 
        int value = 0,
        int minValue = 0x80000000, // -2147483648
        int maxValue = 0x7FFFFFFF,
        int step = 1, 
        bool *ok = 0, 
        Qt::WindowFlags flags = 0);

    static double getDouble(
        QWidget *parent, 
        const QString &title,
        const QString &label, 
        double value = 0, 
        double minValue = -2147483647, 
        double maxValue = 2147483647,
        int decimals = 1, 
        bool *ok = 0, 
        Qt::WindowFlags flags = 0);

    static QString getItem(
        QWidget *parent, 
        const QString &title,
        const QString &label, 
        const QStringList &list, 
        int current = 0,
        bool editable = true, 
        bool *ok = 0, 
        Qt::WindowFlags flags = 0);

    static int getButtonResponse(
        QWidget *parent, 
        const QString &label,
        const QStringList &buttons);

    static bool getUsernamePassword(
        QWidget *parent, 
        QString &username,
        QString &password);

    static void showInfoDialog(QWidget *parent, const QString &label);
    static void showTimedDialog(QWidget *parent, const QString &label);
    static void showTimedMessage(QWidget *parent, const QString &label,	int msec);

private:
    QLabel *label;
    QPushButton *okButton;
    QWidget *input;
    int *m_buttonIndex;
    QDialogButtonBox *m_buttonBox;

    void init(const QString &label, WebDialogProvider::Type);
    void init(const QString &title, const QString &label, QWidget *input);
    void init(const QString &lbl, const QStringList &buttons);
    void init(const QString &lbl);
    void init(QWidget*);

    void tryAccept();

private:
    Q_DISABLE_COPY(WebDialogProvider)
};

#endif // WEBDIALOGPROVIDER_H
