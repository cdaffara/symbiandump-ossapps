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

#include <QPair>
#include <QtXml>
#include <QFile>
#include <hbparameterlengthlimiter.h>
#include <HbExtendedLocale>


#include "cxutils.h"
#include "cxuienums.h"
#include "cxuisettingxmlreader.h"
#include "cxuisettingradiobuttonlist.h"

using namespace CxUiSettings;

namespace
{
    static const char *TAG_ROOT                           = "cxui";
    static const char *TAG_SETTINGS                       = "settings";
    static const char *TAG_SETTING_LIST                   = "setting_list";
    static const char *TAG_SETTING_SLIDER                 = "setting_slider";
    static const char *TAG_ITEM                           = "item";
    static const char *TAG_ITEM_VARIANT_LN                = "lnItem";
    static const char *TAG_ITEM_VARIANT_L1                = "l1Item";
    static const char *TAG_SETUP                          = "setup";

    static const char *ATTRIBUTE_ID                       = "id";
    static const char *ATTRIBUTE_HEADING                  = "heading";
    static const char *ATTRIBUTE_PREVIEW                  = "preview";
    static const char *ATTRIBUTE_LISTBOXTYPE              = "type";
    static const char *ATTRIBUTE_HEADING_ICON             = "setting_icon";
    static const char *ATTRIBUTE_ICON                     = "icon";

    static const char *ATTRIBUTE_VALUE                    = "value";
    static const char *ATTRIBUTE_LOCALIZATION_ID          = "string";
    static const char *ATTRIBUTE_LOCALIZATION_ID_LN_VALUE = "lnValue";
    static const char *ATTRIBUTE_LOCALIZATION_ID_L1_VALUE = "l1Value";

    static const char *ATTRIBUTE_SLIDER_MIN               = "min";
    static const char *ATTRIBUTE_SLIDER_MAX               = "max";
    static const char *ATTRIBUTE_SLIDER_MINOR_STEP        = "minorStep";
    static const char *ATTRIBUTE_SLIDER_MAJOR_STEP        = "majorStep";

    // possible values that can be used in xml for "ATTRIBUTE_LISTBOXTYPE"
    static const char *SINGLE_LINE_LISTBOX                = "SingleLineListBox";
    static const char *TWO_LINE_LISTBOX                   = "TwoLineListBox";
}


CxuiSettingXmlReader::CxuiSettingXmlReader()
  : mXmlUri(),
    mParsed(false),
    mListParams(),
    mSliderParams()
{
}


CxuiSettingXmlReader::~CxuiSettingXmlReader()
{
    clear();
}


/*!
* Get the settings info based on the setting id
* @param setting denotes the setting key id.
* @param Returns list of settings specific to radiobuttonlist.
* @return Were the params found, i.e. is the setting supported.
*/
CxUiSettings::RadioButtonListParams CxuiSettingXmlReader::getListSettingInfo(const QString &setting, bool &found)
{
    CX_DEBUG_ENTER_FUNCTION();
    RadioButtonListParams p;

    // Parse current XML file now if not already done.
    parse();

    if (mListParams.contains(setting)){
        CX_ASSERT_ALWAYS(mListParams.value(setting));
        p = *mListParams.value(setting);
        found = true;
    } else {
        found = false;
    }

    CX_DEBUG_EXIT_FUNCTION();
    return p;
}



/*!
* Get the settings info based on the setting id
* @param setting Denotes the setting key id.
* @param found Were the params found, i.e. is the setting supported.
* @return Returns list of settings specific to slider setting.
*/
CxUiSettings::SliderParams CxuiSettingXmlReader::getSliderSettingInfo(const QString &setting, bool &found)
{
    CX_DEBUG_ENTER_FUNCTION();
    SliderParams p;

    // Parse current XML file now if not already done.
    parse();

    if (mSliderParams.contains(setting)){
        CX_ASSERT_ALWAYS(mSliderParams.value(setting));
        p = *mSliderParams.value(setting);
        found = true;
    } else {
        found = false;
    }

    CX_DEBUG_EXIT_FUNCTION();
    return p;
}

