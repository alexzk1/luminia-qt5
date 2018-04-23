#!BPY
"""
Name: 'Lumina'
Blender: 241
Group: 'Export'
Tooltip: 'Exporter for Lumina'
"""
import math
from math import *
import Blender
from Blender import NMesh
from Blender.BGL import *
from Blender.Draw import *

def quad2tri(msh):
  flist=[]
  for face in msh.faces:
    if (len(face.v)==3):
      flist += [face]
    else:
      d1 =(msh.verts[face.v[0].index].co.x - msh.verts[face.v[2].index].co.x) ** 2 
      d1+=(msh.verts[face.v[0].index].co.y - msh.verts[face.v[2].index].co.y) ** 2 
      d1+=(msh.verts[face.v[0].index].co.z - msh.verts[face.v[2].index].co.z) ** 2 
      d2 =(msh.verts[face.v[1].index].co.x - msh.verts[face.v[3].index].co.x) ** 2 
      d2+=(msh.verts[face.v[1].index].co.y - msh.verts[face.v[3].index].co.y) ** 2 
      d2+=(msh.verts[face.v[1].index].co.z - msh.verts[face.v[3].index].co.z) ** 2 
      if (d1<d2):
        flist += [Blender.NMesh.Face([face.v[0],face.v[1],face.v[2]])]
	if (msh.hasFaceUV()):
	  flist[len(flist)-1].uv=[face.uv[0],face.uv[1],face.uv[2]]
	#flist[len(flist)-1].col=[face.col[0],face.col[1],face.col[2]]
        flist[len(flist)-1].smooth=face.smooth
	
	flist += [Blender.NMesh.Face([face.v[0],face.v[2],face.v[3]])]
	if (msh.hasFaceUV()):
          flist[len(flist)-1].uv=[face.uv[0],face.uv[2],face.uv[3]]
	#flist[len(flist)-1].col=[face.col[0],face.col[1],face.col[2]]
	flist[len(flist)-1].smooth=face.smooth
      else:
	flist += [Blender.NMesh.Face([face.v[0],face.v[1],face.v[3]])]
	if (msh.hasFaceUV()):
          flist[len(flist)-1].uv=[face.uv[0],face.uv[1],face.uv[3]]
	#flist[len(flist)-1].col=[face.col[0],face.col[1],face.col[3]]
	flist[len(flist)-1].smooth=face.smooth
	
	flist += [Blender.NMesh.Face([face.v[1],face.v[2],face.v[3]])]
        if (msh.hasFaceUV()):
	  flist[len(flist)-1].uv=[face.uv[1],face.uv[2],face.uv[3]]
	#flist[len(flist)-1].col=[face.col[1],face.col[2],face.col[3]]
	flist[len(flist)-1].smooth=face.smooth
  msh.faces=flist
  return msh

def process_bone(out, groupdic, bone, mat):
  #head = mat * (bone.head.values()[0] -mat.translationPart())
  head = mat.rotationPart().transpose() * bone.head.values()[0] + mat.translationPart()
  
  out.write('<bone name="%s" id="%i" jointx="%f" jointy="%f" jointz="%f" >\n' % (bone.name,groupdic[bone.name], head.x, head.y , head.z))
  if bone.hasChildren():
    for child in bone.children:
      process_bone(out, groupdic, child, mat)
  out.write('</bone>')
  return
    
    
    
def flipquat(l,quat,processed,flink):
  nex = []
  for i in l:
    processed[i] = 1
    for a in flink[i]:
      #print (a)
      if (processed[a]==0):
        #print([i, quat[i],a,quat[i]])
        dot = quat[i].x * quat[a].x + quat[i].y * quat[a].y + quat[i].z * quat[a].z + quat[i].w * quat[a].w
        if (dot < 0):
          #quat[a] *= Blender.Mathutils.Quaternion(-1,0,0,0)
          #quat[a] = Blender.Mathutils.Quaternion(quat[a].x,quat[a].y,quat[a].z,quat[a].w)* Blender.Mathutils.Quaternion(-1,0,0,0)
	  quat[a] = Blender.Mathutils.Quaternion(quat[a].axis, quat[a].angle + 360)
	nex += [a]
	processed[a] = 1
  if (len(nex) > 0):
    flipquat(nex,quat,processed,flink)
  return

