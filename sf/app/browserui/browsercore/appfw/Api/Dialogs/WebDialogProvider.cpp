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

#include <QComboBox>
#include <QDialogButtonBox>
#include <QGridLayout>
#include <QGroupBox>
#include <QLabel>
#include <QPushButton>
#include <QTimer>
#include <QVBoxLayout>

#include "WebDialogProvider.h"

WebDialogProvider::WebDialogProvider(const QString &text, QWidget* parent,
                               Type type, Qt::WindowFlags flags) :
    BaseDialog(parent, flags)
{
    init(text, type);
}

/*!
  Constructs the dialog. The \a title is the text which is displayed in
  the title bar of the dialog. The \a label is the text which is shown to
  the user (it should tell the user what they are expected to enter).
  The \a parent is the dialog's parent widget. The \a input parameter
  is the dialog to be used. The \a f parameter is passed on to the
  QDialog constructor.

  \sa getText(), getInteger(), getDouble(), getItem()
*/
WebDialogProvider::WebDialogProvider(const QString &title,
                               const QString &label, QWidget *parent, QWidget *input,
                               Qt::WindowFlags flags) :
    BaseDialog(parent, flags)
{
    init(title, label, input);
}

WebDialogProvider::WebDialogProvider(const QString &label, QWidget* parent,
                               const QStringList &buttons, int *buttonIndex) :
    BaseDialog(parent), m_buttonIndex(buttonIndex)
{
    init(label, buttons);
}

WebDialogProvider::WebDialogProvider(const QString &label, QWidget* parent) :
    BaseDialog(parent)
{
    init(label);
}

WebDialogProvider::WebDialogProvider(QWidget* parent, QWidget* widget) :
    BaseDialog(parent)
{
    init(widget);
}

void WebDialogProvider::init(const QString &lbl, WebDialogProvider::Type type)
{
    QVBoxLayout *vbox = new QVBoxLayout(this);

    label = new QLabel(lbl, this);
    vbox->addWidget(label);
    vbox->addStretch(1);

    switch (type)
    {
    case WebDialogProvider::LineEdit:
        input = new QLineEdit(this);
        break;
    case WebDialogProvider::SpinBox:
        input = new QSpinBox(this);
        break;
    case WebDialogProvider::DoubleSpinBox:
        input = new QDoubleSpinBox(this);
        break;
    case WebDialogProvider::ComboBox:
    case WebDialogProvider::EditableComboBox:
    {
        QComboBox *combo = new QComboBox(this);
        if (type == WebDialogProvider::EditableComboBox)
        {
            combo->setEditable(true);
        }
        input = combo;
    }
    break;
    }
    vbox->addWidget(input);
    vbox->addStretch(1);

#ifndef QT_NO_SHORTCUT
    label->setBuddy(input);
#endif

    QDialogButtonBox *buttonBox =
        new QDialogButtonBox(QDialogButtonBox::Cancel, Qt::Horizontal, this);
    buttonBox->setObjectName(QLatin1String("qt_inputdlg_buttonbox"));
    QPushButton *okButton =
        static_cast<QPushButton *> (buttonBox->addButton(QDialogButtonBox::Ok));
    okButton->setDefault(true);
    vbox->addWidget(buttonBox);

    connect(buttonBox, SIGNAL(accepted()), this, SLOT(accept()));
    connect(buttonBox, SIGNAL(rejected()), this, SLOT(reject()));
}

void WebDialogProvider::init(const QString &title, const QString &lbl,
                          QWidget *input)
{
    setWindowTitle(title);
    QVBoxLayout *vbox = new QVBoxLayout(this);

    QLabel *label = new QLabel(lbl, this);
    vbox->addWidget(label);
    vbox->addStretch(1);

    input->setParent(this);
    vbox->addWidget(input);
    vbox->addStretch(1);

#ifndef QT_NO_SHORTCUT
    label->setBuddy(input);
#endif

    QDialogButtonBox *buttonBox =
        new QDialogButtonBox(QDialogButtonBox::Cancel, Qt::Horizontal, this);
    buttonBox->setObjectName(QLatin1String("qt_inputdlg_buttonbox"));
    okButton = static_cast<QPushButton *> (buttonBox->addButton(QDialogButtonBox::Ok));
    okButton->setDefault(true);
    vbox->addWidget(buttonBox);

    connect(buttonBox, SIGNAL(accepted()), this, SLOT(accept()));
    connect(buttonBox, SIGNAL(rejected()), this, SLOT(reject()));

    disableSizeGrip();
}

