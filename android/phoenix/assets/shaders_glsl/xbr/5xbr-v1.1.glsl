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
COMPAT_VARYING     vec4 _t1;
COMPAT_VARYING     vec2 _texCoord1;
COMPAT_VARYING     vec4 _color1;
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
    vec4 _color1;
    vec2 _texCoord1;
    vec4 _t1;
};
out_vertex _ret_0;
input_dummy _IN1;
vec4 _r0006;
COMPAT_ATTRIBUTE vec4 VertexCoord;
COMPAT_ATTRIBUTE vec4 COLOR;
COMPAT_ATTRIBUTE vec4 TexCoord;
COMPAT_VARYING vec4 COL0;
COMPAT_VARYING vec4 TEX0;
COMPAT_VARYING vec4 TEX1;
 
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
    _r0006 = VertexCoord.x*MVPMatrix[0];
    _r0006 = _r0006 + VertexCoord.y*MVPMatrix[1];
    _r0006 = _r0006 + VertexCoord.z*MVPMatrix[2];
    _r0006 = _r0006 + VertexCoord.w*MVPMatrix[3];
    _ps = vec2(1.00000000E+00/TextureSize.x, 1.00000000E+00/TextureSize.y);
    _OUT._t1.xy = vec2(0.00000000E+00, -_ps.y);
    _OUT._t1.zw = vec2(-_ps.x, 0.00000000E+00);
    _ret_0._position1 = _r0006;
    _ret_0._color1 = COLOR;
    _ret_0._texCoord1 = TexCoord.xy;
    _ret_0._t1 = _OUT._t1;
    gl_Position = _r0006;
    COL0 = COLOR;
    TEX0.xy = TexCoord.xy;
    TEX1 = _OUT._t1;
    return;
    COL0 = _ret_0._color1;
    TEX0.xy = _ret_0._texCoord1;
    TEX1 = _ret_0._t1;
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
COMPAT_VARYING     vec4 _t1;
COMPAT_VARYING     vec2 _texCoord;
COMPAT_VARYING     vec4 _color1;
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
    vec4 _color1;
    vec2 _texCoord;
    vec4 _t1;
};
vec4 _ret_0;
vec3 _TMP10;
vec3 _TMP11;
vec3 _TMP12;
float _TMP13;
float _TMP14;
vec4 _TMP9;
vec4 _TMP8;
vec4 _TMP7;
vec4 _TMP6;
vec4 _TMP5;
vec4 _TMP4;
vec4 _TMP3;
vec4 _TMP2;
vec4 _TMP1;
vec4 _TMP0;
uniform sampler2D Texture;
input_dummy _IN1;
vec2 _x0020;
vec2 _c0024;
vec2 _c0026;
vec2 _c0028;
vec2 _c0032;
vec2 _c0034;
vec2 _c0036;
vec2 _c0038;
vec2 _c0040;
vec2 _c0042;
COMPAT_VARYING vec4 TEX0;
COMPAT_VARYING vec4 TEX1;
 
