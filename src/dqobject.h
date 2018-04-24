#ifndef DYNAMIC_QOBJECT_H
#define DYNAMIC_QOBJECT_H

#include <QMetaObject>
#include <QByteArrayData>
#include <memory.h>
#include <QDebug>
#include <stdint.h>
#include <algorithm>

#include <type_traits>
#define CAST_INT(X) (*reinterpret_cast< int(*)>(args[X]))
#define CAST_BOOL(X) (*reinterpret_cast< bool(*)>(args[X]))
#define CAST_QSTRING(X) (*reinterpret_cast< const QString(*)>(args[X]))

template<typename T>
T max(T a, T b)
{
    return std::max<T>(a, b);
}


class DQMetaObject : public QMetaObject
{
public:
    DQMetaObject() = delete;
    DQMetaObject(const QMetaObject *parent, const char *stringdata, const uint *data)
    {
        d.superdata = parent;
        d.extradata = nullptr;
        setValues(stringdata, data);
    }

    DQMetaObject(const DQMetaObject& src)
    {
        operator=(src);
        qDebug() << "copy constructor";
    }

    ~DQMetaObject()
    {
        deleteValues();
    }

    DQMetaObject& operator= (const DQMetaObject& src)
    {
        d.superdata = src.d.superdata;
        d.extradata = src.d.extradata;
        deleteValues();
        setValues(static_cast<const char*>(src.d.stringdata->data()), src.d.data);

        return *this;
    }

    uint registerFunction(const char * type, const char * signature,const char * parameters)
    {

        auto s = dataSize(d.data);
        uint * tmpData = new uint [s + 5];

        memcpy (tmpData + 5, d.data, s * sizeof(uint)); // copy all stuff behind methods
        memcpy (tmpData, d.data  , (d.data[5]+ 5 * d.data[4])* sizeof(uint)); //header inc methods


        //appending strings
        auto sig_len = strlen(signature) + 1;
        auto par_len = strlen(parameters) + 1;
        auto typ_len = strlen(type) + 1;

        auto old_len = stringDataSize(d.stringdata,d.data);
        char * tmpStringData = new char [old_len + sig_len + par_len + typ_len];
        memcpy ( tmpStringData, d.stringdata->data(), old_len * sizeof(char)); //old stuff
        memcpy ( tmpStringData + old_len, signature, sig_len * sizeof(char) ); // signature
        memcpy ( tmpStringData + old_len + sig_len,parameters , par_len * sizeof(char)); // parameters
        memcpy ( tmpStringData + old_len + sig_len + par_len, type , typ_len * sizeof(char)); // parameters

        //insert function
        tmpData[4]++; //adjust other stuff too....

        auto ofs = 5 * d.data[4] + d.data[5];
        tmpData[ofs + 0] = old_len; //signature
        tmpData[ofs + 1] = old_len + sig_len; //parameters
        tmpData[ofs + 2] = old_len + sig_len + par_len; // type
        tmpData[ofs + 3] = 9; // empty string
        tmpData[ofs + 3] =  0x0a; // don't know.....

        //qDebug() << s << old_len << deb << "\n" << tmpStringData + tmpData[ofs -5] << tmpStringData + tmpData[ofs -4];

        deleteValues();
        setValues(tmpStringData, tmpData);

        auto r = tmpData[4] - 1;
        delete [] tmpData;
        delete [] tmpStringData;

        return r; // return the function number
    }

    uint numberOfMethods() const
    {
        return d.data[4];
    }

private:

    void deleteValues()
    {
        if (d.stringdata)
            QByteArrayData::deallocate(const_cast<QByteArrayData*>(d.stringdata), 1, alignof(char));

        if (d.data)
            delete[] d.data;

        d.stringdata = nullptr;
        d.data       = nullptr;
    }

    void setValues(const char *stringdata, const uint *data)
    {
        auto s = dataSize(data);

        auto tmp = new uint [s];
        memcpy (tmp, data, s * sizeof(uint));
        d.data = tmp;

        s = stringDataSize(stringdata, data);
        QByteArrayData *sd = QByteArrayData::allocate(sizeof (char), alignof(char), s);
        if (stringdata)
            memcpy (sd->data(),  stringdata, s * sizeof(char));
        d.stringdata = sd;
    }

    uint stringDataSize(decltype(d.stringdata) _stringdata, const uint *meta_data)
    {
        return stringDataSize(static_cast<const char *>((d.stringdata) ? _stringdata->data() : nullptr), meta_data);
    }

    uint stringDataSize(const char *_stringdata, const uint *meta_data)
    {
        uint maxindex = 0;
        // 4 = number of methods // 5 = offset
        for (auto i = meta_data[5]; i < 5 * meta_data[4] + meta_data[5]; i +=5)
        {
            for (unsigned k = 0; k < 4; k++)
            {
                maxindex = max( maxindex, meta_data[i + k]);
            }
        }
        //add properties and enums/sets handling here if required

        //qDebug()  <<  _stringdata + maxindex <<   maxindex  <<  maxindex + strlen(_stringdata + maxindex);
        return static_cast<uint>(maxindex + ((_stringdata)?strlen(_stringdata + maxindex):0) + 1);
    }

    uint dataSize(const uint *meta_data)
    {
        uint maxend = 0;
        maxend = max (maxend, 5 * meta_data[4] + meta_data[5]); // methods
        //maxend = max (maxend, 3 * meta_data[6] + meta_data[7]); // properties not used
        //maxend = max (maxend, 2 * meta_data[8] + meta_data[9]); // enums/sets not used
        //qDebug() << meta_data[maxend]
        return maxend + 1;
    }
};


