// GLSL shader autogenerated by cg2glsl.py.
#if defined(VERTEX)

#if __VERSION__ >= 130
#define COMPAT_VARYING out
#define COMPAT_ATTRIBUTE in
#define COMPAT_TEXTURE texture
#else
#define COMPAT_VARYING varying 
#define COMPAT_ATTRIBUTE attribute 
#define COMPAT_TEXTURE texture2D
#endif

#ifdef GL_ES
#define COMPAT_PRECISION mediump
#else
#define COMPAT_PRECISION
#endif
COMPAT_VARYING     vec2 _DR;
COMPAT_VARYING     vec2 _DL;
COMPAT_VARYING     vec2 _UR;
COMPAT_VARYING     vec2 _UL;
COMPAT_VARYING     float _frame_rotation;
COMPAT_VARYING     vec4 _color;
struct output_dummy {
    vec4 _color;
};
struct input_dummy {
    vec2 _video_size;
    vec2 _texture_size;
    vec2 _output_dummy_size;
    float _frame_count;
    float _frame_direction;
    float _frame_rotation;
};
struct deltas {
    vec2 _UL;
    vec2 _UR;
    vec2 _DL;
    vec2 _DR;
};
vec4 _oPosition1;
vec4 _r0005;
COMPAT_ATTRIBUTE vec4 VertexCoord;
COMPAT_ATTRIBUTE vec4 TexCoord;
COMPAT_VARYING vec4 TEX0;
 
uniform mat4 MVPMatrix;
uniform int FrameDirection;
uniform int FrameCount;
uniform COMPAT_PRECISION vec2 OutputSize;
uniform COMPAT_PRECISION vec2 TextureSize;
uniform COMPAT_PRECISION vec2 InputSize;
void main()
{
    vec2 _otexCoord;
    _r0005 = VertexCoord.x*MVPMatrix[0];
    _r0005 = _r0005 + VertexCoord.y*MVPMatrix[1];
    _r0005 = _r0005 + VertexCoord.z*MVPMatrix[2];
    _r0005 = _r0005 + VertexCoord.w*MVPMatrix[3];
    _oPosition1 = _r0005;
    _otexCoord = TexCoord.xy;
    gl_Position = _r0005;
    TEX0.xy = TexCoord.xy;
} 
#elif defined(FRAGMENT)

#if __VERSION__ >= 130
#define COMPAT_VARYING in
#define COMPAT_TEXTURE texture
out vec4 FragColor;
#else
#define COMPAT_VARYING varying
#define FragColor gl_FragColor
#define COMPAT_TEXTURE texture2D
#endif

#ifdef GL_ES
#ifdef GL_FRAGMENT_PRECISION_HIGH
precision highp float;
#else
precision mediump float;
#endif
#define COMPAT_PRECISION mediump
#else
#define COMPAT_PRECISION
#endif
COMPAT_VARYING     vec2 _DR;
COMPAT_VARYING     vec2 _DL;
COMPAT_VARYING     vec2 _UR;
COMPAT_VARYING     vec2 _UL;
COMPAT_VARYING     float _frame_rotation;
COMPAT_VARYING     vec4 _color;
struct output_dummy {
    vec4 _color;
};
struct input_dummy {
    vec2 _video_size;
    vec2 _texture_size;
    vec2 _output_dummy_size;
    float _frame_count;
    float _frame_direction;
    float _frame_rotation;
};
struct deltas {
    vec2 _UL;
    vec2 _UR;
    vec2 _DL;
    vec2 _DR;
};
float _TMP9;
vec3 _TMP8;
float _TMP7;
vec3 _TMP6;
vec4 _TMP5;
vec4 _TMP4;
vec4 _TMP3;
vec4 _TMP2;
float _TMP1;
float _TMP0;
input_dummy _IN1;
uniform sampler2D Texture;
vec3 _a0023;
vec3 _a0027;
COMPAT_VARYING vec4 TEX0;
 
uniform int FrameDirection;
uniform int FrameCount;
uniform COMPAT_PRECISION vec2 OutputSize;
uniform COMPAT_PRECISION vec2 TextureSize;
uniform COMPAT_PRECISION vec2 InputSize;
void main()
{
    float _dx;
    float _dy;
    float _m1;
    float _m2;
    output_dummy _OUT;
    vec3 _TMP12;
    deltas _TMP13;
    _TMP0 = 1.00000000E+00/TextureSize.x;
    _dx = _TMP0*2.50000000E-01;
    _TMP1 = 1.00000000E+00/TextureSize.y;
    _dy = _TMP1*2.50000000E-01;
    _TMP13._UL = TEX0.xy + vec2(-_dx, -_dy);
    _TMP13._UR = TEX0.xy + vec2(_dx, -_dy);
    _TMP13._DL = TEX0.xy + vec2(-_dx, _dy);
    _TMP13._DR = TEX0.xy + vec2(_dx, _dy);
    _TMP2 = COMPAT_TEXTURE(Texture, _TMP13._UL);
    _TMP3 = COMPAT_TEXTURE(Texture, _TMP13._UR);
    _TMP4 = COMPAT_TEXTURE(Texture, _TMP13._DL);
    _TMP5 = COMPAT_TEXTURE(Texture, _TMP13._DR);
    _a0023 = _TMP2.xyz - _TMP5.xyz;
    _TMP6 = abs(_a0023);
    _TMP7 = dot(_TMP6, vec3( 1.00000000E+00, 1.00000000E+00, 1.00000000E+00));
    _m1 = _TMP7 + 1.00000005E-03;
    _a0027 = _TMP4.xyz - _TMP3.xyz;
    _TMP8 = abs(_a0027);
    _TMP9 = dot(_TMP8, vec3( 1.00000000E+00, 1.00000000E+00, 1.00000000E+00));
    _m2 = _TMP9 + 1.00000005E-03;
    _TMP12 = (_m1*(_TMP4.xyz + _TMP3.xyz) + _m2*(_TMP5.xyz + _TMP2.xyz))/(2.00000000E+00*(_m1 + _m2));
    _OUT._color = vec4(_TMP12.x, _TMP12.y, _TMP12.z, 1.00000000E+00);
    FragColor = _OUT._color;
    return;
} 
#endif
