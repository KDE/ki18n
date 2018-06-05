/*  This file is part of the KDE libraries
    Copyright (C) 2007 Chusslove Illich <caslav.ilic@gmx.net>
    Copyright (C) 2014 Kevin Krammer <krammer@kde.org>

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Library General Public
    License as published by the Free Software Foundation; either
    version 2 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Library General Public License for more details.

    You should have received a copy of the GNU Library General Public License
    along with this library; see the file COPYING.LIB.  If not, write to
    the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
    Boston, MA 02110-1301, USA.
*/

#include <ktranscript_p.h>
#include <common_helpers_p.h>

#include <ktranscript_export.h>

//#include <unistd.h>

#include <QScriptable>
#include <QScriptEngine>

#include <QDebug>
#include <QVariant>
#include <QStringList>
#include <QList>
#include <QDir>
#include <QHash>
#include <QPair>
#include <QSet>
#include <QFile>
#include <QIODevice>
#include <QTextStream>
#include <QRegExp>
#include <QStandardPaths>
#include <qendian.h>

class KTranscriptImp;
class Scriptface;

typedef QHash<QString, QString> TsConfigGroup;
typedef QHash<QString, TsConfigGroup> TsConfig;

// Transcript implementation (used as singleton).
class KTranscriptImp : public KTranscript
{
public:

    KTranscriptImp();
    ~KTranscriptImp();

    QString eval(const QList<QVariant> &argv,
                 const QString &lang,
                 const QString &ctry,
                 const QString &msgctxt,
                 const QHash<QString, QString> &dynctxt,
                 const QString &msgid,
                 const QStringList &subs,
                 const QList<QVariant> &vals,
                 const QString &ftrans,
                 QList<QStringList> &mods,
                 QString &error,
                 bool &fallback) override;

    QStringList postCalls(const QString &lang) override;

    // Lexical path of the module for the executing code.
    QString currentModulePath;

private:

    void loadModules(const QList<QStringList> &mods, QString &error);
    void setupInterpreter(const QString &lang);

    TsConfig config;

    QHash<QString, Scriptface *> m_sface;
};

// Script-side transcript interface.
class Scriptface : public QObject, public QScriptable
{
    Q_OBJECT
public:
    explicit Scriptface(const TsConfigGroup &config, QObject *parent = nullptr);
    ~Scriptface();

    // Interface functions.
    Q_INVOKABLE QScriptValue load(); // actually has variable length argument list
    Q_INVOKABLE QScriptValue setcall(const QScriptValue &name, const QScriptValue &func,
                                     const QScriptValue &fval = QScriptValue::NullValue);
    Q_INVOKABLE QScriptValue hascall(const QScriptValue &name);
    Q_INVOKABLE QScriptValue acall();  // actually has variable length argument list
    Q_INVOKABLE QScriptValue setcallForall(const QScriptValue &name, const QScriptValue &func,
                                           const QScriptValue &fval = QScriptValue::NullValue);
    Q_INVOKABLE QScriptValue fallback();
    Q_INVOKABLE QScriptValue nsubs();
    Q_INVOKABLE QScriptValue subs(const QScriptValue &index);
    Q_INVOKABLE QScriptValue vals(const QScriptValue &index);
    Q_INVOKABLE QScriptValue msgctxt();
    Q_INVOKABLE QScriptValue dynctxt(const QScriptValue &key);
    Q_INVOKABLE QScriptValue msgid();
    Q_INVOKABLE QScriptValue msgkey();
    Q_INVOKABLE QScriptValue msgstrf();
    Q_INVOKABLE QScriptValue dbgputs(const QScriptValue &str);
    Q_INVOKABLE QScriptValue warnputs(const QScriptValue &str);
    Q_INVOKABLE QScriptValue localeCountry();
    Q_INVOKABLE QScriptValue normKey(const QScriptValue &phrase);
    Q_INVOKABLE QScriptValue loadProps();  // actually has variable length argument list
    Q_INVOKABLE QScriptValue getProp(const QScriptValue &phrase, const QScriptValue &prop);
    Q_INVOKABLE QScriptValue setProp(const QScriptValue &phrase, const QScriptValue &prop, const QScriptValue &value);
    Q_INVOKABLE QScriptValue toUpperFirst(const QScriptValue &str, const QScriptValue &nalt = QScriptValue::NullValue);
    Q_INVOKABLE QScriptValue toLowerFirst(const QScriptValue &str, const QScriptValue &nalt = QScriptValue::NullValue);
    Q_INVOKABLE QScriptValue getConfString(const QScriptValue &key, const QScriptValue &dval = QScriptValue::NullValue);
    Q_INVOKABLE QScriptValue getConfBool(const QScriptValue &key, const QScriptValue &dval = QScriptValue::NullValue);
    Q_INVOKABLE QScriptValue getConfNumber(const QScriptValue &key, const QScriptValue &dval = QScriptValue::NullValue);

    // Helper methods to interface functions.
    QScriptValue load(const QScriptValueList &fnames);
    QString loadProps_text(const QString &fpath);
    QString loadProps_bin(const QString &fpath);
    QString loadProps_bin_00(const QString &fpath);
    QString loadProps_bin_01(const QString &fpath);

    void put(const QString &propertyName, const QScriptValue &value);

    // Link to its script engine
    QScriptEngine *const scriptEngine;

    // Current message data.
    const QString *msgcontext;
    const QHash<QString, QString> *dyncontext;
    const QString *msgId;
    const QStringList *subList;
    const QList<QVariant> *valList;
    const QString *ftrans;
    const QString *ctry;

    // Fallback request handle.
    bool *fallbackRequest;

    // Function register.
    QHash<QString, QScriptValue> funcs;
    QHash<QString, QScriptValue> fvals;
    QHash<QString, QString> fpaths;

    // Ordering of those functions which execute for all messages.
    QList<QString> nameForalls;

    // Property values per phrase (used by *Prop interface calls).
    // Not QStrings, in order to avoid conversion from UTF-8 when
    // loading compiled maps (less latency on startup).
    QHash<QByteArray, QHash<QByteArray, QByteArray> > phraseProps;
    // Unresolved property values per phrase,
    // containing the pointer to compiled pmap file handle and offset in it.
    QHash<QByteArray, QPair<QFile *, quint64> > phraseUnparsedProps;
    QHash<QByteArray, QByteArray> resolveUnparsedProps(const QByteArray &phrase);
    // Set of loaded pmap files by paths and file handle pointers.
    QSet<QString> loadedPmapPaths;
    QSet<QFile *> loadedPmapHandles;

    // User config.
    TsConfigGroup config;
};

