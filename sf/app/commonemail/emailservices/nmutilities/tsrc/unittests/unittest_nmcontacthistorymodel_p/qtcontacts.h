#ifndef QTCONTACTS_H
#define QTCONTACTS_H

#include <QSharedPointer>
#include <QVariant>

extern bool setDetailDefinitionNameCalled;
extern bool setMatchFlagsCalled;
extern bool setValueCalled;
extern bool ContactIdsCalled;
extern bool returnZeroEmails;

typedef quint32 QContactLocalId;
/*
class UnitTestReporter
{
public:
  inline static void resetFlags()
  {
    //mFlagA = false;
    UnitTestReporter::mFlagB = false;
  	//mFlagC = false;
  	UnitTestReporter::mFlagD = false;
  	//mVariant1.clear();
  	//mVariant2.clear();
  }

  inline static void setFlagA()
  {
  	mFlagA = true;
  }
	inline static void setFlagB()
	{
		mFlagB = true;
	}

	inline static void setFlagC()
	{
		mFlagC = true;
	}

	inline static void setFlagD()
	{
		mFlagD = true;
	}

	inline static void setVariant1(QVariant &variant)
	{
		mVariant1 = variant;
	}

	inline static void setVariant2(QVariant &variant)
	{
		mVariant2 = variant;
	}

	static bool mFlagA;
	static bool mFlagB;
	static bool mFlagC;
	static bool mFlagD;
	static QVariant mVariant1;
	static QVariant mVariant2;
};
*/

template <int N> struct QLatin1Constant
{
    char chars[N];

    bool operator ==(const QLatin1Constant& other) const {return (chars == other.chars) || (qstrcmp(chars, other.chars) == 0);}
    bool operator !=(const QLatin1Constant& other) const {return !operator==(other);}

    inline const char * latin1() const {return chars;}

    operator QLatin1String() const {return QLatin1String(chars);}
    operator QString() const {return QString::fromLatin1(chars, N-1);}
    operator QVariant() const {return QVariant(operator QString());}
};

#define QTM_USE_NAMESPACE
#endif

// Stub for ContactManager
#ifndef QCONTACTMANAGER_H
#define QCONTACTMANAGER_H

#include <QObject>
#include <QMap>
#include <QString>
#include <QStringList>
#include <QList>
#include <QDateTime>
#include <QGlobal.h>

class QContactSortOrder
{
public:
    inline QContactSortOrder()
    {
    }

    inline ~QContactSortOrder()
    {
    }

    inline QContactSortOrder(const QContactSortOrder& other)
    {
    	Q_UNUSED(other);
    }

    inline QContactSortOrder& operator=(const QContactSortOrder& other)
    {
    	Q_UNUSED(other);
    	return (const_cast<QContactSortOrder &>(other));
    }

    enum BlankPolicy {
        BlanksFirst,
        BlanksLast,
    };

    /* Mutators */
//    void setDetailDefinitionName(const QString& definitionName, const QString& fieldName);
//    void setBlankPolicy(BlankPolicy blankPolicy);
//    void setDirection(Qt::SortOrder direction);
//    void setCaseSensitivity(Qt::CaseSensitivity sensitivity);

    /* Accessors */
//    QString detailDefinitionName() const;
//    QString detailFieldName() const;
//    BlankPolicy blankPolicy() const;
//    Qt::SortOrder direction() const;
//    Qt::CaseSensitivity caseSensitivity() const;

//    bool isValid() const;

    inline bool operator==(const QContactSortOrder& other) const
    {
    	Q_UNUSED(other);
    	return true;
    }

    inline bool operator!=(const QContactSortOrder& other) const {Q_UNUSED(other); return !operator==(other);}
};

#define Q_DECLARE_LATIN1_CONSTANT(varname, str) static const QLatin1Constant<sizeof(str)> varname
#define Q_DEFINE_LATIN1_CONSTANT(varname, str) const QLatin1Constant<sizeof(str)> varname = {str}

#define Q_DECLARE_CUSTOM_CONTACT_DETAIL(className, definitionNameString) \
    className() : QContactDetail(DefinitionName.latin1()) {} \
    className(const QContactDetail& field) : QContactDetail(field, DefinitionName.latin1()) {} \
    className& operator=(const QContactDetail& other) {assign(other, DefinitionName.latin1()); return *this;} \
    \
    Q_DECLARE_LATIN1_CONSTANT(DefinitionName, definitionNameString);

class QContactDetail
{
public:

    inline QContactDetail()
    {
    }