void WebDialogProvider::init(QWidget* input)
{
    QVBoxLayout *vbox = new QVBoxLayout(this);

    vbox->addWidget(input);
    input->setParent(this);

    QDialogButtonBox *buttonBox =
        new QDialogButtonBox(QDialogButtonBox::Cancel, Qt::Horizontal, this);
    okButton = static_cast<QPushButton *> (buttonBox->addButton(QDialogButtonBox::Ok));
    okButton->setDefault(true);
    vbox->addWidget(buttonBox);
    QObject::connect(buttonBox, SIGNAL(accepted()), this, SLOT(accept()));
    QObject::connect(buttonBox, SIGNAL(rejected()), this, SLOT(reject()));
    vbox->setSizeConstraint(QLayout::SetFixedSize);
    disableSizeGrip();
}

void WebDialogProvider::init(const QString &lbl, const QStringList &buttons)
{
    QVBoxLayout *vbox = new QVBoxLayout(this);

    label = new QLabel(lbl, this);
    vbox->addWidget(label);
    vbox->addStretch(1);

    m_buttonBox = new QDialogButtonBox(QDialogButtonBox::NoButton,
                                       Qt::Horizontal, this);
    m_buttonBox->setObjectName(QLatin1String("multibuttondlg_buttonbox"));

    for (int i = 0; i < buttons.count(); i++)
    {
        QPushButton *b = new QPushButton(buttons[i], this);
        m_buttonBox->addButton(b, QDialogButtonBox::ActionRole);
    }
    vbox->addWidget(m_buttonBox);
    vbox->setSizeConstraint(QLayout::SetFixedSize);
    QObject::connect(m_buttonBox, SIGNAL(clicked(QAbstractButton *)), this,
                     SLOT(clicked(QAbstractButton *)));
}

void WebDialogProvider::init(const QString &lbl)
{
    QVBoxLayout *vbox = new QVBoxLayout(this);

    label = new QLabel(lbl, this);
    vbox->addWidget(label);
    vbox->addStretch(1);

    vbox->setSizeConstraint(QLayout::SetFixedSize);
}

/*!
  Destroys the input dialog.
*/
WebDialogProvider::~WebDialogProvider()
{
}

/*!
  Static convenience function to get a string from the user. \a
  title is the text which is displayed in the title bar of the
  dialog. \a label is the text which is shown to the user (it should
  say what should be entered). \a text is the default text which is
  placed in the line edit. The \a mode is the echo mode the line
  edit will use. If \a ok is non-null \e *\a ok will be set to true
  if the user pressed \gui OK and to false if the user pressed
  \gui Cancel. The dialog's parent is \a parent. The dialog will be
  modal and uses the widget flags \a f.

  This function returns the text which has been entered in the line
  edit. It will not return an empty string.

  Use this static function like this:

  \snippet examples/dialogs/standarddialogs/dialog.cpp 3

  \sa getInteger(), getDouble(), getItem()
*/

QString WebDialogProvider::getText(QWidget *parent,
                                const QString &title, const QString &label,
                                QLineEdit::EchoMode mode, const QString &text, bool *ok,
                                Qt::WindowFlags flags)
{
    QLineEdit *le = new QLineEdit;
    le->setText(text);
    le->setEchoMode(mode);
    le->setFocus();
    le->selectAll();

    WebDialogProvider dlg(title, label, parent, le, flags);

    QString result;
    bool accepted = (dlg.exec() == QDialog::Accepted);
    if (ok)
    {
        *ok = accepted;
    }
    if (accepted)
    {
        result = le->text();
    }

    return result;
}

/*!
  Static convenience function to get an integer input from the
  user. \a title is the text which is displayed in the title bar
  of the dialog.  \a label is the text which is shown to the user
  (it should say what should be entered). \a value is the default
  integer which the spinbox will be set to.  \a minValue and \a
  maxValue are the minimum and maximum values the user may choose,
  and \a step is the amount by which the values change as the user
  presses the arrow buttons to increment or decrement the value.

  If \a ok is non-null *\a ok will be set to true if the user
  pressed \gui OK and to false if the user pressed \gui Cancel. The
  dialog's parent is \a parent. The dialog will be modal and uses
  the widget flags \a f.

  This function returns the integer which has been entered by the user.

  Use this static function like this:

  \snippet examples/dialogs/standarddialogs/dialog.cpp 0

  \sa getText(), getDouble(), getItem()
*/

