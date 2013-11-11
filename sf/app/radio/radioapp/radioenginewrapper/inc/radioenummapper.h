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
* Description:
*
*/

#ifndef RADIOENUMMAPPER_H_
#define RADIOENUMMAPPER_H_

/*!
 * Convenience macros to help define enum maps.
 * Note! It is important to define the values so that the UI value is on the left
 * and the engine value is on the right. The lookup functions expect them in this order.
 *
 * Example:
 * BEGIN_ENUM_MAP( KSomeMap )
 *   ENUM_MAP_ITEM( UiNamespace::UiValue1, EngineNamespace::EEngineValue1 ),
 *   ENUM_MAP_ITEM( UiNamespace::UiValue2, EngineNamespace::EEngineValue2 )
 * END_ENUM_MAP( KSomeMap )
 */
#define BEGIN_ENUM_MAP(name) const RadioEnumMapper::EnumMap name[] = {
#define ENUM_MAP_ITEM(ui_enum, engine_enum ) { ui_enum, engine_enum }
#define END_ENUM_MAP(name) }; const int name ## Count = sizeof ( name ) / sizeof ( name[0] );

/*!
 * Convenience macros to do the mappings
 */
#define MAP_FROM_UI_ENUM(type,uienum, map) RadioEnumMapper::FromUiEnum<type>( uienum, map, map ## Count )
#define MAP_TO_UI_ENUM(type,uienum, map) RadioEnumMapper::ToUiEnum<type>( uienum, map, map ## Count )

class RadioEnumMapper
{
public:

    /*!
     * Item in the enum map. Ties a value in the UI enum to its correspondent in the engine enum
     */
    struct EnumMap
    {
        int iUiEnum;
        int iEngineEnum;
    };

    /*!
     * Maps engine enum value to its ui correspondent.
     *
     * @param aValue Enum value in the engine
     * @param aMap Enum value map
     * @param aCount Amount of items in the map
     * @return Enum value in the ui
     */
    template<typename UI_ENUM, typename ENGINE_ENUM>
    inline static UI_ENUM ToUiEnum( ENGINE_ENUM aValue, const EnumMap* aMap, int aCount )
    {
        return static_cast<UI_ENUM>( MapToUiEnum( aValue, aMap, aCount ) );
    }

    /*!
     * Maps ui enum value to its engine correspondent.
     *
     * @param aValue Enum value in the Ui
     * @param aMap Enum value map
     * @param aCount Amount of items in the map
     * @return Enum value in the engine
     */
    template<typename ENGINE_ENUM, typename UI_ENUM>
    inline static ENGINE_ENUM FromUiEnum( UI_ENUM aValue, const EnumMap* aMap, int aCount )
    {
        return static_cast<ENGINE_ENUM>( MapFromUiEnum( aValue, aMap, aCount ) );
    }

private:

    /*!
     * Maps engine enum value to its ui correspondent.
     * Type-unsafe version. Not to be used directly!
     *
     * @param aValue Enum value in the engine
     * @param aMap Enum value map
     * @param aCount Amount of items in the map
     * @return Enum value in the ui
     */
    static int MapToUiEnum( int aValue, const EnumMap* aMap, int aCount )
    {
        for ( int i = 0; i < aCount; ++i ) {
            if ( aMap[i].iUiEnum == aValue ) {
                return aMap[i].iEngineEnum;
            }
        }
        return -1;
    }

    /*!
     * Maps ui enum value to its engine correspondent.
     * Type-unsafe version. Not to be used directly!
     *
     * @param aValue Enum value in the Ui
     * @param aMap Enum value map
     * @param aCount Amount of items in the map
     * @return Enum value in the engine
     */
    static int MapFromUiEnum( int aValue, const EnumMap* aMap, int aCount )
    {
        for ( int i = 0; i < aCount; ++i ) {
            if ( aMap[i].iEngineEnum == aValue ) {
                return aMap[i].iUiEnum;
            }
        }
        return -1;
    }

    /*
     * Hidden constructor and destructor to prevent instantiation
     */
    RadioEnumMapper() {}
    ~RadioEnumMapper() {}

};

#endif // RADIOENUMMAPPER_H_
