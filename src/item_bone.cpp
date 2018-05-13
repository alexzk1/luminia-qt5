/********************************************************************************
** Lumina is a flexible plattform independent development envrionment for
** GLSL shaders. It uses ECMA-script for tools and emulating opengl engines.
**
** Copyright (C) 2007  oc2k1
**
** This program is free software; you can redistribute it and/or
** modify it under the terms of the GNU General Public License
** as published by the Free Software Foundation; either version 2
** of the License, or (at your option) any later version.
**
** This program is distributed in the hope that it will be useful,
** but Q[3]ITHOUT ANQ[1] Q[3]ARRANTQ[1]; without even the implied warranty of
** MERCHANTABILITQ[1] or FITNESS FOR A PARTICULAR PURPOSE.  See the
** GNU General Public License for more details.
**
** Q[1]ou should have received a copy of the GNU General Public License
** along with this program; if not, write to the Free Software
** Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
*********************************************************************************/

#include "item.h"
#include "incgl.h"
#include <cmath>
#include <QDebug>

Item_bone::Item_bone(Item *parent, const QString &label1):
    Item( parent, label1),
    armature(nullptr),
    id(-1)
{

}

Item_bone::Item_bone(Item *parent, const QString& name, Item_armature *_armature, id_t _id) :
    Item( parent, name),
    armature(_armature),
    id(requestId(_id))
{
    //qDebug() << "Bone ID: " << id;
    setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsEditable | Qt::ItemIsDragEnabled | Qt::ItemIsDropEnabled);
    setIcon( 0, QIcon(":/images/xpm/bone.xpm") );

    quat[0] = 0.0;
    quat[1] = 0.0;
    quat[2] = 0.0;
    quat[3] = 1.0;
}

/*!
internal used function to get a status string for the statusbar on mause over
*/
QString Item_bone::statusText() const
{
    return QString("Bone %1 Joint: (%2; %3; %4)").arg(id).arg(joint[0]).arg(joint[1]).arg(joint[2]);
}

/*!
void addBone(String Name)\n
add a bone as childobject.
*/
QObject* Item_bone::addBone(const QString &name)
{
    return makeNewItem<Item_bone>(name, getArmature());
}

/*!
accept only bones from the same armature
*/
bool Item_bone::dragAccept(Item* i)
{
    if (i->getType() == "Bone")
    {
        if ( static_cast<Item_bone*>(i)->getArmature() == getArmature())
            return true;
    }
    return false;
}

QPointer<Item_armature> Item_bone::getArmature() const
{
    return const_cast<Item_armature*>(armature);
}

/*!
void setJoint(number x,number y, number z)\n
set the joint position to x;y;z (Modelspace)
*/
void Item_bone::setJoint(float x, float y, float z)
{
    initJoint[0] = x;
    initJoint[1] = y;
    initJoint[2] = z;
    joint[0] = x;
    joint[1] = y;
    joint[2] = z;

}
/*!
void getId()\n
get the bone ID
*/
Item_bone::id_t Item_bone::getId() const
{
    return id;
}
/*!
void getParentJoint()\n
returns a pointer to the parent bones joint
*/
float* Item_bone::getParentJoint()
{
    return static_cast<Item_bone*>(parent())->joint;
}
/*!
void Rotate(number x, number y, number z, number w)\n
Rotate a bone with the quaternion x;y;z;w. w is the realpart
*/
void Item_bone::Rotate(float a, float b, float c, float d)
{
    float q[4] = {a, b, c, d};
    float inv[4] = {-quat[0], -quat[1], -quat[2], quat[3]};
    qmult(quat, q, q); //project quaternion into bonespace
    qmult(q, inv, q);

    qmult(q, quat, quat);

    for (QTreeWidgetItemIterator it(this); *it; it++)
    {
        if (*it == this)
        {
            qDebug() << "iterator this";
            continue;
        }
        for (QTreeWidgetItem *i = *it; dynamic_cast<Item_bone*>(i); i = i->parent())
        {
            if (i == this)
                if (auto* bone = dynamic_cast<Item_bone*>(*it))
                {
                    qmult(q, bone->quat,  bone->quat);
                    qrotaround(bone->joint, joint, q, bone->joint);
                    //                qDebug() << bone->QTreeWidgetItem::text(0);
                    //                qDebug() << bone->quat[0] << bone->quat[1] << bone->quat[2] << bone->quat[3];
                    //                qDebug() << bone->joint[0] << bone->joint[1] << bone->joint[2] << "\n";
                }
        }
    }
}
/*!
void Rotate(number x, number y, number z, number w)\n
Set the bone rotation to the quaternion x;y;z;w. w is the realpart
*/
void Item_bone::Rotation(float a, float b, float c, float d)
{
    float q[4] = {a, b, c, d};
    float inv[4] = {-quat[0], -quat[1], -quat[2], quat[3]};

    qmult(inv, q, q);

    qmult(quat, q, quat);

    for (QTreeWidgetItemIterator it(this); *it; it++)
    {
        if (*it == this)
        {
            qDebug() << "iterator this";
            continue;
        }
        for (QTreeWidgetItem *i = *it; dynamic_cast<Item_bone*>(i); i = i->parent())
        {
            if (i == this)
                if (auto* bone = dynamic_cast<Item_bone*>(*it))
                {
                    qmult(bone->quat, q, bone->quat);
                    qrotaround(bone->joint, joint, q, bone->joint);
                    //                qDebug() << bone->QTreeWidgetItem::text(0);
                    //                qDebug() << bone->quat[0] << bone->quat[1] << bone->quat[2] << bone->quat[3];
                    //                qDebug() << bone->joint[0] << bone->joint[1] << bone->joint[2] << "\n";
                }
        }
    }
}

