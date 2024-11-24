#include <GL/glew.h>
#include <windows.h>

#include <caml/alloc.h>
#include <caml/memory.h>
#include <caml/callback.h>
#include <assert.h>
#include <stdbool.h>

GLenum glCheckError_(const char *file, int line)
{
    GLenum errorCode;
    while ((errorCode = glGetError()) != GL_NO_ERROR)
    {
        char* error;
        switch (errorCode)
        {
            case GL_INVALID_ENUM:                  error = "INVALID_ENUM"; break;
            case GL_INVALID_VALUE:                 error = "INVALID_VALUE"; break;
            case GL_INVALID_OPERATION:             error = "INVALID_OPERATION"; break;
            case GL_STACK_OVERFLOW:                error = "STACK_OVERFLOW"; break;
            case GL_STACK_UNDERFLOW:               error = "STACK_UNDERFLOW"; break;
            case GL_OUT_OF_MEMORY:                 error = "OUT_OF_MEMORY"; break;
            case GL_INVALID_FRAMEBUFFER_OPERATION: error = "INVALID_FRAMEBUFFER_OPERATION"; break;
        }
        printf("error: %s, file: %s, line: %d", error, file, line);
    }
    return errorCode;
}
#define glCheckError() glCheckError_(__FILE__, __LINE__) 

CAMLprim
value glew_init()
{
	GLenum err = glewInit();
	if (GLEW_OK != err)
	{
        printf("Failed to initialize opengl with glew");
		return Val_bool(false);
	}

    return Val_bool(true);
}

void PrintOpenGLErrors(char const * const Function, char const * const File, int const Line)
{
	bool Succeeded = true;

	GLenum Error = glGetError();
	if (Error != GL_NO_ERROR)
	{
        printf("error");
	}
}


#define CheckedGLCall(x) do { PrintOpenGLErrors(">>BEFORE<< "#x, __FILE__, __LINE__); (x); PrintOpenGLErrors(#x, __FILE__, __LINE__); } while (0)

#define WithGLErrorCheck(x) ({ (x); GLenum code = glGetError(); bool hasNoError = code == GL_NO_ERROR; hasNoError;})


int get_clear_mask( value mask )
{
    int imask = Int_val(mask);
    switch (imask) {
        case 0: return GL_COLOR_BUFFER_BIT;
        case 1: return GL_DEPTH_BUFFER_BIT;
        case 2: return GL_STENCIL_BUFFER_BIT;
        default: {
            printf("Enum for this mask isn't implemented: ", Int_val(imask));
            assert(0);
            return 0;
        }
    }
} 

CAMLprim
value gl_clear_color(value r, value g, value b, value a)
{
    CAMLparam4(r,g,b,a);
    CAMLreturnT(bool, WithGLErrorCheck(glClearColor(Double_val(r), Double_val(g), Double_val(b), Double_val(a))));
}

CAMLprim
value gl_clear(value mask) 
{
    CAMLparam1(mask);
    CAMLreturn(Val_bool(WithGLErrorCheck(glClear(Int_val(mask)))));
}

CAMLprim
value gl_draw_arrays(value mode, value first, value count) 
{
    CAMLparam3(mode,first,count);
    GLenum drawMode = (GLenum)Int_val(mode);
    GLint ifirst = (GLint)Int_val(first);
    GLsizei icount = (GLsizei)Int_val(count);
    CAMLreturnT(bool, WithGLErrorCheck(glDrawArrays(drawMode, ifirst, count)));
}

CAMLprim
value gl_create_shader(value shader_type) 
{
    CAMLparam1(shader_type);
    CAMLreturn(Val_int(glCreateShader(Int_val(shader_type))));
}

