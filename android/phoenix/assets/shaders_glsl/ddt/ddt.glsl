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
COMPAT_VARYING     vec2 _loc;
COMPAT_VARYING     vec4 _t1;
COMPAT_VARYING     vec2 _texCoord1;
COMPAT_VARYING     vec4 _position1;
COMPAT_VARYING     float _frame_rotation;
struct input_dummy {
    vec2 _video_size;
    vec2 _texture_size;
    vec2 _output_dummy_size;
    float _frame_count;
    float _frame_direction;
    float _frame_rotation;
};
struct out_vertex {
    vec4 _position1;
    vec2 _texCoord1;
    vec4 _t1;
    vec2 _loc;
};
out_vertex _ret_0;
input_dummy _IN1;
vec4 _r0008;
COMPAT_ATTRIBUTE vec4 VertexCoord;
COMPAT_ATTRIBUTE vec4 TexCoord;
COMPAT_VARYING vec4 TEX0;
COMPAT_VARYING vec4 TEX1;
COMPAT_VARYING vec4 TEX2;
 
uniform mat4 MVPMatrix;
uniform int FrameDirection;
uniform int FrameCount;
uniform COMPAT_PRECISION vec2 OutputSize;
uniform COMPAT_PRECISION vec2 TextureSize;
uniform COMPAT_PRECISION vec2 InputSize;
void main()
{
    out_vertex _OUT;
    vec2 _ps;
    _r0008 = VertexCoord.x*MVPMatrix[0];
    _r0008 = _r0008 + VertexCoord.y*MVPMatrix[1];
    _r0008 = _r0008 + VertexCoord.z*MVPMatrix[2];
    _r0008 = _r0008 + VertexCoord.w*MVPMatrix[3];
    _ps = vec2(float((1.00000000E+00/TextureSize.x)), float((1.00000000E+00/TextureSize.y)));
    _OUT._t1.xy = vec2(_ps.x, 0.00000000E+00);
    _OUT._t1.zw = vec2(0.00000000E+00, _ps.y);
    _OUT._loc = vec2(float((TexCoord.xy*TextureSize).x), float((TexCoord.xy*TextureSize).y));
    _ret_0._position1 = _r0008;
    _ret_0._texCoord1 = TexCoord.xy;
    _ret_0._t1 = _OUT._t1;
    _ret_0._loc = _OUT._loc;
    gl_Position = vec4(float(_r0008.x), float(_r0008.y), float(_r0008.z), float(_r0008.w));
    TEX0.xy = TexCoord.xy;
    TEX1 = _OUT._t1;
    TEX2.xy = _OUT._loc;
    return;
    TEX0.xy = _ret_0._texCoord1;
    TEX1 = _ret_0._t1;
    TEX2.xy = _ret_0._loc;
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
COMPAT_VARYING     vec2 _loc;
COMPAT_VARYING     vec4 _t1;
COMPAT_VARYING     vec2 _texCoord;
COMPAT_VARYING     float _frame_rotation;
struct input_dummy {
    vec2 _video_size;
    vec2 _texture_size;
    vec2 _output_dummy_size;
    float _frame_count;
    float _frame_direction;
    float _frame_rotation;
};
struct out_vertex {
    vec2 _texCoord;
    vec4 _t1;
    vec2 _loc;
};
vec4 _ret_0;
float _TMP8;
float _TMP7;
float _TMP6;
float _TMP5;
float _TMP11;
float _TMP13;
float _TMP10;
float _TMP12;
vec4 _TMP4;
vec4 _TMP3;
vec4 _TMP2;
vec4 _TMP1;
vec2 _TMP0;
vec2 _TMP9;
uniform sampler2D Texture;
vec2 _val0021;
vec2 _c0025;
vec2 _c0027;
vec2 _c0029;
float _TMP50;
vec2 _v0051;
float _TMP58;
vec2 _v0059;
float _a0067;
float _a0069;
float _a0071;
float _a0073;
COMPAT_VARYING vec4 TEX0;
COMPAT_VARYING vec4 TEX1;
COMPAT_VARYING vec4 TEX2;
 
uniform int FrameDirection;
uniform int FrameCount;
uniform COMPAT_PRECISION vec2 OutputSize;
uniform COMPAT_PRECISION vec2 TextureSize;
uniform COMPAT_PRECISION vec2 InputSize;
void main()
{
    vec2 _pos;
    vec2 _dir;
    vec2 _g1;
    vec2 _g2;
    vec3 _A1;
    vec3 _B1;
    vec3 _C1;
    vec3 _D1;
    float _a;
    float _b;
    float _c;
    float _d;
    float _p1;
    float _q1;
    vec3 _color1;
    _TMP9 = fract(vec2(float(TEX2.x), float(TEX2.y)));
    _TMP0 = vec2(float(_TMP9.x), float(_TMP9.y));
    _pos = vec2(float(_TMP0.x), float(_TMP0.y)) - vec2( 5.00000000E-01, 5.00000000E-01);
    _val0021 = vec2(float((_pos.x > 0.00000000E+00)), float((_pos.y > 0.00000000E+00)));
    _dir = _val0021 - vec2(float((_pos.x < 0.00000000E+00)), float((_pos.y < 0.00000000E+00)));
    _g1 = _dir*vec2(float(TEX1.x), float(TEX1.y));
    _g2 = _dir*vec2(float(TEX1.z), float(TEX1.w));
    _TMP1 = COMPAT_TEXTURE(Texture, TEX0.xy);
    _A1 = vec3(float(_TMP1.x), float(_TMP1.y), float(_TMP1.z));
    _c0025 = TEX0.xy + _g1;
    _TMP2 = COMPAT_TEXTURE(Texture, _c0025);
    _B1 = vec3(float(_TMP2.x), float(_TMP2.y), float(_TMP2.z));
    _c0027 = TEX0.xy + _g2;
    _TMP3 = COMPAT_TEXTURE(Texture, _c0027);
    _C1 = vec3(float(_TMP3.x), float(_TMP3.y), float(_TMP3.z));
    _c0029 = TEX0.xy + _g1 + _g2;
    _TMP4 = COMPAT_TEXTURE(Texture, _c0029);
    _D1 = vec3(float(_TMP4.x), float(_TMP4.y), float(_TMP4.z));
    _TMP12 = dot(vec3(float(_A1.x), float(_A1.y), float(_A1.z)), vec3( 6.55360000E+04, 2.55000000E+02, 1.00000000E+00));
    _TMP10 = float(_TMP12);
    _a = float(_TMP10);
    _TMP12 = dot(vec3(float(_B1.x), float(_B1.y), float(_B1.z)), vec3( 6.55360000E+04, 2.55000000E+02, 1.00000000E+00));
    _TMP10 = float(_TMP12);
    _b = float(_TMP10);
    _TMP12 = dot(vec3(float(_C1.x), float(_C1.y), float(_C1.z)), vec3( 6.55360000E+04, 2.55000000E+02, 1.00000000E+00));
    _TMP10 = float(_TMP12);
    _c = float(_TMP10);
    _TMP12 = dot(vec3(float(_D1.x), float(_D1.y), float(_D1.z)), vec3( 6.55360000E+04, 2.55000000E+02, 1.00000000E+00));
    _TMP10 = float(_TMP12);
    _d = float(_TMP10);
    _p1 = abs(_pos.x);
    _q1 = abs(_pos.y);
    _v0051 = _g1 - _pos;
    _TMP11 = dot(_v0051, _v0051);
    _TMP13 = inversesqrt(_TMP11);
    _TMP50 = 1.00000000E+00/_TMP13;
    _v0059 = _g2 - _pos;
    _TMP11 = dot(_v0059, _v0059);
    _TMP13 = inversesqrt(_TMP11);
    _TMP58 = 1.00000000E+00/_TMP13;
    _a0067 = _a - _d;
    _TMP5 = abs(_a0067);
    _a0069 = _b - _c;
    _TMP6 = abs(_a0069);
    if (_TMP5 < _TMP6) { 
        if (_TMP50 < _TMP58) { 
            _C1 = (_A1 + _D1) - _B1;
        } else {
            if (_TMP50 > _TMP58) { 
                _B1 = (_A1 + _D1) - _C1;
            } 
        } 
    } else {
        _a0071 = _a - _d;
        _TMP7 = abs(_a0071);
        _a0073 = _b - _c;
        _TMP8 = abs(_a0073);
        if (_TMP7 > _TMP8) { 
            _D1 = (_B1 + _C1) - _A1;
        } 
    } 
    _color1 = ((1.00000000E+00 - _p1)*(1.00000000E+00 - _q1))*vec3(float(_A1.x), float(_A1.y), float(_A1.z)) + (_p1*(1.00000000E+00 - _q1))*vec3(float(_B1.x), float(_B1.y), float(_B1.z)) + ((1.00000000E+00 - _p1)*_q1)*vec3(float(_C1.x), float(_C1.y), float(_C1.z)) + (_p1*_q1)*vec3(float(_D1.x), float(_D1.y), float(_D1.z));
    _ret_0 = vec4(_color1.x, _color1.y, _color1.z, 1.00000000E+00);
    FragColor = _ret_0;
    return;
} 
#endif