// ----------------------------------------------------------------------
// Custom debug and warning output (kdebug not available)
#define DBGP "KTranscript: "
void dbgout(const char *str)
{
#ifndef NDEBUG
    fprintf(stderr, DBGP"%s\n", str);
#else
    Q_UNUSED(str);
#endif
}
template <typename T1>
void dbgout(const char *str, const T1 &a1)
{
#ifndef NDEBUG
    fprintf(stderr, DBGP"%s\n", QString::fromUtf8(str).arg(a1).toLocal8Bit().data());
#else
    Q_UNUSED(str); Q_UNUSED(a1);
#endif
}
template <typename T1, typename T2>
void dbgout(const char *str, const T1 &a1, const T2 &a2)
{
#ifndef NDEBUG
    fprintf(stderr, DBGP"%s\n", QString::fromUtf8(str).arg(a1).arg(a2).toLocal8Bit().data());
#else
    Q_UNUSED(str); Q_UNUSED(a1); Q_UNUSED(a2);
#endif
}
template <typename T1, typename T2, typename T3>
void dbgout(const char *str, const T1 &a1, const T2 &a2, const T3 &a3)
{
#ifndef NDEBUG
    fprintf(stderr, DBGP"%s\n", QString::fromUtf8(str).arg(a1).arg(a2).arg(a3).toLocal8Bit().data());
#else
    Q_UNUSED(str); Q_UNUSED(a1); Q_UNUSED(a2); Q_UNUSED(a3);
#endif
}

#define WARNP "KTranscript: "
void warnout(const char *str)
{
    fprintf(stderr, WARNP"%s\n", str);
}
template <typename T1>
void warnout(const char *str, const T1 &a1)
{
    fprintf(stderr, WARNP"%s\n", QString::fromUtf8(str).arg(a1).toLocal8Bit().data());
}

// ----------------------------------------------------------------------
// Produces a string out of a script exception.

QString expt2str(QScriptEngine *engine)
{
    const QScriptValue expt = engine->uncaughtException();
    if (expt.isObject()) {
        const QScriptValue message = expt.toObject().property(QStringLiteral("message"));
        if (message.isValid()) {
            return QStringLiteral("Error: %1").arg(message.toString());
        }
    }

    QString strexpt = expt.toString();
    return QStringLiteral("Caught exception: %1").arg(strexpt);
}

// ----------------------------------------------------------------------
// Count number of lines in the string,
// up to and excluding the requested position.
int countLines(const QString &s, int p)
{
    int n = 1;
    int len = s.length();
    for (int i = 0; i < p && i < len; ++i) {
        if (s[i] == QLatin1Char('\n')) {
            ++n;
        }
    }
    return n;
}

// ----------------------------------------------------------------------
// Normalize string key for hash lookups,
QByteArray normKeystr(const QString &raw, bool mayHaveAcc = true)
{
    // NOTE: Regexes should not be used here for performance reasons.
    // This function may potentially be called thousands of times
    // on application startup.

    QString key = raw;

    // Strip all whitespace.
    int len = key.length();
    QString nkey;
    for (int i = 0; i < len; ++i) {
        QChar c = key[i];
        if (!c.isSpace()) {
            nkey.append(c);
        }
    }
    key = nkey;

    // Strip accelerator marker.
    if (mayHaveAcc) {
        key = removeAcceleratorMarker(key);
    }

    // Convert to lower case.
    key = key.toLower();

    return key.toUtf8();
}

// ----------------------------------------------------------------------
// Trim multiline string in a "smart" way:
// Remove leading and trailing whitespace up to and including first
// newline from that side, if there is one; otherwise, don't touch.
QString trimSmart(const QString &raw)
{
    // NOTE: This could be done by a single regex, but is not due to
    // performance reasons.
    // This function may potentially be called thousands of times
    // on application startup.

    int len = raw.length();

    int is = 0;
    while (is < len && raw[is].isSpace() && raw[is] != QLatin1Char('\n')) {
        ++is;
    }
    if (is >= len || raw[is] != QLatin1Char('\n')) {
        is = -1;
    }

    int ie = len - 1;
    while (ie >= 0 && raw[ie].isSpace() && raw[ie] != QLatin1Char('\n')) {
        --ie;
    }
    if (ie < 0 || raw[ie] != QLatin1Char('\n')) {
        ie = len;
    }

    return raw.mid(is + 1, ie - is - 1);
}

// ----------------------------------------------------------------------
// Produce a JavaScript object out of Qt variant.

QScriptValue variantToJsValue(const QVariant &val)
{
    QVariant::Type vtype = val.type();
    if (vtype == QVariant::String) {
        return QScriptValue(val.toString());
    } else if (vtype == QVariant::Bool) {
        return QScriptValue(val.toBool());
    } else if (vtype == QVariant::Double
               || vtype == QVariant::Int || vtype == QVariant::UInt
               || vtype == QVariant::LongLong || vtype == QVariant::ULongLong) {
        return QScriptValue(val.toDouble());
    } else {
        return QScriptValue::UndefinedValue;
    }
}

// ----------------------------------------------------------------------
// Parse ini-style config file,
// returning content as hash of hashes by group and key.
// Parsing is not fussy, it will read what it can.
TsConfig readConfig(const QString &fname)
{
    TsConfig config;
    // Add empty group.
    TsConfig::iterator configGroup;
    configGroup = config.insert(QString(), TsConfigGroup());

    QFile file(fname);
    if (!file.open(QIODevice::ReadOnly)) {
        return config;
    }
    QTextStream stream(&file);
    stream.setCodec("UTF-8");
    while (!stream.atEnd()) {
        QString line = stream.readLine();
        int p1, p2;

        // Remove comment from the line.
        p1 = line.indexOf(QLatin1Char('#'));
        if (p1 >= 0) {
            line.truncate(p1);
        }
        line = line.trimmed();
        if (line.isEmpty()) {
            continue;
        }

        if (line[0] == QLatin1Char('[')) {
            // Group switch.
            p1 = 0;
            p2 = line.indexOf(QLatin1Char(']'), p1 + 1);
            if (p2 < 0) {
                continue;
            }
            QString group = line.mid(p1 + 1, p2 - p1 - 1).trimmed();
            configGroup = config.find(group);
            if (configGroup == config.end()) {
                // Add new group.
                configGroup = config.insert(group, TsConfigGroup());
            }
        } else {
            // Field.
            p1 = line.indexOf(QLatin1Char('='));
            if (p1 < 0) {
                continue;
            }
            QStringRef field = line.leftRef(p1).trimmed();
            QStringRef value = line.midRef(p1 + 1).trimmed();
            if (!field.isEmpty()) {
                (*configGroup)[field.toString()] = value.toString();
            }
        }
    }
    file.close();

    return config;
}