hindex = []
def lfind(liste,key):
    global hindex
    #print min (key[0],len (hindex))
    #print key[0]
    #for i in xrange(hindex[min (key[0],len (hindex)-1)],  len( liste)):
    for i in xrange(hindex[min (key[0],len (hindex)-1)], len(liste)):
	
	
	if liste[i]==key:
	   #print i
	   return i
    return 1

def write(filename):
  global num_of_vertexgroups,TBNMenu,Optimise, hindex
#  filename=replace(filename,".blend",".vbo")
  out = file(filename, 'w')
  out.write('<?xml version="1.0" encoding="UTF-8"?>\n')
  out.write('<!DOCTYPE lumina>\n')
  out.write('<lumina version="0.1">\n')
  out.write('<node name="Node">\n')

  #obj = Blender.Object.GetSelected()[0]

  #if obj:
  for obj in Blender.Object.GetSelected():
    if (obj.getType()=='Mesh'):
      msh = quad2tri(obj.getData())
      #project all vertices into a common Worldspace
      verts=[]
      normals=[]
      for i in xrange(0,len(msh.verts)):
        co = obj.mat.rotationPart().transpose() * msh.verts[i].co + obj.mat.translationPart()
        msh.verts[i].co.x = co.x
        msh.verts[i].co.y = co.y
        msh.verts[i].co.z = co.z

        no = obj.mat.rotationPart().transpose() * msh.verts[i].no
        msh.verts[i].no.x = no.x
        msh.verts[i].no.y = no.y
        msh.verts[i].no.z = no.z

      hindex = []
      raw_index = [] #index to find the true vertx
      
      tan = [] #list holding tangent vectors
      quat = [] #list holding the texture space quaternion
      groupdic = {} #dictonary to convert vertexgroup names to numbers
      parentdic = {} # dictonary for finding the parent bone number
      jointdic = {} # dictonary containing the joint positions
      link = [] # list of neigbor vertices
      

      for face in msh.faces:
        for i in range (0,len (face.v)):
           vert = []
           vert += [face.v[i].index]
           if (msh.hasFaceUV()==1):
             vert += [face.uv[i][0]]
             vert += [face.uv[i][1]]
           else:
             vert += [0]
             vert += [0]
           raw_index += [vert] 
      print len(raw_index)   
      raw_index.sort()
      last =raw_index[-1]
      for i in range(len(raw_index)-2, -1, -1):
       if last==raw_index[i]:
         del raw_index[i]
       else: last=raw_index[i]

      print len(raw_index)
      
      print ("Build hindex")
      tmp = -1
      for i in range(0, len(raw_index)):
        if raw_index[i][0] != tmp:
          tmp  = raw_index[i][0]
          hindex += [(i)]

      arm = obj.getParent()
      if arm:
        #parentdic = {}
	parentdic[0] = 0
        bn = arm.getData()
        bns = bn.bones.values()
        for i in range(0,len(bn.bones.values())):
          groupdic[bns[i].name] = i + 1
        for i in range(0,len(bn.bones.values())):
          if bns[i].hasParent():
            parentdic[i+1] = groupdic[bns[i].parent.name]
          else:
            parentdic[i+1] = 0
        print groupdic
        print parentdic

        bonedata = []
        for i in raw_index:
          list = []
          for inf in msh.getVertexInfluences(msh.verts[i[0]].index):
            if groupdic.has_key(inf[0]):
              list += [(inf[1],groupdic[inf[0]])]
          list.sort()
          list.reverse()
          for i in range(0,4):
            list += [(0.0, 0)]
          sum = 0.0
          for i in range(0,4):
            sum += list[i][0]
          if (sum == 0):
            list[0]=(1.0,0)	
          else:
            if(sum <= 1.0 and list[3][1] == 0):
              list[3] = ((1.0 - sum),0)
            else:
              for i in range(0,4):
                list[i] = (list[i][0] / sum * 1.0,list[i][1])
          bonedata += [list]

        
        mat = arm.getMatrix() * 1.0
        jointdic[0] = mat.translationPart() #Blender.Mathutils.Vector(0,0,0)
        for i in range(0,len(bns)):
          jointdic[i+1] = mat.rotationPart().transpose() * bns[i].head.values()[0] + mat.translationPart()
        print jointdic

      if(obj.isSoftBody):
	print("Create interlink")
        for i in msh.verts:
          link.append([])
        for e in msh.edges:
          link[e.v1.index].append([0, e.v2.index])
          link[e.v2.index].append([0, e.v1.index])
        for v in msh.verts:
	  cen = v.co
	  ref = msh.verts[link[v.index][0][1]].co - cen
	  ref.normalize()
          for i in xrange(1,len(link[v.index])):
            aaa = msh.verts[link[v.index][i][1]].co - cen
	    aaa.normalize() 
	    dot = float(Blender.Mathutils.DotVecs (ref ,aaa))
	    if (Blender.Mathutils.DotVecs(Blender.Mathutils.CrossVecs(ref,v.no),aaa) < 0):
	      link[v.index][i][0] = math.acos(dot + 0.000001)
	    else:
              link[v.index][i][0] = 6.2831853071 - math.acos(dot + 0.000001)
          link[v.index].sort() #sort the edges arout the normal
      
      
      if (msh.hasFaceUV()):
        print ("Build tan")
        tan = [Blender.Mathutils.Vector(0,0,0) for i in range(0,len(raw_index))]
        for face in msh.faces:
          v1 = face.v[1].co - face.v[0].co 
          v2 = face.v[2].co - face.v[0].co
          t1 = face.uv[1][1] - face.uv[0][1]
          t2 = face.uv[2][1] - face.uv[0][1]
          sdir = v1 * t2 - v2 * t1 
          sdir.normalize()
          for i in range (0,len (face.v)):
            vert = [face.v[i].index,face.uv[i][0],face.uv[i][1]]
            tan[lfind(raw_index, vert)] += sdir
	#orthogonalize and normalize
	for i in range (0,len(raw_index)):
	  normal = msh.verts[raw_index[i][0]].no
          tan[i] = tan[i] - normal *  Blender.Mathutils.DotVecs( tan[i], normal )
          tan[i].normalize()
          q = Blender.Mathutils.Matrix(tan[i],Blender.Mathutils.CrossVecs(normal,tan[i]),normal).toQuat()
	  quat += [q]
	print(quat)

