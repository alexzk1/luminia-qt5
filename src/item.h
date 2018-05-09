/********************************************************************************
** Lumina is a flexible plattform independent development envrionment for
** GLSL shaders. It uses ECMA-script for tools and emulating opengl engines.
**
** Copyright (C) 2007-2008  oc2k1
**
** This program is free software; you can redistribute it and/or
** modify it under the terms of the GNU General Public License
** as published by the Free Software Foundation; either version 2
** of the License, or (at your option) any later version.
**
** This program is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
** GNU General Public License for more details.
**
** You should have received a copy of the GNU General Public License
** along with this program; if not, write to the Free Software
** Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
*********************************************************************************/

#ifndef ITEM_H
#define ITEM_H

#define DQMENU(classname,menu)
#define SCRIPTSLOTS(classname, classtype)

#include "script_extender_engine.h"
#include "script_extender.h"
#include "profiler.h"

#include <QTreeWidgetItem>
#include <QPointer>
#include <QDockWidget>
#include <QMenu>
#include <QDebug>
#include <QPointer>

class MainWindow;
class Item_world;
class ScriptLauncher;
/*!
The Item class impletments the basic scriptable TreeWidgetItem
*/
class Item : public QObject, public QTreeWidgetItem, public QScriptable
{
    Q_OBJECT
public:
    Item(Item *parent, const QString& label1);
    virtual ~Item() override;
    virtual bool dragAccept(Item*);
    virtual QString statusText() const{return QString();}
    virtual void setData ( int column, int role,  const QVariant &value ) override;
    void setName (const QString& name);
    QString getName() const;
    Item *parent() const;
    void appendToWs(QWidget *);

    virtual void deleteLater();
    QString getFullScriptName() const;
    void bindToEngine(QScriptEngine *eng, bool localNames = false);
    void resetMenu(); //support for reloading plugins maybe ...because menu is built once
public slots:
    QObject *findChild(const QString& name) const;
    QList<Item*> findChildrenByType ( const QString &) const;

    QObject* getParent(){return parent();}
    void contextmenu(const QPoint&);
    virtual QString getType()const;

    void destroyAll();//destroys all childs
protected:
    virtual void addMenu(QMenu *menu){Q_UNUSED(menu);}
    QScriptValue getEngineParentObject(QScriptEngine &eng) const;
    static QScriptValue getEngineObject(QScriptEngine &eng, const Item *object, bool localName = false);

    template<class T, class ...Args>
    T* makeNewItemNoThis(Args... args)
    {
        T* ptr = new T(args...);
        QScriptEngine *eng = engine();
        if (eng)
        {
            ptr->bindToEngine(eng);
        }
        return ptr;
    }

    template<class T, class ...Args>
    T* makeNewItem(Args... args)
    {
        return makeNewItemNoThis<T>(this, args...);
    }
    virtual bool isDeletable() const;
private:
    QPointer<QMenu> menu;
    void buildMenu(QMenu *menu);
public:
    QPointer<QDockWidget> dock;
    static QPointer<MainWindow> ws;
    static QPointer<Item_world> world;
    static Profiler *profiler;
};

/*!
The World class is the root object, it's similar to a node, but only node and cam are
alowed objects. It has some additional gloabl propertys like time, the acutal cam
resolution or nummber of renderpasses.
*/
class Item_world : public Item
{
    Q_OBJECT
public:
    Item_world();
    void setCamSize(int w, int h);
    virtual bool dragAccept(Item*)override;
public slots:

    void setTime(double value);
    double getTime();

    int getCamWidth();
    int getCamHeight();
    void Call(const QString& function, const QVariantList& args = QVariantList());
    virtual QString getType()const override;

protected:
    double timev;
    int CamWidth,CamHeight;
    virtual bool isDeletable() const override;
    virtual void addMenu(QMenu *menu) override;
signals:
    void update();
public:
    Q_INVOKABLE QObject* addCam(const QString& name = "Cam");
    Q_INVOKABLE QObject* addNode(const QString& name = "Node");
    Q_INVOKABLE QObject* addVirtual(const QString& name = "Virtual");
    Q_INVOKABLE QString  timedString(const QString& preffix = "Temp");
};