// ----------------------------------------------------------------------
// throw or log error, depending on context availability
static QScriptValue throwError(QScriptContext *context, QScriptContext::Error errorCode, const QString &message)
{
    if (context) {
        return context->throwError(errorCode, message);
    }

    qCritical() << "Script error" << errorCode << ":" << message;
    return QScriptValue::UndefinedValue;
}

// ----------------------------------------------------------------------
// convert variable number of call arguments to QScriptValueList
static QScriptValueList callArgsFromContext(QScriptContext *context)
{
    QScriptValueList list;

    if (context) {
        list.reserve(context->argumentCount());
        for (int i = 0; i < context->argumentCount(); ++i) {
            list << context->argument(i);
        }
    }

    return list;
}


#ifdef KTRANSCRIPT_TESTBUILD

// ----------------------------------------------------------------------
// Test build creation/destruction hooks
static KTranscriptImp *s_transcriptInstance = nullptr;

KTranscriptImp *globalKTI()
{
    return s_transcriptInstance;
}

KTranscript *autotestCreateKTranscriptImp()
{
    Q_ASSERT(s_transcriptInstance == nullptr);
    s_transcriptInstance = new KTranscriptImp;
    return s_transcriptInstance;
}

void autotestDestroyKTranscriptImp()
{
    Q_ASSERT(s_transcriptInstance != nullptr);
    delete s_transcriptInstance;
    s_transcriptInstance = nullptr;
}

#else

// ----------------------------------------------------------------------
// Dynamic loading.
Q_GLOBAL_STATIC(KTranscriptImp, globalKTI)
extern "C"
{
    KTRANSCRIPT_EXPORT KTranscript *load_transcript()
    {
        return globalKTI();
    }
}
#endif

// ----------------------------------------------------------------------
// KTranscript definitions.

KTranscriptImp::KTranscriptImp()
{
    // Load user configuration.

    QString tsConfigPath = QStandardPaths::locate(QStandardPaths::ConfigLocation, QStringLiteral("ktranscript.ini"));
    if (tsConfigPath.isEmpty()) {
        tsConfigPath = QDir::homePath() + QLatin1Char('/') + QLatin1String(".transcriptrc");
    }
    config = readConfig(tsConfigPath);
}

KTranscriptImp::~KTranscriptImp()
{
    qDeleteAll(m_sface);
}

QString KTranscriptImp::eval(const QList<QVariant> &argv,
                             const QString &lang,
                             const QString &ctry,
                             const QString &msgctxt,
                             const QHash<QString, QString> &dynctxt,
                             const QString &msgid,
                             const QStringList &subs,
                             const QList<QVariant> &vals,
                             const QString &ftrans,
                             QList<QStringList> &mods,
                             QString &error,
                             bool &fallback)
{
    //error = "debug"; return QString();

    error.clear(); // empty error message means successful evaluation
    fallback = false; // fallback not requested

#if 0
    // FIXME: Maybe not needed, as QScriptEngine has no native outside access?
    // Unportable (needs unistd.h)?

    // If effective user id is root and real user id is not root.
    if (geteuid() == 0 && getuid() != 0) {
        // Since scripts are user input, and the program is running with
        // root permissions while real user is not root, do not invoke
        // scripting at all, to prevent exploits.
        error = "Security block: trying to execute a script in suid environment.";
        return QString();
    }
#endif

    // Load any new modules and clear the list.
    if (!mods.isEmpty()) {
        loadModules(mods, error);
        mods.clear();
        if (!error.isEmpty()) {
            return QString();
        }
    }

    // Add interpreters for new languages.
    // (though it should never happen here, but earlier when loading modules;
    // this also means there are no calls set, so the unregistered call error
    // below will be reported).
    if (!m_sface.contains(lang)) {
        setupInterpreter(lang);
    }

    // Shortcuts.
    Scriptface *sface = m_sface[lang];

    QScriptEngine *engine = sface->scriptEngine;
    QScriptValue gobj = engine->globalObject();

    // Link current message data for script-side interface.
    sface->msgcontext = &msgctxt;
    sface->dyncontext = &dynctxt;
    sface->msgId = &msgid;
    sface->subList = &subs;
    sface->valList = &vals;
    sface->ftrans = &ftrans;
    sface->fallbackRequest = &fallback;
    sface->ctry = &ctry;

    // Find corresponding JS function.
    int argc = argv.size();
    if (argc < 1) {
        //error = "At least the call name must be supplied.";
        // Empty interpolation is OK, possibly used just to initialize
        // at a given point (e.g. for Ts.setForall() to start having effect).
        return QString();
    }
    QString funcName = argv[0].toString();
    if (!sface->funcs.contains(funcName)) {
        error = QStringLiteral("Unregistered call to '%1'.").arg(funcName);
        return QString();
    }

    QScriptValue func = sface->funcs[funcName];
    QScriptValue fval = sface->fvals[funcName];

    // Recover module path from the time of definition of this call,
    // for possible load calls.
    currentModulePath = sface->fpaths[funcName];

    // Execute function.
    QScriptValueList arglist;
    arglist.reserve(argc-1);
    for (int i = 1; i < argc; ++i) {
        arglist.append(variantToJsValue(argv[i]));
    }

    QScriptValue val;
    if (fval.isObject()) {
        val = func.call(fval.toObject(), arglist);
    } else { // no object associated to this function, use global
        val = func.call(gobj, arglist);
    }

    if (fallback) {
        // Fallback to ordinary translation requested.

        // Possibly clear exception state.
        if (engine->hasUncaughtException()) {
            engine->clearExceptions();
        }

        return QString();
    } else if (!engine->hasUncaughtException()) {
        // Evaluation successful.

        if (val.isString()) {
            // Good to go.

            return val.toString();
        } else {
            // Accept only strings.

            QString strval = val.toString();
            error = QStringLiteral("Non-string return value: %1").arg(strval);
            return QString();
        }
    } else {
        // Exception raised.

        error = expt2str(engine);

        engine->clearExceptions();

        return QString();
    }
}

QStringList KTranscriptImp::postCalls(const QString &lang)
{
    // Return no calls if scripting was not already set up for this language.
    // NOTE: This shouldn't happen, as postCalls cannot be called in such case.
    if (!m_sface.contains(lang)) {
        return QStringList();
    }

    // Shortcuts.
    Scriptface *sface = m_sface[lang];

    return sface->nameForalls;
}

