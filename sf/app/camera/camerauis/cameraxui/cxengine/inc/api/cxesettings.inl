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


// specializations for getters
// throwing versions
template<>
inline int CxeSettings::get<int>(const QString &key) const {
   QVariant v;
   getValue(key, v);
   return v.value<int>();
}

template<>
inline bool CxeSettings::get<bool>(const QString &key) const {
   QVariant v;
   getValue(key, v);
   return v.value<bool>();
}

template<>
inline qreal CxeSettings::get<qreal>(const QString &key) const {
   QVariant v;
   getValue(key, v);
   return v.value<qreal>();
}

template<>
inline QString CxeSettings::get<QString>(const QString &key) const {
   QVariant v;
   getValue(key, v);
   return v.value<QString>();
}

template<>
inline QVariantMap CxeSettings::get<QVariantMap>(const QString &key) const {
   QVariant v;
   getValue(key, v);
   return v.value<QVariantMap>();
}

// non-throwing versions
template<>
inline int CxeSettings::get<int>(const QString &key, const int &defaultValue) const {
    try {
        return get<int>(key);
    } catch (CxeException &e) {
        return defaultValue;
    }
}

template<>
inline bool CxeSettings::get<bool>(const QString &key, const bool &defaultValue) const {
    try {
        return get<bool>(key);
    } catch (CxeException &e) {
        return defaultValue;
    }
}

template<>
inline qreal CxeSettings::get<qreal>(const QString &key, const qreal &defaultValue) const {
    try {
        return get<qreal>(key);
    } catch (CxeException &e) {
        return defaultValue;
    }
}

template<>
inline QString CxeSettings::get<QString>(const QString &key, const QString &defaultValue) const {
    try {
        return get<QString>(key);
    } catch (CxeException &e) {
        return defaultValue;
    }
}

template<>
inline QVariantMap CxeSettings::get<QVariantMap>(const QString &key, const QVariantMap &defaultValue) const {
    try {
        return get<QVariantMap>(key);
    } catch (CxeException &e) {
        return defaultValue;
    }
}

// specializations for setters
template<>
inline void CxeSettings::set<int>(const QString &key, const int &value) {
   QVariant v;
   v.setValue(value);
   setValue(key, v);
}

template<>
inline void CxeSettings::set<bool>(const QString &key, const bool &value) {
   QVariant v;
   v.setValue(value);
   setValue(key, v);
}

template<>
inline void CxeSettings::set<qreal>(const QString &key, const qreal &value) {
   QVariant v;
   v.setValue(value);
   setValue(key, v);
}

template<>
inline void CxeSettings::set<QString>(const QString &key, const QString &value) {
   QVariant v;
   v.setValue(value);
   setValue(key, v);
}

template<>
inline void CxeSettings::set<QVariantMap>(const QString &key, const QVariantMap &value) {
   QVariant v;
   v.setValue(value);
   setValue(key, v);
}