#face interlink for flipping quaternions 
        flink =[]
	for i in raw_index:
          flink.append([])
        for face in msh.faces:
          
          v0 =  lfind(raw_index, [face.v[0].index,face.uv[0][0],face.uv[0][1]])
          v1 =  lfind(raw_index, [face.v[1].index,face.uv[1][0],face.uv[1][1]])
          v2 =  lfind(raw_index, [face.v[2].index,face.uv[2][0],face.uv[2][1]])

          flink[v0].append(v1)
          flink[v0].append(v2)
          flink[v1].append(v0)
          flink[v1].append(v2)
          flink[v2].append(v0)
          flink[v2].append(v1)

        print flink

	processed = [0 for x in xrange(0,len(raw_index))]
        for i in range (0,len(raw_index)):
          if (processed[i] == 0):
            flipquat([i],quat,processed,flink)
	    print (["start", i])
	print("flipped")
	
#write the data


      out.write('<mesh name="%s" vertices="%i">\n' % (msh.name, len(raw_index)))

      out.write('<component name="Vertex" type="vertex" dim="3" format="float">\n')
      for i in raw_index:
         v = msh.verts[i[0]].co
         out.write('%f %f %f\n' % (v.x,v.y,v.z))
      out.write('</component>\n')

      if (msh.hasFaceUV()==1):
        out.write('<component name="UvCoords" type="uvcoords" dim="2" format="float">\n')
        for i in raw_index:
          out.write('%f %f\n' % (i[1],i[2]))
        out.write('</component>\n')

        out.write('<component name="Tangent" type="vector" dim="3" format="float">\n')
        for t in tan:
           out.write('%f %f %f\n' % (t.x,t.y,t.z))
        out.write('</component>\n')
	
	out.write('<component name="TexSpace" type="quaternion" dim="4" format="float">\n')
        for t in quat:
           out.write('%f %f %f %f \n' % (t.x,t.y,t.z,t.w))
        out.write('</component>\n')
      
      out.write('<component name="Normal" type="vector" dim="3" format="float">\n')
      for i in raw_index:
         v = msh.verts[i[0]].no
         out.write(' %f %f %f\n' % (v.x,v.y,v.z))
      out.write('</component>\n')
      
      
      if arm:
        out.write('<component name="Bones" type="bonedep" dim="4" format="float">\n')
        for l in bonedata:
          out.write( '%f %f %f %f\n' % (0.9999 * l[0][0]+l[0][1],0.9999 * l[1][0]+l[1][1],0.9999 * l[2][0]+l[2][1],0.9999 * l[3][0]+l[3][1]))
        out.write('</component>\n')

        out.write('<component name="RelToJoint" type="vertex" dim="4" format="float">\n')
        for l in xrange(0,len(bonedata)):
          if (bonedata[l][0][1]==parentdic[bonedata[l][1][1]]): #the second is the child
            jointnum = bonedata[l][1][1]
          elif(bonedata[l][1][1]==parentdic[bonedata[l][0][1]]): #the first (or other) is the child
            jointnum = bonedata[l][0][1]
          else:
            jointnum = bonedata[l][0][1]
          rel = msh.verts[raw_index[l][0]].co - jointdic[jointnum]
          out.write( '%f %f %f %f\n' % (rel.x, rel.y, rel.z, jointnum))
        out.write('</component>\n')

      if(obj.isSoftBody):
	out.write('<component name="Softinit1" type="generic" dim="4" format="float">\n')
	
	for i in link:
          for yyy in xrange (len(i),8):
            i.append([0,-1])

	for a in raw_index:
          i = link [a[0]]
          out.write(' %f %f %f %f\n' % (i[0][1],i[1][1],i[2][1],i[3][1]))
        out.write('</component>\n')
	out.write('<component name="Softinit2" type="generic" dim="4" format="float">\n')
	for a in raw_index:
          i = link [a[0]]
          out.write(' %f %f %f %f\n' % (i[4][1],i[5][1],i[6][1],i[7][1]))
        out.write('</component>\n')

        out.write('<component name="Softbody" type="generic" dim="2" format="float">\n')
        for i in raw_index:
          out.write(' %f' % (i[0]))
          softbodyfound = 0
          for inf in msh.getVertexInfluences(msh.verts[i[0]].index):
            if (inf[0] == "Softbody"):
              out.write(' %f\n' % (inf[1]))
              softbodyfound = 1
              print(inf[1])
          if (softbodyfound == 0):
            out.write(' 0.0\n')
        out.write('</component>\n')

      out.write('<index name="Index" primitive="3">\n')
      for face in msh.faces:
	for i in range (0,len (face.v)):
	   if (msh.hasFaceUV()==1):
	     vert = [face.v[i].index,face.uv[i][0],face.uv[i][1]]
	   else:
             vert = [face.v[i].index,0,0]
  	   out.write(' %i' % (lfind(raw_index, vert)))
	out.write('\n')
      out.write('</index>\n')
      out.write('</mesh>\n')

      if arm:
        mat = arm.getMatrix() * 1.0
	out.write('<armature name="Armature">\n')
	for bone in arm.getData().bones.values():
	  if(bone.hasParent()==0):
	    process_bone(out, groupdic, bone,mat)
        out.write('</armature>')
        act = arm.getAction()
        if act:
          frames =act.getFrameNumbers()
          out.write('<buffer name="Pose" frames="%i"  dim="16">\n' % (len(frames)*len(arm.getPose().bones.values())))
          for frame in frames:	
            out.write('1.0 0.0 0.0 0.0  0.0 1.0 0.0 0.0   0.0 0.0 1.0 0.0   0.0 0.0 0.0 1.0\n')
            arm.evaluatePose(frame)
            pose = arm.getPose()
            for bone in pose.bones.values():
              print bone.name
              bmat = arm.getMatrix().invert() * bone.localMatrix * arm.getMatrix()
              out.write('%f %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f\n' % (bmat[0][0],bmat[0][1],bmat[0][2],bmat[0][3],bmat[1][0],bmat[1][1],bmat[1][2],bmat[1][3],bmat[2][0],bmat[2][1],bmat[2][2],bmat[2][3],bmat[3][0],bmat[3][1],bmat[3][2],bmat[3][3]))
          out.write('</buffer>\n')

  out.write('</node>\n')
  out.write('</lumina>\n')

  out.close()
  print ("complete")
  return

Blender.Window.FileSelector(write, "Export")
