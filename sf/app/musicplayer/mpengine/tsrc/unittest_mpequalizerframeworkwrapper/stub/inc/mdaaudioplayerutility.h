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
* Description: mdaaudioplayerutility stub for testing mpequalizerframeworkwrapper
*
*/

#ifndef CMDAAUDIOPLAYERUTILITY_H
#define CMDAAUDIOPLAYERUTILITY_H


#include <e32base.h>
#include <mmf/common/mmfbase.h>

//Forward declarations
class MMdaAudioPlayerCallback;

class CMdaAudioPlayerUtility : public CBase
{
public:  // Constructors and destructor

    static CMdaAudioPlayerUtility* NewL(MMdaAudioPlayerCallback& aCallback,
                                        TInt aPriority = EMdaPriorityNormal,
                                        TInt aPref = EMdaPriorityPreferenceTimeAndQuality);

    /**
    * Destructor.
    */
    virtual ~CMdaAudioPlayerUtility();

    void OpenFileL(const TDesC& aFileName);
    
    void Close();

private:

    /**
    * C++ default constructor.
    */
    CMdaAudioPlayerUtility();

    /**
    * Second Phase Costructor for CMdaAudioPlayerUtility
    */
    void ConstructL();

private: // Data

};

#endif      // CMDAAUDIOPLAYERUTILITY_H

