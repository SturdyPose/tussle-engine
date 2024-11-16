#include <GL/glew.h>
#include <windows.h>

#include <caml/alloc.h>
#include <caml/memory.h>
#include <assert.h>
#include <stdbool.h>


CAMLprim 
value something()
{
    return Val_int(50);
}

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
    return Val_bool(WithGLErrorCheck(glClearColor(Double_val(r), Double_val(g), Double_val(b), Double_val(a))));
}

CAMLprim
value gl_clear(value mask) 
{
    CAMLparam1(mask);
    return Val_bool(WithGLErrorCheck(glClear(Int_val(mask))));
}

CAMLprim
value gl_draw_arrays(value mode, value first, value count) 
{
    GLenum drawMode = (GLenum)Int_val(mode);
    GLint ifirst = (GLint)Int_val(first);
    GLsizei icount = (GLsizei)Int_val(count);
    return Val_bool(WithGLErrorCheck(glDrawArrays(drawMode, ifirst, count)));
}

CAMLprim
value gl_create_shader(value shader_type) 
{
    return Val_int(glCreateShader(Int_val(shader_type)));
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
    unsigned int buffer;
    glGenBuffers(1, &buffer);
    return Val_int(buffer);
}

CAMLprim
value gl_gen_vertex_arrays(value num_of_buffers) 
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
    unsigned int buffer;
    glGenVertexArrays(1, &buffer);
    return Val_int(buffer);
}

CAMLprim
value gl_bind_buffer(value mode, value ibuffer) 
{
    return Val_bool(WithGLErrorCheck(glBindBuffer((GLenum)Int_val(mode), (GLuint)Int_val(ibuffer))));
}

CAMLprim
value gl_bind_vertex_array(value array) 
{
    return Val_bool(WithGLErrorCheck(glBindVertexArray((GLuint)Int_val(array))));
}

CAMLprim
value gl_buffer_data(value target, value size, value data, value usage) 
{
    GLenum glTarget = (GLenum)Int_val(target);
    GLenum glUsage = (GLenum)Int_val(usage);

    const int dataTypeSize = Int_val(size);
    const size_t floatSize = sizeof(float);
    const int numOfElems = (int)(dataTypeSize/floatSize);

    glBufferData(glTarget, dataTypeSize, 0, glUsage);
    void* pBuffer = glMapBuffer(glTarget, GL_WRITE_ONLY);
    if(pBuffer == NULL)
    {
        printf("Couldn't map buffer");
        return Val_bool(false);
    }

    float* pTempBuffer = (float*)malloc(dataTypeSize);

    for(int i = 0; i < numOfElems; ++i)
    {
        pTempBuffer[i] = Double_field(data, i);
    }

    memcpy(pBuffer, pTempBuffer, dataTypeSize);
    glUnmapBuffer(glTarget);
    free(pTempBuffer);

    // TODO
    return Val_bool(1);
}


CAMLprim
value gl_shader_source(value shader, value count, value string, value length) 
{
    const int len = Int_val(length);
    const int lenCopy = 0;
    const GLsizei glCount = (GLsizei)Int_val(count);
    const char* camlSource = String_val(string);
    const char* src[] = { camlSource };

    return Val_bool(WithGLErrorCheck(glShaderSource((GLuint)Int_val(shader), glCount, &camlSource, &len)));
}

CAMLprim
value gl_shader_source_null_length(value shader, value count, value string) 
{
    const int lenCopy = 0;
    const GLsizei glCount = (GLsizei)Int_val(count);
    const char* camlSource = String_val(string);
    const char* src[] = { camlSource };

    return Val_bool(WithGLErrorCheck(glShaderSource((GLuint)Int_val(shader), glCount, &camlSource, NULL)));
}

CAMLprim
value gl_compile_shader(value shader) 
{
    GLuint uishader = (GLuint)Int_val(shader);
    bool success_compiled = Val_bool(WithGLErrorCheck(glCompileShader(uishader)));
    int success = 0;
    glGetShaderiv(uishader, GL_COMPILE_STATUS, &success);
    if(!success)
    {
        char infoLog[512];
        glGetShaderInfoLog(uishader, 512, NULL, infoLog);
        printf("Can't compile shader: %s",infoLog); 
    }
    return success_compiled;
}

CAMLprim
value gl_attach_shader(value program, value shader) 
{
    return Val_bool(WithGLErrorCheck(glAttachShader((GLuint)Int_val(program), (GLuint)Int_val(shader))));
}

CAMLprim
value gl_create_program() 
{
    unsigned int shaderProgram = glCreateProgram();
    return Val_int(shaderProgram);
}

CAMLprim
value gl_link_program(value program) 
{
    return Val_bool(WithGLErrorCheck(glLinkProgram((GLuint)Int_val(program))));
}

CAMLprim
value gl_use_program(value program) 
{
    return Val_bool(WithGLErrorCheck(glUseProgram((GLuint)Int_val(program))));
}

CAMLprim
value gl_delete_shader(value shader) 
{
    return Val_bool(WithGLErrorCheck(glDeleteShader((GLuint)Int_val(shader))));
}

// TODO: direct buffer pointers are unsupported 
CAMLprim
value gl_vertex_attrib_pointer(value index, value size, value type, value normalized, value stride) 
{
    return Val_bool(
        WithGLErrorCheck(
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
    return Val_bool(WithGLErrorCheck(glEnableVertexAttribArray((GLuint)Int_val(index))));
}

CAMLprim
value gl_draw_arrays_instanced(value mode, value first, value count, value instancecount)
{
    GLEnum iMode = (GLenum)Int_val(mode);        
    GLInt iFirst = (GLInt)Int_val(first);        
    GLsizei iCount = (GLsizei)Int_val(count);        
    GLsizei iInstanceCount = (GLsizei)Int_val(instancecount);        

    Val_bool(glDrawArraysInstanced(iMode,iFirst,iCount,iInstanceCount));
}