#define BUFFER_ALLOCA_SIZE 16
CAMLprim
value gl_gen_buffers(value num_of_buffers) 
{
    CAMLparam1( num_of_buffers );

    int iNumberOfBuffers = Int_val(num_of_buffers);
    assert(iNumberOfBuffers > 0);
    // Completely arbitrary value for alloca
    GLuint* pBuffers = {};
    if(iNumberOfBuffers <= BUFFER_ALLOCA_SIZE)
    {
         pBuffers = (GLuint*)alloca(iNumberOfBuffers * sizeof(GLuint)) ;
    } else {
         pBuffers = (GLuint*)malloc(iNumberOfBuffers * sizeof(GLuint)) ;
    }
    assert(pBuffers != NULL);
    glGenBuffers(iNumberOfBuffers, pBuffers);

    // This is where we store the buffers
    CAMLlocal1 (camlBuffer);
    camlBuffer = caml_alloc(iNumberOfBuffers, 0); 
    for (int i = 0; i < iNumberOfBuffers; ++i)
    {
        GLuint bufferVal = pBuffers[i];
        Store_field( camlBuffer, i, Val_int((int)bufferVal));
    }

    if(iNumberOfBuffers > BUFFER_ALLOCA_SIZE)
    {
        free(pBuffers);
    } 

    CAMLreturn(camlBuffer);
}

CAMLprim
value gl_gen_buffer() 
{
    CAMLparam0();
    unsigned int buffer;
    glGenBuffers(1, &buffer);
    CAMLreturn(Val_int((int)buffer));
}

CAMLprim
value gl_gen_vertex_arrays(value num_of_buffers) 
{
    CAMLparam1( num_of_buffers );

    const int iNumberOfBuffers = Int_val(num_of_buffers);
    assert(iNumberOfBuffers > 0);
    // Completely arbitrary value for alloca
    GLuint* pBuffers = {};
    if(iNumberOfBuffers <= BUFFER_ALLOCA_SIZE)
    {
         pBuffers = (GLuint*)alloca(iNumberOfBuffers * sizeof(GLuint)) ;
    } else {
         pBuffers = (GLuint*)malloc(iNumberOfBuffers * sizeof(GLuint)) ;
    }
    assert(pBuffers != NULL);
    glGenVertexArrays(iNumberOfBuffers, pBuffers);

    // This is where we store the buffers
    CAMLlocal1 (camlBuffer);
    camlBuffer = caml_alloc(iNumberOfBuffers, 0); 
    for (int i = 0; i < iNumberOfBuffers; ++i)
    {
        GLuint bufferVal = pBuffers[i];
        Store_field( camlBuffer, i, Val_int((int)bufferVal));
    }

    if(iNumberOfBuffers > BUFFER_ALLOCA_SIZE)
    {
        free(pBuffers);
    } 

    CAMLreturn(camlBuffer);
}

CAMLprim
value gl_gen_vertex_array() 
{
    CAMLparam0();
    unsigned int buffer;
    glGenVertexArrays(1, &buffer);
    CAMLreturn(Val_int(buffer));
}

CAMLprim
value gl_bind_buffer(value mode, value ibuffer) 
{
    CAMLparam2(mode,ibuffer);
    CAMLreturnT(bool,WithGLErrorCheck(glBindBuffer((GLenum)Int_val(mode), (GLuint)Int_val(ibuffer))));
}

CAMLprim
value gl_bind_vertex_array(value array) 
{
    CAMLparam1(array);
    CAMLreturnT(bool, WithGLErrorCheck(glBindVertexArray((GLuint)Int_val(array))));
}

CAMLprim
value gl_buffer_data_float(value target, value size, value data, value usage) 
{
    CAMLparam4(target,size,data,usage);
    const GLenum glTarget = (GLenum)Int_val(target);
    const GLenum glUsage = (GLenum)Int_val(usage);

    // In ocaml getting sizeof variable is pain so we will just pass the number of elements in the buffer instead
    const int numOfElems = Int_val(size);
    const int dataTypeSize = numOfElems * sizeof(GLfloat);

    glBufferData(glTarget, dataTypeSize, 0, glUsage);

    GLfloat* pBuffer = (GLfloat*)glMapBuffer(glTarget, GL_WRITE_ONLY);
    if(pBuffer == NULL)
    {
        printf("Couldn't map buffer");
        CAMLreturn(Val_bool(false));
    }

    for(int i = 0; i < numOfElems; ++i)
    {
        pBuffer[i] = (GLfloat)Double_field(data, i);
    }

    glUnmapBuffer(glTarget);

    // TODO
    CAMLreturn(Val_bool(true)); 
}