/*!
Implementation for rotating and translating objects. It is also as interface for the spaceball functionality.
*/
class GLCam;
class Item_matrix : public Item
{
    Q_OBJECT
    friend class GLCam;


public:
    Item_matrix(Item *parent, const QString& label1);
    virtual float* getMatrix();
public slots:
    virtual void LoadIdenty();
    virtual void Rotate(float angle, float x, float y, float z);
    virtual void Translate(float x, float y, float z);
    virtual void Scale(float x, float y, float z);

protected:
    virtual void MultMatrix(float *mat);
    void getInverseMatrix(float *);
    float matrix[16];
};

class Item_shader;
class Item_script;
class Item_edit;
class Item_texture;
class Item_mesh;

/*!
The node object is the container for shader, scripts, textures, other nodes,
stream or armatures.
*/
class Item_node : public Item_matrix
{
    Q_OBJECT
public:
    Item_node( Item *parent, const QString& label1);
    virtual bool dragAccept(Item*) override;
public slots:
    virtual QString getType()const override;
    void Call(const QString& function, const QVariantList& args = QVariantList());
    //Model importer section: Source in importer/
    void importModel(const QString& filename = "");
public:
    Q_INVOKABLE QObject *addBuffer(const QString& label1 = "Buffer", unsigned dim = 1, unsigned size = 1, unsigned keyframes = 1, int type= GL_FLOAT, bool normalized_int = true);
    Q_INVOKABLE QObject *addMesh(const QString& name = "Mesh", int numOfVertices = 0);
    Q_INVOKABLE QObject *addText(const QString& name = "Text");
    Q_INVOKABLE QObject *addImage(const QString& name = "Image");
    Q_INVOKABLE QObject *addArmature(const QString& name = "Armature");
    Q_INVOKABLE QObject *addNode(const QString& name = "Node");
    Q_INVOKABLE QObject *addScript(const QString& name = "Script");
    Q_INVOKABLE QObject *addVertexshader(const QString& name="Vertexshader");
    Q_INVOKABLE QObject *addGeometryshader(const QString& name="Geometryshader");
    Q_INVOKABLE QObject *addFragmentshader(const QString& name="Fragmentshader");
    Q_INVOKABLE QObject *addTexture(const QString& name="Texture");
    Q_INVOKABLE QObject *addUniform(const QString &name = "Uniform", unsigned dim = 1, unsigned size = 1, unsigned keyframes = 1, int type= GL_FLOAT);
protected:
    void importMD2(const QString& filename);
    void importMD3(const QString& filename);
    void importOBJ(const QString& filename);
    void import3DS(const QString& filename);
    void importCR2(const QString& filename);
    void importX  (const QString& filename);
    void importCMF(const QString& filename);

    virtual void addMenu(QMenu *menu) override;
};

//**********************************************************
class GLCam;
/*!
This is the tree representation for the cam.
*/
class Item_cam : public Item_matrix
{
    Q_OBJECT
    Q_PROPERTY(int Near READ getNear WRITE Near)
    Q_PROPERTY(int Far READ getFar WRITE Far)
public:
    Item_cam(Item_world *parent, const QString& label1);
    virtual ~Item_cam() override = default;
public slots:
    virtual QString getType() const override;
    virtual void deleteLater() override;

protected:
    double getNear();
    double getFar();
    void Near(double val);
    void Far(double val);
    GLCam *cam;
    virtual bool isDeletable() const override;
};


//**********************************************************
class SourceEdit;
class LineNumberWidget;
class Highlighter;

/*!
The basis class for scripts and shaders.
*/
class Item_edit : public Item
{
    Q_OBJECT
public:
    Item_edit( Item *parent, const QString& label1);
    virtual ~Item_edit() override;
public slots:

    void saveas(const QString& filename = "");
    void load(const QString& filename = "");

    void reload();
    QString raw_text() const;

    QString text() const;

