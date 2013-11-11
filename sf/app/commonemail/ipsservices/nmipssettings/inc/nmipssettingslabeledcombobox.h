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

#ifndef NMIPSSETTINGSLABELEDCOMBOBOX_H
#define NMIPSSETTINGSLABELEDCOMBOBOX_H

#include <QStringList>
#include <QHash>
#include <HbWidget>

class QGraphicsLinearLayout;
class QGraphicsItem;
class HbComboBox;
class HbLabel;

class NmIpsSettingsLabeledComboBox : public HbWidget
{
    Q_OBJECT

    Q_PROPERTY(int currentIndex READ currentIndex WRITE setCurrentIndex)
    Q_PROPERTY(QStringList comboItems READ comboItems WRITE setComboItems)
    Q_PROPERTY(QStringList labelTexts READ labelTexts WRITE setLabelTexts)

public:

    NmIpsSettingsLabeledComboBox(QGraphicsItem *parent = 0, Qt::WindowFlags wFlags = 0);
    ~NmIpsSettingsLabeledComboBox();

    int currentIndex() const;
    void setCurrentIndex(int index);

    QStringList comboItems() const;
    void setComboItems(QStringList comboItems);

    QStringList labelTexts() const;
    void setLabelTexts(QStringList labelTexts);

signals:

    void currentIndexChanged(int index);
    void propertyChanged(QMap<QString, QVariant> properties);

private slots:

    void comboBoxIndexChanged(int index);

private: // data

    // Now owned.
    HbLabel *mLabel;
    // Not owned.
    HbComboBox *mComboBox;

    QHash<int, QString> mLabelTexts;
};

#endif // NMIPSSETTINGSLABELEDCOMBOBOX_H
