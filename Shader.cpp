//
// Book:      OpenGL(R) ES 2.0 Programming Guide
// Authors:   Aaftab Munshi, Dan Ginsburg, Dave Shreiner
// ISBN-10:   0321502795
// ISBN-13:   9780321502797
// Publisher: Addison-Wesley Professional
// URLs:      http://safari.informit.com/9780321563835
//            http://www.opengles-book.com
//

// ESShader.c
//
//    Utility functions for loading shaders and creating program objects.
//

#include "Shader.h"

std::vector<Shader> shaderObjects;

void  esLogMessage ( const char *formatStr, ... )
{
    va_list params;
    char buf[BUFSIZ];

    va_start ( params, formatStr );
    vsprintf ( buf, formatStr, params );

    printf ( "%s", buf );

    va_end ( params );
}

//
///
/// \brief Load a shader, check for compile errors, print error messages to output log
/// \param type Type of shader (GL_VERTEX_SHADER or GL_FRAGMENT_SHADER)
/// \param shaderSrc Shader source string
/// \return A new shader object on success, 0 on failure
//
GLuint  esLoadShader ( GLenum type, const char *shaderSrc )
{
   GLuint shader;
   GLint compiled;

   // Create the shader object
   shader = glCreateShader ( type );

   if ( shader == 0 )
    return 0;

   // Load the shader source
   glShaderSource ( shader, 1, &shaderSrc, NULL );

   // Compile the shader
   glCompileShader ( shader );

   // Check the compile status
   glGetShaderiv ( shader, GL_COMPILE_STATUS, &compiled );

   if ( !compiled )
   {
      GLint infoLen = 0;

      glGetShaderiv ( shader, GL_INFO_LOG_LENGTH, &infoLen );

      if ( infoLen > 1 )
      {
         char* infoLog = (char*) malloc (sizeof(char) * infoLen );

         glGetShaderInfoLog ( shader, infoLen, NULL, infoLog );
         esLogMessage ( "Error compiling shader:\n%s\n", infoLog );

         free ( infoLog );
      }

      glDeleteShader ( shader );
      return 0;
   }

   return shader;

}


//
///
/// \brief Load a vertex and fragment shader, create a program object, link program.
//         Errors output to log.
/// \param vertShaderSrc Vertex shader source code
/// \param fragShaderSrc Fragment shader source code
/// \return A new program object linked with the vertex/fragment shader pair, 0 on failure
//
GLuint  esLoadProgram ( const char *vertShaderSrc, const char *fragShaderSrc )
{
   GLuint vertexShader;
   GLuint fragmentShader;
   GLuint programObject;
   GLint linked;

   // Load the vertex/fragment shaders
   vertexShader = esLoadShader ( GL_VERTEX_SHADER, vertShaderSrc );
   if ( vertexShader == 0 )
      return 0;

   fragmentShader = esLoadShader ( GL_FRAGMENT_SHADER, fragShaderSrc );
   if ( fragmentShader == 0 )
   {
      glDeleteShader( vertexShader );
      return 0;
   }

   // Create the program object
   programObject = glCreateProgram ( );

   if ( programObject == 0 )
      return 0;

   glAttachShader ( programObject, vertexShader );
   glAttachShader ( programObject, fragmentShader );

   // Link the program
   glLinkProgram ( programObject );

   // Check the link status
   glGetProgramiv ( programObject, GL_LINK_STATUS, &linked );

   if ( !linked )
   {
      GLint infoLen = 0;

      glGetProgramiv ( programObject, GL_INFO_LOG_LENGTH, &infoLen );

      if ( infoLen > 1 )
      {
         char* infoLog = (char*) malloc (sizeof(char) * infoLen );

         glGetProgramInfoLog ( programObject, infoLen, NULL, infoLog );
         esLogMessage ( "Error linking program:\n%s\n", infoLog );

         free ( infoLog );
      }

      glDeleteProgram ( programObject );
      return 0;
   }

   // Free up no longer needed shader resources
   glDeleteShader ( vertexShader );
   glDeleteShader ( fragmentShader );

   return programObject;
}

static char *
esShaderLoadSource(const char *filePath)
{
        const size_t blockSize = 512;
        FILE *fp;
        char buf[blockSize];
        char *source = NULL;
        size_t tmp, sourceLength = 0;

        /* open file */
        fp = fopen(filePath, "r");
        if(!fp) {
                fprintf(stderr, "shaderLoadSource(): Unable to open %s for reading\n", filePath);
                return NULL;
        }

        /* read the entire file into a string */
        while((tmp = fread(buf, 1, blockSize, fp)) > 0) {
                char *newSource = (char*) malloc(sourceLength + tmp + 1);
                if(!newSource) {
                        fprintf(stderr, "shaderLoadSource(): malloc failed\n");
                        if(source)
                                free(source);
                        return NULL;
                }

                if(source) {
                        memcpy(newSource, source, sourceLength);
                        free(source);
                }
                memcpy(newSource + sourceLength, buf, tmp);

                source = newSource;
                sourceLength += tmp;
        }

        /* close the file and null terminate the string */
        fclose(fp);
        if(source)
                source[sourceLength] = '\0';

        return source;
}


static GLuint esShaderCompileFromFile( GLenum type, const char* filePath) {
    char* source;
    GLuint shader;
    GLint length, result;

    /* get shader source */
    source = esShaderLoadSource(filePath);
    if(!source)
        return 0;

    /* create shader object, set the source, and compile */
    shader = glCreateShader(type);
    length = strlen(source);
    glShaderSource(shader, 1, (const char **)&source, &length);
    glCompileShader(shader);
    free(source);

    /* make sure the compilation was successful */
    glGetShaderiv(shader, GL_COMPILE_STATUS, &result);
    if(result == GL_FALSE) {
        char *log;

        /* get the shader info log */
        glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &length);
        log = (char*) malloc(length);
        glGetShaderInfoLog(shader, length, &result, log);

        /* print an error message and the info log */
        fprintf(stderr, "shaderCompileFromFile(): Unable to compile %s: %s\n", filePath, log);
        free(log);

        glDeleteShader(shader);
        return 0;
    }

    return shader;
}

void
esShaderAttachFromFile(GLuint program, GLenum type, const char *filePath)
{
    /* compile the shader */
    GLuint shader = esShaderCompileFromFile(type, filePath);
    if(shader != 0) {
        /* attach the shader to the program */
        glAttachShader(program, shader);

        /* delete the shader - it won't actually be
         * destroyed until the program that it's attached
         * to has been destroyed */
        glDeleteShader(shader);
    }
}
