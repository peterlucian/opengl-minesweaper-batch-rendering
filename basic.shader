#shader vertex
#version 330 core
     layout (location = 0) in vec2 aPos;   
     layout (location = 1) in vec2 texCoord; 
     layout (location = 2) in float a_TexIndex; 
     out vec2 v_TexCoord; 
     out float v_TexIndex;
     uniform mat4 u_MVP;
     void main()   
     {   
        //gl_Position= aPos; 
        gl_Position = u_MVP * vec4(aPos, -1.0f, 1.0f); 
        //gl_Position = vec4(pos, 0.0, 1.0); 
        v_TexCoord = texCoord;
        v_TexIndex = a_TexIndex; 
     };

#shader fragment
#version 330 core  
     layout (location = 0) out vec4 FragColor;
     in vec2 v_TexCoord;
     in float v_TexIndex;
     uniform sampler2DArray textureArray;   
     void main()   
     {   
          //int index = int(v_TexIndex);
          FragColor = texture(textureArray, vec3(v_TexCoord, v_TexIndex));   
     };