/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Base class for the Agenda Model event adapter managers
 *
*/


#ifndef CPIMEVENTADAPTERMANAGER_H
#define CPIMEVENTADAPTERMANAGER_H

// INCLUDES
#include "mpimeventadaptermanager.h"
#include "mpimadaptermanager.h"
#include "cpimeventlistadapter.h"
#include "pimevent.h"
#include "pimlocalizationids.h"
#include "agneventsupport.h"

// CONSTANTS
const TInt KPIMSupportedRepeatRuleFieldsCount = 2;

const TInt KPIMSupportedRepeatRuleFields[KPIMSupportedRepeatRuleFieldsCount] =
{
    EPIMRepeatRuleFrequency,
    EPIMRepeatRuleInterval
};

const TInt KPIMRepeatRuleSupportedWeeklyInterval = 2;
const TInt KPIMSupportedRepeatRuleIntervalsCount = 1;

const TInt
KPIMSupportedRepeatRuleIntervals[KPIMSupportedRepeatRuleIntervalsCount] =
    { KPIMRepeatRuleSupportedWeeklyInterval };

// CLASS DECLARATION

/**
 * PIM Event Adapter Manager base class
 */
NONSHARABLE_CLASS(CPIMEventAdapterManager): public CBase,
        public MPIMAdapterManager,
        public MPIMEventAdapterManager
{
public: // destructor

    /**
     * Destructor.
     */
    virtual ~CPIMEventAdapterManager();

public: // MPIMEventAdapterManager

    /**
     * Provides access to the \ref MPIMAdapterManager representation of
     * this object.
     *
     * @return The \ref MPIMAdapterManager representation of this object.
     */
    virtual MPIMAdapterManager* GetAdapterManager() = 0;

    /**
     * Provides the supported repeat rule fields for a given frequency.
     *
     * @param aFrequency A frequency.
     *
     * @return Supported repeat rule fields for \a aFrequency. If no
     *         fields are supported for the frequency, an empty array
     *         is returned.
     *
     * @par Leaving:
     * @li \c KErrArgument - \a aFrequency is not a valid frequency.
     */
    virtual const CArrayFix< TPIMField>& GetSupportedRepeatRuleFieldsL(
        const TPIMRepeatRuleFrequency& aFrequency);

    /**
     * Provides the supported interval values for given frequency.
     *
     * @param aFrequency A frequency.
     *
     * @return Supported interval values for given frequency. If the
     *         interval field is not supported for given frequency,
     *         an empty array is returned.
     *
     * @par Leaving:
     * @li \c KErrArgument - \a aFrequency is not a valid frequency.
     */
    virtual const CArrayFix< TInt>& GetSupportedIntervalsL(
        const TPIMRepeatRuleFrequency& aFrequency);

public: // Functions from generic MPIMAdapterManager

    const TDesC& ListNameL();

    /**
     * Provides attributes supported for a field, combined into single
     * integer value.
     *
     * @param aField The field to which the attributes are associated with.
     *
     * @return An integer holding the combination of all attributes
     *         supported for \a aField.
     *
     * @par Leaving.
     * The method leaves on error. Error codes should be interpreted as
     * follows:
     * @li \c KErrArgument - \a aField is not valid.
     */
    TPIMAttribute GetSupportedAttributesCombinedL(
        TPIMField aField);

    /**
     * Provides all attributes supported for all fields, combined into
     * single integer value.
     *
     * @return An integer holding the combination of all attributes
     *         supported for all fields.
     */
    TPIMAttribute GetAllSupportedAttributesCombined();

    /**
     * Provides number of maximum categories supported by the list adapter.
     * @return Number of maximum supported categories .
     */
    TInt MaxCategories();

    /**
     * Checks whether a specific field is supported by the list adapter.
     * @param aField The field to be checked.
     * @return ETrue if the given field is supported, EFalse otherwise.
     */
    TBool IsSupportedField(
        TPIMField aField);

    /**
     * Provides all fields supported by the list adapter.
     *
     * @return An array containing all supported fields. The order of the
     *         fields must be the same as in the native side application
     *         UI.
     *
     * @par Leaving:
     * The method leaves on error. Such error always means that the adapter
     * is non-functional.
     */
    const CArrayFix<TPIMField>& GetSupportedFieldsL();

    /**
     * Checks whether a specific attribute is supported by the list adapter.
     *
     * @param aField The field to which the attribute might be associated.
     * @param aAttribute The attribute to be checked.
     *
     * @return ETrue if the given attribute is supported, EFalse otherwise,
     *         also on unsupported \a aField.
     */
    TBool IsSupportedAttribute(
        TPIMField aField,
        TPIMAttribute aAttribute);

    /**
     * Provides attributes supported for a field.
     *
     * @param aField The field to which the attributes are associated with.
     *
     * @return An array containing attributes supported for the \a aField.
     *         The order of attributes must reflect the order of field and
     *         attribute combinations in the native application UI.
     *
     * @par Leaving:
     * The method leaves on error. Error codes should be interpreted as
     * follows:
     * @li \c KErrArgument - \a aField is not valid.
     * @li Other - Array could not be created.
     */
    const CArrayFix<TPIMAttribute>& GetSupportedAttributesL(
        TPIMField aField);

    /**
     * Checks whether a specific array element is supported by the list adapter.
     *
     * @param aStringArrayField The field to which the array element belongs to.
     * @param aArrayElement The array element to be checked.
     *
     * @return ETrue if the given array element is supported, EFalse otherwise.
     */
    TBool IsSupportedArrayElement(
        TPIMField aStringArrayElement,
        TPIMArrayElement aArrayElement);

    /**
     * Provides supported array elements for a specific field.
     *
     * @param aStringArrayField A string array field.
     *
     * @return An array containing supported array elements for the field.
     *
     * @par Leaving:
     * The method leaves on error. Error codes should be interpreted as
     * follows:
     * @li \c KErrArgument - \a aField is not valid.
     * @li Other - Array could not be created.
     */
    const CArrayFix<TPIMArrayElement>& GetSupportedArrayElementsL(
        TPIMField aStringArrayField);

    /**
     * Provides the number of maximum values supported for a specific field.
     *
     * @param aField The field to be evaluated.
     *
     * @return Number of values supported for the field. -1 for unlimited,
     *         0 if not supported.
     */
    virtual TInt MaxValues(
        TPIMField aField);

    /**
     * Provides the array size of a specific string array field.
     * The values in a string array field are arrays themselves.
     * StringArraySize method returns the number of elements in every value
     * of the field.
     *
     * @param aStringArrayField The string array field to be evaluated.
     * @return Number of elements in a single string array value.
     *
     * @par Leaving:
     * The method leaves with \c KErrArgument if \a aStringArrayField
     * is not a string array field or is not supported.
     */
    TInt StringArraySizeL(
        TPIMField aStringArrayField);

    /**
     * Provides pointer to a function which implements an algorithm that
     * determines the order of two items.
     *
     * @return Item comparison function.
     */
    TPIMItemComparisonFunc ItemOrder();

    /**
     * Compares two MPIMItemData objects.
     * Used for determining their ordering in an item enumeration.
     *
     * @param aFirst first object to compare
     * @param aSecond second object to compare
     * @return
     * @li < 0, if aFirst becomes \b before aSecond
     * @li 0, if the objects are equal in the ordering
     * @li > 0, if aFirst becomes \b after aSecond
     */
    static TInt ItemComparisonFunc(
        const MPIMItem& aFirst,
        const MPIMItem& aSecond);

    /**
     * Compares two MPIMItemData objects.
     * Used for determining their ordering in an item enumeration.
     * This is the leaving version of the comparison function.
     *
     * @param aFirst first object to compare
     * @param aSecond second object to compare
     * @return
     * @li < 0, if aFirst becomes \b before aSecond
     * @li 0, if the objects are equal in the ordering
     * @li > 0, if aFirst becomes \b after aSecond
     */
    static TInt ItemComparisonFuncL(
        const MPIMItem& aFirst,
        const MPIMItem& aSecond);

protected:

    // Allow derivation with protected default constructor.
    CPIMEventAdapterManager()
    {}

    /**
     * 2nd phase constructor.
     *
     * @param aListName Name of the list.
     */
    void ConstructL(const TDesC& aListName);

private: // Member data

    // All the following arrays are owned by CPIMEventAdapterManager class
    CArrayFixFlat< TPIMField>* iRepeatRuleFieldArray;
    CArrayFixFlat< TPIMField>* iRepeatRuleFieldArrayEmpty;
    CArrayFixFlat< TInt>* iRepeatRuleIntervalArray;
    CArrayFixFlat< TInt>* iRepeatRuleIntervalArrayEmpty;
    CArrayFixFlat< TPIMAttribute>* iEventAttributeArray;

protected:

    /** Owned. */
    TDesC* iListName;

    CArrayFixFlat<TPIMField>* iSupportedFields;
};

#endif // CPIMEVENTADAPTERMANAGER_H
// End of File