/*!
void EulerRotate(number alpha, number beta, number gamma)\n
Euler angle equalient for rotate. Could be buggy if more than one argument is != 0
*/
void Item_bone::EulerRotate(float a, float b, float c)
{
    float c1 = std::cos(a / 2.0f);
    float c2 = std::cos(b / 2.0f);
    float c3 = std::cos(c / 2.0f);
    float s1 = std::sin(a / 2.0f);
    float s2 = std::sin(b / 2.0f);
    float s3 = std::sin(c / 2.0f);
    float w = c1 * c2 * c3 - s1 * s2 * s3;
    float x = s1 * s2 * c3 + c1 * c2 * s3;
    float y = s1 * c2 * c3 + c1 * s2 * s3;
    float z = c1 * s2 * c3 - s1 * c2 * s3;
    Rotate(x, y, z, w);
}
/*!
void EulerRotate(number alpha, number beta, number gamma)\n
Set the bones rotation Euler angle. Could be buggy if more than one argument is != 0
*/
void Item_bone::EulerRotation(float a, float b, float c)
{
    float c1 = std::cos(a / 2.0f);
    float c2 = std::cos(b / 2.0f);
    float c3 = std::cos(c / 2.0f);
    float s1 = std::sin(a / 2.0f);
    float s2 = std::sin(b / 2.0f);
    float s3 = std::sin(c / 2.0f);
    float w = c1 * c2 * c3 - s1 * s2 * s3;
    float x = s1 * s2 * c3 + c1 * c2 * s3;
    float y = s1 * c2 * c3 + c1 * s2 * s3;
    float z = c1 * s2 * c3 - s1 * c2 * s3;
    Rotation(x, y, z, w);
}

void Item_bone::Reset()
{
    quat[0] = 0.0;
    quat[1] = 0.0;
    quat[2] = 0.0;
    quat[3] = 1.0;

    joint[0] = initJoint[0];
    joint[1] = initJoint[1];
    joint[2] = initJoint[2];
}

QString Item_bone::getType() const
{
    return "Bone";
}


