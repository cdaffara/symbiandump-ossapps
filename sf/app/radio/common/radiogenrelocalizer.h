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
* Description: Small helper utility to localize radio genres
*
*/

#ifndef RADIOGENRELOCALIZER_H
#define RADIOGENRELOCALIZER_H

#include "radio_global.h"

// Constants

namespace RadioGenreLocalizer
{

    struct GenreMap
    {
        int mGenreCode;
        const char* mInCarousel;
        const char* mInStationsList;
        const char* mInHomeScreen;
    };

    static QString genreToString( RadioRegion::Region region, int genre, GenreTarget::Target target );
}

// Convenience macros to help in the construction of the localization tables
#define BEGIN_GENRE_MAP(name) const RadioGenreLocalizer::GenreMap name[] = {
#define EUROPEAN_GENRE(code, inCarousel, inStationsList, inHomescreen ) { GenreEurope::code, inCarousel, inStationsList, inHomescreen }
#define AMERICAN_GENRE(code, inCarousel, inStationsList, inHomescreen ) { GenreAmerica::code, inCarousel, inStationsList, inHomescreen }
#define END_GENRE_MAP(name) }; const int name ## Count = sizeof ( name ) / sizeof ( name[0] );

BEGIN_GENRE_MAP( EuropeanGenres )
                    // Genre code           // Text in Carousel                     // Text in Stations list                            // Text in Homescreen widget
    EUROPEAN_GENRE( RdsNone,                "",                                     "",                                                 "" ),
    EUROPEAN_GENRE( RdsNews,                "txt_rad_info_news",                    "txt_rad_dblist_l1_mhz_val_news",                   "txt_fmradiohswidget_rad_info_news_hs" ),
    EUROPEAN_GENRE( RdsCurrentAffairs,      "txt_rad_info_current_affairs",         "txt_rad_dblist_l1_mhz_val_current_affairs",        "txt_fmradiohswidget_rad_info_current_affairs_hs" ),
    EUROPEAN_GENRE( RdsInformation,         "txt_rad_info_information",             "txt_rad_dblist_l1_mhz_val_information",            "txt_fmradiohswidget_rad_info_information_hs" ),
    EUROPEAN_GENRE( RdsSport,               "txt_rad_info_sport",                   "txt_rad_dblist_l1_mhz_val_sport",                  "txt_fmradiohswidget_rad_info_sport_hs" ),
    EUROPEAN_GENRE( RdsEducation,           "txt_rad_info_education",               "txt_rad_dblist_l1_mhz_val_education",              "txt_fmradiohswidget_rad_info_education_hs" ),
    EUROPEAN_GENRE( RdsDrama,               "txt_rad_info_drama",                   "txt_rad_dblist_l1_mhz_val_drama",                  "txt_fmradiohswidget_rad_info_drama_hs" ),
    EUROPEAN_GENRE( RdsCulture,             "txt_rad_info_culture",                 "txt_rad_dblist_l1_mhz_val_culture",                "txt_fmradiohswidget_rad_info_culture_hs" ),
    EUROPEAN_GENRE( RdsScience,             "txt_rad_info_science",                 "txt_rad_dblist_l1_mhz_val_science",                "txt_fmradiohswidget_rad_info_science_hs" ),
    EUROPEAN_GENRE( RdsVariedSpeech,        "txt_rad_info_varied",                  "txt_rad_dblist_l1_mhz_val_varied",                 "txt_fmradiohswidget_rad_info_varied_hs" ),
    EUROPEAN_GENRE( RdsPopMusic,            "txt_rad_info_pop_music",               "txt_rad_dblist_l1_mhz_val_pop_music",              "txt_fmradiohswidget_rad_info_pop_music_hs" ),
    EUROPEAN_GENRE( RdsRockMusic,           "txt_rad_info_rock_music",              "txt_rad_dblist_l1_mhz_val_rock_music",             "txt_fmradiohswidget_rad_info_rock_music_hs" ),
    EUROPEAN_GENRE( RdsEasyListening,       "txt_rad_info_easy_listening",          "txt_rad_dblist_l1_mhz_val_easy_listening",         "txt_fmradiohswidget_rad_info_easy_listening_hs" ),
    EUROPEAN_GENRE( RdsLightClassical,      "txt_rad_info_light_classical",         "txt_rad_dblist_l1_mhz_val_light_classical",        "txt_fmradiohswidget_rad_info_light_classical_hs" ),
    EUROPEAN_GENRE( RdsSeriousClassical,    "txt_rad_info_serious_classical",       "txt_rad_dblist_l1_mhz_val_serious_classical",      "txt_fmradiohswidget_rad_info_serious_classical_hs" ),
    EUROPEAN_GENRE( RdsOtherMusic,          "txt_rad_info_other_music",             "txt_rad_dblist_l1_mhz_val_other_music",            "txt_fmradiohswidget_rad_info_other_music_hs" ),
    EUROPEAN_GENRE( RdsWeather,             "txt_rad_info_weather",                 "txt_rad_dblist_l1_mhz_val_weather",                "txt_fmradiohswidget_rad_info_weather_hs" ),
    EUROPEAN_GENRE( RdsFinance,             "txt_rad_info_finance",                 "txt_rad_dblist_l1_mhz_val_finance",                "txt_fmradiohswidget_rad_info_finance_hs" ),
    EUROPEAN_GENRE( RdsChildrensProgrammes, "txt_rad_info_childrens_programmes",    "txt_rad_dblist_l1_mhz_val_childrens_programmes",   "txt_fmradiohswidget_rad_info_childrens_programmes_hs" ),
    EUROPEAN_GENRE( RdsSocialAffairs,       "txt_rad_info_social_affairs",          "txt_rad_dblist_l1_mhz_val_social_affairs",         "txt_fmradiohswidget_rad_info_social_affairs_hs" ),
    EUROPEAN_GENRE( RdsReligion,            "txt_rad_info_religion",                "txt_rad_dblist_l1_mhz_val_religion",               "txt_fmradiohswidget_rad_info_religion_hs" ),
    EUROPEAN_GENRE( RdsPhoneIn,             "txt_rad_info_phone_in",                "txt_rad_dblist_l1_mhz_val_phone_in",               "txt_fmradiohswidget_rad_info_phone_in_hs" ),
    EUROPEAN_GENRE( RdsTravel,              "txt_rad_info_travel",                  "txt_rad_dblist_l1_mhz_val_travel",                 "txt_fmradiohswidget_rad_info_travel_hs" ),
    EUROPEAN_GENRE( RdsLeisure,             "txt_rad_info_leisure",                 "txt_rad_dblist_l1_mhz_val_leisure",                "txt_fmradiohswidget_rad_info_leisure_hs" ),
    EUROPEAN_GENRE( RdsJazzMusic,           "txt_rad_info_jazz_music",              "txt_rad_dblist_l1_mhz_val_jazz_music",             "txt_fmradiohswidget_rad_info_jazz_music_hs" ),
    EUROPEAN_GENRE( RdsCountryMusic,        "txt_rad_info_country_music",           "txt_rad_dblist_l1_mhz_val_country_music",          "txt_fmradiohswidget_rad_info_country_music_hs" ),
    EUROPEAN_GENRE( RdsNationalMusic,       "txt_rad_info_national_music",          "txt_rad_dblist_l1_mhz_val_national_music",         "txt_fmradiohswidget_rad_info_national_music_hs" ),
    EUROPEAN_GENRE( RdsOldiesMusic,         "txt_rad_info_oldies_music",            "txt_rad_dblist_l1_mhz_val_oldies_music",           "txt_fmradiohswidget_rad_info_oldies_music_hs" ),
    EUROPEAN_GENRE( RdsFolkMusic,           "txt_rad_info_folk_music",              "txt_rad_dblist_l1_mhz_val_folk_music",             "txt_fmradiohswidget_rad_info_folk_music_hs" ),
    EUROPEAN_GENRE( RdsDocumentary,         "txt_rad_info_documentary",             "txt_rad_dblist_l1_mhz_val_documentary",            "txt_fmradiohswidget_rad_info_documentary_hs" ),
    EUROPEAN_GENRE( RdsAlarmTest,           "txt_rad_info_alarm_test",              "txt_rad_dblist_l1_mhz_val_alarm_test",             "txt_fmradiohswidget_rad_info_alarm_test_hs" ),
    EUROPEAN_GENRE( RdsAlarm,               "txt_rad_info_alarm",                   "txt_rad_dblist_l1_mhz_val_alarm",                  "txt_fmradiohswidget_rad_info_alarm_hs" )
