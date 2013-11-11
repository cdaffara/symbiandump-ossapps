#ifndef AGNVERSIT2_H
#define AGNVERSIT2_H/*
* Copyright (c) 2005 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   Implements the import functionality of CAgnVersit2
*
*/



// User includes
#include "AgnExternalInterface.h"	//CAgnExternalInterface

// Forward class declarations.
class CAgnVersit2StringProvider;
class CAgnVersit2Exporter;
class CAgnVersit2Importer;
class RReadStream;
class RWriteStream;

/**
Implementation of CAgnExternalInterface for iCalendar files. Objects of this
type are instantiated through the ECOM plugin mechanism in response to a mime
type of text/calendar.
@internalTechnology
*/
class CAgnVersit2 : public CAgnExternalInterface
    {
public:		// Construction/destruction.
    static CAgnVersit2* NewL();
    ~CAgnVersit2();
    
public:		// Methods.
    void ImportL(RPointerArray<CCalEntry>& aEntries, RReadStream& aReadStream, TUint aFlags, MAgnImportObserver& aObserver);
    void ExportL(const CCalEntry& aEntry, RWriteStream& aWriteStream, TUint aFlags, MAgnExportObserver& aObserver);
    void ExportL(RPointerArray<CCalEntry>& aEntries, RWriteStream& aWriteStream, TUint aFlags, MAgnExportObserver& aObserver);
    
private:	// Construction/destruction.
	CAgnVersit2();
    void ConstructL();

private:	// Members.
	CAgnVersit2StringProvider* iStringProvider;
	CAgnVersit2Exporter* iExporter;
	CAgnVersit2Importer* iImporter;
    };
    
#endif // AGNVERSIT2_H

// End of file.