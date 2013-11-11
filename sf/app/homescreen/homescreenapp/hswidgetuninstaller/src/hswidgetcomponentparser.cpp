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



#include "hswidgetcomponentparser.h"
#include <QFile>

const char WIDGETPROVIDER[] = "widgetprovider";
const char WIDGET[] = "widget";
const char WIDGETMANIFEST[] = "hswidgetmanifest";

const char URI[] = "uri";
const char TITLE[] = "title";
const char ICONURI[] = "iconuri";
const char ICON[] = "icon";
const char DESCRIPTION[] = "description";
const char HIDDEN[] = "hidden";
const char SERVICEXML[] = "servicexml";
const char TRANSLATIONFILE[] = "translationfile";
const char VERSION[] = "version";
const char VERSION_1_0[] = "1.0";

/*!
    \class HsWidgetComponentParser
    \ingroup group_hswidgetuninstaller
    \brief Parses widget manifest xml file.
*/


/*!
    Constructor, \a aComponentManifest path to file \a parent.
*/
HsWidgetComponentParser::HsWidgetComponentParser(const QString &aComponentManifest, QObject *parent)
    : QObject(parent),mError(false)
{
    parseFile(aComponentManifest);
}

/*!
    Destructor.
*/
HsWidgetComponentParser::~HsWidgetComponentParser()
{

}

/*!
    Returns parsed widget component descriptor.
*/
HsWidgetComponentDescriptor HsWidgetComponentParser::widgetComponentDescriptor()
{
    return mComponentDescriptor;
}

/*!
    Returns true if error occurred in parsing.
*/
bool HsWidgetComponentParser::error()
{
    return mError;
}

/*!
    Parse widget component descriptor from \a aComponentManifest file.
*/
void HsWidgetComponentParser::parseFile(const QString &aComponentManifest)
{
    QFile file(aComponentManifest);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        mError = true;
        return;
    }
    mParser.setDevice(&file);
    if (mParser.readNextStartElement()) {
        if (mParser.name() == WIDGETMANIFEST){
             parseWidgetManifestTag();
        } else if(mParser.name() == WIDGETPROVIDER){
            parseWidgetProviderTag();
        } else {
            mParser.raiseError(QObject::tr("Malformed manifest"));
        }
    } else {
        mParser.raiseError(QObject::tr("Malformed manifest"));
    }
    mError = mParser.error();

}

/*!
    Parse manifest tag.
*/
void HsWidgetComponentParser::parseWidgetManifestTag()
{
    Q_ASSERT(mParser.isStartElement() && mParser.name() == WIDGETMANIFEST);
    
    /* Detect version */
    if (mParser.attributes().hasAttribute(VERSION)) {
        QStringRef version = mParser.attributes().value(VERSION);
        mComponentDescriptor.setVersion(version.toString());
        if (version == VERSION_1_0) { 
            parseVersion1_0();
        } else {
            mParser.raiseError(QObject::tr("Unsupported version"));
        }
        
    } else {
        // newest is default
        parseVersion1_0();
        mComponentDescriptor.setVersion(VERSION_1_0);
    }
}

/*!
    Parse provider tag.
*/
void HsWidgetComponentParser::parseWidgetProviderTag()
{
    Q_ASSERT(mParser.isStartElement() && mParser.name() == WIDGETPROVIDER);
    if (!mParser.readNextStartElement() || mParser.name() != WIDGET) {
        mParser.raiseError(QObject::tr("Malformed manifest"));
        return;
    }
   
    // everything is in attributes
    QXmlStreamAttributes attrs = mParser.attributes();
    foreach (QXmlStreamAttribute attr, attrs) {
        QStringRef name = attr.name();
        if (name == URI) {
            mComponentDescriptor.setUri(attr.value().toString());
        } else if (name == ICONURI) {
            mComponentDescriptor.setIconUri(attr.value().toString());   
        } else if (name == TITLE) {
            mComponentDescriptor.setTitle(attr.value().toString());    
        } else if (name == DESCRIPTION) {
            mComponentDescriptor.setDescription(attr.value().toString());
        } else if (name == HIDDEN) {
            mComponentDescriptor.setHidden(attr.value().toString());
        } 
    }

}
/*!
    Parse version 1.0.
*/
void HsWidgetComponentParser::parseVersion1_0()
{
    Q_ASSERT(mParser.isStartElement() && mParser.name() == WIDGETMANIFEST);
    /* We'll parse the XML until we reach </hswidgetmanifest> tag, end of cml or have an error. */
	while( !(mParser.tokenType() == QXmlStreamReader::EndElement && mParser.name() == WIDGETMANIFEST) && 
        !mParser.hasError() && !mParser.atEnd()) {
        
        mParser.readNext();
        if (mParser.tokenType() != QXmlStreamReader::StartElement) {
            continue;
        }
		/* Read next start element.*/
	    QStringRef tag = mParser.name();
		if(tag == URI) {
			parseUri();
        } else if (tag == ICON) {
            parseIcon();
        } else if (tag == TITLE) {
            parseTitle();
        } else if (tag == DESCRIPTION) {
            parseDescription();
        } else if (tag == HIDDEN) {
            parseHidden();
        } else if (tag == SERVICEXML) {
            parseServiceXml();
        } else if (tag == TRANSLATIONFILE) {
            parseTranslationFilename();
        }        
	}
}


/*!
    Parse uri.
*/
void HsWidgetComponentParser::parseUri()
{
    mParser.readNext();
    if(mParser.tokenType() != QXmlStreamReader::Characters) {
		return;
	}
    mComponentDescriptor.setUri(mParser.text().toString());
}

/*!
    Parse icon.
*/
void HsWidgetComponentParser::parseIcon()
{
    mParser.readNext();
    if(mParser.tokenType() != QXmlStreamReader::Characters) {
		return;
	}
    mComponentDescriptor.setIconUri(mParser.text().toString());
}

/*!
    Parse title.
*/
void HsWidgetComponentParser::parseTitle()
{
    mParser.readNext();
    if(mParser.tokenType() != QXmlStreamReader::Characters) {
		return;
	}
    mComponentDescriptor.setTitle(mParser.text().toString());
}

/*!
    Parse description.
*/
void HsWidgetComponentParser::parseDescription()
{
    mParser.readNext();
    if(mParser.tokenType() != QXmlStreamReader::Characters) {
		return;
	}
    mComponentDescriptor.setDescription(mParser.text().toString());
}

/*!
    Parse hidden.
*/
void HsWidgetComponentParser::parseHidden()
{
    mParser.readNext();
    if(mParser.tokenType() != QXmlStreamReader::Characters) {
		return;
	}
    mComponentDescriptor.setHidden(mParser.text().toString());
}

/*!
    Parse service xml.
*/
void HsWidgetComponentParser::parseServiceXml()
{
    mParser.readNext();
    if(mParser.tokenType() != QXmlStreamReader::Characters) {
		return;
	}
    mComponentDescriptor.setServiceXml(mParser.text().toString());
}
/*!
    Parse service translation file name.
*/
void HsWidgetComponentParser::parseTranslationFilename()
{
    mParser.readNext();
    if(mParser.tokenType() != QXmlStreamReader::Characters) {
		return;
	}
    mComponentDescriptor.setTranslationFilename(mParser.text().toString());
}