void KTranscriptImp::loadModules(const QList<QStringList> &mods,
                                 QString &error)
{
    QList<QString> modErrors;

    foreach (const QStringList &mod, mods) {
        QString mpath = mod[0];
        QString mlang = mod[1];

        // Add interpreters for new languages.
        if (!m_sface.contains(mlang)) {
            setupInterpreter(mlang);
        }

        // Setup current module path for loading submodules.
        // (sort of closure over invocations of loadf)
        int posls = mpath.lastIndexOf(QLatin1Char('/'));
        if (posls < 1) {
            modErrors.append(QStringLiteral(
                                 "Funny module path '%1', skipping.").arg(mpath));
            continue;
        }
        currentModulePath = mpath.left(posls);
        QString fname = mpath.mid(posls + 1);
        // Scriptface::loadf() wants no extension on the filename
        fname = fname.left(fname.lastIndexOf(QLatin1Char('.')));

        // Load the module.
        QScriptEngine *engine = m_sface[mlang]->scriptEngine;
        QScriptValueList alist;
        alist.append(QScriptValue(fname));

        m_sface[mlang]->load(alist);

        // Handle any exception.
        if (engine->hasUncaughtException()) {
            modErrors.append(expt2str(engine));
            engine->clearExceptions();
        }
    }

    // Unset module path.
    currentModulePath.clear();

    foreach (const QString &merr, modErrors) {
        error.append(merr + QLatin1Char('\n'));
    }
}

#define SFNAME "Ts"
void KTranscriptImp::setupInterpreter(const QString &lang)
{
    // Add scripting interface
    // Creates its own script engine and registers with it
    // NOTE: Config may not contain an entry for the language, in which case
    // it is automatically constructed as an empty hash. This is intended.
    Scriptface *sface = new Scriptface(config[lang]);

    // Store scriptface
    m_sface[lang] = sface;

    //dbgout("=====> Created interpreter for '%1'", lang);
}

Scriptface::Scriptface(const TsConfigGroup &config_, QObject *parent)
    : QObject(parent), scriptEngine(new QScriptEngine(this)), fallbackRequest(nullptr), config(config_)
{
    QScriptEngine::QObjectWrapOptions wrapOptions;
    wrapOptions |= QScriptEngine::ExcludeSuperClassContents;
    wrapOptions |= QScriptEngine::ExcludeDeleteLater;
    wrapOptions |= QScriptEngine::ExcludeChildObjects;
    wrapOptions |= QScriptEngine::ExcludeSlots;

    QScriptValue object = scriptEngine->newQObject(this, QScriptEngine::QtOwnership, wrapOptions);
    scriptEngine->globalObject().setProperty(QStringLiteral(SFNAME), object);
}

Scriptface::~Scriptface()
{
    qDeleteAll(loadedPmapHandles);
}

void Scriptface::put(const QString &propertyName, const QScriptValue &value)
{
    QScriptValue internalObject = scriptEngine->globalObject().property(QStringLiteral("ScriptfaceInternal"));
    if (!internalObject.isValid()) {
        internalObject = scriptEngine->newObject();
        scriptEngine->globalObject().setProperty(QStringLiteral("ScriptfaceInternal"), internalObject);
    }

    internalObject.setProperty(propertyName, value);
}

// ----------------------------------------------------------------------
// Scriptface interface functions.

#ifdef _MSC_VER
// Work around bizarre MSVC (2013) bug preventing use of QStringLiteral for concatenated string literals
#    define SPREF(X) QString::fromLatin1(SFNAME "." X)
#else
#    define SPREF(X) QStringLiteral(SFNAME "." X)
#endif

QScriptValue Scriptface::load()
{
    const QScriptValueList fnames = callArgsFromContext(context());
    return load(fnames);
}

QScriptValue Scriptface::setcall(const QScriptValue &name, const QScriptValue &func, const QScriptValue &fval)
{
    if (!name.isString()) {
        return throwError(context(), QScriptContext::TypeError,
                          SPREF("setcall: expected string as first argument"));
    }
    if (!func.isFunction()) {
        return throwError(context(), QScriptContext::TypeError,
                          SPREF("setcall: expected function as second argument"));
    }
    if (!(fval.isObject() || fval.isNull())) {
        return throwError(context(), QScriptContext::TypeError,
                          SPREF("setcall: expected object or null as third argument"));
    }

    QString qname = name.toString();
    funcs[qname] = func;
    fvals[qname] = fval;

    // Register values to keep GC from collecting them. Is this needed?
    put(QStringLiteral("#:f<%1>").arg(qname), func);
    put(QStringLiteral("#:o<%1>").arg(qname), fval);

    // Set current module path as module path for this call,
    // in case it contains load subcalls.
    fpaths[qname] = globalKTI()->currentModulePath;

    return QScriptValue::UndefinedValue;
}

QScriptValue Scriptface::hascall(const QScriptValue &name)
{
    if (!name.isString()) {
        return throwError(context(), QScriptContext::TypeError,
                          SPREF("hascall: expected string as first argument"));
    }

    QString qname = name.toString();
    return QScriptValue(funcs.contains(qname));
}

QScriptValue Scriptface::acall()
{
    const QScriptValueList argv = callArgsFromContext(context());

    if (argv.size() < 1) {
        return throwError(context(), QScriptContext::SyntaxError,
                          SPREF("acall: expected at least one argument (call name)"));
    }
    if (!argv[0].isString()) {
        return throwError(context(), QScriptContext::SyntaxError,
                          SPREF("acall: expected string as first argument (call name)"));
    }

    // Get the function and its context object.
    QString callname = argv[0].toString();
    if (!funcs.contains(callname)) {
        return throwError(context(), QScriptContext::ReferenceError,
                          SPREF("acall: unregistered call to '%1'").arg(callname));
    }
    QScriptValue func = funcs[callname];
    QScriptValue fval = fvals[callname];

    // Recover module path from the time of definition of this call,
    // for possible load calls.
    globalKTI()->currentModulePath = fpaths[callname];

    // Execute function.
    QScriptValueList arglist;
    arglist.reserve(argv.size()-1);
    for (int i = 1; i < argv.size(); ++i) {
        arglist.append(argv[i]);
    }
    QScriptValue val;
    if (fval.isObject()) {
        // Call function with the context object.
        val = func.call(fval.toObject(), arglist);
    } else {
        // No context object associated to this function, use global.
        val = func.call(scriptEngine->globalObject(), arglist);
    }
    return val;
}

QScriptValue Scriptface::setcallForall(const QScriptValue &name, const QScriptValue &func, const QScriptValue &fval)
{
    if (!name.isString()) {
        return throwError(context(), QScriptContext::TypeError,
                          SPREF("setcallForall: expected string as first argument"));
    }
    if (!func.isFunction()) {
        return throwError(context(), QScriptContext::TypeError,
                          SPREF("setcallForall: expected function as second argument"));
    }
    if (!(fval.isObject() || fval.isNull())) {
        return throwError(context(), QScriptContext::TypeError,
                          SPREF("setcallForall: expected object or null as third argument"));
    }

    QString qname = name.toString();
    funcs[qname] = func;
    fvals[qname] = fval;

    // Register values to keep GC from collecting them. Is this needed?
    put(QStringLiteral("#:fall<%1>").arg(qname), func);
    put(QStringLiteral("#:oall<%1>").arg(qname), fval);

    // Set current module path as module path for this call,
    // in case it contains load subcalls.
    fpaths[qname] = globalKTI()->currentModulePath;

    // Put in the queue order for execution on all messages.
    nameForalls.append(qname);

    return QScriptValue::UndefinedValue;
}