/*!
* Set new source for setting XML.
* @param uri URI for the setting XML. Note: Currently only local file path is supported.
*/
void CxuiSettingXmlReader::setXmlSource(const QString& uri)
{
    CX_DEBUG_ENTER_FUNCTION();
    CX_DEBUG(("Settings mapping XML: XML source set to [%s]", uri.toAscii().constData()));

    if(mXmlUri.compare(uri) != 0) {
        // Clear any old content and set flag that parsing needs to be done again.
        clear();
        mXmlUri = uri;
    }
    CX_DEBUG_EXIT_FUNCTION();
}

/*!
* Clear existing setting info and raise flag for needed parsing.
*/
void CxuiSettingXmlReader::clear()
{
    CX_DEBUG_ENTER_FUNCTION();

    mParsed = false;

    qDeleteAll(mListParams.values());
    mListParams.clear();

    qDeleteAll(mSliderParams.values());
    mSliderParams.clear();

    CX_DEBUG_EXIT_FUNCTION();
}

/*!
* Parses the XML and creates all settings list defined in XML
*/
void CxuiSettingXmlReader::parse()
{
    CX_DEBUG_ENTER_FUNCTION();

    if (mParsed) {
        CX_DEBUG(("Settings mapping XML: Already parsed our XML, using cached content") );
        CX_DEBUG_EXIT_FUNCTION();
        return;
    }

    QDomDocument document("dummy");
    QFile file(mXmlUri);

    if (!file.open(QFile::ReadOnly|QFile::Text)) {
        CX_DEBUG(("Settings mapping XML: could not open file [%s]", mXmlUri.toAscii().constData()));
        return;
    }

    bool ok = document.setContent(&file);
    file.close();

    if (!ok) {
        CX_DEBUG(("Settings mapping XML: could not set content"));
        return;
    }

    QDomElement root = document.documentElement();
    if (root.tagName() != TAG_ROOT) {
        CX_DEBUG(("Settings mapping XML: root name wrong %s", root.tagName().toAscii().constData()));
        return;
    }

    QDomElement e = findChildElement(root, TAG_SETTINGS);
    if (e.isNull()) {
        CX_DEBUG(("Settings mapping XML: could not find settings tag"));
        return;
    }

    parseSettings(e);

    // We've done parsing. No need to do it again.
    mParsed = true;

    CX_DEBUG_EXIT_FUNCTION();
}


/*!
* Parses the settings in XML and creates settings requested using the DOM element
*/
void CxuiSettingXmlReader::parseSettings(const QDomElement& element) {
    CX_DEBUG_ENTER_FUNCTION();

    if (element.isNull()) {
        //!@todo throw exception
        return;
    }

    QString tagName;
    QDomElement e;
    // Go through list of items
    for(QDomNode n = element.firstChild(); !n.isNull(); n = n.nextSibling()) {
        e = n.toElement();
        tagName = e.tagName();

        if (tagName == TAG_SETTING_LIST) {
            // <setting_list id="WHITE_BALANCE" heading="qtn_setting_title_wb">
            RadioButtonListParams *p(parseSettingList(e));
            if (p) {
                mListParams.insert(p->mSettingId,p);
            }
        } else if(tagName == TAG_SETTING_SLIDER) {
            // <setting_slider id="EXPOSURE_COMPENSATION" heading="qtn_setting_title_ev">
            SliderParams *p(parseSettingSlider(e));
            if (p) {
                mSliderParams.insert(p->mSettingId,p);
            }
        }
    }

    CX_DEBUG_EXIT_FUNCTION();
}