/*!
internal function for muliply two quaternions
*/
void Item_bone::qmult(float *qa, float *qb, float *out)
{
    float t[4];
    t[0] = + qa[0] * qb[3] + qa[1] * qb[2] - qa[2] * qb[1] + qa[3] * qb[0] ; //i
    t[1] = - qa[0] * qb[2] + qa[1] * qb[3] + qa[2] * qb[0] + qa[3] * qb[1] ; //j
    t[2] = + qa[0] * qb[1] - qa[1] * qb[0] + qa[2] * qb[3] + qa[3] * qb[2] ; //k
    t[3] = - qa[0] * qb[0] - qa[1] * qb[1] - qa[2] * qb[2] + qa[3] * qb[3] ; //1
    out[0] = t[0];
    out[1] = t[1];
    out[2] = t[2];
    out[3] = t[3];
}
/*!
internal function for rotating vector (or point) v with a quaternion q
*/
void Item_bone::qrot(float *v, float *q, float *out)
{
    float t[4];

    t[0] =             + q[1] * v[2] - q[2] * v[1] + q[3] * v[0]; //i
    t[1] = - q[0] * v[2]             + q[2] * v[0] + q[3] * v[1]; //j
    t[2] = + q[0] * v[1] - q[1] * v[0]             + q[3] * v[2]; //k
    t[3] = - q[0] * v[0] - q[1] * v[1] - q[2] * v[2]            ; //1

    out[0] = + t[0] * q[3] - t[1] * q[2] + t[2] * q[1] - t[3] * q[0]; //x
    out[1] = + t[0] * q[2] + t[1] * q[3] - t[2] * q[0] - t[3] * q[1]; //y
    out[2] = - t[0] * q[1] + t[1] * q[0] + t[2] * q[3] - t[3] * q[2]; //z
}
/*!
internal function for rotation a vector vin arount the point p with quaternion q
*/
void Item_bone::qrotaround(float *vin, float * p, float *q, float *out)
{
    //Vector, Point , Quaternion, Out
    float t[4], v[3];

    v[0] = vin[0] - p[0];
    v[1] = vin[1] - p[1];
    v[2] = vin[2] - p[2];

    t[0] =             + q[1] * v[2] - q[2] * v[1] + q[3] * v[0]; //i
    t[1] = - q[0] * v[2]             + q[2] * v[0] + q[3] * v[1]; //j
    t[2] = + q[0] * v[1] - q[1] * v[0]             + q[3] * v[2]; //k
    t[3] = - q[0] * v[0] - q[1] * v[1] - q[2] * v[2]            ; //1

    out[0] = p[0] + t[0] * q[3] - t[1] * q[2] + t[2] * q[1] - t[3] * q[0]; //x
    out[1] = p[1] + t[0] * q[2] + t[1] * q[3] - t[2] * q[0] - t[3] * q[1]; //y
    out[2] = p[2] - t[0] * q[1] + t[1] * q[0] + t[2] * q[3] - t[3] * q[2]; //z
}

Item_bone::id_t Item_bone::requestId(id_t id)
{
    if (getArmature())
        return getArmature()->requestId(id);
    return -1;
}

/******************************************************************************************************/
#include <cmath>

#include "glwrapper.h"

Item_armature::Item_armature( Item *parent, const QString& name) :
    Item_bone(parent, name),
    used_ids{-1}, //putting -1 as "used" forcing to create new ID
    current_max_id( -1)
{
    setIcon( 0, QIcon(":/images/xpm/armature.xpm") );
    joint[0] = 0.0;
    joint[1] = 0.0;
    joint[2] = 0.0;
}

QString Item_armature::statusText() const
{
    return QString("Armature - Bones: %1").arg(used_ids.size() - 1); //-1 because we added 1 fake in ctor
}

QPointer<Item_armature> Item_armature::getArmature() const
{
    return const_cast<Item_armature*>(this);
}

/*!
returns self instead parent joint
*/
float* Item_armature::getParentJoint()
{
    return joint;
}

Item_bone::id_t Item_armature::requestId(id_t id)
{
    if (id < 0)
        id = -1;
    while (used_ids.count(id))
        id = std::max(id + 1, current_max_id + 1);
    current_max_id = id;
    used_ids.insert(id);
    return id;
}

/*!
void Reset()\n
Resets the armature into default pose
*/
void Item_armature::Reset()
{

    quat[0] = 0.0;
    quat[1] = 0.0;
    quat[2] = 0.0;
    quat[3] = 1.0;
    for (QTreeWidgetItemIterator it(this); *it; ++it)
    {
        if (auto* bone = dynamic_cast<Item_bone*>(*it))
        {
            if (bone != this)
                bone->Reset();
        }
    }
}

QString Item_armature::getType() const
{
    return "Armature";
}

/*!
void Joints(Shaderobject shader, String uniform_variable)\n
script function to pass the transformed armature joint positions as uniform
array to a vertex shader
*/
void Item_armature::Joints(QObject* _shader, const QString& var)
{
    if (auto* shader = dynamic_cast<glwrapper_shader*>(_shader))
    {
        auto bones = getSortedBones();
        std::vector<float> tmp;
        tmp.resize(bones.size() * 3 + 3);
        float* space = tmp.data();

        size_t index = 0; //can use such index, bcs iterator is ordered already, no need for extra order
        for (const auto bone : bones)
        {
            space[index * 3 + 0]    = bone->joint[0];
            space[index * 3 + 1]    = bone->joint[1];
            space[index * 3 + 2]    = bone->joint[2];
            ++index;
        }
        shader->Bind();
        int loc = glGetUniformLocationARB(shader->getShaderHandle(), var.toLatin1().constData());
        if (loc == -1) return;
        glUniform3fvARB(loc, bones.size() + 1, space);
    }
    else
        qDebug() << "Item_armature::Joints parameter is not a shader object";
}

