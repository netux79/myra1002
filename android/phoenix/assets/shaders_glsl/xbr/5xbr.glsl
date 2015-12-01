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
};
out_vertex _ret_0;
input_dummy _IN1;
vec4 _r0006;
COMPAT_ATTRIBUTE vec4 VertexCoord;
COMPAT_ATTRIBUTE vec4 TexCoord;
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
    _ret_0._texCoord1 = TexCoord.xy;
    _ret_0._t1 = _OUT._t1;
    gl_Position = _r0006;
    TEX0.xy = TexCoord.xy;
    TEX1 = _OUT._t1;
    return;
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
};
vec4 _ret_0;
vec3 _TMP16;
vec3 _TMP26;
vec3 _TMP36;
vec3 _TMP37;
vec3 _TMP38;
vec3 _TMP39;
vec3 _TMP27;
vec3 _TMP32;
vec3 _TMP33;
vec3 _TMP34;
vec3 _TMP35;
vec3 _TMP28;
vec3 _TMP29;
vec3 _TMP30;
vec3 _TMP31;
vec3 _TMP17;
vec3 _TMP22;
vec3 _TMP23;
vec3 _TMP24;
vec3 _TMP25;
vec3 _TMP18;
vec3 _TMP19;
vec3 _TMP20;
vec3 _TMP21;
float _TMP40;
float _TMP41;
vec4 _TMP15;
vec4 _TMP14;
vec4 _TMP13;
vec4 _TMP12;
vec4 _TMP11;
vec4 _TMP10;
vec4 _TMP9;
vec4 _TMP8;
float _TMP7;
float _TMP6;
float _TMP5;
float _TMP4;
float _TMP3;
float _TMP2;
float _TMP1;
float _TMP0;
uniform sampler2D Texture;
input_dummy _IN1;
vec2 _x0047;
vec2 _c0065;
vec2 _c0067;
vec2 _c0069;
vec2 _c0073;
vec2 _c0075;
vec2 _c0077;
vec2 _c0079;
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
    vec2 _g1;
    vec2 _g2;
    vec3 _B;
    vec3 _C;
    vec3 _D;
    vec3 _E;
    vec3 _F;
    vec3 _G;
    vec3 _H;
    vec3 _I;
    vec3 _E14;
    vec3 _E19;
    vec3 _E24;
    float _b;
    float _c;
    float _d;
    float _e;
    float _f;
    float _g;
    float _h;
    float _i;
    vec3 _res;
    _x0047 = TEX0.xy*TextureSize;
    _fp = fract(_x0047);
    _TMP0 = float((_fp.x >= 5.00000000E-01));
    _TMP1 = float((_fp.y >= 5.00000000E-01));
    _TMP2 = float((_fp.x >= 5.00000000E-01));
    _TMP3 = float((_fp.y >= 5.00000000E-01));
    _g1 = TEX1.xy*((_TMP0 + _TMP1) - 1.00000000E+00) + TEX1.zw*(_TMP2 - _TMP3);
    _TMP4 = float((_fp.y >= 5.00000000E-01));
    _TMP5 = float((_fp.x >= 5.00000000E-01));
    _TMP6 = float((_fp.x >= 5.00000000E-01));
    _TMP7 = float((_fp.y >= 5.00000000E-01));
    _g2 = TEX1.xy*(_TMP4 - _TMP5) + TEX1.zw*((_TMP6 + _TMP7) - 1.00000000E+00);
    _c0065 = TEX0.xy + _g1;
    _TMP8 = COMPAT_TEXTURE(Texture, _c0065);
    _B = vec3(float(_TMP8.x), float(_TMP8.y), float(_TMP8.z));
    _c0067 = (TEX0.xy + _g1) - _g2;
    _TMP9 = COMPAT_TEXTURE(Texture, _c0067);
    _C = vec3(float(_TMP9.x), float(_TMP9.y), float(_TMP9.z));
    _c0069 = TEX0.xy + _g2;
    _TMP10 = COMPAT_TEXTURE(Texture, _c0069);
    _D = vec3(float(_TMP10.x), float(_TMP10.y), float(_TMP10.z));
    _TMP11 = COMPAT_TEXTURE(Texture, TEX0.xy);
    _E = vec3(float(_TMP11.x), float(_TMP11.y), float(_TMP11.z));
    _c0073 = TEX0.xy - _g2;
    _TMP12 = COMPAT_TEXTURE(Texture, _c0073);
    _F = vec3(float(_TMP12.x), float(_TMP12.y), float(_TMP12.z));
    _c0075 = (TEX0.xy - _g1) + _g2;
    _TMP13 = COMPAT_TEXTURE(Texture, _c0075);
    _G = vec3(float(_TMP13.x), float(_TMP13.y), float(_TMP13.z));
    _c0077 = TEX0.xy - _g1;
    _TMP14 = COMPAT_TEXTURE(Texture, _c0077);
    _H = vec3(float(_TMP14.x), float(_TMP14.y), float(_TMP14.z));
    _c0079 = (TEX0.xy - _g1) - _g2;
    _TMP15 = COMPAT_TEXTURE(Texture, _c0079);
    _I = vec3(float(_TMP15.x), float(_TMP15.y), float(_TMP15.z));
    _E14 = _E;
    _E19 = _E;
    _E24 = _E;
    _TMP41 = dot(vec3(float(_B.x), float(_B.y), float(_B.z)), vec3( 6.55360000E+04, 2.55000000E+02, 1.00000000E+00));
    _TMP40 = float(_TMP41);
    _b = float(_TMP40);
    _TMP41 = dot(vec3(float(_C.x), float(_C.y), float(_C.z)), vec3( 6.55360000E+04, 2.55000000E+02, 1.00000000E+00));
    _TMP40 = float(_TMP41);
    _c = float(_TMP40);
    _TMP41 = dot(vec3(float(_D.x), float(_D.y), float(_D.z)), vec3( 6.55360000E+04, 2.55000000E+02, 1.00000000E+00));
    _TMP40 = float(_TMP41);
    _d = float(_TMP40);
    _TMP41 = dot(vec3(float(_E.x), float(_E.y), float(_E.z)), vec3( 6.55360000E+04, 2.55000000E+02, 1.00000000E+00));
    _TMP40 = float(_TMP41);
    _e = float(_TMP40);
    _TMP41 = dot(vec3(float(_F.x), float(_F.y), float(_F.z)), vec3( 6.55360000E+04, 2.55000000E+02, 1.00000000E+00));
    _TMP40 = float(_TMP41);
    _f = float(_TMP40);
    _TMP41 = dot(vec3(float(_G.x), float(_G.y), float(_G.z)), vec3( 6.55360000E+04, 2.55000000E+02, 1.00000000E+00));
    _TMP40 = float(_TMP41);
    _g = float(_TMP40);
    _TMP41 = dot(vec3(float(_H.x), float(_H.y), float(_H.z)), vec3( 6.55360000E+04, 2.55000000E+02, 1.00000000E+00));
    _TMP40 = float(_TMP41);
    _h = float(_TMP40);
    _TMP41 = dot(vec3(float(_I.x), float(_I.y), float(_I.z)), vec3( 6.55360000E+04, 2.55000000E+02, 1.00000000E+00));
    _TMP40 = float(_TMP41);
    _i = float(_TMP40);
    if (_h == _f && _h != _e && (_e == _g && (_h == _i || _e == _d) || _e == _c && (_h == _i || _e == _b))) { 
        _E24 = _F;
        _E19 = _E + 8.75000000E-01*(_F - _E);
        _E14 = _E + 1.25000000E-01*(_F - _E);
    } 
    if (_fp.x < 4.00000006E-01) { 
        if (_fp.x < 2.00000003E-01) { 
            if (_fp.y < 2.00000003E-01) { 
                _TMP18 = _E24;
            } else {
                if (_fp.y < 4.00000006E-01) { 
                    _TMP19 = _E19;
                } else {
                    if (_fp.y < 6.00000024E-01) { 
                        _TMP20 = _E14;
                    } else {
                        if (_fp.y < 8.00000012E-01) { 
                            _TMP21 = _E19;
                        } else {
                            _TMP21 = _E24;
                        } 
                        _TMP20 = _TMP21;
                    } 
                    _TMP19 = _TMP20;
                } 
                _TMP18 = _TMP19;
            } 
            _TMP17 = _TMP18;
        } else {
            if (_fp.y < 2.00000003E-01) { 
                _TMP22 = _E19;
            } else {
                if (_fp.y < 4.00000006E-01) { 
                    _TMP23 = _E14;
                } else {
                    if (_fp.y < 6.00000024E-01) { 
                        _TMP24 = _E;
                    } else {
                        if (_fp.y < 8.00000012E-01) { 
                            _TMP25 = _E14;
                        } else {
                            _TMP25 = _E19;
                        } 
                        _TMP24 = _TMP25;
                    } 
                    _TMP23 = _TMP24;
                } 
                _TMP22 = _TMP23;
            } 
            _TMP17 = _TMP22;
        } 
        _TMP16 = _TMP17;
    } else {
        if (_fp.x < 8.00000012E-01) { 
            if (_fp.x < 6.00000024E-01) { 
                if (_fp.y < 2.00000003E-01) { 
                    _TMP28 = _E14;
                } else {
                    if (_fp.y < 4.00000006E-01) { 
                        _TMP29 = _E;
                    } else {
                        if (_fp.y < 6.00000024E-01) { 
                            _TMP30 = _E;
                        } else {
                            if (_fp.y < 8.00000012E-01) { 
                                _TMP31 = _E;
                            } else {
                                _TMP31 = _E14;
                            } 
                            _TMP30 = _TMP31;
                        } 
                        _TMP29 = _TMP30;
                    } 
                    _TMP28 = _TMP29;
                } 
                _TMP27 = _TMP28;
            } else {
                if (_fp.y < 2.00000003E-01) { 
                    _TMP32 = _E19;
                } else {
                    if (_fp.y < 4.00000006E-01) { 
                        _TMP33 = _E14;
                    } else {
                        if (_fp.y < 6.00000024E-01) { 
                            _TMP34 = _E;
                        } else {
                            if (_fp.y < 8.00000012E-01) { 
                                _TMP35 = _E14;
                            } else {
                                _TMP35 = _E19;
                            } 
                            _TMP34 = _TMP35;
                        } 
                        _TMP33 = _TMP34;
                    } 
                    _TMP32 = _TMP33;
                } 
                _TMP27 = _TMP32;
            } 
            _TMP26 = _TMP27;
        } else {
            if (_fp.y < 2.00000003E-01) { 
                _TMP36 = _E24;
            } else {
                if (_fp.y < 4.00000006E-01) { 
                    _TMP37 = _E19;
                } else {
                    if (_fp.y < 6.00000024E-01) { 
                        _TMP38 = _E14;
                    } else {
                        if (_fp.y < 8.00000012E-01) { 
                            _TMP39 = _E19;
                        } else {
                            _TMP39 = _E24;
                        } 
                        _TMP38 = _TMP39;
                    } 
                    _TMP37 = _TMP38;
                } 
                _TMP36 = _TMP37;
            } 
            _TMP26 = _TMP36;
        } 
        _TMP16 = _TMP26;
    } 
    _res = vec3(float(_TMP16.x), float(_TMP16.y), float(_TMP16.z));
    _ret_0 = vec4(_res.x, _res.y, _res.z, 1.00000000E+00);
    FragColor = _ret_0;
    return;
} 
#endif