    void setText(const QString&);
    virtual QString getType() const override;

protected:
    QPointer<SourceEdit> edit;
    QList<QPointer<QAction>> commonActions;
    QString fn;
    virtual void addMenu(QMenu *menu) override;
};

//**********************************************************
/*!
The shader editors, including the completation handler
*/
class Item_shader : public Item_edit
{
    Q_OBJECT
    Q_ENUMS(shadertype)
public:
    Item_shader( Item *parent, const QString& label1, int shadertype);
    virtual ~Item_shader() override = default;
    enum shadertype{Vertexshader,Geometryshader,Fragmentshader};
    int getShaderType();
public slots:
    virtual QString getType()const override {return QString("Shader");}
private slots:
    void completationHandler(const QString&);
    void helpHandler(const QString&);
protected:
    int shadertype;
};
//**********************************************************

class QSInterpreter;
class glwrapper;
/*!
The script editor, including the completation handler
and scriptinterpreter
*/
class Item_script : public Item_edit
{
    Q_OBJECT
public:
    Item_script( Item *parent, const QString& label1);
    virtual ~Item_script() override;

public slots:
    void run();
    void stop();
    bool isRunning() const;
    void Call(const QString& function, const QVariantList& args = QVariantList());
    virtual QString getType()const override;
private slots:
    void completationHandler(const QString&);
    void helpHandler(const QString&);
    void switchIcon(bool isRunning);
private:
    void deleteEngine();
    bool running;
    QPointer<SEngine> engine;
    const QMetaObject *meta;
};


//**********************************************************

/*!
class for uniform arrays, including keyframe interpolating on CPU. This Class is required because the Buffer
objects memory is located on the opengl servers side, The memory of this object is located on the client side.
*/
class Item_uniform: public Item
{
    Q_OBJECT
    friend class glwrapper;
    friend class glwrapper_shader;
public:
    Item_uniform( Item *parent, const QString& label1, unsigned dim = 1, unsigned size = 1, unsigned keyframes = 1, int type= GL_FLOAT);
    virtual ~Item_uniform() override;
    virtual QString statusText()const override;

    double& operator()(unsigned dim=0, unsigned index=0, unsigned keyframe=0);

    void setData(const QString&);
    QString getData();


public slots:
    virtual void setDim( int dimension = 1, int size = 1, int keyframes = 1, int format= GL_FLOAT);
    void setInKeyFrame(int keyframe, int index, double x, double y=0.0, double z=0.0, double w=0.0);
    void setInKeyFrame(int keyframe, int index, const QList<double>& l);
    void set(int index, double x, double y=0.0, double z=0.0, double w=0.0);
    void set(int index, const QList<double>& l);

    void Uniform(QObject* _shader, QString var);
    void UniformInterpolator( QObject* _shader, const QString& var, float pos);
    void UniformInterpolator( QObject* _shader, const QString& var, float mix, int keyframe1, int keyframe2);

    void PropertiesDialog();

    int getDim();
    int getSize();
    int getKeyFrames();

    int getFormat();

    virtual QString getType() const override;

protected:
    void refresh();
    bool need_refresh;

    unsigned size;
    unsigned dim;
    unsigned keyframes;
    unsigned format;

    double normalized_scalefactor;
    union Buf{
        int *i;
        float *f;
    }buf,tmp_buf;
    virtual void addMenu(QMenu *menu) override;
private:
    double ref_buf; //doublebuffering for reference
    int ref_pos; //position in double buffer
};
/*************************************************************************************/

class half;

/*!
class for bindable buffer and prototype for mesh component
*/
class Item_buffer: public Item
{
    Q_OBJECT
    friend class glwrapper;
    friend class glwrapper_shader;
public:
    Item_buffer(Item *parent, const QString& label1, unsigned dim = 1, unsigned size = 1, unsigned keyframes = 1, int type= GL_FLOAT, bool normalized_int = true);
    virtual ~Item_buffer() override;
    virtual QString statusText()const override;

    double& operator()(unsigned dim=1, unsigned index=1, unsigned keyframe=1);