END_GENRE_MAP( EuropeanGenres )

BEGIN_GENRE_MAP( AmericanGenres )
                    // Genre code           // Text in Carousel                     // Text in Stations list                            // Text in Homescreen widget
    AMERICAN_GENRE( RbdsNone,               "",                                     "",                                                 "" ),
    AMERICAN_GENRE( RbdsNews,               "txt_rad_info_news",                    "txt_rad_dblist_l1_mhz_val_news",                   "txt_fmradiohswidget_rad_info_news_hs" ),
    AMERICAN_GENRE( RbdsInformation,        "txt_rad_info_information",             "txt_rad_dblist_l1_mhz_val_information",            "txt_fmradiohswidget_rad_info_information_hs" ),
    AMERICAN_GENRE( RbdsSports,             "txt_rad_info_sport",                   "txt_rad_dblist_l1_mhz_val_sport",                  "txt_fmradiohswidget_rad_info_sport_hs" ),
    AMERICAN_GENRE( RbdsTalk,               "txt_rad_info_talk",                    "txt_rad_dblist_l1_mhz_val_talk",                   "txt_fmradiohswidget_rad_info_talk_hs" ),
    AMERICAN_GENRE( RbdsRock,               "txt_rad_info_rock_music",              "txt_rad_dblist_l1_mhz_val_rock_music",             "txt_fmradiohswidget_rad_info_rock_music_hs" ),
    AMERICAN_GENRE( RbdsClassicRock,        "txt_rad_info_classic_rock",            "txt_rad_dblist_l1_mhz_val_classic_rock",           "txt_fmradiohswidget_rad_info_classic_rock_hs" ),
    AMERICAN_GENRE( RbdsAdultHits,          "txt_rad_info_adult_hits",              "txt_rad_dblist_l1_mhz_val_adult_hits",             "txt_fmradiohswidget_rad_info_adult_hits_hs" ),
    AMERICAN_GENRE( RbdsSoftRock,           "txt_rad_info_soft_rock",               "txt_rad_dblist_l1_mhz_val_soft_rock",              "txt_fmradiohswidget_rad_info_soft_rock_hs" ),
    AMERICAN_GENRE( RbdsTop40,              "txt_rad_info_top_40",                  "txt_rad_dblist_l1_mhz_val_top_40",                 "txt_fmradiohswidget_rad_info_top_40_hs" ),
    AMERICAN_GENRE( RbdsCountry,            "txt_rad_info_country_music",           "txt_rad_dblist_l1_mhz_val_country_music",          "txt_fmradiohswidget_rad_info_country_music_hs" ),
    AMERICAN_GENRE( RbdsOldies,             "txt_rad_info_oldies_music",            "txt_rad_dblist_l1_mhz_val_oldies_music",           "txt_fmradiohswidget_rad_info_oldies_music_hs" ),
    AMERICAN_GENRE( RbdsSoft,               "txt_rad_info_soft",                    "txt_rad_dblist_l1_mhz_val_soft",                   "txt_fmradiohswidget_rad_info_soft_hs" ),
    AMERICAN_GENRE( RbdsNostalgia,          "txt_rad_info_nostalgia",               "txt_rad_dblist_l1_mhz_val_nostalgia",              "txt_fmradiohswidget_rad_info_nostalgia_hs" ),
    AMERICAN_GENRE( RbdsJazz,               "txt_rad_info_jazz_music",              "txt_rad_dblist_l1_mhz_val_jazz_music",             "txt_fmradiohswidget_rad_info_jazz_music_hs" ),
    AMERICAN_GENRE( RbdsClassical,          "txt_rad_info_classical",               "txt_rad_dblist_l1_mhz_val_classical",              "txt_fmradiohswidget_rad_info_classical_hs" ),
    AMERICAN_GENRE( RbdsRhythmAndBlues,     "txt_rad_info_rhythm_and_blues",        "txt_rad_dblist_l1_mhz_val_rhythm_and_blues",       "txt_fmradiohswidget_rad_info_rhythm_and_blues_hs" ),
    AMERICAN_GENRE( RbdsSoftRhythmAndBlues, "txt_rad_info_soft_rhythm_and_blues",   "txt_rad_dblist_l1_mhz_val_soft_rhythm_and_blues",  "txt_fmradiohswidget_rad_info_soft_rhythm_and_blues_hs" ),
    AMERICAN_GENRE( RbdsLanguage,           "txt_rad_info_language",                "txt_rad_dblist_l1_mhz_val_language",               "txt_fmradiohswidget_rad_info_language_hs" ),
    AMERICAN_GENRE( RbdsReligiousMusic,     "txt_rad_info_religious_music",         "txt_rad_dblist_l1_mhz_val_religious_music",        "txt_fmradiohswidget_rad_info_religious_music_hs" ),
    AMERICAN_GENRE( RbdsReligiousTalk,      "txt_rad_info_religious_talk",          "txt_rad_dblist_l1_mhz_val_religious_talk",         "txt_fmradiohswidget_rad_info_religious_talk_hs" ),
    AMERICAN_GENRE( RbdsPersonality,        "txt_rad_info_personality",             "txt_rad_dblist_l1_mhz_val_personality",            "txt_fmradiohswidget_rad_info_personality_hs" ),
    AMERICAN_GENRE( RbdsPublic,             "txt_rad_info_public",                  "txt_rad_dblist_l1_mhz_val_public",                 "txt_fmradiohswidget_rad_info_public_hs" ),
    AMERICAN_GENRE( RbdsCollege,            "txt_rad_info_college",                 "txt_rad_dblist_l1_mhz_val_college",                "txt_fmradiohswidget_rad_info_college_hs" ),
    AMERICAN_GENRE( RbdsUnassigned1,        "",                                     "",                                                 "" ),
    AMERICAN_GENRE( RbdsUnassigned2,        "",                                     "",                                                 "" ),
    AMERICAN_GENRE( RbdsUnassigned3,        "",                                     "",                                                 "" ),
    AMERICAN_GENRE( RbdsUnassigned4,        "",                                     "",                                                 "" ),
    AMERICAN_GENRE( RbdsUnassigned5,        "",                                     "",                                                 "" ),
    AMERICAN_GENRE( RbdsWeather,            "txt_rad_info_weather",                 "txt_rad_dblist_l1_mhz_val_weather",                "txt_fmradiohswidget_rad_info_weather_hs" ),
    AMERICAN_GENRE( RbdsEmergencyTest,      "txt_rad_info_alarm_test",              "txt_rad_dblist_l1_mhz_val_alarm_test",             "txt_fmradiohswidget_rad_info_alarm_test_hs" ),
    AMERICAN_GENRE( RbdsEmergency,          "txt_rad_info_alarm",                   "txt_rad_dblist_l1_mhz_val_alarm",                  "txt_fmradiohswidget_rad_info_alarm_hs" ),
END_GENRE_MAP( AmericanGenres )

/*!
 * Function to find the localized text of the given frequency in the given region and target
 */
static QString RadioGenreLocalizer::genreToString( RadioRegion::Region region, int genre, GenreTarget::Target target )
{
    const RadioGenreLocalizer::GenreMap* genreArray = region == RadioRegion::America ? AmericanGenres : EuropeanGenres;
    const int genreCount = region == RadioRegion::America ? AmericanGenresCount : EuropeanGenresCount;

    for( int i = 0; i < genreCount; ++i ) {
        if ( genreArray[i].mGenreCode == genre ) {
            if ( target == GenreTarget::Carousel ) {
                return qtTrId( genreArray[i].mInCarousel );
            } else if ( target == GenreTarget::StationsList ) {
                return qtTrId( genreArray[i].mInStationsList );
            } else if ( target == GenreTarget::HomeScreen ) {
                return qtTrId( genreArray[i].mInHomeScreen );
            }
        }
    }

    return "";
}

#endif // RADIOGENRELOCALIZER_H