CAMLprim
value gl_buffer_data_int(value target, value size, value data, value usage) 
{
    CAMLparam4(target,size,data,usage);
    const GLenum glTarget = (GLenum)Int_val(target);
    const GLenum glUsage = (GLenum)Int_val(usage);

    // In ocaml getting sizeof variable is pain so we will just pass the number of elements in the buffer instead
    const int numOfElems = Int_val(size);
    const int dataTypeSize = numOfElems * sizeof(GLint);

    glBufferData(glTarget, dataTypeSize, 0, glUsage);
    GLint* pBuffer = (GLint*)glMapBuffer(glTarget, GL_WRITE_ONLY);
    if(pBuffer == NULL)
    {
        printf("Couldn't map buffer");
        CAMLreturn(Val_bool(false));
    }

    for(GLint i = 0; i < numOfElems; ++i)
    {
        pBuffer[i] = Int_val(Field(data, (int)i));
    }

    glUnmapBuffer(glTarget);

    // TODO
    CAMLreturn(Val_bool(true)); 
}

CAMLprim
value gl_shader_source(value shader, value count, value string, value length) 
{
    CAMLparam4(shader,count,string,length);
    const int len = Int_val(length);
    const int lenCopy = 0;
    const GLsizei glCount = (GLsizei)Int_val(count);
    const char* camlSource = String_val(string);
    const char* src[] = { camlSource };

    CAMLreturnT(bool, WithGLErrorCheck(glShaderSource((GLuint)Int_val(shader), glCount, &camlSource, &len)));
}

CAMLprim
value gl_shader_source_null_length(value shader, value count, value string) 
{
    CAMLparam3(shader,count,string);
    const int lenCopy = 0;
    const GLsizei glCount = (GLsizei)Int_val(count);
    const char* camlSource = String_val(string);
    const char* src[] = { camlSource };

    CAMLreturnT(bool,WithGLErrorCheck(glShaderSource((GLuint)Int_val(shader), glCount, &camlSource, NULL)));
}

CAMLprim
value gl_compile_shader(value shader) 
{
    CAMLparam1(shader);
    GLuint uishader = (GLuint)Int_val(shader);
    bool success_compiled = WithGLErrorCheck(glCompileShader(uishader));
    int success = 0;
    glGetShaderiv(uishader, GL_COMPILE_STATUS, &success);
    if(!success)
    {
        char infoLog[512];
        glGetShaderInfoLog(uishader, 512, NULL, infoLog);
        printf("Can't compile shader: %s", infoLog); 
    }
    CAMLreturnT(bool, success_compiled);
}

CAMLprim
value gl_attach_shader(value program, value shader) 
{
    CAMLparam2(program,shader);
    CAMLreturnT(bool, WithGLErrorCheck(glAttachShader((GLuint)Int_val(program), (GLuint)Int_val(shader))));
}

CAMLprim
value gl_create_program() 
{
    CAMLparam0();
    const unsigned int shaderProgram = glCreateProgram();
    CAMLreturn(Val_int((int)shaderProgram));
}

CAMLprim
value gl_link_program(value program) 
{
    CAMLparam1(program);
    CAMLreturnT(bool,WithGLErrorCheck(glLinkProgram((GLuint)Int_val(program))));
}

CAMLprim
value gl_use_program(value program) 
{
    CAMLparam1(program);
    CAMLreturnT(bool, WithGLErrorCheck(glUseProgram((GLuint)Int_val(program))));
}

CAMLprim
value gl_delete_shader(value shader) 
{
    CAMLparam1(shader);
    CAMLreturnT(bool,WithGLErrorCheck(glDeleteShader((GLuint)Int_val(shader))));
}

// TODO: direct buffer pointers are unsupported 
CAMLprim
value gl_vertex_attrib_pointer(value index, value size, value type, value normalized, value stride) 
{
    CAMLparam5(index,size,type,normalized,stride);
    CAMLreturnT(bool, WithGLErrorCheck(
            glVertexAttribPointer(
                (GLuint)Int_val(index),
                (GLint)Int_val(size),
                (GLenum)Int_val(type),
                (GLboolean)Bool_val(normalized),
                (GLsizei)Int_val(stride),
                (const void*)0
                )));
}

