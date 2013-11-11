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
* Description:  Calendar Lunar Plugin
*
*/

#ifndef __CALENLUNARINFO_H__
#define __CALENLUNARINFO_H__

#include <e32def.h>
#include <calendarconverter.h>

class TCalenLunarInfo 
    {
public:
    enum TFestival
        {
        ENoFestival = KErrNotFound,
        EFestivalSpring,
        EFestivalLantern,
        EFestivalDragonBoat,
        EFestivalSeventhNight,
        EFestivalGhost,
        EFestivalMidAutumn,
        EFestivalDoubleNinth,
        EFestivalTwelfthMonth,
        EFestivalNewYearEve
        };

    enum TAnimalYear
        {
        EYearOfRat = 0,
        EYearOfOx,
        EYearOfTiger,
        EYearOfRabbit,
        EYearOfDragon,
        EYearOfSnake,
        EYearOfHorse,
        EYearOfSheep,
        EYearOfMonkey,
        EYearOfRooster,
        EYearOfDog,
        EYearOfPig
        };
        
    typedef TInt TSolarTerm;
    typedef TInt THeavenlyStem;
    typedef TInt TTerrestialBranch;
    
public:
    IMPORT_C    TBool HasFestival();
    IMPORT_C    TBool HasSolarTerm();
    
    IMPORT_C TAnimalYear AnimalYear();
    
public:
    TFestival     iFestival;
    TSolarTerm    iSolarTerm;
    TChineseDate  iLunarDate;
    TTime         iGregorianDate;

    THeavenlyStem iHeavenlyStem;    
    TTerrestialBranch iTerrestialBranch; 
    };


#endif // __CALENLUNARINFO_H__