    inline explicit QContactDetail(const char* definitionName)
    {
    	Q_UNUSED(definitionName);
		}

    inline explicit QContactDetail(const QString& definitionName)
    {
    	Q_UNUSED(definitionName);
    }

    inline ~QContactDetail()
    {
    }

    inline QContactDetail(const QContactDetail& other)
    {
    	Q_UNUSED(other);
    }

    inline QContactDetail& operator=(const QContactDetail& other)
    {
   	Q_UNUSED(other);
   	return (const_cast<QContactDetail &>(other));
    }

    enum AccessConstraint {
        NoConstraint = 0,
        ReadOnly = 0x01,
        Irremovable = 0x02
    };

    Q_DECLARE_FLAGS(AccessConstraints, AccessConstraint)

    inline AccessConstraints accessConstraints() const
    {
    	return AccessConstraints();
    }

    // Predefined attribute names and values
#ifdef Q_QDOC
    static const QLatin1Constant FieldContext;
    static const QLatin1Constant ContextHome;
    static const QLatin1Constant ContextWork;
    static const QLatin1Constant ContextOther;
    static const QLatin1Constant FieldDetailUri;
    static const QLatin1Constant FieldLinkedDetailUris;
#else
    Q_DECLARE_LATIN1_CONSTANT(FieldContext, "Context");
    Q_DECLARE_LATIN1_CONSTANT(ContextHome, "Home");
    Q_DECLARE_LATIN1_CONSTANT(ContextWork, "Work");
    Q_DECLARE_LATIN1_CONSTANT(ContextOther, "Other");
    Q_DECLARE_LATIN1_CONSTANT(FieldDetailUri, "DetailUri");
    Q_DECLARE_LATIN1_CONSTANT(FieldLinkedDetailUris, "LinkedDetailUris");
#endif

    bool operator==(const QContactDetail& other) const;
    bool operator!=(const QContactDetail& other) const {return !(other == *this);}

    inline QString definitionName() const
    {
    	return QString();
    }

    inline bool isEmpty() const
    {
    	return false;
    }

    int key() const;
    void resetKey();

		inline QString value(const QString& key) const
		{
			Q_UNUSED(key);
			return QString();
		}

		inline bool setValue(const QString& key, const QVariant& value)
		{
			Q_UNUSED(key);
			Q_UNUSED(value);
			return true;
		}

    bool removeValue(const QString& key);
    bool hasValue(const QString& key) const;

    QVariantMap variantValues() const;
    QVariant variantValue(const QString& key) const;
    template <typename T> T value(const QString& key) const
    {
        return variantValue(key).value<T>();
    }

    /* These are probably internal */
    inline QString value(const char* key) const
    {
        return key;
    }

    inline bool setValue(const char* key, const QVariant& value)
    {
        Q_UNUSED(key);
        Q_UNUSED(value);
        return true;
    }

    inline bool removeValue(const char* key)
    {
        Q_UNUSED(key);
        return true;
    }

    inline bool hasValue(const char* key) const
    {
        Q_UNUSED(key);
        return true;
    }

    QVariant variantValue(const char *key) const;

    template<typename T> T value(const char *key) const
    {
        return variantValue(key).value<T>();
    }

#ifdef Q_QDOC
    QString value(const QLatin1Constant& key) const;
    bool setValue(const QLatin1Constant& key, const QVariant& value);
    bool removeValue(const QLatin1Constant& key);
    bool hasValue(const QLatin1Constant& key) const;
    QVariant variantValue(const QLatin1Constant& key) const;
    T value(const QLatin1Constant& key) const;
#else

    template<int N> QString value(const QLatin1Constant<N>& key) const
    {
         return value(key.latin1());
    }
    template<int N> bool setValue(const QLatin1Constant<N>& key, const QVariant& value)
    {
        return setValue(key.latin1(), value);
    }
    template<int N> bool removeValue(const QLatin1Constant<N>& key)
    {
        return removeValue(key.latin1());
    }
    template<int N> bool hasValue(const QLatin1Constant<N>& key) const
    {
        return hasValue(key.latin1());
    }
    template<int N> QVariant variantValue(const QLatin1Constant<N>& key) const
    {
        return variantValue(key.latin1());
    }
    template<typename T, int N> T value(const QLatin1Constant<N>& key) const
    {
        return value<T>(key.latin1());
    }
#endif

    inline void setContexts(const QStringList& contexts)
    {
        setValue(FieldContext, contexts);
    }

