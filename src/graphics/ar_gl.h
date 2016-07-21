#ifndef AR_GL_INCLUDED
#define AR_GL_INCLUDED

#include <stddef.h>
#include <GL/gl.h>

/*
#define GL_FALSE 0
#define GL_TRUE 1
#define GL_ZERO 0
#define GL_ONE 1
#define GL_NONE 0
#define GL_NO_ERROR 0
*/

#define GL_DEPTH_BUFFER_BIT 0x00000100
#define GL_STENCIL_BUFFER_BIT 0x00000400
#define GL_COLOR_BUFFER_BIT 0x00004000
#define GL_POINTS 0x0000
#define GL_LINES 0x0001
#define GL_LINE_LOOP 0x0002
#define GL_LINE_STRIP 0x0003
#define GL_TRIANGLES 0x0004
#define GL_TRIANGLE_STRIP 0x0005
#define GL_TRIANGLE_FAN 0x0006
#define GL_SRC_COLOR 0x0300
#define GL_ONE_MINUS_SRC_COLOR 0x0301
#define GL_SRC_ALPHA 0x0302
#define GL_ONE_MINUS_SRC_ALPHA 0x0303
#define GL_DST_ALPHA 0x0304
#define GL_ONE_MINUS_DST_ALPHA 0x0305
#define GL_DST_COLOR 0x0306
#define GL_ONE_MINUS_DST_COLOR 0x0307
#define GL_SRC_ALPHA_SATURATE 0x0308
#define GL_FUNC_ADD 0x8006
#define GL_BLEND_EQUATION 0x8009
#define GL_BLEND_EQUATION_RGB 0x8009
#define GL_BLEND_EQUATION_ALPHA 0x883D
#define GL_FUNC_SUBTRACT 0x800A
#define GL_FUNC_REVERSE_SUBTRACT 0x800B
#define GL_BLEND_DST_RGB 0x80C8
#define GL_BLEND_SRC_RGB 0x80C9
#define GL_BLEND_DST_ALPHA 0x80CA
#define GL_BLEND_SRC_ALPHA 0x80CB
#define GL_CONSTANT_COLOR 0x8001
#define GL_ONE_MINUS_CONSTANT_COLOR 0x8002
#define GL_CONSTANT_ALPHA 0x8003
#define GL_ONE_MINUS_CONSTANT_ALPHA 0x8004
#define GL_BLEND_COLOR 0x8005
#define GL_ARRAY_BUFFER 0x8892
#define GL_ELEMENT_ARRAY_BUFFER 0x8893
#define GL_ARRAY_BUFFER_BINDING 0x8894
#define GL_ELEMENT_ARRAY_BUFFER_BINDING 0x8895
#define GL_STREAM_DRAW 0x88E0
#define GL_STATIC_DRAW 0x88E4
#define GL_DYNAMIC_DRAW 0x88E8
#define GL_BUFFER_SIZE 0x8764
#define GL_BUFFER_USAGE 0x8765
#define GL_CURRENT_VERTEX_ATTRIB 0x8626
#define GL_FRONT 0x0404
#define GL_BACK 0x0405
#define GL_FRONT_AND_BACK 0x0408
#define GL_TEXTURE_2D 0x0DE1
#define GL_CULL_FACE 0x0B44
#define GL_BLEND 0x0BE2
#define GL_DITHER 0x0BD0
#define GL_STENCIL_TEST 0x0B90
#define GL_DEPTH_TEST 0x0B71
#define GL_SCISSOR_TEST 0x0C11
#define GL_POLYGON_OFFSET_FILL 0x8037
#define GL_SAMPLE_ALPHA_TO_COVERAGE 0x809E
#define GL_SAMPLE_COVERAGE 0x80A0
#define GL_INVALID_ENUM 0x0500
#define GL_INVALID_VALUE 0x0501
#define GL_INVALID_OPERATION 0x0502
#define GL_OUT_OF_MEMORY 0x0505
#define GL_CW 0x0900
#define GL_CCW 0x0901
#define GL_LINE_WIDTH 0x0B21
#define GL_ALIASED_POINT_SIZE_RANGE 0x846D
#define GL_ALIASED_LINE_WIDTH_RANGE 0x846E
#define GL_CULL_FACE_MODE 0x0B45
#define GL_FRONT_FACE 0x0B46
#define GL_DEPTH_RANGE 0x0B70
#define GL_DEPTH_WRITEMASK 0x0B72
#define GL_DEPTH_CLEAR_VALUE 0x0B73
#define GL_DEPTH_FUNC 0x0B74
#define GL_STENCIL_CLEAR_VALUE 0x0B91
#define GL_STENCIL_FUNC 0x0B92
#define GL_STENCIL_FAIL 0x0B94
#define GL_STENCIL_PASS_DEPTH_FAIL 0x0B95
#define GL_STENCIL_PASS_DEPTH_PASS 0x0B96
#define GL_STENCIL_REF 0x0B97
#define GL_STENCIL_VALUE_MASK 0x0B93
#define GL_STENCIL_WRITEMASK 0x0B98
#define GL_STENCIL_BACK_FUNC 0x8800
#define GL_STENCIL_BACK_FAIL 0x8801
#define GL_STENCIL_BACK_PASS_DEPTH_FAIL 0x8802
#define GL_STENCIL_BACK_PASS_DEPTH_PASS 0x8803
#define GL_STENCIL_BACK_REF 0x8CA3
#define GL_STENCIL_BACK_VALUE_MASK 0x8CA4
#define GL_STENCIL_BACK_WRITEMASK 0x8CA5
#define GL_VIEWPORT 0x0BA2
#define GL_SCISSOR_BOX 0x0C10
#define GL_COLOR_CLEAR_VALUE 0x0C22
#define GL_COLOR_WRITEMASK 0x0C23
#define GL_UNPACK_ALIGNMENT 0x0CF5
#define GL_PACK_ALIGNMENT 0x0D05
#define GL_MAX_TEXTURE_SIZE 0x0D33
#define GL_MAX_VIEWPORT_DIMS 0x0D3A
#define GL_SUBPIXEL_BITS 0x0D50
#define GL_RED_BITS 0x0D52
#define GL_GREEN_BITS 0x0D53
#define GL_BLUE_BITS 0x0D54
#define GL_ALPHA_BITS 0x0D55
#define GL_DEPTH_BITS 0x0D56
#define GL_STENCIL_BITS 0x0D57
#define GL_POLYGON_OFFSET_UNITS 0x2A00
#define GL_POLYGON_OFFSET_FACTOR 0x8038
#define GL_TEXTURE_BINDING_2D 0x8069
#define GL_SAMPLE_BUFFERS 0x80A8
#define GL_SAMPLES 0x80A9
#define GL_SAMPLE_COVERAGE_VALUE 0x80AA
#define GL_SAMPLE_COVERAGE_INVERT 0x80AB
#define GL_NUM_COMPRESSED_TEXTURE_FORMATS 0x86A2
#define GL_COMPRESSED_TEXTURE_FORMATS 0x86A3
#define GL_DONT_CARE 0x1100
#define GL_FASTEST 0x1101
#define GL_NICEST 0x1102
#define GL_GENERATE_MIPMAP_HINT 0x8192
#define GL_BYTE 0x1400
#define GL_UNSIGNED_BYTE 0x1401
#define GL_SHORT 0x1402
#define GL_UNSIGNED_SHORT 0x1403
#define GL_INT 0x1404
#define GL_UNSIGNED_INT 0x1405
#define GL_FLOAT 0x1406
#define GL_FIXED 0x140C
#define GL_DEPTH_COMPONENT 0x1902
#define GL_ALPHA 0x1906
#define GL_RGB 0x1907
#define GL_RGBA 0x1908
#define GL_LUMINANCE 0x1909
#define GL_LUMINANCE_ALPHA 0x190A
#define GL_UNSIGNED_SHORT_4_4_4_4 0x8033
#define GL_UNSIGNED_SHORT_5_5_5_1 0x8034
#define GL_UNSIGNED_SHORT_5_6_5 0x8363
#define GL_FRAGMENT_SHADER 0x8B30
#define GL_VERTEX_SHADER 0x8B31
#define GL_MAX_VERTEX_ATTRIBS 0x8869
#define GL_MAX_VERTEX_UNIFORM_VECTORS 0x8DFB
#define GL_MAX_VARYING_VECTORS 0x8DFC
#define GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS 0x8B4D
#define GL_MAX_VERTEX_TEXTURE_IMAGE_UNITS 0x8B4C
#define GL_MAX_TEXTURE_IMAGE_UNITS 0x8872
#define GL_MAX_FRAGMENT_UNIFORM_VECTORS 0x8DFD
#define GL_SHADER_TYPE 0x8B4F
#define GL_DELETE_STATUS 0x8B80
#define GL_LINK_STATUS 0x8B82
#define GL_VALIDATE_STATUS 0x8B83
#define GL_ATTACHED_SHADERS 0x8B85
#define GL_ACTIVE_UNIFORMS 0x8B86
#define GL_ACTIVE_UNIFORM_MAX_LENGTH 0x8B87
#define GL_ACTIVE_ATTRIBUTES 0x8B89
#define GL_ACTIVE_ATTRIBUTE_MAX_LENGTH 0x8B8A
#define GL_SHADING_LANGUAGE_VERSION 0x8B8C
#define GL_CURRENT_PROGRAM 0x8B8D
#define GL_NEVER 0x0200
#define GL_LESS 0x0201
#define GL_EQUAL 0x0202
#define GL_LEQUAL 0x0203
#define GL_GREATER 0x0204
#define GL_NOTEQUAL 0x0205
#define GL_GEQUAL 0x0206
#define GL_ALWAYS 0x0207
#define GL_KEEP 0x1E00
#define GL_REPLACE 0x1E01
#define GL_INCR 0x1E02
#define GL_DECR 0x1E03
#define GL_INVERT 0x150A
#define GL_INCR_WRAP 0x8507
#define GL_DECR_WRAP 0x8508
#define GL_VENDOR 0x1F00
#define GL_RENDERER 0x1F01
#define GL_VERSION 0x1F02
#define GL_EXTENSIONS 0x1F03
#define GL_NEAREST 0x2600
#define GL_LINEAR 0x2601
#define GL_NEAREST_MIPMAP_NEAREST 0x2700
#define GL_LINEAR_MIPMAP_NEAREST 0x2701
#define GL_NEAREST_MIPMAP_LINEAR 0x2702
#define GL_LINEAR_MIPMAP_LINEAR 0x2703
#define GL_TEXTURE_MAG_FILTER 0x2800
#define GL_TEXTURE_MIN_FILTER 0x2801
#define GL_TEXTURE_WRAP_S 0x2802
#define GL_TEXTURE_WRAP_T 0x2803
#define GL_TEXTURE 0x1702
#define GL_TEXTURE_CUBE_MAP 0x8513
#define GL_TEXTURE_BINDING_CUBE_MAP 0x8514
#define GL_TEXTURE_CUBE_MAP_POSITIVE_X 0x8515
#define GL_TEXTURE_CUBE_MAP_NEGATIVE_X 0x8516
#define GL_TEXTURE_CUBE_MAP_POSITIVE_Y 0x8517
#define GL_TEXTURE_CUBE_MAP_NEGATIVE_Y 0x8518
#define GL_TEXTURE_CUBE_MAP_POSITIVE_Z 0x8519
#define GL_TEXTURE_CUBE_MAP_NEGATIVE_Z 0x851A
#define GL_MAX_CUBE_MAP_TEXTURE_SIZE 0x851C
#define GL_TEXTURE0 0x84C0
#define GL_TEXTURE1 0x84C1
#define GL_TEXTURE2 0x84C2
#define GL_TEXTURE3 0x84C3
#define GL_TEXTURE4 0x84C4
#define GL_TEXTURE5 0x84C5
#define GL_TEXTURE6 0x84C6
#define GL_TEXTURE7 0x84C7
#define GL_TEXTURE8 0x84C8
#define GL_TEXTURE9 0x84C9
#define GL_TEXTURE10 0x84CA
#define GL_TEXTURE11 0x84CB
#define GL_TEXTURE12 0x84CC
#define GL_TEXTURE13 0x84CD
#define GL_TEXTURE14 0x84CE
#define GL_TEXTURE15 0x84CF
#define GL_TEXTURE16 0x84D0
#define GL_TEXTURE17 0x84D1
#define GL_TEXTURE18 0x84D2
#define GL_TEXTURE19 0x84D3
#define GL_TEXTURE20 0x84D4
#define GL_TEXTURE21 0x84D5
#define GL_TEXTURE22 0x84D6
#define GL_TEXTURE23 0x84D7
#define GL_TEXTURE24 0x84D8
#define GL_TEXTURE25 0x84D9
#define GL_TEXTURE26 0x84DA
#define GL_TEXTURE27 0x84DB
#define GL_TEXTURE28 0x84DC
#define GL_TEXTURE29 0x84DD
#define GL_TEXTURE30 0x84DE
#define GL_TEXTURE31 0x84DF
#define GL_ACTIVE_TEXTURE 0x84E0
#define GL_REPEAT 0x2901
#define GL_CLAMP_TO_EDGE 0x812F
#define GL_MIRRORED_REPEAT 0x8370
#define GL_FLOAT_VEC2 0x8B50
#define GL_FLOAT_VEC3 0x8B51
#define GL_FLOAT_VEC4 0x8B52
#define GL_INT_VEC2 0x8B53
#define GL_INT_VEC3 0x8B54
#define GL_INT_VEC4 0x8B55
#define GL_BOOL 0x8B56
#define GL_BOOL_VEC2 0x8B57
#define GL_BOOL_VEC3 0x8B58
#define GL_BOOL_VEC4 0x8B59
#define GL_FLOAT_MAT2 0x8B5A
#define GL_FLOAT_MAT3 0x8B5B
#define GL_FLOAT_MAT4 0x8B5C
#define GL_SAMPLER_2D 0x8B5E
#define GL_SAMPLER_CUBE 0x8B60
#define GL_VERTEX_ATTRIB_ARRAY_ENABLED 0x8622
#define GL_VERTEX_ATTRIB_ARRAY_SIZE 0x8623
#define GL_VERTEX_ATTRIB_ARRAY_STRIDE 0x8624
#define GL_VERTEX_ATTRIB_ARRAY_TYPE 0x8625
#define GL_VERTEX_ATTRIB_ARRAY_NORMALIZED 0x886A
#define GL_VERTEX_ATTRIB_ARRAY_POINTER 0x8645
#define GL_VERTEX_ATTRIB_ARRAY_BUFFER_BINDING 0x889F
#define GL_IMPLEMENTATION_COLOR_READ_TYPE 0x8B9A
#define GL_IMPLEMENTATION_COLOR_READ_FORMAT 0x8B9B
#define GL_COMPILE_STATUS 0x8B81
#define GL_INFO_LOG_LENGTH 0x8B84
#define GL_SHADER_SOURCE_LENGTH 0x8B88
#define GL_SHADER_COMPILER 0x8DFA
#define GL_SHADER_BINARY_FORMATS 0x8DF8
#define GL_NUM_SHADER_BINARY_FORMATS 0x8DF9
#define GL_LOW_FLOAT 0x8DF0
#define GL_MEDIUM_FLOAT 0x8DF1
#define GL_HIGH_FLOAT 0x8DF2
#define GL_LOW_INT 0x8DF3
#define GL_MEDIUM_INT 0x8DF4
#define GL_HIGH_INT 0x8DF5
#define GL_FRAMEBUFFER 0x8D40
#define GL_RENDERBUFFER 0x8D41
#define GL_RGBA4 0x8056
#define GL_RGB5_A1 0x8057
#define GL_RGB565 0x8D62
#define GL_DEPTH_COMPONENT16 0x81A5
#define GL_STENCIL_INDEX8 0x8D48
#define GL_RENDERBUFFER_WIDTH 0x8D42
#define GL_RENDERBUFFER_HEIGHT 0x8D43
#define GL_RENDERBUFFER_INTERNAL_FORMAT 0x8D44
#define GL_RENDERBUFFER_RED_SIZE 0x8D50
#define GL_RENDERBUFFER_GREEN_SIZE 0x8D51
#define GL_RENDERBUFFER_BLUE_SIZE 0x8D52
#define GL_RENDERBUFFER_ALPHA_SIZE 0x8D53
#define GL_RENDERBUFFER_DEPTH_SIZE 0x8D54
#define GL_RENDERBUFFER_STENCIL_SIZE 0x8D55
#define GL_FRAMEBUFFER_ATTACHMENT_OBJECT_TYPE 0x8CD0
#define GL_FRAMEBUFFER_ATTACHMENT_OBJECT_NAME 0x8CD1
#define GL_FRAMEBUFFER_ATTACHMENT_TEXTURE_LEVEL 0x8CD2
#define GL_FRAMEBUFFER_ATTACHMENT_TEXTURE_CUBE_MAP_FACE 0x8CD3
#define GL_COLOR_ATTACHMENT0 0x8CE0
#define GL_DEPTH_ATTACHMENT 0x8D00
#define GL_STENCIL_ATTACHMENT 0x8D20
#define GL_FRAMEBUFFER_COMPLETE 0x8CD5
#define GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT 0x8CD6
#define GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT 0x8CD7
#define GL_FRAMEBUFFER_INCOMPLETE_DIMENSIONS 0x8CD9
#define GL_FRAMEBUFFER_UNSUPPORTED 0x8CDD
#define GL_FRAMEBUFFER_BINDING 0x8CA6
#define GL_RENDERBUFFER_BINDING 0x8CA7
#define GL_MAX_RENDERBUFFER_SIZE 0x84E8
#define GL_INVALID_FRAMEBUFFER_OPERATION 0x0506

