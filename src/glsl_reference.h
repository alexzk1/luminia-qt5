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
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
** GNU General Public License for more details.
**
** You should have received a copy of the GNU General Public License
** along with this program; if not, write to the Free Software
** Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
*********************************************************************************
**
**  This informations are based on the GLSL specification 1.20.8
**  http://www.opengl.org/registry/doc/GLSLangSpec.Full.1.20.8.pdf
**
*********************************************************************************/
const char *glsl_ref[] ={
"radians",		"genType radians(genType degrees)\nConverts degrees to radians, i.e. Pi/180*degrees.",
"degrees",		"genType degrees(genType radinace)\nConverts radians to degrees, i.e, 180/Pi*radian.",
"sin",			"genType sin(genType angle)\nThe standard trigonomic sine function.",
"cos",			"genType cos(genType angle)\nThe standard trigonomic cosine function.",
"tan",			"genType tan(genType angle)\nThe standard trigonomic tangent.",
"asin",			"genType asin(genType x)\nArc sine. Retuns an angle whose sine is x. The range of values returned by this function is [-Pi/2;Pi/2] Results are undefined if |x|>1.",
"acos",			"genType acos(genType x)\nArc cosine. Returns an angle whose cosine is x. The range of values returned by this function is [0, Pi]. Results are undefined if |x|>1.",
"atan",			"genType atan(genType y [, genType x])\nArc tangent. Returns an angle whose tangent is y/x. The signs of x and y are used to determine what quadrant the angle is in. The returned range is [-Pi,Pi]. Without x it will be the standard atan function",
"atan",			"genType atan (genType y_over_x)\nArc tangent. Returns an angle whose tangent is y_over_x. The range of values returned by this function is [-Pi/2;Pi/2].",
"pow",			"genType pow (genType x, genType y)\nReturns x raised to the y power, Results are undefined if x < 0. Results are undefined if x = 0 and y <= 0.",
"exp",			"genType exp (genType x)\nReturns the natural exponentiation of x, i.e., ex.",
"log",			"genType log (genType x)\nReturns the natural logarithm of x, i.e., returns the value y which satisfies the equation x = ey. Results are undefined if x <= 0.",
"exp2",			"genType exp2 (genType x)\nReturns 2 raised to the x power",
"log2",			"genType log2 (genType x)\nReturns the base 2 logarithm of x, i.e., returns the value y which satisfies the equation x= 2^y Results are undefined if x <= 0.",
"sqrt",			"genType sqrt (genType x)\nReturns the square root of x. Results are undefined if x < 0.",
"inversesqrt",		"genType inversesqrt (genType x)\nReturns the inverse square root of x. Results are undefined if x <= 0.",
"abs",			"genType abs (genType x)\nReturns x if x >= 0, otherwise it returns -x.",
"sign",			"genType sign (genType x)\nReturns 1.0 if x > 0, 0.0 if x = 0, or -1.0 if x < 0",
"floor",		"genType floor (genType x)\nReturns a value equal to the nearest integer that is less than or equal to x",
"ceil",			"genType ceil (genType x)\nReturns a value equal to the nearest integer that is greater than or equal to x",
"fract",		"genType fract (genType x)\nReturns x - floor (x)",
"mod",			"genType mod (genType x, genType|float y)\nModulus. Returns x - y * floor (x/y)",
"min",			"genType min (genType x, genType|float y)\nReturns y if y < x, otherwise it returns x",
"max",			"genType max (genType x, genType|float y)\nReturns y if y > x, otherwise it returns x",
"clamp",		"genType clamp (genType x, genType|float minVal, genType|float maxVal)\nReturns min(max(x,minVal),maxVal) Results are undefined if minVal > maxVal.",
"mix",			"genType mix (genType x, genType y, genType|float a)\nReturns the linear blend of x and y, i.e. x*(1-a)+y*a",
"step",			"genType step (genType|float edge, genType x)\nReturns 0.0 if x < edge, otherwise it returns 1.0",
"smoothstep",		"genType smoothstep (genType|float edge0, genType|float edge1, genType x)\nReturns 0.0 if x <= edge0 and 1.0 if x >= edge1 and performs smooth Hermite interpolation between 0 and 1 when edge0 < x < edge1.",
"length",		"float length(genType x)\nReturns the length of vector x",
"distance",		"float distance(genType p0, genType p1)\nReturns the distance between p0 and p1",
"dot",			"float(genType x, genType y)\nRetruns the dot product of x and y",
"cross",		"vec3 cross(vec3 x, vec3 y)\nReturns the cross product of x and y",
"normalize",		"genType normalize(genType x)\n Returns a normalized vector of x",
"ftransform",		"vec4 ftransform()\n Transform a vertex like the fixed functionality path",
"faceforward",		"genType faceforward(genType N, genType I, genType Nref)\nIf dot(Nref,I) < 0 return N, otherwise return -N.",
"reflect",		"genType reflect(genType I, genType N)\nReturns the at the surface with normal N reflected vector I",
"refract",		"genType refract(genType I, genType N, float eta)\nReturns an refracted vector.",
"matrixCompMult",	"mat(matrixCompMult(mat x, mat y)\n Multiply matrix x by matrix y component wise.",
"outerProduct", 	"mat outerProduct(vec c, vec r)\nReturns a Matrix with the outer product of c and r",
"transpose",		"mat transpose(mat m)\nReturns the transposed matrix of m",
"lessThan",		"bvec lessThan(vec x, vec y)\nReturns the component-wise compare of x < y.",
"lessThanEqual",	"bvec lessThanEqual(vec x, vec y)\nReturns the component-wise compare of x <= y.",
"greaterThan",		"bvec greaterThan(vec x, vec y)\nReturns the component-wise compare of x > y.",
"greaterThanEqual",	"bvec greaterThanEqual(vec x, vec y)\nReturns the component-wise compare of x >= y.",
"equal",		"bvec equal(vec x, vec y)\nReturns the component-wise compare of x == y.",
"notEqual",		"bvec notEqual(vec x, vec y)\nReturns the component-wise compare of x != y",
"any",			"bool any(bvec x)\nReturns true if any component of x is true.",
"all",			"bool all(bvec x)\nReturns true only if all components of x are true.",
"not",			"bvec not(bvec x)\nReturns the component-wise logical complement of x.",
"texture1D",		"vec4 texture1D (sampler1D sampler, float coord [, float bias])\n",
"texture1DProj",	"vec4 texture1DProj (sampler1D sampler, vec2|vec4 coord [, float bias])\n",
"texture1DLod",		"vec4 texture1DLod (sampler1D sampler, float coord, float lod)\n",
"texture1DProjLod",	"vec4 texture1DProjLod (sampler1D sampler, vec2|vec4 coord, float lod)\n",
"texture2D",		"vec4 texture2D (sampler2D sampler, vec2 coord [, float bias])\n",
"texture2DProj",	"vec4 texture2DProj (sampler2D sampler, vec3|vec4 coord [, float bias])\n",
"texture2DLod",		"vec4 texture2DLod (sampler2D sampler, vec2 coord, float lod)\n",
"texture2DProjLod",	"vec4 texture2DProjLod (sampler2D sampler, vec3|vec4 coord, float lod)\n",
"texture3D",		"vec4 texture3D (sampler3D sampler, vec3 coord [, float bias])\n",
"texture3DProj",	"vec4 texture3DProj (sampler3D sampler, vec4 coord [, float bias])\n",
"texture3DLod",		"vec4 texture3DLod (sampler3D sampler, vec3 coord, float lod)\n",
"texture3DProjLod",	"vec4 texture3DProjLod (sampler3D sampler, vec4 coord, float lod)\n",
"textureCube",		"vec4 textureCube (samplerCube sampler, vec3 coord [, float bias])\n",
"textureCubeLod",	"vec4 textureCubeLod (samplerCube sampler, vec3 coord, float lod)\n",
"shadow1D",		"vec4 shadow1D (sampler1DShadow sampler, vec3 coord [, float bias])\n",
"shadow2D",		"vec4 shadow2D (sampler2DShadow sampler, vec3 coord [, float bias])\n",
"shadow1DProj",		"vec4 shadow1DProj (sampler1DShadow sampler, vec4 coord [, float bias])\n",
"shadow2DProj",		"vec4 shadow2DProj (sampler1DShadow sampler, vec4 coord [, float bias])\n",
"shadow1DLod",		"vec4 shadow1DLod (sampler1DShadow sampler, vec3 coord, float lod)\n",
"shadow2DLod",		"vec4 shadow2DLod (sampler2DShadow sampler, vec3 coord, float lod)\n",
"shadow1DProjLod",	"vec4 shadow1DProjLod(sampler1DShadow sampler, vec4 coord, float lod)\n",
"shadow2DProjLod",	"vec4 shadow2DProjLod(sampler2DShadow sampler, vec4 coord, float lod)\n",
"dFdx",			"genType dFdx (genType p)\nReturns the derivative in x using local differencing for the input argument p.",
"dFdy",			"genType dFdy (genType p)\nReturns the derivative in y using local differencing for the input argument p.",
"fwidth",		"genType fwidth (genType p)\nReturns the sum of the absolute derivative in x and y using local differencing for the input argument p.",
"noise1",		"float noise1 (genType x)\nReturns a 1D noise value based on the input value x.",
"noise2",		"vec2 noise2 (genType x)\nReturns a 2D noise value based on the input value x.",
"noise3",		"vec3 noise3 (genType x)\nReturns a 3D noise value based on the input value x.",
"noise4",		"vec4 noise4 (genType x)\nReturns a 4D noise value based on the input value x.",
0,0};








