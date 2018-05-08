#include "script_extender_engine.h"
#include "factory/factory.h"
#include "item.h"
#include "mainwindow.h"
#include "glwrapper.h"
#include "script_header_parser.h"

SEngine::SEngine(QObject *itm):
    QObject (itm),
    eng(),
    obj(nullptr),
    ogl(nullptr),
    filename("")

{
    setupEngine(itm);
}

SEngine::SEngine(QObject *o, const QString &fileName):
    QObject (o),
    eng(),
    obj(nullptr),
    ogl(nullptr),
    filename(fileName)
{
    setupEngine(o);
}

SEngine::~SEngine()
{
    qDebug() << "SEngine destroyed";
    if (ogl)
        delete ogl;
}

bool SEngine::equals(const SEngine *c) const
{
    return c->obj==obj && c->filename == filename;
}

QScriptValue SEngine::run()
{
    QFile f(filename);
    return run(f);
}

QScriptValue SEngine::run(const QString &src)
{
    QScriptValue r = eng.evaluate(src);
    testErrors();
    return r;
}

QScriptValue SEngine::run(QFile &file)
{
    QString script = ScriptFile::loadScriptFile(file);

    if (!script.isEmpty())
        return run(script);

    emit scriptError(tr("Could not open source file."));
    return QScriptValue();
}

QScriptValue SEngine::execJsFunc(const QString &function, const QVariantList &args)
{
    QScriptValueList qsarglist = QScriptValueList();
    for (const auto& v : args)
    {
        QObject* obj = v.value<QObject*>();
        if(obj)
            qsarglist << eng.newQObject (obj);
        else
        {
            switch(v.type())
            {
                case QVariant::Double:
                case QVariant::Bool:
                case QVariant::Int:
                case QVariant::UInt:
                case QVariant::ULongLong:
                    qsarglist <<  QScriptValue (&eng, v.toDouble());
                    break;
                default:
                    qsarglist <<  QScriptValue (&eng, v.toString());
            }
        }
    }
    QScriptValue f = eng.globalObject().property(function).call(eng.globalObject(), qsarglist);
    if (!testErrors())
    {
        //not sure if I need that
        if (!f.isUndefined() && f.isValid())
        {
            if (f.toString().contains("Error:"))
                emit scriptError(f.toString());
        }
    }
    return f;
}

void SEngine::bindItem(QPointer<Item> itm)
{
    if (itm)
        itm->bindToEngine(&eng);
}

void SEngine::useDefaultError()
{
    connect(this, &SEngine::scriptError, this, &SEngine::defaultError, Qt::QueuedConnection);
}

QScriptEngine &SEngine::getEngine()
{
    return eng;
}

bool SEngine::testErrors() const
{
    if (eng.hasUncaughtException())
    {
        QString err = eng.uncaughtException().toString();
        qDebug() << "TestError: " << err;
        emit scriptError(err);
        return true;
    }
    return false;
}

void SEngine::setupEngine(QObject *o)
{
    obj = qobject_cast<Item*>(o);
    qDebug() <<"Binding " << o << obj;
    if (obj)
    {
        qDebug() <<"Binding " << obj->getType() << " as OBJ to javascr";
        connect(obj, SIGNAL(destroyed()), this, SLOT(deleteLater()));
        eng.globalObject().setProperty("obj" , eng.newQObject(obj));
    }

    ogl = new glwrapper(this, "gl");
    ogl->cleartrasher();

    QScriptValue ogl_sv = eng.newQObject(ogl);
    ogl_sv.setPrototype(eng.scriptValueFromQMetaObject<glwrapper>());
    eng.globalObject().setProperty("gl" , ogl_sv );
    Factory::Factory(eng);

    ScriptExtender::loadImported(this);
    bindItem(Item::world.data());
}

#include <QMessageBox>
void SEngine::defaultError(const QString &err) const
{
    qDebug() << "SEngine: " << err;
    QMessageBox::critical(Item::ws, QString(tr("Script Error")), QString(tr("Error processing \"%1\":\n%2")).arg(filename).arg(err));
}