CAMLprim
value gl_vertex_attrib_pointer_params(value * argv, int /*argn*/)
{
    return gl_vertex_attrib_pointer(argv[0],argv[1],argv[2],argv[3],argv[4]);
} 

CAMLprim
value gl_enable_vertex_attrib_array(value index)
{
    CAMLparam1(index);
    glEnableVertexAttribArray((GLuint)Int_val(index));
    CAMLreturn(Val_unit);
}

CAMLprim
value gl_draw_arrays_instanced(value mode, value first, value count, value instancecount)
{
    CAMLparam4(mode,first,count,instancecount);
    const GLenum iMode = (GLenum)Int_val(mode);        
    const GLint iFirst = (GLint)Int_val(first);        
    const GLsizei iCount = (GLsizei)Int_val(count);        
    const GLsizei iInstanceCount = (GLsizei)Int_val(instancecount);        

    CAMLreturnT(bool, WithGLErrorCheck(glDrawArraysInstanced(iMode,iFirst,iCount,iInstanceCount)));
}

CAMLprim
value gl_draw_elements(value mode, value count, value type, value indices)
{
    CAMLparam4(mode,count,type,indices);
    const GLsizei iCount = (GLsizei)Int_val(count);        
    if(iCount == 0)
    {
        return Val_bool(true);
    }

    const GLenum iMode = (GLenum)Int_val(mode);        
    const GLenum iType = (GLenum)Int_val(type);

    unsigned int buff;
    glGenBuffers(1,&buff); 
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buff);

    switch(iType)
    {
        case GL_UNSIGNED_BYTE:
        {
            glBufferData(GL_ELEMENT_ARRAY_BUFFER, iCount * sizeof(GLbyte), 0, GL_STATIC_DRAW);
            GLbyte* pBuffer = (GLbyte*)glMapBuffer(GL_ELEMENT_ARRAY_BUFFER, GL_WRITE_ONLY);

            for(int i = 0; i < iCount; ++i)
            {
                pBuffer[i] = (GLbyte)Int_val(Field(indices, i));
            }
            break;
        }
        case GL_UNSIGNED_SHORT:
        {
            glBufferData(GL_ELEMENT_ARRAY_BUFFER, iCount * sizeof(GLshort), 0, GL_STATIC_DRAW);
            GLshort* pBuffer = (GLshort*)glMapBuffer(GL_ELEMENT_ARRAY_BUFFER, GL_WRITE_ONLY);

            for(int i = 0; i < iCount; ++i)
            {
                pBuffer[i] = (GLshort)Int_val(Field(indices, i));
            }
            break;
        }
        case GL_UNSIGNED_INT:
        {
            glBufferData(GL_ELEMENT_ARRAY_BUFFER, iCount * sizeof(GLuint), 0, GL_STATIC_DRAW);
            GLuint* pBuffer = (GLuint*)glMapBuffer(GL_ELEMENT_ARRAY_BUFFER, GL_WRITE_ONLY);

            for(int i = 0; i < iCount; ++i)
            {
                pBuffer[i] = (GLuint)Int_val(Field(indices, i));
            }
            break;
        }
        default:
        {
            printf("gl_draw_elements doesn't accept this type: %d \n", iType);
            return Val_bool(false);
        }
    }

    glUnmapBuffer(GL_ELEMENT_ARRAY_BUFFER);
    bool res = WithGLErrorCheck(glDrawElements(iMode,iCount, iType, NULL));
    glDeleteBuffers(1, &buff);
    CAMLreturnT(bool, res);
}

CAMLprim
value gl_get_uniform_block_index(value program, value uniformBlockName)
{
    CAMLparam2(program, uniformBlockName);
    const GLuint uprogram = (GLuint)Int_val(program);        
    const char* uniformBlockNameStr = String_val(uniformBlockName);
    const GLuint v = glGetUniformBlockIndex(uprogram, uniformBlockNameStr);
    CAMLreturn(Val_int((int)v));
}