uniform int FrameDirection;
uniform int FrameCount;
uniform COMPAT_PRECISION vec2 OutputSize;
uniform COMPAT_PRECISION vec2 TextureSize;
uniform COMPAT_PRECISION vec2 InputSize;
void main()
{
    vec2 _fp;
    vec2 _st;
    vec2 _g1;
    vec2 _g2;
    float _AO;
    float _BO;
    float _CO;
    vec3 _B;
    vec3 _C;
    vec3 _D;
    vec3 _E;
    vec3 _F;
    vec3 _G;
    vec3 _H;
    vec3 _I;
    vec3 _C4;
    vec3 _G5;
    float _b;
    float _c;
    float _d;
    float _e;
    float _f;
    float _g;
    float _h;
    float _i;
    float _c4;
    float _g5;
    vec3 _res;
    bool _fx_1;
    bool _condition_11;
    bool _condition_21;
    bool _condition_31;
    _x0020 = TEX0.xy*TextureSize;
    _fp = fract(_x0020);
    _st = vec2(float((_fp.x >= 5.00000000E-01)), float((_fp.y >= 5.00000000E-01)));
    _g1 = TEX1.xy*((_st.x + _st.y) - 1.00000000E+00) + TEX1.zw*(_st.x - _st.y);
    _g2 = TEX1.xy*(_st.y - _st.x) + TEX1.zw*((_st.x + _st.y) - 1.00000000E+00);
    _AO = 2.00000000E+00*_st.y - 1.00000000E+00;
    _BO = 2.00000000E+00*_st.x - 1.00000000E+00;
    _CO = (_st.x + _st.y) - 5.00000000E-01;
    _c0024 = TEX0.xy + _g1;
    _TMP0 = COMPAT_TEXTURE(Texture, _c0024);
    _B = vec3(float(_TMP0.x), float(_TMP0.y), float(_TMP0.z));
    _c0026 = (TEX0.xy + _g1) - _g2;
    _TMP1 = COMPAT_TEXTURE(Texture, _c0026);
    _C = vec3(float(_TMP1.x), float(_TMP1.y), float(_TMP1.z));
    _c0028 = TEX0.xy + _g2;
    _TMP2 = COMPAT_TEXTURE(Texture, _c0028);
    _D = vec3(float(_TMP2.x), float(_TMP2.y), float(_TMP2.z));
    _TMP3 = COMPAT_TEXTURE(Texture, TEX0.xy);
    _E = vec3(float(_TMP3.x), float(_TMP3.y), float(_TMP3.z));
    _c0032 = TEX0.xy - _g2;
    _TMP4 = COMPAT_TEXTURE(Texture, _c0032);
    _F = vec3(float(_TMP4.x), float(_TMP4.y), float(_TMP4.z));
    _c0034 = (TEX0.xy - _g1) + _g2;
    _TMP5 = COMPAT_TEXTURE(Texture, _c0034);
    _G = vec3(float(_TMP5.x), float(_TMP5.y), float(_TMP5.z));
    _c0036 = TEX0.xy - _g1;
    _TMP6 = COMPAT_TEXTURE(Texture, _c0036);
    _H = vec3(float(_TMP6.x), float(_TMP6.y), float(_TMP6.z));
    _c0038 = (TEX0.xy - _g1) - _g2;
    _TMP7 = COMPAT_TEXTURE(Texture, _c0038);
    _I = vec3(float(_TMP7.x), float(_TMP7.y), float(_TMP7.z));
    _c0040 = (TEX0.xy + _g1) - 2.00000000E+00*_g2;
    _TMP8 = COMPAT_TEXTURE(Texture, _c0040);
    _C4 = vec3(float(_TMP8.x), float(_TMP8.y), float(_TMP8.z));
    _c0042 = (TEX0.xy - 2.00000000E+00*_g1) + _g2;
    _TMP9 = COMPAT_TEXTURE(Texture, _c0042);
    _G5 = vec3(float(_TMP9.x), float(_TMP9.y), float(_TMP9.z));
    _TMP14 = dot(vec3(float(_B.x), float(_B.y), float(_B.z)), vec3( 6.55360000E+04, 2.55000000E+02, 1.00000000E+00));
    _TMP13 = float(_TMP14);
    _b = float(_TMP13);
    _TMP14 = dot(vec3(float(_C.x), float(_C.y), float(_C.z)), vec3( 6.55360000E+04, 2.55000000E+02, 1.00000000E+00));
    _TMP13 = float(_TMP14);
    _c = float(_TMP13);
    _TMP14 = dot(vec3(float(_D.x), float(_D.y), float(_D.z)), vec3( 6.55360000E+04, 2.55000000E+02, 1.00000000E+00));
    _TMP13 = float(_TMP14);
    _d = float(_TMP13);
    _TMP14 = dot(vec3(float(_E.x), float(_E.y), float(_E.z)), vec3( 6.55360000E+04, 2.55000000E+02, 1.00000000E+00));
    _TMP13 = float(_TMP14);
    _e = float(_TMP13);
    _TMP14 = dot(vec3(float(_F.x), float(_F.y), float(_F.z)), vec3( 6.55360000E+04, 2.55000000E+02, 1.00000000E+00));
    _TMP13 = float(_TMP14);
    _f = float(_TMP13);
    _TMP14 = dot(vec3(float(_G.x), float(_G.y), float(_G.z)), vec3( 6.55360000E+04, 2.55000000E+02, 1.00000000E+00));
    _TMP13 = float(_TMP14);
    _g = float(_TMP13);
    _TMP14 = dot(vec3(float(_H.x), float(_H.y), float(_H.z)), vec3( 6.55360000E+04, 2.55000000E+02, 1.00000000E+00));
    _TMP13 = float(_TMP14);
    _h = float(_TMP13);
    _TMP14 = dot(vec3(float(_I.x), float(_I.y), float(_I.z)), vec3( 6.55360000E+04, 2.55000000E+02, 1.00000000E+00));
    _TMP13 = float(_TMP14);
    _i = float(_TMP13);
    _TMP14 = dot(vec3(float(_C4.x), float(_C4.y), float(_C4.z)), vec3( 6.55360000E+04, 2.55000000E+02, 1.00000000E+00));
    _TMP13 = float(_TMP14);
    _c4 = float(_TMP13);
    _TMP14 = dot(vec3(float(_G5.x), float(_G5.y), float(_G5.z)), vec3( 6.55360000E+04, 2.55000000E+02, 1.00000000E+00));
    _TMP13 = float(_TMP14);
    _g5 = float(_TMP13);
    _res = vec3(float(_E.x), float(_E.y), float(_E.z));
    _fx_1 = _AO*_fp.y + _BO*_fp.x > _CO;
    if (_fx_1) { 
        _condition_11 = _e != _h && (_h == _f && (_e != _i && (_e != _b || _e != _d || _f != _b && _f != _c || _h != _d && _h != _g) || _e == _g && (_i == _h || _e == _d || _h != _d) || _e == _c && (_i == _h || _e == _b || _f != _b)) || _e != _f && _h != _i && _e == _g && (_f == _i && _h != _d || _e != _i && _h == _g5));
        _condition_21 = _e != _h && _e != _f && _f != _i && _e == _c && (_h == _i && _f != _b || _e != _i && _f == _c4);
        _condition_31 = _e != _h && _g == _e && _e == _c && _e != _i && _e != _f;
        if (_condition_11) { 
            _TMP10 = _H;
        } else {
            if (_condition_21) { 
                _TMP11 = _F;
            } else {
                if (_condition_31) { 
                    _TMP12 = (_F + _H)*5.00000000E-01;
                } else {
                    _TMP12 = _E;
                } 
                _TMP11 = _TMP12;
            } 
            _TMP10 = _TMP11;
        } 
        _res = vec3(float(_TMP10.x), float(_TMP10.y), float(_TMP10.z));
    } 
    _ret_0 = vec4(_res.x, _res.y, _res.z, 1.00000000E+00);
    FragColor = _ret_0;
    return;
} 
#endif