    inline void setContexts(const QString& context)
    {
        setValue(FieldContext, QStringList(context));
    }

    inline QStringList contexts() const
    {
        return value<QStringList>(FieldContext);
    }

    inline void setDetailUri(const QString& detailUri)
    {
        setValue(FieldDetailUri, detailUri);
    }

    inline QString detailUri() const
    {
        return value(FieldDetailUri);
    }

    inline void setLinkedDetailUris(const QStringList& linkedDetailUris)
    {
        setValue(FieldLinkedDetailUris, linkedDetailUris);
    }

    inline void setLinkedDetailUris(const QString& linkedDetailUri)
    {
        setValue(FieldLinkedDetailUris, QStringList(linkedDetailUri));
    }

    inline QStringList linkedDetailUris() const
    {
        return value<QStringList>(FieldLinkedDetailUris);
    }

// Protected..

    inline QContactDetail(const QContactDetail &other, const QString& expectedDefinitionId)
    {
    	Q_UNUSED(other);
    	Q_UNUSED(expectedDefinitionId);
    }

    inline QContactDetail& assign(const QContactDetail &other, const QString& expectedDefinitionId)
    {
    	Q_UNUSED(other);
    	Q_UNUSED(expectedDefinitionId);
    	return (const_cast<QContactDetail &>(other));
    }

    inline QContactDetail(const QContactDetail &other, const char* expectedDefinitionId)
    {
    	Q_UNUSED(other);
    	Q_UNUSED(expectedDefinitionId);
    }

    inline QContactDetail& assign(const QContactDetail &other, const char* expectedDefinitionId)
    {
    	Q_UNUSED(other);
    	Q_UNUSED(expectedDefinitionId);
    	return (const_cast<QContactDetail &>(other));
    }
};

// Stub for QContactFilter
class QContactFilter
{
public:
    inline QContactFilter()
    {
    }

    inline ~QContactFilter()
    {
    }

    inline QContactFilter(const QContactFilter& other)
    {
    	Q_UNUSED(other);
  	}

    inline QContactFilter& operator=(const QContactFilter& other)
    {
    	Q_UNUSED(other);
			return (const_cast<QContactFilter &>(other));
    }

    enum FilterType {
        InvalidFilter,
        ContactDetailFilter,
        ContactDetailRangeFilter,
        ChangeLogFilter,
        ActionFilter,
        RelationshipFilter,
        IntersectionFilter,
        UnionFilter,
        LocalIdFilter,
        DefaultFilter
    };

    inline FilterType type() const
    {
    	return FilterType();
  	}

    // Qt::MatchFlags don't quite match here
    enum MatchFlag {
        MatchExactly = Qt::MatchExactly,    // 0
        MatchContains = Qt::MatchContains,  // 1
        MatchStartsWith = Qt::MatchStartsWith,  // 2
        MatchEndsWith = Qt::MatchEndsWith, // 3
        MatchFixedString = Qt::MatchFixedString, // 8
        MatchCaseSensitive = Qt::MatchCaseSensitive, // 16
        MatchPhoneNumber = 1024,
        MatchKeypadCollation = 2048
    };
    Q_DECLARE_FLAGS(MatchFlags, MatchFlag)
};

class QContactEmailAddress : public QContactDetail
{
public:

#ifdef Q_QDOC
    static const QLatin1Constant DefinitionName;
    static const QLatin1Constant FieldEmailAddress;
#else
    Q_DECLARE_CUSTOM_CONTACT_DETAIL(QContactEmailAddress, "EmailAddress")
    Q_DECLARE_LATIN1_CONSTANT(FieldEmailAddress, "EmailAddress");
#endif

    void setEmailAddress(const QString& emailAddress) {Q_UNUSED(emailAddress);}
    QString emailAddress() const {return value(FieldEmailAddress);}

    static QContactFilter match(const QString& emailAddress)
		{
			Q_UNUSED(emailAddress);
			return QContactFilter();
		}
};

class QContactName : public QContactDetail
{
public:

#ifdef Q_QDOC
    //static const QLatin1Constant DefinitionName;
    static const QLatin1Constant FieldPrefix;
    static const QLatin1Constant FieldFirstName;
    static const QLatin1Constant FieldMiddleName;
    static const QLatin1Constant FieldLastName;
    static const QLatin1Constant FieldSuffix;
    static const QLatin1Constant FieldCustomLabel;
#else
    Q_DECLARE_CUSTOM_CONTACT_DETAIL(QContactName, "Name")
    Q_DECLARE_LATIN1_CONSTANT(FieldPrefix, "Prefix");
    Q_DECLARE_LATIN1_CONSTANT(FieldFirstName, "FirstName");
    Q_DECLARE_LATIN1_CONSTANT(FieldMiddleName, "MiddleName");
    Q_DECLARE_LATIN1_CONSTANT(FieldLastName, "LastName");
    Q_DECLARE_LATIN1_CONSTANT(FieldSuffix, "Suffix");
    Q_DECLARE_LATIN1_CONSTANT(FieldCustomLabel, "CustomLabel");
#endif
    //static const QString DefinitionName;

