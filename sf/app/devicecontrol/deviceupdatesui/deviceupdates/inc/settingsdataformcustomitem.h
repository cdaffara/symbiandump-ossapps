/*
 * Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
 * Description:  This class provides custom list item for the
 * DataForm
 *
 */

#ifndef SETTINGS_DATAFORM_CUSTOMITEM_H
#define SETTINGS_DATAFORM_CUSTOMITEM_H

#include <hbdataformviewitem.h>

class HbLineEdit;
class SettingsDataFormCustomItem : public HbDataFormViewItem
{
Q_OBJECT

public:
    explicit SettingsDataFormCustomItem(QGraphicsItem *parent);
    ~SettingsDataFormCustomItem();

    virtual HbAbstractViewItem* createItem();
     bool canSetModelIndex( const QModelIndex& aIndex ) const; 

protected:
    virtual HbWidget* createCustomWidget();
    
public slots:
virtual void restore();

private:
HbLineEdit* mLineEdit;
};

#endif // SETTINGS_DATAFORM_CUSTOMITEM_H
