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
* Description:  
*
*/

#ifndef CELLULARDATASETTINGS_H
#define CELLULARDATASETTINGS_H

// System includes

#include <QObject>

// User includes

// Forward declarations

class CellularDataSettingsPrivate;

// External data types

// Constants

// Class declaration

class CellularDataSettings : public QObject
{
    Q_OBJECT

public:

    // Data types
    
    // Setting values for "Data usage in home network"
    enum DataUsageHome {
        DataUsageHomeAutomatic = 0,     // These values must match the indexes in dropdown menu
        DataUsageHomeConfirm,
        DataUsageHomeWlanOnly
    };
    
    // Setting values for "Data usage when roaming"
    enum DataUsageAbroad {
        DataUsageAbroadAutomatic = 0,   // These values must match the indexes in dropdown menu
        DataUsageAbroadConfirm,
        DataUsageAbroadWlanOnly
    };

    explicit CellularDataSettings();
    virtual ~CellularDataSettings();

    int dataUseHome();
    void setDataUseHome(int value);
    
    int dataUseRoam();
    void setDataUseRoam(int value);

signals:

public slots:

protected:

protected slots:

private:

    Q_DISABLE_COPY(CellularDataSettings)

private slots:

private: // data

    // Friend classes

    // Private implementation
    CellularDataSettingsPrivate *d_ptr;
};

#endif // CELLULARDATASETTINGS_H
