/*
* Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:
*
*/

#include <QGraphicsLinearLayout>
#include <HbPushButton>
#include <HbDialog>
#include <HbLabel>
#include <HbDateTimePicker>
#include <HbAction>
#include <HbExtendedLocale>
#include <HbStringUtil>
#include "nmipssettingstimeeditor.h"

/*!
    \class NmIpsSettingsTimeEditor
    \brief The class implements a custom HbDataFormViewItem for selecting time with a tumbler.

*/

// ======== MEMBER FUNCTIONS ========

/*!
    Constructor of NmIpsSettingsTimeEditor.
*/
NmIpsSettingsTimeEditor::NmIpsSettingsTimeEditor(QGraphicsItem *parent, Qt::WindowFlags wFlags)
    : HbWidget(parent, wFlags),
      mButton(0),
      mTimePickerDialog(0),
      mPrimaryAction(0)
{
    // Create widget layout.
    QGraphicsLinearLayout* layout = new QGraphicsLinearLayout(Qt::Vertical);
    layout->setContentsMargins(0,0,0,0);
    this->setLayout(layout); // Takes ownership

    // Create label.
    mButton = new HbPushButton();
    layout->addItem(mButton); // Takes ownership

    connect( mButton, SIGNAL(clicked()), this, SLOT(launchTimePicker()));
}

/*!
    Destructor of NmIpsSettingsTimeEditor.
*/
NmIpsSettingsTimeEditor::~NmIpsSettingsTimeEditor()
{
    delete mTimePickerDialog;
}

/*!
    Returns the currently set time.
    \return The time.
 */
QTime NmIpsSettingsTimeEditor::time() const
{
    return mTime;
}

/*!
    Sets the current time.
    \param time The time to set.
 */
void NmIpsSettingsTimeEditor::setTime(QTime time)
{
    mTime = time;
    HbExtendedLocale locale(HbExtendedLocale::system());
    mButton->setText(HbStringUtil::convertDigits(locale.format( mTime, r_qtn_time_usual )));
}

/*!
    Returns the selection dialog heading label.
    \return The heading label.
 */
QString NmIpsSettingsTimeEditor::heading() const
    {
    return mLabel;
    }

/*!
    Sets the time selection dialog heading label.
    \param label The label to set.
 */
void NmIpsSettingsTimeEditor::setHeading(QString label)
{
    mLabel = label;
}


/*!
    Launches the time picker dialog.
 */
void NmIpsSettingsTimeEditor::launchTimePicker()
{
    if (mTimePickerDialog) {
        delete mTimePickerDialog;
        mTimePickerDialog = 0;
    }

    // Create the dialog.
    mTimePickerDialog = new HbDialog();
    mTimePickerDialog->setTimeout(HbDialog::NoTimeout);
    mTimePickerDialog->setDismissPolicy(HbDialog::NoDismiss);

    // Set the heading for the dialog.
    HbLabel *timeLabel = new HbLabel(mLabel, mTimePickerDialog);
    mTimePickerDialog->setHeadingWidget(timeLabel);

    // Create the tumbler.
    HbDateTimePicker *timePicker = new HbDateTimePicker(mTimePickerDialog);

    // Set the tumbler as the content widget.
    mTimePickerDialog->setContentWidget(timePicker);

    // Set tumbler data.
    timePicker->setDisplayFormat(timeFormat());
    timePicker->setTime(mTime);

    // Set dialog actions.
    HbAction *okAction = new HbAction(QString(hbTrId("txt_common_button_ok")), mTimePickerDialog);
    mTimePickerDialog->addAction(okAction);
    mPrimaryAction = okAction;
    
    HbAction *cancelAction = new HbAction(QString(hbTrId("txt_common_button_cancel")), 
                                          mTimePickerDialog);
    mTimePickerDialog->addAction(cancelAction);

    // Show the dialog.
    mTimePickerDialog->open(this, SLOT(handleTimeAction(HbAction *)));
}

/*!
    Handles the action from the time picker dialog.
 */
void NmIpsSettingsTimeEditor::handleTimeAction(HbAction *action)
{
    if (action==mPrimaryAction) {
        // Get the time from the picker.
        QTime newTime = static_cast<HbDateTimePicker *> (mTimePickerDialog->contentWidget())->time();

        // Store the new time.
        setTime(newTime);

        // Emit the signal.
        emit timeChanged(newTime);
    }
}

/*!
    Returns locale specific formatting string for hours and minutes.
    \return Time format string.
*/
QString NmIpsSettingsTimeEditor::timeFormat()
{
    // Set the correct time format string based on locale.
    HbExtendedLocale locale = HbExtendedLocale::system();
    QString timeFormat("h:mm ap");
    if (HbExtendedLocale::Time24 == locale.timeStyle()) {
        timeFormat = QString("h:mm");
    }
    // Set the separator between hours and minutes.
    timeFormat.replace(QString(":"), locale.timeSeparator(1));

    return timeFormat;
}