int WebDialogProvider::getInteger(QWidget *parent,
                               const QString &title, const QString &label, int value,
                               int minValue, int maxValue, int step, bool *ok, Qt::WindowFlags flags)
{
    WebDialogProviderValidatedSpinBox *sb =
        new WebDialogProviderValidatedSpinBox(minValue, maxValue, step, value);
    WebDialogProvider dlg(title, label, parent, sb, flags);
    connect(sb, SIGNAL(textChanged(bool)), dlg.okButton, SLOT(setEnabled(bool)));
    bool accepted = (dlg.exec() == QDialog::Accepted);
    if (ok)
    {
        *ok = accepted;
    }
    return sb->value();
}

/*!
  Static convenience function to get a floating point number from
  the user. \a title is the text which is displayed in the title
  bar of the dialog. \a label is the text which is shown to the user
  (it should say what should be entered). \a value is the default
  floating point number that the line edit will be set to. \a
  minValue and \a maxValue are the minimum and maximum values the
  user may choose, and \a decimals is the maximum number of decimal
  places the number may have.

  If \a ok is non-null, *\a ok will be set to true if the user
  pressed \gui OK and to false if the user pressed \gui Cancel. The
  dialog's parent is \a parent. The dialog will be modal and uses
  the widget flags \a f.

  This function returns the floating point number which has been
  entered by the user.

  Use this static function like this:

  \snippet examples/dialogs/standarddialogs/dialog.cpp 1

  \sa getText(), getInteger(), getItem()
*/

double WebDialogProvider::getDouble(QWidget *parent,
                                 const QString &title, const QString &label, double value,
                                 double minValue, double maxValue, int decimals, bool *ok,
                                 Qt::WindowFlags flags)
{
    WebDialogProviderValidatedDoubleSpinBox *sb =
        new WebDialogProviderValidatedDoubleSpinBox(minValue, maxValue,
                                                 decimals, value);
    WebDialogProvider dlg(title, label, parent, sb, flags);
    connect(sb, SIGNAL(textChanged(bool)), dlg.okButton, SLOT(setEnabled(bool)));
    bool accepted = (dlg.exec() == QDialog::Accepted);
    if (ok)
    {
        *ok = accepted;
    }
    return sb->value();
}

/*!
  Static convenience function to let the user select an item from a
  string list. \a title is the text which is displayed in the title
  bar of the dialog. \a label is the text which is shown to the user (it
  should say what should be entered). \a list is the
  string list which is inserted into the combobox, and \a current is the number
  of the item which should be the current item. If \a editable is true
  the user can enter their own text; if \a editable is false the user
  may only select one of the existing items.

  If \a ok is non-null \e *\a ok will be set to true if the user
  pressed \gui OK and to false if the user pressed \gui Cancel. The
  dialog's parent is \a parent. The dialog will be modal and uses
  the widget flags \a f.

  This function returns the text of the current item, or if \a
  editable is true, the current text of the combobox.

  Use this static function like this:

  \snippet examples/dialogs/standarddialogs/dialog.cpp 2

  \sa getText(), getInteger(), getDouble()
*/

QString WebDialogProvider::getItem(QWidget *parent,
                                const QString &title, const QString &label,
                                const QStringList &list, int current, bool editable, bool *ok,
                                Qt::WindowFlags flags)
{
    QComboBox *combo = new QComboBox;
    combo->addItems(list);
    combo->setCurrentIndex(current);
    combo->setEditable(editable);
    WebDialogProvider dlg(title, label, parent, combo, flags);
    bool accepted = (dlg.exec() == QDialog::Accepted);
    if (ok)
    {
        *ok = accepted;
    }
    return combo->currentText();
}

/*
 *   displays a dialog with multiple buttons according to the string list
 *   and returns the index of the button clicked (starting at 0).
 */
int WebDialogProvider::getButtonResponse(QWidget *parent,
                                      const QString &label, const QStringList &buttons)
{
    int buttonIndex(-1);
    WebDialogProvider dlg(label, parent, buttons, &buttonIndex);
    dlg.exec();
    return buttonIndex;
}