static const uint qt_meta_data_DQObject[] = {
    // content:
    1,       // revision
    0,       // classname
    0,    0, // classinfo
    1,   10, // methods
    0,    0, // properties
    0,    0, // enums/sets

    // slots: signature, parameters, type, tag, flags
    15,   10,   9,   9, 0x0a,

    0        // eod
};

static const char qt_meta_stringdata_DQObject[] = {
    "DQObject\0\0name\0createSlot(QString)\0"
};




typedef void (*Callback)( QObject* obj, int id , void** args);

class CallBackSlot{
public:
    CallBackSlot(Callback _callback, int _id, int _slot_id){
        callback = _callback;
        id = _id;
        slot_id = _slot_id;
    }

    Callback callback;
    int id;
    int slot_id;
};


class Action{
public:
    Action(const char * _icon, const char * _text, const char * _member){
        icon = _icon;
        text = _text;
        member = _member;
    }

    const char *icon;
    const char *text;
    const char *member;
};


template<class T>
class DQObject : public T {
public:

    template<class X>
    DQObject(X *parent, const QString& name, int a ) : T(parent, name, a){}
    template<class X>
    DQObject(X *parent, const QString& name, int a, int b) : T(parent, name, a, b){}
    template<class X>
    DQObject(X *parent, const QString& name, int a, int b, int c) : T(parent, name, a, b, c){}
    template<class X>
    DQObject(X *parent, const QString& name, int a, int b, int c, int d) : T(parent, name, a, b, c, d){}
    template<class X>
    DQObject(X *parent, const QString& name, int a, int b, int c, int d, bool e ) : T(parent, name, a, b, c, d, e){}
    template<class X>
    DQObject(X *parent, const QString& name, int a, int b, int c, int d, int e ) : T(parent, name, a, b, c, d, e){}
    template<class X>
    DQObject(X *parent, const QString& name, int a, int b, int c, int d, int e, bool f) : T(parent, name, a, b, c, d, e, f){}


    template<class X>
    DQObject(X *parent, const QString& name) : T(parent, name){}
    template<class X>
    DQObject(X *parent) : T(parent){}

    static QList<Action> actionlist;
private:

    static DQMetaObject staticMetaObject;

    //QList<int> proxySlots;
    static QList<CallBackSlot> callBackSlots;



    const QMetaObject* metaObject() const{
        //qDebug() << "QMetaObject* metaObject() const";
        return &staticMetaObject;
    }

    void* qt_metacast(const char *_clname){
        if (!_clname) return 0;
        if (!strcmp(_clname, qt_meta_stringdata_DQObject)){
            return static_cast<void*>(const_cast< DQObject*>(this));
        }
        return QObject::qt_metacast(_clname);
    }

    virtual int qt_metacall(QMetaObject::Call _c, int _id, void **_a){
        _id = T::qt_metacall(_c, _id, _a);
        if (_id < 0)
            return _id;
        if (_c == QMetaObject::InvokeMetaMethod) {
            switch (_id) {
                case 0:
                    qDebug() << "call createSlot(...)";
                    createSlot((*reinterpret_cast< const QString(*)>(_a[1])));
                    break;
                default:
                    for (int i = 0; i < callBackSlots.size(); i++){
                        if (callBackSlots.at(i).slot_id == _id){
                            //qDebug() << "calling "  << callBackSlots.at(i).id << (void*)callBackSlots.at(i).callback;
                            callBackSlots.at(i).callback(this, callBackSlots.at(i).id,  _a);
                        }
                    }
            }

            _id -= staticMetaObject.numberOfMethods();
        }
        return _id;
    }

    void createSlot(const QString& /*name*/){
        //qDebug() << " createSlot(name)" << name << staticMetaObject.d.stringdata;
        //proxySlots << staticMetaObject.registerFunction(name.toLatin1().data (), "");
        //staticMetaObject.registerFunction(name.toLatin1().data (), "");
        /*QString n = name;
        if(name.contains("()")){
            n = n.replace("(","_signal(QObject*");
            }
        else{
            n = n.replace("(","_signal(QObject*,");
            }

        staticMetaObject.registerFunction(n.toLatin1().data (), ""); */
    }

public:
    static void createCallBackSlot(const char* type, const char* signature, const char* args, Callback callback, int id){
        int slot = staticMetaObject.registerFunction(type, signature, args);
        //qDebug() << "createCallBackSlot" << type << signature << slot << (void*)callback;
        callBackSlots << CallBackSlot(callback, id, slot );
    }

    //overloaded member for void slots
    /*static void createCallBackSlot(const char* signature, const char* args,  Callback callback, int id){
        int slot = staticMetaObject.registerFunction("", signature, args);
        qDebug() << "createCallBackSlot void" << signature << slot << (void*)callback;
        callBackSlots << CallBackSlot(callback, id, slot );

        }*/



};

template<class T>
DQMetaObject DQObject<T>::staticMetaObject = DQMetaObject(&T::staticMetaObject, qt_meta_stringdata_DQObject, qt_meta_data_DQObject);

template<class T>
QList<CallBackSlot> DQObject<T>::callBackSlots = QList<CallBackSlot>();

template<class T>
QList<Action> DQObject<T>::actionlist = QList<Action>();

#endif
