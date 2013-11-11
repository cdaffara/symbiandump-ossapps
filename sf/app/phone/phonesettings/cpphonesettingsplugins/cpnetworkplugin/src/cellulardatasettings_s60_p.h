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

#ifndef CELLULARDATASETTINGS_S60_P_H
#define CELLULARDATASETTINGS_S60_P_H

// System includes

// User includes

// Forward declarations

class CellularDataSettings;
class TCmGenConnSettings;

// External data types

// Constants

// Class declaration

class CellularDataSettingsPrivate
{

public:

    // Data types
    
    explicit CellularDataSettingsPrivate(CellularDataSettings *q_pointer);
    virtual ~CellularDataSettingsPrivate();

    int dataUseHome();
    void setDataUseHome(int value);
    
    int dataUseRoam();
    void setDataUseRoam(int value);

signals:

public slots:

protected:

protected slots:

private:

    Q_DISABLE_COPY(CellularDataSettingsPrivate)

    void readSettings(TCmGenConnSettings *settings);
    
    void writeSettings(TCmGenConnSettings *settings);

private slots:

private: // data

    // Friend classes

    // Public implementation pointer
    CellularDataSettings *q_ptr;
};

#endif // CELLULARDATASETTINGS_S60_P_H