/*!
* Creates the settings list based on the DomElement
returns radiobutton list data
*/
CxUiSettings::RadioButtonListParams *CxuiSettingXmlReader::parseSettingList(const QDomElement& element)
{
    CX_DEBUG_ENTER_FUNCTION();

    // Read the heading and id.
    if (element.isNull()) {
        return NULL;
    }

    RadioButtonListParams *p = new RadioButtonListParams();

    CX_DEBUG(("Parsing radio button list for setting id [%s]", element.attribute(ATTRIBUTE_ID).toAscii().constData()));

    // XML something like this
    // <setting_list id="WHITE_BALANCE" heading="qtn_setting_title_wb">
    p->mHeading = hbTrId(element.attribute(ATTRIBUTE_HEADING).toAscii().constData());
    p->mSettingId = element.attribute(ATTRIBUTE_ID);
    p->mHeadingIcon = element.attribute(ATTRIBUTE_HEADING_ICON);

    // Set the list box attributes

    // Example: preview="1"
    p->mPreview = element.attribute(ATTRIBUTE_PREVIEW).toInt() == 1;

    // Example: type="SingleLineListBox"
    if (element.attribute(ATTRIBUTE_LISTBOXTYPE) == SINGLE_LINE_LISTBOX) {
        p->mListboxType = CxuiSettingRadioButtonList::SingleLine;
    } else if(element.attribute(ATTRIBUTE_LISTBOXTYPE) == TWO_LINE_LISTBOX) {
        p->mListboxType = CxuiSettingRadioButtonList::TwoLine;
    }

    QDomElement e;
    p->mSettingPairList.clear();

    // Go through list of items
    // XML something like this
    // <item value="0" string="qtn_whitebalance_auto" />
    bool ok(false);
    int intValue(0);
    QVariant value;
    QString string;
    for (QDomNode n = element.firstChild(); !n.isNull(); n = n.nextSibling()) {
        e = n.toElement();
        CX_DEBUG(("tag name <%s>", e.tagName().toAscii().constData()));

        // Get the item equivalent engine value from xml.
        // We need to know later if it is a string or int type, hence the extra effort.
        // QVariant has auto-conversion even from string to int, so can't use QVariant::canConvert<T>().
        string = e.attribute(ATTRIBUTE_VALUE);
        intValue = string.toInt(&ok);
        if (ok) {
            value.setValue(intValue);
        } else {
            value.setValue(string);
        }

        // get the string text id from the xml, and format based on type.
        string = e.attribute(ATTRIBUTE_LOCALIZATION_ID);
        if (e.tagName() == TAG_ITEM) {
            // string without parameters: get localised string
            string = hbTrId(string.toAscii().constData());
            CX_DEBUG(("tag name <%s>", e.tagName().toAscii().constData()));
        } else if (e.tagName() == TAG_ITEM_VARIANT_L1) {
            // string with numeric parameter: get localised string and add numeric parameter
            QString l1Value = e.attribute(ATTRIBUTE_LOCALIZATION_ID_L1_VALUE);
            string = hbTrId(string.toAscii().constData()).arg(l1Value.toInt());
            CX_DEBUG(("tag name <%s>", e.tagName().toAscii().constData()));
            CX_DEBUG(("attribute [%s] value[%s]", ATTRIBUTE_LOCALIZATION_ID_L1_VALUE,
                     string.toAscii().constData()));
        } else if (e.tagName() == TAG_ITEM_VARIANT_LN) {
            QString lnValue = e.attribute(ATTRIBUTE_LOCALIZATION_ID_LN_VALUE);
            // string with count parameter: get localised string and add plural form numeric parameter
            string = HbParameterLengthLimiter(string.toAscii().constData(), lnValue.toInt());
            CX_DEBUG(("tag name <%s>", e.tagName().toAscii().constData()));
            CX_DEBUG(("attribute [%s] value[%s]", ATTRIBUTE_LOCALIZATION_ID_LN_VALUE, string.toAscii().constData()));
        }

        if (!string.isNull()) {
            CxUiSettings::SettingItem setting;
            setting.mItem = string;
            setting.mValue = value;
            setting.mIcon = e.attribute(ATTRIBUTE_ICON);
            p->mSettingPairList.append(setting);
        }
    }

    CX_DEBUG_EXIT_FUNCTION();
    return p;
}