    QString prefix() const {return value(FieldPrefix);}
    QString firstName() const {return value(FieldFirstName);}
    QString middleName() const {return value(FieldMiddleName);}
    QString lastName() const {return value(FieldLastName);}
    QString suffix() const {return value(FieldSuffix);}
    QString customLabel() const{return value(FieldCustomLabel);}

    void setPrefix(const QString& prefix) {setValue(FieldPrefix, prefix);}
    void setFirstName(const QString& firstName) {setValue(FieldFirstName, firstName);}
    void setMiddleName(const QString& middleName) {setValue(FieldMiddleName, middleName);}
    void setLastName(const QString& lastName) {setValue(FieldLastName, lastName);}
    void setSuffix(const QString& suffix) {setValue(FieldSuffix, suffix);}
    void setCustomLabel(const QString& customLabel) {setValue(FieldCustomLabel, customLabel);}

    // Convenience filter
    static QContactFilter match(const QString& name);
    static QContactFilter match(const QString& firstName, const QString& lastName);
};

// Stub for QContactDetailFilter
class QContactDetailFilter : public QContactFilter
{
public:
    inline QContactDetailFilter()
    {
    }

	  inline QContactDetailFilter(const QContactFilter& other)
	  {
    	Q_UNUSED(other);
	  }

    inline void setDetailDefinitionName(const QString& definition, const QString& fieldName = QString())
    {
    	Q_UNUSED(definition);
    	Q_UNUSED(fieldName);
    	setDetailDefinitionNameCalled = true;
    }

    inline void setMatchFlags(QContactFilter::MatchFlags flags)
    {
    	Q_UNUSED(flags);
    	setMatchFlagsCalled = true;
    }

    inline void setValue(const QVariant& value)
    {
			Q_UNUSED(value);
			setValueCalled = true;
    }
};

// Stub for QContactFetchHint
class QContactFetchHint {
public:

    inline QContactFetchHint()
    {
    }

    inline QContactFetchHint(const QContactFetchHint& other)
    {
    	Q_UNUSED(other);
    }

    inline ~QContactFetchHint()
    {
    }

};

class QContact
{
public:
    inline QContact()
    {
    }

    inline ~QContact()
    {
    }

    QContact(const QContact& other);
    QContact& operator=(const QContact& other);

    bool operator==(const QContact &other) const;
    bool operator!=(const QContact &other) const {return !(other == *this);}

    /* Access details of particular type or which support a particular action */
    QContactDetail detail(const QString& definitionId) const;
    QList<QContactDetail> details(const QString& definitionId = QString()) const;

    QList<QContactDetail> details(const QString& definitionName, const QString& fieldName, const QString& value) const;

    inline QContactDetail detail(const char* definitionId) const
    {
        Q_UNUSED(definitionId);
        QContactDetail detail;

        detail.setValue(definitionId, definitionId);

        return detail;
    }

    inline QList<QContactDetail> details(const char* definitionId) const
    {
        QList<QContactDetail> tmpList;
        QContactDetail detail;

        if(!returnZeroEmails)
        {
            detail.setValue(definitionId, definitionId);
            tmpList.append(detail);
        }

        return tmpList;
    }

    QList<QContactDetail> details(const char* definitionId, const char* fieldName, const QString& value) const;

    /* Templated retrieval for definition names */
#ifdef Q_QDOC
    QContactDetail detail(const QLatin1Constant& definitionName) const;
    QList<QContactDetail> details(const QLatin1Constant& definitionName) const;
    QList<QContactDetail> details(const QLatin1Constant& definitionName, const QLatin1Constant& fieldName, const QString& value);
#else
    template <int N> QContactDetail detail(const QLatin1Constant<N>& definitionName) const
    {
        return detail(definitionName.latin1());
    }
    template <int N> QList<QContactDetail> details(const QLatin1Constant<N>& definitionName) const
    {
        return details(definitionName.latin1());
    }
    template <int N, int M> QList<QContactDetail> details(const QLatin1Constant<N>& definitionName, const QLatin1Constant<M>& fieldName, const QString& value)
    {
        return details(definitionName.latin1(), fieldName.latin1(), value);
    }
#endif