CAMLprim
value gl_uniform_block_binding(value program, value uniformBlockIndex, value uniformBlockBinding)
{
    CAMLparam3(program,uniformBlockIndex, uniformBlockBinding);
    const GLuint uprogram = (GLuint)Int_val(program);        
    const GLuint uuniformBlockIndex = (GLuint)Int_val(uniformBlockIndex);
    const GLuint uuniformBlockBinding = (GLuint)Int_val(uniformBlockBinding);
    CAMLreturnT(bool, WithGLErrorCheck(glUniformBlockBinding(uprogram,uuniformBlockIndex, uuniformBlockBinding)));
}

CAMLprim
value gl_bind_buffer_base(value target, value index, value buffer)
{
    CAMLparam3(target,index,buffer);
    const GLenum enumTarget = (GLenum)Int_val(target);
    const GLuint uIndex = (GLuint)Int_val(index);
    const GLuint uBuffer = (GLuint)Int_val(buffer);
    CAMLreturn(Val_bool(WithGLErrorCheck(glBindBufferBase(enumTarget, uIndex, uBuffer))));
}

CAMLprim
value gl_buffer_sub_data_float(value target, value offset, value size, value data)
{
    CAMLparam4(target,offset,size,data);
    const GLenum glTarget = (GLenum)Int_val(target);
    const GLintptr uOffset = (GLintptr)Int_val(offset);
    const GLsizeiptr numOfElems = (GLsizeiptr)Int_val(size);

    GLfloat* pBuffer = (GLfloat*)glMapBuffer(glTarget, GL_WRITE_ONLY);
    if(pBuffer == NULL)
    {
        printf("Couldn't map buffer");
        CAMLreturn(Val_bool(false));
    }

    for(int i = uOffset; i < numOfElems; ++i)
    {
        pBuffer[i] = (GLfloat)Double_field(data, i);
    }

    glUnmapBuffer(glTarget);

    // TODO
    CAMLreturn(Val_bool(1));
}

#define MAPWRITER(type, converter) {                                  \
    type* pBuffer = (type*)glMapBuffer(glTarget, GL_WRITE_ONLY);      \
    if(pBuffer == NULL)                                               \
    {                                                                 \
        printf("Couldn't map buffer\n");                              \
        return Val_bool(false);                                       \
    }                                                                 \
    int iteration = 0;                                                \
    while(true)                                                       \
    {                                                                 \
        result = caml_callback(writeCallback, Val_int(iteration));    \
        bool finished = Bool_val(Field(result, 0));                   \
        int offsetWrite = Int_val(Field(result, 1));                  \
        type writeValue = (type)converter(Field(result, 2));          \
        *(pBuffer + offsetWrite) = writeValue;                        \
        ++pBuffer;                                                    \
        ++iteration;                                                  \
        if(finished)                                                  \
        {                                                             \
            break;                                                    \
        }                                                             \
    }                                                                 \
}

// GL map buffer but using ocaml's callbacks
CAMLprim
value ocaml_gl_map_write_buffer(value target, value type, value writeCallback)
{
    CAMLparam3(target,type,writeCallback);
    CAMLlocal1(result);
    GLenum glTarget = (GLenum)Int_val(target);
    GLenum enumType = (GLenum)Int_val(type);

    switch(enumType)
    {
        case GL_UNSIGNED_BYTE:
        {
            MAPWRITER(GLbyte, Int_val);
            break;
        }
        case GL_UNSIGNED_SHORT:
        {
            MAPWRITER(GLshort, Int_val);
            break;
        }
        case GL_UNSIGNED_INT:
        {
            MAPWRITER(GLuint, Int_val);
            break;
        }
        case GL_FLOAT:
        {
            MAPWRITER(GLfloat, Double_val);
            break;
        }
        default:
        {
            printf("ocaml_gl_map_write_buffer doesn't accept this type: %d \n", enumType);
            CAMLreturn(Val_bool(false));
        }
    }
    glUnmapBuffer(glTarget);

    // TODO
    CAMLreturn(Val_bool(true));
} 