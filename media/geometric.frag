#version 120 
 #extension GL_EXT_geometry_shader4 : enable
 void main(void)
 {
      for(int i=0; i<4; i++) 
       {
             gl_TexCoord = gl_TexCoordIn[i][0]
             gl_Position = gl_PositionIn[i];
             EmitVertex();
       }
       EndPrimitive();
 
}