QScriptValue Scriptface::fallback()
{
    if (fallbackRequest) {
        *fallbackRequest = true;
    }
    return QScriptValue::UndefinedValue;
}

QScriptValue Scriptface::nsubs()
{
    return QScriptValue(subList->size());
}

QScriptValue Scriptface::subs(const QScriptValue &index)
{
    if (!index.isNumber()) {
        return throwError(context(), QScriptContext::TypeError,
                          SPREF("subs: expected number as first argument"));
    }

    int i = qRound(index.toNumber());
    if (i < 0 || i >= subList->size()) {
        return throwError(context(), QScriptContext::RangeError,
                          SPREF("subs: index out of range"));
    }

    return QScriptValue(subList->at(i));
}

QScriptValue Scriptface::vals(const QScriptValue &index)
{
    if (!index.isNumber()) {
        return throwError(context(), QScriptContext::TypeError,
                          SPREF("vals: expected number as first argument"));
    }

    int i = qRound(index.toNumber());
    if (i < 0 || i >= valList->size()) {
        return throwError(context(), QScriptContext::RangeError,
                          SPREF("vals: index out of range"));
    }

    return variantToJsValue(valList->at(i));
}

QScriptValue Scriptface::msgctxt()
{
    return QScriptValue(*msgcontext);
}

QScriptValue Scriptface::dynctxt(const QScriptValue &key)
{
    if (!key.isString()) {
        return throwError(context(), QScriptContext::TypeError,
                          SPREF("dynctxt: expected string as first argument"));
    }

    QString qkey = key.toString();
    if (dyncontext->contains(qkey)) {
        return QScriptValue(dyncontext->value(qkey));
    }
    return QScriptValue::UndefinedValue;
}

QScriptValue Scriptface::msgid()
{
    return QScriptValue(*msgId);
}

QScriptValue Scriptface::msgkey()
{
    return QScriptValue(QString(*msgcontext + QLatin1Char('|') + *msgId));
}

QScriptValue Scriptface::msgstrf()
{
    return QScriptValue(*ftrans);
}

QScriptValue Scriptface::dbgputs(const QScriptValue &str)
{
    if (!str.isString()) {
        return throwError(context(), QScriptContext::TypeError,
                          SPREF("dbgputs: expected string as first argument"));
    }

    QString qstr = str.toString();

    dbgout("[JS-debug] %1", qstr);

    return QScriptValue::UndefinedValue;
}

QScriptValue Scriptface::warnputs(const QScriptValue &str)
{
    if (!str.isString()) {
        return throwError(context(), QScriptContext::TypeError,
                          SPREF("warnputs: expected string as first argument"));
    }

    QString qstr = str.toString();

    warnout("[JS-warning] %1", qstr);

    return QScriptValue::UndefinedValue;
}

QScriptValue Scriptface::localeCountry()
{
    return QScriptValue(*ctry);
}

QScriptValue Scriptface::normKey(const QScriptValue &phrase)
{
    if (!phrase.isString()) {
        return throwError(context(), QScriptContext::TypeError,
                          SPREF("normKey: expected string as argument"));
    }

    QByteArray nqphrase = normKeystr(phrase.toString());
    return QScriptValue(QString::fromUtf8(nqphrase));
}

QScriptValue Scriptface::loadProps()
{
    const QScriptValueList fnames = callArgsFromContext(context());

    if (globalKTI()->currentModulePath.isEmpty()) {
        return throwError(context(), QScriptContext::UnknownError,
                          SPREF("loadProps: no current module path, aiiie..."));
    }

    for (int i = 0; i < fnames.size(); ++i) {
        if (!fnames[i].isString()) {
            return throwError(context(), QScriptContext::TypeError,
                              SPREF("loadProps: expected string as file name"));
        }
    }

    for (int i = 0; i < fnames.size(); ++i) {
        QString qfname = fnames[i].toString();
        QString qfpath_base = globalKTI()->currentModulePath + QLatin1Char('/') + qfname;

        // Determine which kind of map is available.
        // Give preference to compiled map.
        QString qfpath = qfpath_base + QLatin1String(".pmapc");
        bool haveCompiled = true;
        QFile file_check(qfpath);
        if (!file_check.open(QIODevice::ReadOnly)) {
            haveCompiled = false;
            qfpath = qfpath_base + QLatin1String(".pmap");
            QFile file_check(qfpath);
            if (!file_check.open(QIODevice::ReadOnly)) {
                return throwError(context(), QScriptContext::UnknownError,
                                  SPREF("loadProps: cannot read map '%1'")
                                  .arg(qfpath));
            }
        }
        file_check.close();

        // Load from appropriate type of map.
        if (!loadedPmapPaths.contains(qfpath)) {
            QString errorString;
            if (haveCompiled) {
                errorString = loadProps_bin(qfpath);
            } else {
                errorString = loadProps_text(qfpath);
            }
            if (!errorString.isEmpty()) {
                return throwError(context(), QScriptContext::SyntaxError, errorString);
            }
            dbgout("Loaded property map: %1", qfpath);
            loadedPmapPaths.insert(qfpath);
        }
    }

    return QScriptValue::UndefinedValue;
}

QScriptValue Scriptface::getProp(const QScriptValue &phrase, const QScriptValue &prop)
{
    if (!phrase.isString()) {
        return throwError(context(), QScriptContext::TypeError,
                          SPREF("getProp: expected string as first argument"));
    }
    if (!prop.isString()) {
        return throwError(context(), QScriptContext::TypeError,
                          SPREF("getProp: expected string as second argument"));
    }

    QByteArray qphrase = normKeystr(phrase.toString());
    QHash<QByteArray, QByteArray> props = phraseProps.value(qphrase);
    if (props.isEmpty()) {
        props = resolveUnparsedProps(qphrase);
    }
    if (!props.isEmpty()) {
        QByteArray qprop = normKeystr(prop.toString());
        QByteArray qval = props.value(qprop);
        if (!qval.isEmpty()) {
            return QScriptValue(QString::fromUtf8(qval));
        }
    }
    return QScriptValue::UndefinedValue;
}

