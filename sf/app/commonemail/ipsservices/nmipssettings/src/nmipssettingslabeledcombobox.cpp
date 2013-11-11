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
#include <HbComboBox>
#include <HbLabel>

#include "nmipssettingslabeledcombobox.h"

/*!
    \class NmIpsSettingsLabelItem
    \brief The class implements a custom HbDataFormViewItem that combines a HbComboBox and HbLabel
    widgets that are linked. I.e. When the user selects changes the combobox's value, the label
    text is updated accordingly.
*/

// ======== MEMBER FUNCTIONS ========

/*!
    Constructor of NmIpsSettingsLabeledComboBox.
*/
NmIpsSettingsLabeledComboBox::NmIpsSettingsLabeledComboBox(
    QGraphicsItem *parent, Qt::WindowFlags wFlags)
: HbWidget(parent, wFlags)
{
    // Create widget layout.
    QGraphicsLinearLayout* layout = new QGraphicsLinearLayout(Qt::Vertical);
    layout->setContentsMargins(0,0,0,0);
    this->setLayout(layout); // Takes ownership

    // Create combobox.
    mComboBox = new HbComboBox();
    layout->addItem(mComboBox); // Takes ownership

    // Create label.
    mLabel = new HbLabel();
    mLabel->setElideMode(Qt::ElideNone);
    mLabel->setTextWrapping(Hb::TextWordWrap);
    HbFontSpec spec(HbFontSpec::PrimarySmall);
    mLabel->setFontSpec(spec);
    layout->addItem(mLabel); // Takes ownership

    // Connect signals and slots.
    connect(mComboBox, SIGNAL(currentIndexChanged(int)),
        this, SLOT(comboBoxIndexChanged(int)), Qt::UniqueConnection);
}

/*!
    Destructor of NmIpsSettingsLabeledComboBox.
*/
NmIpsSettingsLabeledComboBox::~NmIpsSettingsLabeledComboBox()
{
}

/*!
    Returns the current index.
    \return Index.
*/
int NmIpsSettingsLabeledComboBox::currentIndex() const
{
    return mComboBox->currentIndex();
}

/*!
    Sets the current index.
    \param currentIndex Index to set.
*/
void NmIpsSettingsLabeledComboBox::setCurrentIndex(int index)
{
    mComboBox->setCurrentIndex(index);
}

/*!
    Returns the current combobox items.
    \return Combobox items.
*/
QStringList NmIpsSettingsLabeledComboBox::comboItems() const
{
    return mComboBox->items();
}

/*!
    Sets the current combobox items.
    \param comboItem Combobox items to set.
*/
void NmIpsSettingsLabeledComboBox::setComboItems(QStringList comboItems)
{
    mComboBox->setItems(comboItems);
}

/*!
    Returns the current label texts.
    \return Label texts.
*/
QStringList NmIpsSettingsLabeledComboBox::labelTexts() const
{
    return QStringList(mLabelTexts.values());
}

/*!
    Sets the current label texts.
    \param labelTexts Label texts to set.
*/
void NmIpsSettingsLabeledComboBox::setLabelTexts(QStringList labelTexts)
{
    // Set the label texts.
    mLabelTexts.clear();
    for (int i=0; i<labelTexts.count(); ++i) {
        mLabelTexts.insert(i, labelTexts.at(i));
    }
}

/*!
    Updates the label text based on the combobox's value.
    \param index Current combobox index.
*/
void NmIpsSettingsLabeledComboBox::comboBoxIndexChanged(int index)
{
    QString label(mLabelTexts.value(index));
    mLabel->setPlainText(label);
    QMap<QString, QVariant> properties;
    properties.insert("currentIndex", index);
    emit propertyChanged(properties);
    emit currentIndexChanged(index);
}
