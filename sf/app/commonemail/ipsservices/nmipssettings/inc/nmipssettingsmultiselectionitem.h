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

#ifndef NMIPSSETTINGSMULTISELECTIONITEM_H
#define NMIPSSETTINGSMULTISELECTIONITEM_H

#include <HbWidget>

class HbPushButton;
class HbAction;
class HbSelectionDialog;

class NmIpsSettingsMultiSelectionItem : public HbWidget
{
    Q_OBJECT

    Q_PROPERTY(QString text READ text WRITE setText)
    Q_PROPERTY(QStringList items READ stringItems WRITE setStringItems)
    Q_PROPERTY(QList<QVariant> selectedItems READ selectedItems WRITE setSelectedItems)
    Q_PROPERTY(QString heading READ heading WRITE setHeading)

public:

    NmIpsSettingsMultiSelectionItem(QGraphicsItem *parent = 0, Qt::WindowFlags wFlags = 0);
    virtual ~NmIpsSettingsMultiSelectionItem();

    QString text() const;
    void setText(QString text);

    QStringList stringItems() const;
    void setStringItems(QStringList items);

    QList<QVariant> selectedItems()const;
    void setSelectedItems(QList<QVariant> selectedItems);

    QString heading() const;
    void setHeading(QString heading);

signals:

    void editingFinished();
    void propertyChanged(QMap<QString, QVariant> properties);

private slots:

    void launchSelectionDialog();
    void selectionDialogClosed(HbAction *action);

private:

    void generateButtonText();

private: // data

    // Not owned.
    HbPushButton *mButton;
    // Owned.
    HbSelectionDialog *mSelectionDialog;
    QStringList mItems;
    QList<QVariant> mSelectedItems;
    QString mHeading;
};

#endif // NMIPSSETTINGSMULTISELECTIONITEM_H