#ifdef _WIN32

#define AR_APIENTRYP __stdcall*
#define AR_GLAPI extern

typedef ptrdiff_t GLsizeiptr;
typedef ptrdiff_t GLintptr;
typedef char GLchar;
typedef unsigned int GLenum;
typedef int GLint;
typedef unsigned int GLuint;
typedef int GLsizei;
typedef float GLfloat;
typedef unsigned int GLbitfield;
typedef unsigned char GLboolean;
typedef unsigned char GLubyte;

typedef void (AR_APIENTRYP PFNGLACTIVETEXTUREPROC)(GLenum texture);
typedef void (AR_APIENTRYP PFNGLATTACHSHADERPROC)(GLuint program, GLuint shader);
typedef void (AR_APIENTRYP PFNGLBINDATTRIBLOCATIONPROC)(GLuint program, GLuint index, const GLchar* name);
typedef void (AR_APIENTRYP PFNGLBINDBUFFERPROC)(GLenum target, GLuint buffer);
typedef void (AR_APIENTRYP PFNGLBINDFRAMEBUFFERPROC)(GLenum target, GLuint framebuffer);
typedef void (AR_APIENTRYP PFNGLBINDRENDERBUFFERPROC)(GLenum target, GLuint renderbuffer);
typedef void (AR_APIENTRYP PFNGLBINDTEXTUREPROC)(GLenum target, GLuint texture);
typedef void (AR_APIENTRYP PFNGLBLENDCOLORPROC)(GLfloat red, GLfloat green, GLfloat blue, GLfloat alpha);
typedef void (AR_APIENTRYP PFNGLBLENDEQUATIONPROC)(GLenum mode);
typedef void (AR_APIENTRYP PFNGLBLENDEQUATIONSEPARATEPROC)(GLenum modeRGB, GLenum modeAlpha);
typedef void (AR_APIENTRYP PFNGLBLENDFUNCPROC)(GLenum sfactor, GLenum dfactor);
typedef void (AR_APIENTRYP PFNGLBLENDFUNCSEPARATEPROC)(GLenum sfactorRGB, GLenum dfactorRGB, GLenum sfactorAlpha, GLenum dfactorAlpha);
typedef void (AR_APIENTRYP PFNGLBUFFERDATAPROC)(GLenum target, GLsizeiptr size, const void* data, GLenum usage);
typedef void (AR_APIENTRYP PFNGLBUFFERSUBDATAPROC)(GLenum target, GLintptr offset, GLsizeiptr size, const void* data);
typedef GLenum (AR_APIENTRYP PFNGLCHECKFRAMEBUFFERSTATUSPROC)(GLenum target);
typedef void (AR_APIENTRYP PFNGLCLEARCOLORPROC)(GLfloat red, GLfloat green, GLfloat blue, GLfloat alpha);
typedef void (AR_APIENTRYP PFNGLCLEARDEPTHFPROC)(GLfloat d);
typedef void (AR_APIENTRYP PFNGLCLEARPROC)(GLbitfield mask);
typedef void (AR_APIENTRYP PFNGLCLEARSTENCILPROC)(GLint s);
typedef void (AR_APIENTRYP PFNGLCOLORMASKPROC)(GLboolean red, GLboolean green, GLboolean blue, GLboolean alpha);
typedef void (AR_APIENTRYP PFNGLCOMPILESHADERPROC)(GLuint shader);
typedef void (AR_APIENTRYP PFNGLCOMPRESSEDTEXIMAGE2DPROC)(GLenum target, GLint level, GLenum internalformat, GLsizei width, GLsizei height, GLint border, GLsizei imageSize, const void* data);
typedef void (AR_APIENTRYP PFNGLCOMPRESSEDTEXSUBIMAGE2DPROC)(GLenum target, GLint level, GLint xoffset, GLint yoffset, GLsizei width, GLsizei height, GLenum format, GLsizei imageSize, const void* data);
typedef void (AR_APIENTRYP PFNGLCOPYTEXIMAGE2DPROC)(GLenum target, GLint level, GLenum internalformat, GLint x, GLint y, GLsizei width, GLsizei height, GLint border);
typedef void (AR_APIENTRYP PFNGLCOPYTEXSUBIMAGE2DPROC)(GLenum target, GLint level, GLint xoffset, GLint yoffset, GLint x, GLint y, GLsizei width, GLsizei height);
typedef GLuint (AR_APIENTRYP PFNGLCREATEPROGRAMPROC)();
typedef GLuint (AR_APIENTRYP PFNGLCREATESHADERPROC)(GLenum type);
typedef void (AR_APIENTRYP PFNGLCULLFACEPROC)(GLenum mode);
typedef void (AR_APIENTRYP PFNGLDELETEBUFFERSPROC)(GLsizei n, const GLuint* buffers);
typedef void (AR_APIENTRYP PFNGLDELETEFRAMEBUFFERSPROC)(GLsizei n, const GLuint* framebuffers);
typedef void (AR_APIENTRYP PFNGLDELETEPROGRAMPROC)(GLuint program);
typedef void (AR_APIENTRYP PFNGLDELETERENDERBUFFERSPROC)(GLsizei n, const GLuint* renderbuffers);
typedef void (AR_APIENTRYP PFNGLDELETESHADERPROC)(GLuint shader);
typedef void (AR_APIENTRYP PFNGLDELETETEXTURESPROC)(GLsizei n, const GLuint* textures);
typedef void (AR_APIENTRYP PFNGLDEPTHFUNCPROC)(GLenum func);
typedef void (AR_APIENTRYP PFNGLDEPTHMASKPROC)(GLboolean flag);
typedef void (AR_APIENTRYP PFNGLDEPTHRANGEFPROC)(GLfloat n, GLfloat f);
typedef void (AR_APIENTRYP PFNGLDETACHSHADERPROC)(GLuint program, GLuint shader);
typedef void (AR_APIENTRYP PFNGLDISABLEPROC)(GLenum cap);
typedef void (AR_APIENTRYP PFNGLDISABLEVERTEXATTRIBARRAYPROC)(GLuint index);
typedef void (AR_APIENTRYP PFNGLDRAWARRAYSPROC)(GLenum mode, GLint first, GLsizei count);
typedef void (AR_APIENTRYP PFNGLDRAWELEMENTSPROC)(GLenum mode, GLsizei count, GLenum type, const void* indices);
typedef void (AR_APIENTRYP PFNGLENABLEPROC)(GLenum cap);
typedef void (AR_APIENTRYP PFNGLENABLEVERTEXATTRIBARRAYPROC)(GLuint index);
typedef void (AR_APIENTRYP PFNGLFINISHPROC)();
typedef void (AR_APIENTRYP PFNGLFLUSHPROC)();
typedef void (AR_APIENTRYP PFNGLFRAMEBUFFERRENDERBUFFERPROC)(GLenum target, GLenum attachment, GLenum renderbuffertarget, GLuint renderbuffer);
typedef void (AR_APIENTRYP PFNGLFRAMEBUFFERTEXTURE2DPROC)(GLenum target, GLenum attachment, GLenum textarget, GLuint texture, GLint level);
typedef void (AR_APIENTRYP PFNGLFRONTFACEPROC)(GLenum mode);
typedef void (AR_APIENTRYP PFNGLGENBUFFERSPROC)(GLsizei n, GLuint* buffers);
typedef void (AR_APIENTRYP PFNGLGENERATEMIPMAPPROC)(GLenum target);
typedef void (AR_APIENTRYP PFNGLGENFRAMEBUFFERSPROC)(GLsizei n, GLuint* framebuffers);
typedef void (AR_APIENTRYP PFNGLGENRENDERBUFFERSPROC)(GLsizei n, GLuint* renderbuffers);
typedef void (AR_APIENTRYP PFNGLGENTEXTURESPROC)(GLsizei n, GLuint* textures);
typedef void (AR_APIENTRYP PFNGLGETACTIVEATTRIBPROC)(GLuint program, GLuint index, GLsizei bufSize, GLsizei* length, GLint* size, GLenum* type, GLchar* name);
typedef void (AR_APIENTRYP PFNGLGETACTIVEUNIFORMPROC)(GLuint program, GLuint index, GLsizei bufSize, GLsizei* length, GLint* size, GLenum* type, GLchar* name);
typedef void (AR_APIENTRYP PFNGLGETATTACHEDSHADERSPROC)(GLuint program, GLsizei maxCount, GLsizei* count, GLuint* shaders);
typedef GLint (AR_APIENTRYP PFNGLGETATTRIBLOCATIONPROC)(GLuint program, const GLchar* name);
typedef void (AR_APIENTRYP PFNGLGETBOOLEANVPROC)(GLenum pname, GLboolean* data);
typedef void (AR_APIENTRYP PFNGLGETBUFFERPARAMETERIVPROC)(GLenum target, GLenum pname, GLint* params);
typedef GLenum (AR_APIENTRYP PFNGLGETERRORPROC)();
typedef void (AR_APIENTRYP PFNGLGETFLOATVPROC)(GLenum pname, GLfloat* data);
typedef void (AR_APIENTRYP PFNGLGETFRAMEBUFFERATTACHMENTPARAMETERIVPROC)(GLenum target, GLenum attachment, GLenum pname, GLint* params);
typedef void (AR_APIENTRYP PFNGLGETINTEGERVPROC)(GLenum pname, GLint* data);
typedef void (AR_APIENTRYP PFNGLGETPROGRAMINFOLOGPROC)(GLuint program, GLsizei bufSize, GLsizei* length, GLchar* infoLog);
typedef void (AR_APIENTRYP PFNGLGETPROGRAMIVPROC)(GLuint program, GLenum pname, GLint* params);
typedef void (AR_APIENTRYP PFNGLGETRENDERBUFFERPARAMETERIVPROC)(GLenum target, GLenum pname, GLint* params);
typedef void (AR_APIENTRYP PFNGLGETSHADERINFOLOGPROC)(GLuint shader, GLsizei bufSize, GLsizei* length, GLchar* infoLog);
typedef void (AR_APIENTRYP PFNGLGETSHADERIVPROC)(GLuint shader, GLenum pname, GLint* params);
typedef void (AR_APIENTRYP PFNGLGETSHADERPRECISIONFORMATPROC)(GLenum shadertype, GLenum precisiontype, GLint* range, GLint* precision);
typedef void (AR_APIENTRYP PFNGLGETSHADERSOURCEPROC)(GLuint shader, GLsizei bufSize, GLsizei* length, GLchar* source);
typedef const GLubyte* (AR_APIENTRYP PFNGLGETSTRINGPROC)(GLenum name);
typedef void (AR_APIENTRYP PFNGLGETTEXPARAMETERFVPROC)(GLenum target, GLenum pname, GLfloat* params);
typedef void (AR_APIENTRYP PFNGLGETTEXPARAMETERIVPROC)(GLenum target, GLenum pname, GLint* params);
typedef void (AR_APIENTRYP PFNGLGETUNIFORMFVPROC)(GLuint program, GLint location, GLfloat* params);
typedef void (AR_APIENTRYP PFNGLGETUNIFORMIVPROC)(GLuint program, GLint location, GLint* params);
typedef GLint (AR_APIENTRYP PFNGLGETUNIFORMLOCATIONPROC)(GLuint program, const GLchar* name);
typedef void (AR_APIENTRYP PFNGLGETVERTEXATTRIBFVPROC)(GLuint index, GLenum pname, GLfloat* params);
typedef void (AR_APIENTRYP PFNGLGETVERTEXATTRIBIVPROC)(GLuint index, GLenum pname, GLint* params);
typedef void (AR_APIENTRYP PFNGLGETVERTEXATTRIBPOINTERVPROC)(GLuint index, GLenum pname, void** pointer);
typedef void (AR_APIENTRYP PFNGLHINTPROC)(GLenum target, GLenum mode);
typedef GLboolean (AR_APIENTRYP PFNGLISBUFFERPROC)(GLuint buffer);
typedef GLboolean (AR_APIENTRYP PFNGLISENABLEDPROC)(GLenum cap);
typedef GLboolean (AR_APIENTRYP PFNGLISFRAMEBUFFERPROC)(GLuint framebuffer);
typedef GLboolean (AR_APIENTRYP PFNGLISPROGRAMPROC)(GLuint program);
typedef GLboolean (AR_APIENTRYP PFNGLISRENDERBUFFERPROC)(GLuint renderbuffer);
typedef GLboolean (AR_APIENTRYP PFNGLISSHADERPROC)(GLuint shader);
typedef GLboolean (AR_APIENTRYP PFNGLISTEXTUREPROC)(GLuint texture);
typedef void (AR_APIENTRYP PFNGLLINEWIDTHPROC)(GLfloat width);
typedef void (AR_APIENTRYP PFNGLLINKPROGRAMPROC)(GLuint program);
typedef void (AR_APIENTRYP PFNGLPIXELSTOREIPROC)(GLenum pname, GLint param);
typedef void (AR_APIENTRYP PFNGLPOLYGONOFFSETPROC)(GLfloat factor, GLfloat units);
typedef void (AR_APIENTRYP PFNGLREADPIXELSPROC)(GLint x, GLint y, GLsizei width, GLsizei height, GLenum format, GLenum type, void* pixels);
typedef void (AR_APIENTRYP PFNGLRELEASESHADERCOMPILERPROC)();
typedef void (AR_APIENTRYP PFNGLRENDERBUFFERSTORAGEPROC)(GLenum target, GLenum internalformat, GLsizei width, GLsizei height);
typedef void (AR_APIENTRYP PFNGLSAMPLECOVERAGEPROC)(GLfloat value, GLboolean invert);
typedef void (AR_APIENTRYP PFNGLSCISSORPROC)(GLint x, GLint y, GLsizei width, GLsizei height);
typedef void (AR_APIENTRYP PFNGLSHADERBINARYPROC)(GLsizei count, const GLuint* shaders, GLenum binaryformat, const void* binary, GLsizei length);
typedef void (AR_APIENTRYP PFNGLSHADERSOURCEPROC)(GLuint shader, GLsizei count, const GLchar** string, const GLint* length);
typedef void (AR_APIENTRYP PFNGLSTENCILFUNCPROC)(GLenum func, GLint ref, GLuint mask);
typedef void (AR_APIENTRYP PFNGLSTENCILFUNCSEPARATEPROC)(GLenum face, GLenum func, GLint ref, GLuint mask);
typedef void (AR_APIENTRYP PFNGLSTENCILMASKPROC)(GLuint mask);
typedef void (AR_APIENTRYP PFNGLSTENCILMASKSEPARATEPROC)(GLenum face, GLuint mask);
typedef void (AR_APIENTRYP PFNGLSTENCILOPPROC)(GLenum fail, GLenum zfail, GLenum zpass);
typedef void (AR_APIENTRYP PFNGLSTENCILOPSEPARATEPROC)(GLenum face, GLenum sfail, GLenum dpfail, GLenum dppass);
typedef void (AR_APIENTRYP PFNGLTEXIMAGE2DPROC)(GLenum target, GLint level, GLint internalformat, GLsizei width, GLsizei height, GLint border, GLenum format, GLenum type, const void* pixels);
typedef void (AR_APIENTRYP PFNGLTEXPARAMETERFPROC)(GLenum target, GLenum pname, GLfloat param);
typedef void (AR_APIENTRYP PFNGLTEXPARAMETERFVPROC)(GLenum target, GLenum pname, const GLfloat* params);
typedef void (AR_APIENTRYP PFNGLTEXPARAMETERIPROC)(GLenum target, GLenum pname, GLint param);
typedef void (AR_APIENTRYP PFNGLTEXPARAMETERIVPROC)(GLenum target, GLenum pname, const GLint* params);
typedef void (AR_APIENTRYP PFNGLTEXSUBIMAGE2DPROC)(GLenum target, GLint level, GLint xoffset, GLint yoffset, GLsizei width, GLsizei height, GLenum format, GLenum type, const void* pixels);
typedef void (AR_APIENTRYP PFNGLUNIFORM1FPROC)(GLint location, GLfloat v0);
typedef void (AR_APIENTRYP PFNGLUNIFORM1FVPROC)(GLint location, GLsizei count, const GLfloat* value);
typedef void (AR_APIENTRYP PFNGLUNIFORM1IPROC)(GLint location, GLint v0);
typedef void (AR_APIENTRYP PFNGLUNIFORM1IVPROC)(GLint location, GLsizei count, const GLint* value);
typedef void (AR_APIENTRYP PFNGLUNIFORM2FPROC)(GLint location, GLfloat v0, GLfloat v1);
typedef void (AR_APIENTRYP PFNGLUNIFORM2FVPROC)(GLint location, GLsizei count, const GLfloat* value);
typedef void (AR_APIENTRYP PFNGLUNIFORM2IPROC)(GLint location, GLint v0, GLint v1);
typedef void (AR_APIENTRYP PFNGLUNIFORM2IVPROC)(GLint location, GLsizei count, const GLint* value);
typedef void (AR_APIENTRYP PFNGLUNIFORM3FPROC)(GLint location, GLfloat v0, GLfloat v1, GLfloat v2);
typedef void (AR_APIENTRYP PFNGLUNIFORM3FVPROC)(GLint location, GLsizei count, const GLfloat* value);
typedef void (AR_APIENTRYP PFNGLUNIFORM3IPROC)(GLint location, GLint v0, GLint v1, GLint v2);
typedef void (AR_APIENTRYP PFNGLUNIFORM3IVPROC)(GLint location, GLsizei count, const GLint* value);
typedef void (AR_APIENTRYP PFNGLUNIFORM4FPROC)(GLint location, GLfloat v0, GLfloat v1, GLfloat v2, GLfloat v3);
typedef void (AR_APIENTRYP PFNGLUNIFORM4FVPROC)(GLint location, GLsizei count, const GLfloat* value);
typedef void (AR_APIENTRYP PFNGLUNIFORM4IPROC)(GLint location, GLint v0, GLint v1, GLint v2, GLint v3);
typedef void (AR_APIENTRYP PFNGLUNIFORM4IVPROC)(GLint location, GLsizei count, const GLint* value);
typedef void (AR_APIENTRYP PFNGLUNIFORMMATRIX2FVPROC)(GLint location, GLsizei count, GLboolean transpose, const GLfloat* value);
typedef void (AR_APIENTRYP PFNGLUNIFORMMATRIX3FVPROC)(GLint location, GLsizei count, GLboolean transpose, const GLfloat* value);
typedef void (AR_APIENTRYP PFNGLUNIFORMMATRIX4FVPROC)(GLint location, GLsizei count, GLboolean transpose, const GLfloat* value);
typedef void (AR_APIENTRYP PFNGLUSEPROGRAMPROC)(GLuint program);
typedef void (AR_APIENTRYP PFNGLVALIDATEPROGRAMPROC)(GLuint program);
typedef void (AR_APIENTRYP PFNGLVERTEXATTRIB1FPROC)(GLuint index, GLfloat x);
typedef void (AR_APIENTRYP PFNGLVERTEXATTRIB1FVPROC)(GLuint index, const GLfloat* v);
typedef void (AR_APIENTRYP PFNGLVERTEXATTRIB2FPROC)(GLuint index, GLfloat x, GLfloat y);
typedef void (AR_APIENTRYP PFNGLVERTEXATTRIB2FVPROC)(GLuint index, const GLfloat* v);
typedef void (AR_APIENTRYP PFNGLVERTEXATTRIB3FPROC)(GLuint index, GLfloat x, GLfloat y, GLfloat z);
typedef void (AR_APIENTRYP PFNGLVERTEXATTRIB3FVPROC)(GLuint index, const GLfloat* v);
typedef void (AR_APIENTRYP PFNGLVERTEXATTRIB4FPROC)(GLuint index, GLfloat x, GLfloat y, GLfloat z, GLfloat w);
typedef void (AR_APIENTRYP PFNGLVERTEXATTRIB4FVPROC)(GLuint index, const GLfloat* v);
typedef void (AR_APIENTRYP PFNGLVERTEXATTRIBPOINTERPROC)(GLuint index, GLint size, GLenum type, GLboolean normalized, GLsizei stride, const void* pointer);