QScriptValue Scriptface::setProp(const QScriptValue &phrase, const QScriptValue &prop, const QScriptValue &value)
{
    if (!phrase.isString()) {
        return throwError(context(), QScriptContext::TypeError,
                          SPREF("setProp: expected string as first argument"));
    }
    if (!prop.isString()) {
        return throwError(context(), QScriptContext::TypeError,
                          SPREF("setProp: expected string as second argument"));
    }
    if (!value.isString()) {
        return throwError(context(), QScriptContext::TypeError,
                          SPREF("setProp: expected string as third argument"));
    }

    QByteArray qphrase = normKeystr(phrase.toString());
    QByteArray qprop = normKeystr(prop.toString());
    QByteArray qvalue = value.toString().toUtf8();
    // Any non-existent key in first or second-level hash will be created.
    phraseProps[qphrase][qprop] = qvalue;
    return QScriptValue::UndefinedValue;
}

static QString toCaseFirst(const QString &qstr, int qnalt, bool toupper)
{
    static const QLatin1String head("~@");
    static const int hlen = 2; //head.length()

    // If the first letter is found within an alternatives directive,
    // change case of the first letter in each of the alternatives.
    QString qstrcc = qstr;
    int len = qstr.length();
    QChar altSep;
    int remainingAlts = 0;
    bool checkCase = true;
    int numChcased = 0;
    int i = 0;
    while (i < len) {
        QChar c = qstr[i];

        if (qnalt && !remainingAlts && qstr.midRef(i, hlen) == head) {
            // An alternatives directive is just starting.
            i += 2;
            if (i >= len) {
                break;    // malformed directive, bail out
            }
            // Record alternatives separator, set number of remaining
            // alternatives, reactivate case checking.
            altSep = qstrcc[i];
            remainingAlts = qnalt;
            checkCase = true;
        } else if (remainingAlts && c == altSep) {
            // Alternative separator found, reduce number of remaining
            // alternatives and reactivate case checking.
            --remainingAlts;
            checkCase = true;
        } else if (checkCase && c.isLetter()) {
            // Case check is active and the character is a letter; change case.
            if (toupper) {
                qstrcc[i] = c.toUpper();
            } else {
                qstrcc[i] = c.toLower();
            }
            ++numChcased;
            // No more case checks until next alternatives separator.
            checkCase = false;
        }

        // If any letter has been changed, and there are no more alternatives
        // to be processed, we're done.
        if (numChcased > 0 && remainingAlts == 0) {
            break;
        }

        // Go to next character.
        ++i;
    }

    return qstrcc;
}

QScriptValue Scriptface::toUpperFirst(const QScriptValue &str, const QScriptValue &nalt)
{
    if (!str.isString()) {
        return throwError(context(), QScriptContext::TypeError,
                          SPREF("toUpperFirst: expected string as first argument"));
    }
    if (!(nalt.isNumber() || nalt.isNull())) {
        return throwError(context(), QScriptContext::TypeError,
                          SPREF("toUpperFirst: expected number as second argument"));
    }

    QString qstr = str.toString();
    int qnalt = nalt.isNull() ? 0 : nalt.toInteger();

    QString qstruc = toCaseFirst(qstr, qnalt, true);

    return QScriptValue(qstruc);
}

QScriptValue Scriptface::toLowerFirst(const QScriptValue &str, const QScriptValue &nalt)
{
    if (!str.isString()) {
        return throwError(context(), QScriptContext::TypeError,
                          SPREF("toLowerFirst: expected string as first argument"));
    }
    if (!(nalt.isNumber() || nalt.isNull())) {
        return throwError(context(), QScriptContext::TypeError,
                          SPREF("toLowerFirst: expected number as second argument"));
    }

    QString qstr = str.toString();
    int qnalt = nalt.isNull() ? 0 : nalt.toInteger();

    QString qstrlc = toCaseFirst(qstr, qnalt, false);

    return QScriptValue(qstrlc);
}

QScriptValue Scriptface::getConfString(const QScriptValue &key, const QScriptValue &dval)
{
    if (!key.isString()) {
        return throwError(context(), QScriptContext::TypeError,
                          QStringLiteral("getConfString: expected string as first argument"));
    }
    if (!(dval.isString() || dval.isNull())) {
        return throwError(context(), QScriptContext::TypeError,
                          SPREF("getConfString: expected string as second argument (when given)"));
    }

    QString qkey = key.toString();
    if (config.contains(qkey)) {
        return QScriptValue(config.value(qkey));
    }

    return dval.isNull() ? QScriptValue::UndefinedValue : dval;
}

QScriptValue Scriptface::getConfBool(const QScriptValue &key, const QScriptValue &dval)
{
    if (!key.isString()) {
        return throwError(context(), QScriptContext::TypeError,
                          SPREF("getConfBool: expected string as first argument"));
    }
    if (!(dval.isBoolean() || dval.isNull())) {
        return throwError(context(), QScriptContext::TypeError,
                          SPREF("getConfBool: expected boolean as second argument (when given)"));
    }

    static QStringList falsities;
    if (falsities.isEmpty()) {
        falsities.append(QString(QLatin1Char('0')));
        falsities.append(QStringLiteral("no"));
        falsities.append(QStringLiteral("false"));
    }

    QString qkey = key.toString();
    if (config.contains(qkey)) {
        QString qval = config.value(qkey).toLower();
        return QScriptValue(!falsities.contains(qval));
    }

    return dval.isNull() ? QScriptValue::UndefinedValue : dval;
}

QScriptValue Scriptface::getConfNumber(const QScriptValue &key, const QScriptValue &dval)
{
    if (!key.isString()) {
        return throwError(context(), QScriptContext::TypeError,
                          SPREF("getConfNumber: expected string "
                          "as first argument"));
    }
    if (!(dval.isNumber() || dval.isNull())) {
        return throwError(context(), QScriptContext::TypeError,
                          SPREF("getConfNumber: expected number "
                          "as second argument (when given)"));
    }

    QString qkey = key.toString();
    if (config.contains(qkey)) {
        QString qval = config.value(qkey);
        bool convOk;
        double qnum = qval.toDouble(&convOk);
        if (convOk) {
            return QScriptValue(qnum);
        }
    }

    return dval.isNull() ? QScriptValue::UndefinedValue : dval;
}

// ----------------------------------------------------------------------
// Scriptface helpers to interface functions.