    /* Templated (type-specific) detail retrieval */
    template<typename T> QList<T> details() const
    {
        QList<QContactDetail> props = details(T::DefinitionName.latin1());
        QList<T> ret;
        foreach(QContactDetail prop, props)
            ret.append(T(prop));
        return ret;
    }

    /* Templated (type-specific) detail retrieval base on given detail field name and field value */
    template<typename T> QList<T> details(const QString& fieldName, const QString& value) const
    {
        QList<QContactDetail> props = details(T::DefinitionName, fieldName, value);
        QList<T> ret;
        foreach(QContactDetail prop, props)
            ret.append(T(prop));
        return ret;
    }

    template<typename T> QList<T> details(const char* fieldName, const QString& value) const
    {
        QList<QContactDetail> props = details(T::DefinitionName.latin1(), fieldName, value);
        QList<T> ret;
        foreach(QContactDetail prop, props)
            ret.append(T(prop));
        return ret;
    }

    template<typename T> T detail() const
    {
        return T(detail(T::DefinitionName.latin1()));
    }

};

// Stub for QContactManager
//class QContactManager : public QObject
class QContactManager
{
//    Q_OBJECT

public:

#if Q_QDOC // qdoc's parser fails to recognise the default map argument

    inline explicit QContactManager(const QString& managerName = QString(), const QMap<QString, QString>& parameters = 0, QObject* parent = 0)
    {
			Q_UNUSED(managerName);
    	Q_UNUSED(parameters;
			Q_UNUSED(parent);
    }

    inline QContactManager(const QString& managerName, int implementationVersion, const QMap<QString, QString>& parameters = 0, QObject* parent = 0)
    {
			Q_UNUSED(managerName);
			Q_UNUSED(implementationVersion);
    	Q_UNUSED(parameters);
			Q_UNUSED(parent);
  	}

#else

     inline explicit QContactManager(const QString& managerName = QString(), const QMap<QString, QString>& parameters = (QMap<QString, QString>()), QObject* parent = 0)
    {
			Q_UNUSED(managerName);
    	Q_UNUSED(parameters);
			Q_UNUSED(parent);
    }

    inline QContactManager(const QString& managerName, int implementationVersion, const QMap<QString, QString>& parameters = (QMap<QString, QString>()), QObject* parent = 0)
    {
    	Q_UNUSED(managerName);
			Q_UNUSED(implementationVersion);
    	Q_UNUSED(parameters);
			Q_UNUSED(parent);
    }

#endif
//    explicit QContactManager(QObject* parent);

//    static QContactManager* fromUri(const QString& uri, QObject* parent = 0);
    inline ~QContactManager()
    {

    }

    /* Contacts - Accessors and Mutators */
//    QList<QContactLocalId> contactIds(const QList<QContactSortOrder>& sortOrders = QList<QContactSortOrder>()) const;

		// QContactLocalId - Check
		// QContactFilter - Check
		// QContactSortOrder - Check
		// QContactFetchHint - Check


    inline QList<QContactLocalId> contactIds(const QContactFilter& filter, const QList<QContactSortOrder>& sortOrders = QList<QContactSortOrder>()) const
    {
    	Q_UNUSED(filter);
    	Q_UNUSED(sortOrders);

    	QList<QContactLocalId> list;
    	list.append(12);
    	list.append(24);
    	list.append(48);

    	ContactIdsCalled = true;

    	return list;
    }

//    QList<QContact> contacts(const QList<QContactSortOrder>& sortOrders = QList<QContactSortOrder>(), const QContactFetchHint& fetchHint = QContactFetchHint()) const;
//    QList<QContact> contacts(const QContactFilter& filter, const QList<QContactSortOrder>& sortOrders = QList<QContactSortOrder>(), const QContactFetchHint& fetchHint = QContactFetchHint()) const;
    inline QContact contact(const QContactLocalId& contactId, const QContactFetchHint& fetchHint = QContactFetchHint()) const  // retrieve a contact
    {
        Q_UNUSED(fetchHint);

        QContact cnt;

        int keu = contactId;

        switch (keu)
            {
            case 12:

            break;

            case 24:

            break;

            case 48:

            break;

            default:
            break;
            }



			return cnt;
    }



};

//QTM_END_NAMESPACE

#endif