typedef void (AR_APIENTRYP PFNGLVIEWPORTPROC)(GLint x, GLint y, GLsizei width, GLsizei height);
AR_GLAPI PFNGLACTIVETEXTUREPROC ar_glActiveTexture;
AR_GLAPI PFNGLATTACHSHADERPROC ar_glAttachShader;
AR_GLAPI PFNGLBINDATTRIBLOCATIONPROC ar_glBindAttribLocation;
AR_GLAPI PFNGLBINDBUFFERPROC ar_glBindBuffer;
AR_GLAPI PFNGLBINDFRAMEBUFFERPROC ar_glBindFramebuffer;
AR_GLAPI PFNGLBINDRENDERBUFFERPROC ar_glBindRenderbuffer;
AR_GLAPI PFNGLBINDTEXTUREPROC ar_glBindTexture;
AR_GLAPI PFNGLBLENDCOLORPROC ar_glBlendColor;
AR_GLAPI PFNGLBLENDEQUATIONPROC ar_glBlendEquation;
AR_GLAPI PFNGLBLENDEQUATIONSEPARATEPROC ar_glBlendEquationSeparate;
AR_GLAPI PFNGLBLENDFUNCPROC ar_glBlendFunc;
AR_GLAPI PFNGLBLENDFUNCSEPARATEPROC ar_glBlendFuncSeparate;
AR_GLAPI PFNGLBUFFERDATAPROC ar_glBufferData;
AR_GLAPI PFNGLBUFFERSUBDATAPROC ar_glBufferSubData;
AR_GLAPI PFNGLCHECKFRAMEBUFFERSTATUSPROC ar_glCheckFramebufferStatus;
AR_GLAPI PFNGLCLEARPROC ar_glClear;
AR_GLAPI PFNGLCLEARCOLORPROC ar_glClearColor;
AR_GLAPI PFNGLCLEARDEPTHFPROC ar_glClearDepthf;
AR_GLAPI PFNGLCLEARSTENCILPROC ar_glClearStencil;
AR_GLAPI PFNGLCOLORMASKPROC ar_glColorMask;
AR_GLAPI PFNGLCOMPILESHADERPROC ar_glCompileShader;
AR_GLAPI PFNGLCOMPRESSEDTEXIMAGE2DPROC ar_glCompressedTexImage2D;
AR_GLAPI PFNGLCOMPRESSEDTEXSUBIMAGE2DPROC ar_glCompressedTexSubImage2D;
AR_GLAPI PFNGLCOPYTEXIMAGE2DPROC ar_glCopyTexImage2D;
AR_GLAPI PFNGLCOPYTEXSUBIMAGE2DPROC ar_glCopyTexSubImage2D;
AR_GLAPI PFNGLCREATEPROGRAMPROC ar_glCreateProgram;
AR_GLAPI PFNGLCREATESHADERPROC ar_glCreateShader;
AR_GLAPI PFNGLCULLFACEPROC ar_glCullFace;
AR_GLAPI PFNGLDELETEBUFFERSPROC ar_glDeleteBuffers;
AR_GLAPI PFNGLDELETEFRAMEBUFFERSPROC ar_glDeleteFramebuffers;
AR_GLAPI PFNGLDELETEPROGRAMPROC ar_glDeleteProgram;
AR_GLAPI PFNGLDELETERENDERBUFFERSPROC ar_glDeleteRenderbuffers;
AR_GLAPI PFNGLDELETESHADERPROC ar_glDeleteShader;
AR_GLAPI PFNGLDELETETEXTURESPROC ar_glDeleteTextures;
AR_GLAPI PFNGLDEPTHFUNCPROC ar_glDepthFunc;
AR_GLAPI PFNGLDEPTHMASKPROC ar_glDepthMask;
AR_GLAPI PFNGLDEPTHRANGEFPROC ar_glDepthRangef;
AR_GLAPI PFNGLDETACHSHADERPROC ar_glDetachShader;
AR_GLAPI PFNGLDISABLEPROC ar_glDisable;
AR_GLAPI PFNGLDISABLEVERTEXATTRIBARRAYPROC ar_glDisableVertexAttribArray;
AR_GLAPI PFNGLDRAWARRAYSPROC ar_glDrawArrays;
AR_GLAPI PFNGLDRAWELEMENTSPROC ar_glDrawElements;
AR_GLAPI PFNGLENABLEPROC ar_glEnable;
AR_GLAPI PFNGLENABLEVERTEXATTRIBARRAYPROC ar_glEnableVertexAttribArray;
AR_GLAPI PFNGLFINISHPROC ar_glFinish;
AR_GLAPI PFNGLFLUSHPROC ar_glFlush;
AR_GLAPI PFNGLFRAMEBUFFERRENDERBUFFERPROC ar_glFramebufferRenderbuffer;
AR_GLAPI PFNGLFRAMEBUFFERTEXTURE2DPROC ar_glFramebufferTexture2D;
AR_GLAPI PFNGLFRONTFACEPROC ar_glFrontFace;
AR_GLAPI PFNGLGENBUFFERSPROC ar_glGenBuffers;
AR_GLAPI PFNGLGENFRAMEBUFFERSPROC ar_glGenFramebuffers;
AR_GLAPI PFNGLGENRENDERBUFFERSPROC ar_glGenRenderbuffers;
AR_GLAPI PFNGLGENTEXTURESPROC ar_glGenTextures;
AR_GLAPI PFNGLGENERATEMIPMAPPROC ar_glGenerateMipmap;
AR_GLAPI PFNGLGETACTIVEATTRIBPROC ar_glGetActiveAttrib;
AR_GLAPI PFNGLGETACTIVEUNIFORMPROC ar_glGetActiveUniform;
AR_GLAPI PFNGLGETATTACHEDSHADERSPROC ar_glGetAttachedShaders;
AR_GLAPI PFNGLGETATTRIBLOCATIONPROC ar_glGetAttribLocation;
AR_GLAPI PFNGLGETBOOLEANVPROC ar_glGetBooleanv;
AR_GLAPI PFNGLGETBUFFERPARAMETERIVPROC ar_glGetBufferParameteriv;
AR_GLAPI PFNGLGETERRORPROC ar_glGetError;
AR_GLAPI PFNGLGETFLOATVPROC ar_glGetFloatv;
AR_GLAPI PFNGLGETFRAMEBUFFERATTACHMENTPARAMETERIVPROC ar_glGetFramebufferAttachmentParameteriv;
AR_GLAPI PFNGLGETINTEGERVPROC ar_glGetIntegerv;
AR_GLAPI PFNGLGETPROGRAMINFOLOGPROC ar_glGetProgramInfoLog;
AR_GLAPI PFNGLGETPROGRAMIVPROC ar_glGetProgramiv;
AR_GLAPI PFNGLGETRENDERBUFFERPARAMETERIVPROC ar_glGetRenderbufferParameteriv;
AR_GLAPI PFNGLGETSHADERINFOLOGPROC ar_glGetShaderInfoLog;
AR_GLAPI PFNGLGETSHADERPRECISIONFORMATPROC ar_glGetShaderPrecisionFormat;
AR_GLAPI PFNGLGETSHADERSOURCEPROC ar_glGetShaderSource;
AR_GLAPI PFNGLGETSHADERIVPROC ar_glGetShaderiv;
AR_GLAPI PFNGLGETSTRINGPROC ar_glGetString;
AR_GLAPI PFNGLGETTEXPARAMETERFVPROC ar_glGetTexParameterfv;
AR_GLAPI PFNGLGETTEXPARAMETERIVPROC ar_glGetTexParameteriv;
AR_GLAPI PFNGLGETUNIFORMLOCATIONPROC ar_glGetUniformLocation;
AR_GLAPI PFNGLGETUNIFORMFVPROC ar_glGetUniformfv;
AR_GLAPI PFNGLGETUNIFORMIVPROC ar_glGetUniformiv;
AR_GLAPI PFNGLGETVERTEXATTRIBPOINTERVPROC ar_glGetVertexAttribPointerv;
AR_GLAPI PFNGLGETVERTEXATTRIBFVPROC ar_glGetVertexAttribfv;
AR_GLAPI PFNGLGETVERTEXATTRIBIVPROC ar_glGetVertexAttribiv;
AR_GLAPI PFNGLHINTPROC ar_glHint;
AR_GLAPI PFNGLISBUFFERPROC ar_glIsBuffer;
AR_GLAPI PFNGLISENABLEDPROC ar_glIsEnabled;
AR_GLAPI PFNGLISFRAMEBUFFERPROC ar_glIsFramebuffer;
AR_GLAPI PFNGLISPROGRAMPROC ar_glIsProgram;
AR_GLAPI PFNGLISRENDERBUFFERPROC ar_glIsRenderbuffer;
AR_GLAPI PFNGLISSHADERPROC ar_glIsShader;
AR_GLAPI PFNGLISTEXTUREPROC ar_glIsTexture;
AR_GLAPI PFNGLLINEWIDTHPROC ar_glLineWidth;
AR_GLAPI PFNGLLINKPROGRAMPROC ar_glLinkProgram;
AR_GLAPI PFNGLPIXELSTOREIPROC ar_glPixelStorei;
AR_GLAPI PFNGLPOLYGONOFFSETPROC ar_glPolygonOffset;
AR_GLAPI PFNGLREADPIXELSPROC ar_glReadPixels;
AR_GLAPI PFNGLRELEASESHADERCOMPILERPROC ar_glReleaseShaderCompiler;
AR_GLAPI PFNGLRENDERBUFFERSTORAGEPROC ar_glRenderbufferStorage;
AR_GLAPI PFNGLSAMPLECOVERAGEPROC ar_glSampleCoverage;
AR_GLAPI PFNGLSCISSORPROC ar_glScissor;
AR_GLAPI PFNGLSHADERBINARYPROC ar_glShaderBinary;
AR_GLAPI PFNGLSHADERSOURCEPROC ar_glShaderSource;
AR_GLAPI PFNGLSTENCILFUNCPROC ar_glStencilFunc;
AR_GLAPI PFNGLSTENCILFUNCSEPARATEPROC ar_glStencilFuncSeparate;
AR_GLAPI PFNGLSTENCILMASKPROC ar_glStencilMask;
AR_GLAPI PFNGLSTENCILMASKSEPARATEPROC ar_glStencilMaskSeparate;
AR_GLAPI PFNGLSTENCILOPPROC ar_glStencilOp;
AR_GLAPI PFNGLSTENCILOPSEPARATEPROC ar_glStencilOpSeparate;
AR_GLAPI PFNGLTEXIMAGE2DPROC ar_glTexImage2D;
AR_GLAPI PFNGLTEXPARAMETERFPROC ar_glTexParameterf;
AR_GLAPI PFNGLTEXPARAMETERFVPROC ar_glTexParameterfv;
AR_GLAPI PFNGLTEXPARAMETERIPROC ar_glTexParameteri;
AR_GLAPI PFNGLTEXPARAMETERIVPROC ar_glTexParameteriv;
AR_GLAPI PFNGLTEXSUBIMAGE2DPROC ar_glTexSubImage2D;
AR_GLAPI PFNGLUNIFORM1FPROC ar_glUniform1f;
AR_GLAPI PFNGLUNIFORM1FVPROC ar_glUniform1fv;
AR_GLAPI PFNGLUNIFORM1IPROC ar_glUniform1i;
AR_GLAPI PFNGLUNIFORM1IVPROC ar_glUniform1iv;
AR_GLAPI PFNGLUNIFORM2FPROC ar_glUniform2f;
AR_GLAPI PFNGLUNIFORM2FVPROC ar_glUniform2fv;
AR_GLAPI PFNGLUNIFORM2IPROC ar_glUniform2i;
AR_GLAPI PFNGLUNIFORM2IVPROC ar_glUniform2iv;
AR_GLAPI PFNGLUNIFORM3FPROC ar_glUniform3f;
AR_GLAPI PFNGLUNIFORM3FVPROC ar_glUniform3fv;
AR_GLAPI PFNGLUNIFORM3IPROC ar_glUniform3i;
AR_GLAPI PFNGLUNIFORM3IVPROC ar_glUniform3iv;
AR_GLAPI PFNGLUNIFORM4FPROC ar_glUniform4f;
AR_GLAPI PFNGLUNIFORM4FVPROC ar_glUniform4fv;
AR_GLAPI PFNGLUNIFORM4IPROC ar_glUniform4i;
AR_GLAPI PFNGLUNIFORM4IVPROC ar_glUniform4iv;
AR_GLAPI PFNGLUNIFORMMATRIX2FVPROC ar_glUniformMatrix2fv;
AR_GLAPI PFNGLUNIFORMMATRIX3FVPROC ar_glUniformMatrix3fv;
AR_GLAPI PFNGLUNIFORMMATRIX4FVPROC ar_glUniformMatrix4fv;
AR_GLAPI PFNGLUSEPROGRAMPROC ar_glUseProgram;
AR_GLAPI PFNGLVALIDATEPROGRAMPROC ar_glValidateProgram;
AR_GLAPI PFNGLVERTEXATTRIB1FPROC ar_glVertexAttrib1f;
AR_GLAPI PFNGLVERTEXATTRIB1FVPROC ar_glVertexAttrib1fv;
AR_GLAPI PFNGLVERTEXATTRIB2FPROC ar_glVertexAttrib2f;
AR_GLAPI PFNGLVERTEXATTRIB2FVPROC ar_glVertexAttrib2fv;
AR_GLAPI PFNGLVERTEXATTRIB3FPROC ar_glVertexAttrib3f;
AR_GLAPI PFNGLVERTEXATTRIB3FVPROC ar_glVertexAttrib3fv;
AR_GLAPI PFNGLVERTEXATTRIB4FPROC ar_glVertexAttrib4f;
AR_GLAPI PFNGLVERTEXATTRIB4FVPROC ar_glVertexAttrib4fv;
AR_GLAPI PFNGLVERTEXATTRIBPOINTERPROC ar_glVertexAttribPointer;
AR_GLAPI PFNGLVIEWPORTPROC ar_glViewport;

#elif __APPLE__

#import <OpenGLES/ES2/glext.h>

#else

#inclued <GL/glext.h>

#endif

typedef int (*ar_gl_func_ptr)(void);

void ar_gl_init(void);
void ar_gl_check(int line);

#define AR_GL_CHECK ar_gl_check(__LINE__);

#endif