/*!
void Quaternions(Shaderobject shader, String uniform_variable)\n
Function to pass the quaternions as uniform array to a vertexshader
*/
void Item_armature::Quaternions(QObject* _shader, const QString& var)
{
    if (auto* shader = dynamic_cast<glwrapper_shader*>(_shader))
    {
        auto bones = getSortedBones();
        std::vector<float> tmp;
        tmp.resize(bones.size() * 4 + 4);
        float* space = tmp.data();

        size_t index = 0; //can use such index, bcs iterator is ordered already, no need for extra order
        for (const auto bone : bones)
        {
            space[index * 4 + 0]    = bone->quat[0];
            space[index * 4 + 1]    = bone->quat[1];
            space[index * 4 + 2]    = bone->quat[2];
            space[index * 4 + 3]    = bone->quat[3];
            ++index;
        }
        shader->Bind();
        int loc = glGetUniformLocationARB(shader->getShaderHandle(), var.toLatin1().constData());
        glUniform4fvARB(loc, bones.size() + 1, space);
    }
    else
        qDebug() << "Item_armature::Joints parameter is not a shader object";
}
/*!
void Matrices(Shaderobject shader, String uniform_variable)\n
Function to pass 4x4 matrices as uniform array to a vertexshader
*/
void Item_armature::Matrices(QObject* _shader, const QString& var)
{
    if (auto* shader = dynamic_cast<glwrapper_shader*>(_shader))
    {
        auto bones = getSortedBones();
        std::vector<float> tmp;
        tmp.resize(bones.size() * 16 + 16);
        float* space = tmp.data();

        size_t index = 0;
        for (const auto bone : bones)
        {
            float xx = bone->quat[0] * bone->quat[0];
            float xy = bone->quat[0] * bone->quat[1];
            float xz = bone->quat[0] * bone->quat[2];
            float xw = bone->quat[0] * bone->quat[3];

            float yy = bone->quat[1] * bone->quat[1];
            float yz = bone->quat[1] * bone->quat[2];
            float yw = bone->quat[1] * bone->quat[3];

            float zz = bone->quat[2] * bone->quat[2];
            float zw = bone->quat[2] * bone->quat[3];

            //Matrix in OpenGL order!!!
            size_t ofs =  16 * index++;
            space[ofs +  0] = 1.0f - 2.0f * (yy + zz);
            space[ofs +  1] =       2.0f * (xy + zw);
            space[ofs +  2] =       2.0f * (xz - yw);
            space[ofs +  3] = 0.0f;

            space[ofs +  4] =       2.0f * (xy - zw);
            space[ofs +  5] = 1.0f - 2.0f * (xx + zz);
            space[ofs +  6] =       2.0f * (yz + xw);
            space[ofs +  7] = 0.0f;

            space[ofs +  8] =       2.0f * (xz + yw);
            space[ofs +  9] =       2.0f * (yz - xw);
            space[ofs + 10] = 1.0f - 2.0f * (xx + yy);
            space[ofs + 11] = 0.f;

            float x = bone->initJoint[0];
            float y = bone->initJoint[1];
            float z = bone->initJoint[2];

            space[ofs + 12] =  bone->joint[0] - (space[ofs + 0] * x + space[ofs + 4] * y + space[ofs + 8] * z);
            space[ofs + 13] =  bone->joint[1] - (space[ofs + 1] * x + space[ofs + 5] * y + space[ofs + 9] * z);
            space[ofs + 14] =  bone->joint[2] - (space[ofs + 2] * x + space[ofs + 6] * y + space[ofs + 10] * z);
            space[ofs + 15] = 1.0;

        }
        shader->Bind();
        int loc = glGetUniformLocationARB(shader->getShaderHandle(), var.toLatin1().constData());

        glUniformMatrix4fvARB(loc, bones.size() + 1, GL_FALSE, space);

        GL_CHECK_ERROR();
    }
    else
        qDebug() << "Item_armature::Joints parameter is not a shader object";
}

std::vector<Item_bone *> Item_armature::getSortedBones()
{
    std::vector<Item_bone *> bones;
    bones.reserve(used_ids.size()); //FIXME: because those never grow down now, we should avoid having too much long live armature
    for (QTreeWidgetItemIterator it(this); *it; ++it)
    {
        if (auto* bone = dynamic_cast<Item_bone*>(*it))
            bones.push_back(bone);
    }
    std::sort(bones.begin(), bones.end(), [](auto a, auto b)->bool
    {
        return a->getId() < b->getId();
    });
    return bones;
}
