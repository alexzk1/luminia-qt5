#include <QMessageBox>
#include "script_extender_engine.h"
#include "factory/factory.h"
#include "script_extender.h"
#include "item.h"
#include "mainwindow.h"
#include "glwrapper.h"

SEngine::SEngine(QObject *itm):
    QObject (itm),
    filename("")
{
    setupEngine(itm);
}

SEngine::SEngine(QObject *o, const QString &fileName):
    QObject (o),
    filename(fileName)
{
    setupEngine(o);
}

SEngine::~SEngine()
{
    ScriptExtender::engineList.removeAll (this);
    qDebug() << "SEngine destroyed";
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
    if (!file.isOpen())
        file.open(QIODevice::ReadOnly | QIODevice::Text );

    if (file.isOpen())
    {
        QString script(file.readAll());
        return run(script);
    }
    else
        emit scriptError(tr("Could not open source file."));
    return QScriptValue();
}

void SEngine::useDefaultError()
{
    connect(this, &SEngine::scriptError, this, &SEngine::defaultError, Qt::QueuedConnection);
}

QScriptEngine &SEngine::getEngine()
{
    return eng;
}

void SEngine::defaultError(const QString &err) const
{
    qDebug() << "SEngine: " << err;
    QMessageBox::critical(Item::ws, QString(tr("Script Error")), QString(tr("Error processing \"%1\":\n%2")).arg(filename).arg(err));
}

void SEngine::testErrors() const
{
    if (eng.hasUncaughtException())
    {
        QString err = eng.uncaughtException().toString();
        qDebug() << "TestError: " << err;
        emit scriptError(err);
    }
}

void SEngine::setupEngine(QObject *o)
{
    obj = qobject_cast<Item*>(o);
    if (obj)
    {
        connect(obj, SIGNAL(destroyed()), this, SLOT(deleteLater()));
        QScriptValue obj_sv = eng.newQObject(obj);
        eng.globalObject().setProperty("obj" , obj_sv );
    }

    ogl = new glwrapper(this, "gl");
    ogl->cleartrasher();

    QScriptValue ogl_sv = eng.newQObject(ogl);
    ogl_sv.setPrototype(eng.scriptValueFromQMetaObject<glwrapper>());
    eng.globalObject().setProperty("gl" , ogl_sv );
    Factory::Factory(eng);
    Item::world->bindToEngine(&eng);
}