QScriptValue Scriptface::load(const QScriptValueList &fnames)
{
    if (globalKTI()->currentModulePath.isEmpty()) {
        return throwError(context(), QScriptContext::UnknownError,
                          SPREF("load: no current module path, aiiie..."));
    }

    for (int i = 0; i < fnames.size(); ++i) {
        if (!fnames[i].isString()) {
            return throwError(context(), QScriptContext::TypeError,
                              SPREF("load: expected string as file name"));
        }
    }

    for (int i = 0; i < fnames.size(); ++i) {
        QString qfname = fnames[i].toString();
        QString qfpath = globalKTI()->currentModulePath + QLatin1Char('/') + qfname + QLatin1String(".js");

        QFile file(qfpath);
        if (!file.open(QIODevice::ReadOnly)) {
            return throwError(context(), QScriptContext::UnknownError,
                              SPREF("load: cannot read file '%1'") \
                              .arg(qfpath));
        }

        QTextStream stream(&file);
        stream.setCodec("UTF-8");
        QString source = stream.readAll();
        file.close();

        /**
         * fixup QScriptContext
         * else the nested evaluate will not work :/
         * see http://www.qtcentre.org/threads/31027-QtScript-nesting-with-include-imports-or-spawned-script-engines
         * http://www.qtcentre.org/threads/20432-Can-I-include-a-script-from-script
         */
        QScriptContext *currentContext = scriptEngine->currentContext();
        QScriptContext *parentContext = currentContext->parentContext();
        if (parentContext) {
            currentContext->setActivationObject(currentContext->parentContext()->activationObject());
            currentContext->setThisObject(currentContext->parentContext()->thisObject());
        }

        QScriptValue comp = scriptEngine->evaluate(source, qfpath, 0);

        if (comp.isError()) {
            QString msg = comp.toString();

            QString line;
            if (comp.isObject()) {
                QScriptValue lval = comp.toObject().property(QStringLiteral("line"));
                if (lval.isNumber()) {
                    line = QString::number(lval.toInt32());
                }
            }

            return throwError(context(), QScriptContext::TypeError,
                              QStringLiteral("at %1:%2: %3")
                              .arg(qfpath, line, msg));
        }
        dbgout("Loaded module: %1", qfpath);
    }
    return QScriptValue::UndefinedValue;
}

QString Scriptface::loadProps_text(const QString &fpath)
{
    QFile file(fpath);
    if (!file.open(QIODevice::ReadOnly)) {
        return SPREF("loadProps_text: cannot read file '%1'")
               .arg(fpath);
    }
    QTextStream stream(&file);
    stream.setCodec("UTF-8");
    QString s = stream.readAll();
    file.close();

    // Parse the map.
    // Should care about performance: possibly executed on each KDE
    // app startup and reading houndreds of thousands of characters.
    enum {s_nextEntry, s_nextKey, s_nextValue};
    QList<QByteArray> ekeys; // holds keys for current entry
    QHash<QByteArray, QByteArray> props; // holds properties for current entry
    int slen = s.length();
    int state = s_nextEntry;
    QByteArray pkey;
    QChar prop_sep, key_sep;
    int i = 0;
    while (1) {
        int i_checkpoint = i;

        if (state == s_nextEntry) {
            while (s[i].isSpace()) {
                ++i;
                if (i >= slen) {
                    goto END_PROP_PARSE;
                }
            }
            if (i + 1 >= slen) {
                return SPREF("loadProps_text: unexpected end "
                                           "of file in %1").arg(fpath);
            }
            if (s[i] != QLatin1Char('#')) {
                // Separator characters for this entry.
                key_sep = s[i];
                prop_sep = s[i + 1];
                if (key_sep.isLetter() || prop_sep.isLetter()) {
                    return  SPREF("loadProps_text: separator "
                                                "characters must not be letters at %1:%2")
                            .arg(fpath).arg(countLines(s, i));
                }

                // Reset all data for current entry.
                ekeys.clear();
                props.clear();
                pkey.clear();

                i += 2;
                state = s_nextKey;
            } else {
                // This is a comment, skip to EOL, don't change state.
                while (s[i] != QLatin1Char('\n')) {
                    ++i;
                    if (i >= slen) {
                        goto END_PROP_PARSE;
                    }
                }
            }
        } else if (state == s_nextKey) {
            int ip = i;
            // Proceed up to next key or property separator.
            while (s[i] != key_sep && s[i] != prop_sep) {
                ++i;
                if (i >= slen) {
                    goto END_PROP_PARSE;
                }
            }
            if (s[i] == key_sep) {
                // This is a property key,
                // record for when the value gets parsed.
                pkey = normKeystr(s.mid(ip, i - ip), false);

                i += 1;
                state = s_nextValue;
            } else { // if (s[i] == prop_sep) {
                // This is an entry key, or end of entry.
                QByteArray ekey = normKeystr(s.mid(ip, i - ip), false);
                if (!ekey.isEmpty()) {
                    // An entry key.
                    ekeys.append(ekey);

                    i += 1;
                    state = s_nextKey;
                } else {
                    // End of entry.
                    if (ekeys.size() < 1) {
                        return SPREF("loadProps_text: no entry key "
                                                   "for entry ending at %1:%2")
                               .arg(fpath).arg(countLines(s, i));
                    }

                    // Add collected entry into global store,
                    // once for each entry key (QHash implicitly shared).
                    foreach (const QByteArray &ekey, ekeys) {
                        phraseProps[ekey] = props;
                    }

                    i += 1;
                    state = s_nextEntry;
                }
            }
        } else if (state == s_nextValue) {
            int ip = i;
            // Proceed up to next property separator.
            while (s[i] != prop_sep) {
                ++i;
                if (i >= slen) {
                    goto END_PROP_PARSE;
                }
                if (s[i] == key_sep) {
                    return SPREF("loadProps_text: property separator "
                                               "inside property value at %1:%2")
                           .arg(fpath).arg(countLines(s, i));
                }
            }
            // Extract the property value and store the property.
            QByteArray pval = trimSmart(s.mid(ip, i - ip)).toUtf8();
            props[pkey] = pval;

            i += 1;
            state = s_nextKey;
        } else {
            return SPREF("loadProps: internal error 10 at %1:%2")
                   .arg(fpath).arg(countLines(s, i));
        }

        // To avoid infinite looping and stepping out.
        if (i == i_checkpoint || i >= slen) {
            return SPREF("loadProps: internal error 20 at %1:%2")
                   .arg(fpath).arg(countLines(s, i));
        }
    }

END_PROP_PARSE:

    if (state != s_nextEntry) {
        return SPREF("loadProps: unexpected end of file in %1")
               .arg(fpath);
    }

    return QString();
}

// Read big-endian integer of nbytes length at position pos
// in character array fc of length len.
// Update position to point after the number.
// In case of error, pos is set to -1.
template <typename T>
static int bin_read_int_nbytes(const char *fc, qlonglong len, qlonglong &pos, int nbytes)
{
    if (pos + nbytes > len) {
        pos = -1;
        return 0;
    }
    T num = qFromBigEndian<T>((uchar *) fc + pos);
    pos += nbytes;
    return num;
}