/*!
* Creates the slider settings based on the DomElement
@param element specifies the dom element
returns slider parameters
*/
CxUiSettings::SliderParams *CxuiSettingXmlReader::parseSettingSlider(const QDomElement& element)
{
    const double SLIDER_ZERO = 0.0;

    CX_DEBUG_ENTER_FUNCTION();

    // Read the heading and id.
    if (element.isNull()) {
        return NULL;
    }

    SliderParams *p = new SliderParams();

    CX_DEBUG(("Parsing slider for setting id [%s]", element.attribute(ATTRIBUTE_ID).toAscii().constData()));

    // XML something like this
    // <setting_list id="WHITE_BALANCE" heading="qtn_setting_title_wb">
    p->mHeading = hbTrId(element.attribute(ATTRIBUTE_HEADING).toAscii().constData());
    p->mSettingId = element.attribute(ATTRIBUTE_ID);
    p->mHeadingIcon = element.attribute(ATTRIBUTE_HEADING_ICON);

    QDomNode n = element.firstChild();
    QDomElement e = n.toElement();

    p->mSettingStrings.clear();

    CX_DEBUG(("tag name <%s>", e.tagName().toAscii().constData()));

    if (e.tagName() == TAG_SETUP) {
        // get the min text id from the xml
        QString minString = e.attribute(ATTRIBUTE_SLIDER_MIN);

        // get the max text id from the xml
        QString maxString = e.attribute(ATTRIBUTE_SLIDER_MAX);

        // get the step text id from the xml
        QVariant stepString = e.attribute(ATTRIBUTE_SLIDER_MINOR_STEP);
        p->mMinorStep = stepString.toReal();
        stepString = e.attribute(ATTRIBUTE_SLIDER_MAJOR_STEP);
        p->mMajorStep = stepString.toReal();

        // get the l1 value from xml to generate strings for slider
        QString l1Value = e.attribute(ATTRIBUTE_LOCALIZATION_ID_L1_VALUE);

        // Don't create labels for slider if no strings are defined in settinsg xml
        // Create labels only for major ticks
        if (!minString.isEmpty() && !maxString.isEmpty()) {
            qreal step = p->mMajorStep;
            qreal value = l1Value.toDouble();
            // generating all negative valued strings for slider setting
            while (value > 0) {
                // format the setting string
                QString str =  hbTrId(minString.toAscii().constData()).arg(value,0,'f',1);
                p->mSettingStrings.append(str);
                value = value - step;
            }

            HbExtendedLocale locale = HbExtendedLocale::system();
            QString zeroString = locale.toString(SLIDER_ZERO, 'f', 1);
            p->mSettingStrings.append(zeroString);

            // generating all non-negative valued strings for slider setting
            step = p->mMajorStep;
            value = step;
            while(value <= l1Value.toInt()) {
                // format the setting string
                QString str = hbTrId(maxString.toAscii().constData()).arg(value,0,'f',1);
                p->mSettingStrings.append(str);
                value = value + step;
            }
        }

        int min = -l1Value.toInt();
        int max = l1Value.toInt();

        p->mRange = qMakePair(min, max);
    }

    CX_DEBUG_EXIT_FUNCTION();
    return p;
}



/*!
* CxuiSettingXmlReader::findChildElement
*/
QDomElement CxuiSettingXmlReader::findChildElement(const QDomNode& parent, const QString& name) {
    CX_DEBUG(("findChildElement name[%s]", name.toAscii().constData()));

    for (QDomNode n = parent.firstChild(); !n.isNull(); n = n.nextSibling()) {
        QDomElement e = n.toElement();
        if (e.tagName() == name) {
            return e;
        }
    }

    return QDomElement();
}
