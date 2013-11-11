/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description: The document class for ConnTest, stores the settings
*
*/

#ifndef CONNTESTDOCUMENT_H
#define CONNTESTDOCUMENT_H

// INCLUDES
#include <AknDoc.h>

#include "conntest.hrh"

// CONSTANTS

// FORWARD DECLARATIONS
class CEikAppUi;
class CSettingData;
class CCustomPrefsData;

// CLASS DECLARATION

/**
*  CConnTestDocument document class.
*  Note! Base class is CEikDocument, because CAknDocument does
*  not provide support for storing persistent data into file.
*/
class CConnTestDocument : public CEikDocument
{
public: // Constructors and destructor
    
    /**
    * Two-phased constructor.
    */
    static CConnTestDocument* NewL(CEikApplication& aApp);
    
    /**
    * Destructor.
    */
    virtual ~CConnTestDocument();
        
protected:  // Functions from base classes
    
    /**
    * Store current connection settings to file. Uses default file name,
    * which is \\private\\101F6D2B\\ConnTest.
    */
    void StoreL(CStreamStore& aStore, CStreamDictionary& aStreamDic) const;
    
    /**
    * Read previous connection settings from file.
    */
    void RestoreL(const CStreamStore& aStore, const CStreamDictionary& aStreamDic);
    
private:
    
    /**
    * Symbian default constructor.
    */
    CConnTestDocument(CEikApplication& aApp);
    void ConstructL();
    
private:
    
    /**
    * From CEikDocument, create CConnTestAppUi "App UI" object.
    */
    CEikAppUi* CreateAppUiL();
    
private: // Data
    TFixedArray<CSettingData*, KConnTestViews> iSettingDataArray;
    TFixedArray<CCustomPrefsData*, KConnTestViews> iCustomPrefsDataArray;

};

#endif

// End of File