// Read 64-bit big-endian integer.
static quint64 bin_read_int64(const char *fc, qlonglong len, qlonglong &pos)
{
    return bin_read_int_nbytes<quint64>(fc, len, pos, 8);
}

// Read 32-bit big-endian integer.
static quint32 bin_read_int(const char *fc, qlonglong len, qlonglong &pos)
{
    return bin_read_int_nbytes<quint32>(fc, len, pos, 4);
}

// Read string at position pos of character array fc of length n.
// String is represented as 32-bit big-endian byte length followed by bytes.
// Update position to point after the string.
// In case of error, pos is set to -1.
static QByteArray bin_read_string(const char *fc, qlonglong len, qlonglong &pos)
{
    // Binary format stores strings as length followed by byte sequence.
    // No null-termination.
    int nbytes = bin_read_int(fc, len, pos);
    if (pos < 0) {
        return QByteArray();
    }
    if (nbytes < 0 || pos + nbytes > len) {
        pos = -1;
        return QByteArray();
    }
    QByteArray s(fc + pos, nbytes);
    pos += nbytes;
    return s;
}

QString Scriptface::loadProps_bin(const QString &fpath)
{
    QFile file(fpath);
    if (!file.open(QIODevice::ReadOnly)) {
        return SPREF("loadProps: cannot read file '%1'")
               .arg(fpath);
    }
    // Collect header.
    QByteArray head(8, '0');
    file.read(head.data(), head.size());
    file.close();

    // Choose pmap loader based on header.
    if (head == "TSPMAP00") {
        return loadProps_bin_00(fpath);
    } else if (head == "TSPMAP01") {
        return loadProps_bin_01(fpath);
    } else {
        return SPREF("loadProps: unknown version of compiled map '%1'")
               .arg(fpath);
    }
}

QString Scriptface::loadProps_bin_00(const QString &fpath)
{
    QFile file(fpath);
    if (!file.open(QIODevice::ReadOnly)) {
        return SPREF("loadProps: cannot read file '%1'")
               .arg(fpath);
    }
    QByteArray fctmp = file.readAll();
    file.close();
    const char *fc = fctmp.data();
    const int fclen = fctmp.size();

    // Indicates stream state.
    qlonglong pos = 0;

    // Match header.
    QByteArray head(fc, 8);
    pos += 8;
    if (head != "TSPMAP00") {
        goto END_PROP_PARSE;
    }

    // Read total number of entries.
    int nentries;
    nentries = bin_read_int(fc, fclen, pos);
    if (pos < 0) {
        goto END_PROP_PARSE;
    }

    // Read all entries.
    for (int i = 0; i < nentries; ++i) {

        // Read number of entry keys and all entry keys.
        QList<QByteArray> ekeys;
        int nekeys = bin_read_int(fc, fclen, pos);
        if (pos < 0) {
            goto END_PROP_PARSE;
        }
        ekeys.reserve(nekeys); //nekeys are appended if data is not corrupted
        for (int j = 0; j < nekeys; ++j) {
            QByteArray ekey = bin_read_string(fc, fclen, pos);
            if (pos < 0) {
                goto END_PROP_PARSE;
            }
            ekeys.append(ekey);
        }
        //dbgout("--------> ekey[0]={%1}", QString::fromUtf8(ekeys[0]));

        // Read number of properties and all properties.
        QHash<QByteArray, QByteArray> props;
        int nprops = bin_read_int(fc, fclen, pos);
        if (pos < 0) {
            goto END_PROP_PARSE;
        }
        for (int j = 0; j < nprops; ++j) {
            QByteArray pkey = bin_read_string(fc, fclen, pos);
            if (pos < 0) {
                goto END_PROP_PARSE;
            }
            QByteArray pval = bin_read_string(fc, fclen, pos);
            if (pos < 0) {
                goto END_PROP_PARSE;
            }
            props[pkey] = pval;
        }

        // Add collected entry into global store,
        // once for each entry key (QHash implicitly shared).
        foreach (const QByteArray &ekey, ekeys) {
            phraseProps[ekey] = props;
        }
    }

END_PROP_PARSE:

    if (pos < 0) {
        return SPREF("loadProps: corrupt compiled map '%1'")
               .arg(fpath);
    }

    return QString();
}

QString Scriptface::loadProps_bin_01(const QString &fpath)
{
    QFile *file = new QFile(fpath);
    if (!file->open(QIODevice::ReadOnly)) {
        return SPREF("loadProps: cannot read file '%1'")
               .arg(fpath);
    }

    QByteArray fstr;
    qlonglong pos;

    // Read the header and number and length of entry keys.
    fstr = file->read(8 + 4 + 8);
    pos = 0;
    QByteArray head = fstr.left(8);
    pos += 8;
    if (head != "TSPMAP01") {
        return SPREF("loadProps: corrupt compiled map '%1'")
               .arg(fpath);
    }
    quint32 numekeys = bin_read_int(fstr, fstr.size(), pos);
    quint64 lenekeys = bin_read_int64(fstr, fstr.size(), pos);

    // Read entry keys.
    fstr = file->read(lenekeys);
    pos = 0;
    for (quint32 i = 0; i < numekeys; ++i) {
        QByteArray ekey = bin_read_string(fstr, lenekeys, pos);
        quint64 offset = bin_read_int64(fstr, lenekeys, pos);
        phraseUnparsedProps[ekey] = QPair<QFile *, quint64>(file, offset);
    }

    // // Read property keys.
    // ...when it becomes necessary

    loadedPmapHandles.insert(file);
    return QString();
}

QHash<QByteArray, QByteArray> Scriptface::resolveUnparsedProps(const QByteArray &phrase)
{
    QPair<QFile *, quint64> ref = phraseUnparsedProps.value(phrase);
    QFile *file = ref.first;
    quint64 offset = ref.second;
    QHash<QByteArray, QByteArray> props;
    if (file && file->seek(offset)) {
        QByteArray fstr = file->read(4 + 4);
        qlonglong pos = 0;
        quint32 numpkeys = bin_read_int(fstr, fstr.size(), pos);
        quint32 lenpkeys = bin_read_int(fstr, fstr.size(), pos);
        fstr = file->read(lenpkeys);
        pos = 0;
        for (quint32 i = 0; i < numpkeys; ++i) {
            QByteArray pkey = bin_read_string(fstr, lenpkeys, pos);
            QByteArray pval = bin_read_string(fstr, lenpkeys, pos);
            props[pkey] = pval;
        }
        phraseProps[phrase] = props;
        phraseUnparsedProps.remove(phrase);
    }
    return props;
}

#include "ktranscript.moc"
