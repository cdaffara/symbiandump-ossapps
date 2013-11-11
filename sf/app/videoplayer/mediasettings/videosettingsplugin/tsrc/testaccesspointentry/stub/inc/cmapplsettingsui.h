/*
* Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
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
* Description: CmApplSettingsUi stub  
*
*/

#ifndef CMAPPLSETTINGSUI_H
#define CMAPPLSETTINGSUI_H

#include <QObject>
#include <QFlags>
#include <QSet>

class CmApplSettingsUi : public QObject
{
    Q_OBJECT
    
public:

    enum SelectionType
        {
        //! The user selected a destination
        SelectionTypeDestination,
        //! The user selected a connection method
        SelectionTypeConnectionMethod
        };

    enum SelectionDialogItems
        {
        //! Support destination selection
        ShowDestinations = 0x01,
        //! Support connection method selection
        ShowConnectionMethods = 0x02                              
        };
    
    enum ApplSettingsError {
        //! No error. Dialog succeeded.
        ApplSettingsErrorNone = 0,
        //! Selection was canceled.
        ApplSettingsErrorCancel,
        //! No items to show (e.g. too strict filters).
        ApplSettingsErrorNoContent,
        //! Settings reading failed.
        ApplSettingsErrorSettingsRead
    };

    struct SettingSelection
        {
        SelectionType result;
        uint id;
        };
    
    typedef uint BearerTypeFilter;
    
    /*!
        Constructor.
        
        @param parent Parent object.
     */
    explicit CmApplSettingsUi(QObject *parent = 0);
    
    /*!
        Destructor.
     */
    virtual ~CmApplSettingsUi();

    void setOptions(
        const QFlags<SelectionDialogItems> &listItems,
        const QSet<BearerTypeFilter> &bearerFilter);

    void setSelection(const SettingSelection &selection);

    SettingSelection selection() const;

    void open();
    
    void emitFinished(uint status);

signals:

    void finished(uint status);

public: // data

    SettingSelection mSelection;
    
    QFlags<SelectionDialogItems> mListItems;
    
    QSet<BearerTypeFilter> mBearerTypeFilter;
    
    int mOpenCallAmount;
};

#endif // CMAPPLSETTINGSUI_H
