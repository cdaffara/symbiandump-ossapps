/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0""
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

#ifndef CPSETTINGFORMENTRYITEMDATA_H
#define CPSETTINGFORMENTRYITEMDATA_H

#include <QObject>
#include <hbdataformmodelitem.h>

class CpItemDataHelper;
class CpBaseSettingView;

class CpSettingFormEntryItemData : public QObject, public HbDataFormModelItem 
{
    Q_OBJECT
public:
    enum EntryItemType {
        ListEntryItem   = HbDataFormModelItem::CustomItemBase + 1,
        ButtonEntryItem = HbDataFormModelItem::CustomItemBase + 2
    };

    explicit CpSettingFormEntryItemData(
            EntryItemType type,
            CpItemDataHelper &itemDataHelper,
            const QString &text = QString(),
            const QString &description = QString(),
            const QString &iconName = QString(),
            const HbDataFormModelItem *parent = 0);  
    
    virtual ~CpSettingFormEntryItemData();  
    
    QString text() const;
    void setText(const QString &text);
    
    QString description() const;
    void setDescription(const QString &description);
    
    CpBaseSettingView* callCreateSettingView() const;

private:
    virtual CpBaseSettingView *createSettingView() const = 0;

public:
    
    QString mText;
    
    QString mDescription;
    
};

#endif  //CPSETTINGFORMENTRYITEMDATA_H