    void setData(const QString&);
    QString getData();


public slots:
    virtual void setDim(unsigned dimension = 1, unsigned size = 1, unsigned keyframes = 1, unsigned format= GL_FLOAT, bool normalized_int = true);
    void setInKeyFrame(int keyframe, int index, double x, double y=0.0, double z=0.0, double w=0.0);
    void setInKeyFrame(int keyframe, int index, const QList<double>& l);
    void set(int index, double x, double y=0.0, double z=0.0, double w=0.0);
    void set(int index, const QList<double>& l);
    void TextureBind(unsigned tmu);

    static void Unbind(int location);
    static void UnbindAll();

    void Bind(int location);
    void BindKeyFrame(unsigned key, int location);

    void TransformFeedbackBind(const QString& name, int keyframe=0);

    void PropertiesDialog();

    unsigned getDim();
    unsigned getSize();
    unsigned getKeyFrames();
    unsigned getFormat();
    bool isNormalizedInt();

    virtual QString getType() const override{return QString("Buffer");}



protected:
    void refresh();
    unsigned size;
    unsigned dim;
    unsigned keyframes;
    unsigned format;

    static QList<int> boundedAttributeLocations;

    //transformfeedback
    static QList<Item_buffer*> boundedTransformFeedback;
    QString transformFeedbackName;
    unsigned transformFeedbackKeyFrame;
    void transformFeedbackBindPrivate(unsigned n);
    //bool modifiedByGPU;

    void bindableUniformBindPrivate(GLhandleARB shader, int location);

    bool normalized_int;
    double normalized_scalefactor;
    union Buf{
        char *b;
        unsigned char *ub;
        short *s;
        unsigned short *us;
        int *i;
        unsigned int *ui;
        float *f;
        half *h;
        GLvoid *v;
    }buf,tmp_buf;

    unsigned glbuf,gltexture;
    bool need_refresh;
    bool bound_by_GPU; // bound by GPU
    bool is_mapped;

    virtual void addMenu(QMenu *menu) override;
private:
    double ref_buf,ref_buf_old; //doublebuffering for reference, old to detect writing.
    int ref_pos; //position in double buffer
};

//**********************************************************

class Item_armature;
/*!
Quaternion based bon class. Always a child of a Bone or Armature
*/
class Item_bone : public Item
{
    Q_OBJECT
    friend class Item_armature;
public:
    Item_bone( Item *parent, const QString& label1, Item_armature* armature, int id = -1);
    virtual ~Item_bone();
    virtual QString statusText() const;
    virtual bool dragAccept(Item*);
    Item_armature* getArmature(){return armature;}
    float* getJoint(){return initJoint;}
    int getId();

public slots:
    QObject* addBone(QString Name);
    void addBone(){addBone(QString("Bone"));}
    void setJoint(float x,float y, float z);
    void Rotate(float a, float b , float c, float d); // relative rotation
    void Rotation(float a, float b , float c, float d); //absolute rotation

    void EulerRotate(float,float,float);
    void EulerRotation(float,float,float);

    virtual QString getType() const override;

protected:
    int id;
    float joint[3];
    float initJoint[3];

    float quat[4];
    Item_armature *armature;

    virtual float* getParentJoint();
    void qmult(float *qa, float *qb, float *out);
    void qrot(float *v, float *q, float *out);
    void qrotaround(float *v, float *p, float *q, float *out);
    virtual void addMenu(QMenu *menu) override;
};


//**********************************************************

/*!
Armature: a framework for handling quaternion and matrix based bone vertexshaders
*/
class Item_armature : public Item_bone
{
    Q_OBJECT
    friend class Item_bone;
public:
    Item_armature( Item *parent, const QString& label1);
    virtual QString statusText() const ;

public slots:
    void Quaternions(QObject* shader, QString var);		//Quaternion
    void Joints(QObject* shader, QString var);		//Joints
    void Matrices(QObject* _shader, QString var);		//4x4 matrices

    void Reset();
    virtual QString getType() const {return QString("Armature");}

protected:
    int max_bone_id;
    virtual float* getParentJoint();
};




#endif
