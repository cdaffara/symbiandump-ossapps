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




#ifndef CREATORFILEELEMENT_H_
#define CREATORFILEELEMENT_H_

#include "creator_scriptelement.h"

namespace creatorfile
{
    _LIT(KFile, "file");
    _LIT(KType, "type");
    _LIT(KDirectory, "directory");
    _LIT(KDefaultDir, "c:\\data");
    _LIT(KEncryption, "encryption");
    _LIT(KDRMRight, "right");
    _LIT(KDRMFL, "DRM-FL");
    _LIT(KDRMCD, "DRM-CD");
    _LIT(KDRMPlayRight, "play");
    _LIT(KDRMDisplayRight, "display");
    _LIT(KDRMPrintRight, "print");
    _LIT(KDRMExecuteRight, "execute");
    _LIT(KDRMCount, "count");
    _LIT(KDRMInterval, "interval");
    _LIT(KDRMStartTime, "starttime");
    _LIT(KDRMEndTime, "endtime");
    _LIT(KDRMAccumulated, "accumulated");
}

class CDRMConstraint;
class CFilesParameters;

class CCreatorFileElement : public CCreatorScriptElement
{
public:
    static CCreatorFileElement* NewL(CCreatorEngine* aEngine, const TDesC& aName, const TDesC& aContext = KNullDesC);
    void ExecuteCommandL();
    
protected:
    CCreatorFileElement(CCreatorEngine* aEngine);
    TInt GetFileCommandL( const TDesC& aFileIdStr, TBool aRandom ) const;
private:
    void ParseDRMElementsL( CCreatorScriptElement* aField, CFilesParameters* aParam );
    void ParseDRMConstraintsL( CCreatorScriptElement* aRight, CDRMConstraint* aConstraint );
    TTimeIntervalSeconds ParseTimeInterval( TDesC& aTimeString );
private: // Data
    TInt64 iTimeZone;
};

#endif // CREATORFILEELEMENT_H_