/*! 
  Static convenience function to let the user can enter username and password 
  \a username which holds the input text which is entered by user.
  \a password which holds the input text which is entered by user.
  \a returns true if user accept otherwise false.
  \sa getText(), getInteger(), getDouble()
*/
bool WebDialogProvider::getUsernamePassword(QWidget *parent,
                                         QString &username, QString &password)
{
    QGroupBox* groupBox = new QGroupBox;
    QLineEdit* usernameWidget = new QLineEdit(groupBox);
    usernameWidget->setInputMethodHints(Qt::ImhNoAutoUppercase);
    QLineEdit* passwordWidget = new QLineEdit(groupBox);
    passwordWidget->setEchoMode(QLineEdit::PasswordEchoOnEdit);
    passwordWidget->setInputMethodHints(Qt::ImhNoAutoUppercase);
    QGridLayout* gLayout = new QGridLayout(groupBox);
    groupBox->setLayout(gLayout);
    gLayout->addWidget(new QLabel(tr("User Name : "), groupBox), 0, 0);
    gLayout->addWidget(usernameWidget, 0, 1);
    gLayout->addWidget(new QLabel(tr("Password : "), groupBox), 1, 0);
    gLayout->addWidget(passwordWidget, 1, 1);

    WebDialogProvider dlg(parent, groupBox);

    bool accepted = (dlg.exec() == QDialog::Accepted);
    if (accepted)
    {
        username = usernameWidget->text();
        password = passwordWidget->text();
        return true;
    }
    return false;
}

/*
 *   displays a dialog with text and a close button
 */
void WebDialogProvider::showInfoDialog(QWidget *parent, const QString &label)
{
    int buttonIndex(-1);
    QStringList buttons(tr("Close"));
    WebDialogProvider dlg(label, parent, buttons, &buttonIndex);
    dlg.exec();
}

/*
 *   displays a dialog with text. closes after 2 second interval
 */
void WebDialogProvider::showTimedDialog(QWidget *parent, const QString &label)
{
    int buttonIndex(-1);
    QStringList buttons(tr("Ok"));
    WebDialogProvider dlg(label, parent, buttons, &buttonIndex);
    QTimer::singleShot(2000, &dlg, SLOT(accept()));
    dlg.exec();
}

void WebDialogProvider::showTimedMessage(QWidget *parent, const QString &label, int msec)
{
// The way to make showTimedMessage a modeless dialog 
    if ( getCount() <= 0 ) {
        WebDialogProvider dlg(label, parent);
        QTimer::singleShot(msec, &dlg, SLOT(accept()));
        dlg.exec();
    } else {
       return; 
}	

}

void WebDialogProvider::clicked(QAbstractButton *button)
{
    for (int i = 0; i < m_buttonBox->buttons().count(); i++)
    {
        if (m_buttonBox->buttons()[i] == button)
        {
            *m_buttonIndex = i;
            break;
        }
    }
    QDialog::accept();
}

/*!
  \fn QString WebDialogProvider::getText(const QString &title, const QString &label,
  QLineEdit::EchoMode echo = QLineEdit::Normal,
  const QString &text = QString(), bool *ok = 0,
  QWidget *parent = 0, const char *name = 0, Qt::WindowFlags f = 0)

  Call getText(\a parent, \a title, \a label, \a echo, \a text, \a
  ok, \a f) instead.

  The \a name parameter is ignored.
*/

/*!
  \fn int WebDialogProvider::getInteger(const QString &title, const QString &label, int value = 0,
  int minValue = -2147483647, int maxValue = 2147483647,
  int step = 1, bool *ok = 0,
  QWidget *parent = 0, const char *name = 0, Qt::WindowFlags f = 0)


  Call getInteger(\a parent, \a title, \a label, \a value, \a
  minValue, \a maxValue, \a step, \a ok, \a f) instead.

  The \a name parameter is ignored.
*/

/*!
  \fn double WebDialogProvider::getDouble(const QString &title, const QString &label, double value = 0,
  double minValue = -2147483647, double maxValue = 2147483647,
  int decimals = 1, bool *ok = 0,
  QWidget *parent = 0, const char *name = 0, Qt::WindowFlags f = 0)

  Call getDouble(\a parent, \a title, \a label, \a value, \a
  minValue, \a maxValue, \a decimals, \a ok, \a f).

  The \a name parameter is ignored.
*/

/*!
  \fn QString WebDialogProvider::getItem(const QString &title, const QString &label, const QStringList &list,
  int current = 0, bool editable = true, bool *ok = 0,
  QWidget *parent = 0, const char *name = 0, Qt::WindowFlags f = 0)

  Call getItem(\a parent, \a title, \a label, \a list, \a current,
  \a editable, \a ok, \a f) instead.

  The \a name parameter is ignored.
*/
