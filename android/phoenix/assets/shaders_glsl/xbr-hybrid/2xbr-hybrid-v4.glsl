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
COMPAT_VARYING     vec4 VARt7;
COMPAT_VARYING     vec4 VARt6;
COMPAT_VARYING     vec4 VARt5;
COMPAT_VARYING     vec4 VARt4;
COMPAT_VARYING     vec4 VARt3;
COMPAT_VARYING     vec4 VARt2;
COMPAT_VARYING     vec4 VARt1;
COMPAT_VARYING     vec2 _texCoord1;
COMPAT_VARYING     vec4 _color1;
COMPAT_VARYING     vec4 _position1;
struct input_dummy {
    vec2 _video_size;
    vec2 _texture_size;
    vec2 _output_dummy_size;
};
struct out_vertex {
    vec4 _position1;
    vec4 _color1;
    vec2 _texCoord1;
    vec4 VARt1;
    vec4 VARt2;
    vec4 VARt3;
    vec4 VARt4;
    vec4 VARt5;
    vec4 VARt6;
    vec4 VARt7;
};
out_vertex _ret_0;
input_dummy _IN1;
vec4 _r0010;
COMPAT_ATTRIBUTE vec4 VertexCoord;
COMPAT_ATTRIBUTE vec4 COLOR;
COMPAT_ATTRIBUTE vec4 TexCoord;
COMPAT_VARYING vec4 COL0;
COMPAT_VARYING vec4 TEX0;
 
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
    _r0010 = VertexCoord.x*MVPMatrix[0];
    _r0010 = _r0010 + VertexCoord.y*MVPMatrix[1];
    _r0010 = _r0010 + VertexCoord.z*MVPMatrix[2];
    _r0010 = _r0010 + VertexCoord.w*MVPMatrix[3];
    _ps = vec2(1.00000000E+00/TextureSize.x, 1.00000000E+00/TextureSize.y);
    _OUT.VARt1 = TexCoord.xxxy + vec4(float(float(-_ps.x)), 0.00000000E+00, float(float(_ps.x)), float(float((-2.00000000E+00*_ps.y))));
    _OUT.VARt2 = TexCoord.xxxy + vec4(float(float(-_ps.x)), 0.00000000E+00, float(float(_ps.x)), float(float(-_ps.y)));
    _OUT.VARt3 = TexCoord.xxxy + vec4(float(float(-_ps.x)), 0.00000000E+00, float(float(_ps.x)), 0.00000000E+00);
    _OUT.VARt4 = TexCoord.xxxy + vec4(float(float(-_ps.x)), 0.00000000E+00, float(float(_ps.x)), float(float(_ps.y)));
    _OUT.VARt5 = TexCoord.xxxy + vec4(float(float(-_ps.x)), 0.00000000E+00, float(float(_ps.x)), float(float((2.00000000E+00*_ps.y))));
    _OUT.VARt6 = TexCoord.xyyy + vec4(float(float((-2.00000000E+00*_ps.x))), float(float(-_ps.y)), 0.00000000E+00, float(float(_ps.y)));
    _OUT.VARt7 = TexCoord.xyyy + vec4(float(float((2.00000000E+00*_ps.x))), float(float(-_ps.y)), 0.00000000E+00, float(float(_ps.y)));
    _ret_0._position1 = _r0010;
    _ret_0._color1 = COLOR;
    _ret_0._texCoord1 = TexCoord.xy;
    VARt1 = _OUT.VARt1;
    VARt2 = _OUT.VARt2;
    VARt3 = _OUT.VARt3;
    VARt4 = _OUT.VARt4;
    VARt5 = _OUT.VARt5;
    VARt6 = _OUT.VARt6;
    VARt7 = _OUT.VARt7;
    gl_Position = vec4(float(_r0010.x), float(_r0010.y), float(_r0010.z), float(_r0010.w));
    COL0 = COLOR;
    TEX0.xy = TexCoord.xy;
    return;
    COL0 = _ret_0._color1;
    TEX0.xy = _ret_0._texCoord1;
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
COMPAT_VARYING     vec4 VARt7;
COMPAT_VARYING     vec4 VARt6;
COMPAT_VARYING     vec4 VARt5;
COMPAT_VARYING     vec4 VARt4;
COMPAT_VARYING     vec4 VARt3;
COMPAT_VARYING     vec4 VARt2;
COMPAT_VARYING     vec4 VARt1;
COMPAT_VARYING     vec2 _texCoord;
COMPAT_VARYING     vec4 _color;
struct input_dummy {
    vec2 _video_size;
    vec2 _texture_size;
    vec2 _output_dummy_size;
};
struct out_vertex {
    vec4 _color;
    vec2 _texCoord;
    vec4 VARt1;
    vec4 VARt2;
    vec4 VARt3;
    vec4 VARt4;
    vec4 VARt5;
    vec4 VARt6;
    vec4 VARt7;
};
vec4 _ret_0;
float _TMP67;
float _TMP66;
float _TMP65;
vec3 _TMP76;
vec3 _TMP64;
vec3 _TMP63;
vec3 _TMP62;
vec3 _TMP61;
vec3 _TMP60;
vec3 _TMP59;
vec3 _TMP58;
vec3 _TMP57;
vec3 _TMP56;
vec3 _TMP55;
vec4 _TMP54;
float _TMP53;
float _TMP52;
float _TMP51;
vec3 _TMP74;
vec3 _TMP49;
vec3 _TMP48;
vec3 _TMP47;
vec3 _TMP46;
vec4 _TMP39;
vec4 _TMP38;
vec4 _TMP77;
bvec4 _TMP37;
bvec4 _TMP36;
bvec4 _TMP35;
bvec4 _TMP34;
bvec4 _TMP33;
bvec4 _TMP32;
bvec4 _TMP31;
bvec4 _TMP30;
bvec4 _TMP29;
bvec4 _TMP28;
bvec4 _TMP27;
float _TMP75;
vec4 _TMP20;
vec4 _TMP19;
vec4 _TMP18;
vec4 _TMP17;
vec4 _TMP16;
vec4 _TMP15;
vec4 _TMP14;
vec4 _TMP13;
vec4 _TMP12;
vec4 _TMP11;
vec4 _TMP10;
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
out_vertex _VAR1;
uniform sampler2D Texture;
input_dummy _IN1;
vec2 _x0090;
vec4 _r0134;
vec4 _r0144;
vec4 _r0154;
vec4 _r0164;
vec4 _r0174;
vec4 _r0184;
vec4 _TMP195;
vec4 _a0198;
vec4 _TMP201;
vec4 _a0204;
vec4 _TMP207;
vec4 _a0210;
vec4 _TMP213;
vec4 _a0216;
vec4 _TMP219;
vec4 _a0222;
vec4 _TMP225;
vec4 _a0228;
vec4 _TMP231;
vec4 _a0234;
vec4 _TMP237;
vec4 _a0240;
vec4 _TMP243;
vec4 _a0246;
vec4 _TMP249;
vec4 _a0252;
vec4 _TMP255;
vec4 _a0258;
vec4 _x0262;
vec4 _TMP263;
vec4 _x0272;
vec4 _TMP273;
vec4 _x0282;
vec4 _TMP283;
vec4 _TMP291;
vec4 _a0294;
vec4 _TMP295;
vec4 _a0298;
vec4 _TMP299;
vec4 _a0302;
vec4 _TMP303;
vec4 _a0306;
vec4 _TMP307;
vec4 _a0310;
vec4 _TMP313;
vec4 _a0316;
vec4 _TMP317;
vec4 _a0320;
vec4 _TMP321;
vec4 _a0324;
vec4 _TMP325;
vec4 _a0328;
vec4 _TMP329;
vec4 _a0332;
vec4 _TMP333;
vec4 _a0336;
vec4 _TMP337;
vec4 _a0340;
vec4 _TMP341;
vec4 _a0344;
vec4 _TMP345;
vec4 _a0348;
vec4 _TMP349;
vec4 _a0352;
vec4 _TMP353;
vec4 _a0356;
vec3 _b0360;
vec3 _b0364;
vec3 _TMP365;
vec3 _a0366;
vec3 _b0374;
vec3 _b0378;
vec3 _TMP379;
vec3 _a0380;
vec4 _a0386;
vec4 _a0388;
vec4 _a0390;
vec3 _b0396;
vec3 _b0398;
vec3 _df0400;
vec3 _a0402;
vec3 _df0404;
vec3 _a0406;
COMPAT_VARYING vec4 TEX0;
 
uniform int FrameDirection;
uniform int FrameCount;
uniform COMPAT_PRECISION vec2 OutputSize;
uniform COMPAT_PRECISION vec2 TextureSize;
uniform COMPAT_PRECISION vec2 InputSize;
void main()
{
    bvec4 _edr;
    bvec4 _edr_left;
    bvec4 _edr_up;
    bvec4 _px;
    bvec4 _interp_restriction_lv1;
    bvec4 _interp_restriction_lv2_left;
    bvec4 _interp_restriction_lv2_up;
    bvec4 _nc;
    bvec4 _nc30;
    bvec4 _nc60;
    bvec4 _nc45;
    vec4 _fx;
    vec4 _fx_left;
    vec4 _fx_up;
    vec3 _res1;
    vec3 _res2;
    vec3 _pix1;
    vec3 _pix2;
    float _blend1;
    float _blend2;
    vec2 _fp;
    vec3 _A11;
    vec3 _B11;
    vec3 _C1;
    vec3 _A2;
    vec3 _B2;
    vec3 _C;
    vec3 _D;
    vec3 _E;
    vec3 _F;
    vec3 _G;
    vec3 _H;
    vec3 _I;
    vec3 _G5;
    vec3 _H5;
    vec3 _I5;
    vec3 _A0;
    vec3 _D0;
    vec3 _G0;
    vec3 _C4;
    vec3 _F4;
    vec3 _I4;
    vec4 _b1;
    vec4 _c3;
    vec4 _e1;
    vec4 _i4;
    vec4 _i5;
    vec4 _h5;
    vec4 _fx45;
    vec4 _fx30;
    vec4 _fx60;
    vec3 _res;
    vec3 _n1;
    vec3 _n2;
    vec3 _n3;
    vec3 _n4;
    vec3 _s;
    vec3 _aa;
    vec3 _bb;
    vec3 _cc;
    vec3 _dd;
    vec3 _t;
    vec3 _m;
    vec3 _s1;
    vec3 _s0;
    vec4 _maximo;
    _x0090 = TEX0.xy*TextureSize;
    _fp = fract(_x0090);
    _TMP0 = COMPAT_TEXTURE(Texture, VARt1.xw);
    _A11 = vec3(float(_TMP0.x), float(_TMP0.y), float(_TMP0.z));
    _TMP1 = COMPAT_TEXTURE(Texture, VARt1.yw);
    _B11 = vec3(float(_TMP1.x), float(_TMP1.y), float(_TMP1.z));
    _TMP2 = COMPAT_TEXTURE(Texture, VARt1.zw);
    _C1 = vec3(float(_TMP2.x), float(_TMP2.y), float(_TMP2.z));
    _TMP3 = COMPAT_TEXTURE(Texture, VARt2.xw);
    _A2 = vec3(float(_TMP3.x), float(_TMP3.y), float(_TMP3.z));
    _TMP4 = COMPAT_TEXTURE(Texture, VARt2.yw);
    _B2 = vec3(float(_TMP4.x), float(_TMP4.y), float(_TMP4.z));
    _TMP5 = COMPAT_TEXTURE(Texture, VARt2.zw);
    _C = vec3(float(_TMP5.x), float(_TMP5.y), float(_TMP5.z));
    _TMP6 = COMPAT_TEXTURE(Texture, VARt3.xw);
    _D = vec3(float(_TMP6.x), float(_TMP6.y), float(_TMP6.z));
    _TMP7 = COMPAT_TEXTURE(Texture, VARt3.yw);
    _E = vec3(float(_TMP7.x), float(_TMP7.y), float(_TMP7.z));
    _TMP8 = COMPAT_TEXTURE(Texture, VARt3.zw);
    _F = vec3(float(_TMP8.x), float(_TMP8.y), float(_TMP8.z));
    _TMP9 = COMPAT_TEXTURE(Texture, VARt4.xw);
    _G = vec3(float(_TMP9.x), float(_TMP9.y), float(_TMP9.z));
    _TMP10 = COMPAT_TEXTURE(Texture, VARt4.yw);
    _H = vec3(float(_TMP10.x), float(_TMP10.y), float(_TMP10.z));
    _TMP11 = COMPAT_TEXTURE(Texture, VARt4.zw);
    _I = vec3(float(_TMP11.x), float(_TMP11.y), float(_TMP11.z));
    _TMP12 = COMPAT_TEXTURE(Texture, VARt5.xw);
    _G5 = vec3(float(_TMP12.x), float(_TMP12.y), float(_TMP12.z));
    _TMP13 = COMPAT_TEXTURE(Texture, VARt5.yw);
    _H5 = vec3(float(_TMP13.x), float(_TMP13.y), float(_TMP13.z));
    _TMP14 = COMPAT_TEXTURE(Texture, VARt5.zw);
    _I5 = vec3(float(_TMP14.x), float(_TMP14.y), float(_TMP14.z));
    _TMP15 = COMPAT_TEXTURE(Texture, VARt6.xy);
    _A0 = vec3(float(_TMP15.x), float(_TMP15.y), float(_TMP15.z));
    _TMP16 = COMPAT_TEXTURE(Texture, VARt6.xz);
    _D0 = vec3(float(_TMP16.x), float(_TMP16.y), float(_TMP16.z));
    _TMP17 = COMPAT_TEXTURE(Texture, VARt6.xw);
    _G0 = vec3(float(_TMP17.x), float(_TMP17.y), float(_TMP17.z));
    _TMP18 = COMPAT_TEXTURE(Texture, VARt7.xy);
    _C4 = vec3(float(_TMP18.x), float(_TMP18.y), float(_TMP18.z));
    _TMP19 = COMPAT_TEXTURE(Texture, VARt7.xz);
    _F4 = vec3(float(_TMP19.x), float(_TMP19.y), float(_TMP19.z));
    _TMP20 = COMPAT_TEXTURE(Texture, VARt7.xw);
    _I4 = vec3(float(_TMP20.x), float(_TMP20.y), float(_TMP20.z));
    _TMP75 = dot(vec3(float(_B2.x), float(_B2.y), float(_B2.z)), vec3( 1.43593750E+01, 2.81718750E+01, 5.47265625E+00));
    _r0134.x = float(_TMP75);
    _TMP75 = dot(vec3(float(_D.x), float(_D.y), float(_D.z)), vec3( 1.43593750E+01, 2.81718750E+01, 5.47265625E+00));
    _r0134.y = float(_TMP75);
    _TMP75 = dot(vec3(float(_H.x), float(_H.y), float(_H.z)), vec3( 1.43593750E+01, 2.81718750E+01, 5.47265625E+00));
    _r0134.z = float(_TMP75);
    _TMP75 = dot(vec3(float(_F.x), float(_F.y), float(_F.z)), vec3( 1.43593750E+01, 2.81718750E+01, 5.47265625E+00));
    _r0134.w = float(_TMP75);
    _b1 = vec4(float(_r0134.x), float(_r0134.y), float(_r0134.z), float(_r0134.w));
    _TMP75 = dot(vec3(float(_C.x), float(_C.y), float(_C.z)), vec3( 1.43593750E+01, 2.81718750E+01, 5.47265625E+00));
    _r0144.x = float(_TMP75);
    _TMP75 = dot(vec3(float(_A2.x), float(_A2.y), float(_A2.z)), vec3( 1.43593750E+01, 2.81718750E+01, 5.47265625E+00));
    _r0144.y = float(_TMP75);
    _TMP75 = dot(vec3(float(_G.x), float(_G.y), float(_G.z)), vec3( 1.43593750E+01, 2.81718750E+01, 5.47265625E+00));
    _r0144.z = float(_TMP75);
    _TMP75 = dot(vec3(float(_I.x), float(_I.y), float(_I.z)), vec3( 1.43593750E+01, 2.81718750E+01, 5.47265625E+00));
    _r0144.w = float(_TMP75);
    _c3 = vec4(float(_r0144.x), float(_r0144.y), float(_r0144.z), float(_r0144.w));
    _TMP75 = dot(vec3(float(_E.x), float(_E.y), float(_E.z)), vec3( 1.43593750E+01, 2.81718750E+01, 5.47265625E+00));
    _r0154.x = float(_TMP75);
    _TMP75 = dot(vec3(float(_E.x), float(_E.y), float(_E.z)), vec3( 1.43593750E+01, 2.81718750E+01, 5.47265625E+00));
    _r0154.y = float(_TMP75);
    _TMP75 = dot(vec3(float(_E.x), float(_E.y), float(_E.z)), vec3( 1.43593750E+01, 2.81718750E+01, 5.47265625E+00));
    _r0154.z = float(_TMP75);
    _TMP75 = dot(vec3(float(_E.x), float(_E.y), float(_E.z)), vec3( 1.43593750E+01, 2.81718750E+01, 5.47265625E+00));
    _r0154.w = float(_TMP75);
    _e1 = vec4(float(_r0154.x), float(_r0154.y), float(_r0154.z), float(_r0154.w));
    _TMP75 = dot(vec3(float(_I4.x), float(_I4.y), float(_I4.z)), vec3( 1.43593750E+01, 2.81718750E+01, 5.47265625E+00));
    _r0164.x = float(_TMP75);
    _TMP75 = dot(vec3(float(_C1.x), float(_C1.y), float(_C1.z)), vec3( 1.43593750E+01, 2.81718750E+01, 5.47265625E+00));
    _r0164.y = float(_TMP75);
    _TMP75 = dot(vec3(float(_A0.x), float(_A0.y), float(_A0.z)), vec3( 1.43593750E+01, 2.81718750E+01, 5.47265625E+00));
    _r0164.z = float(_TMP75);
    _TMP75 = dot(vec3(float(_G5.x), float(_G5.y), float(_G5.z)), vec3( 1.43593750E+01, 2.81718750E+01, 5.47265625E+00));
    _r0164.w = float(_TMP75);
    _i4 = vec4(float(_r0164.x), float(_r0164.y), float(_r0164.z), float(_r0164.w));
    _TMP75 = dot(vec3(float(_I5.x), float(_I5.y), float(_I5.z)), vec3( 1.43593750E+01, 2.81718750E+01, 5.47265625E+00));
    _r0174.x = float(_TMP75);
    _TMP75 = dot(vec3(float(_C4.x), float(_C4.y), float(_C4.z)), vec3( 1.43593750E+01, 2.81718750E+01, 5.47265625E+00));
    _r0174.y = float(_TMP75);
    _TMP75 = dot(vec3(float(_A11.x), float(_A11.y), float(_A11.z)), vec3( 1.43593750E+01, 2.81718750E+01, 5.47265625E+00));
    _r0174.z = float(_TMP75);
    _TMP75 = dot(vec3(float(_G0.x), float(_G0.y), float(_G0.z)), vec3( 1.43593750E+01, 2.81718750E+01, 5.47265625E+00));
    _r0174.w = float(_TMP75);
    _i5 = vec4(float(_r0174.x), float(_r0174.y), float(_r0174.z), float(_r0174.w));
    _TMP75 = dot(vec3(float(_H5.x), float(_H5.y), float(_H5.z)), vec3( 1.43593750E+01, 2.81718750E+01, 5.47265625E+00));
    _r0184.x = float(_TMP75);
    _TMP75 = dot(vec3(float(_F4.x), float(_F4.y), float(_F4.z)), vec3( 1.43593750E+01, 2.81718750E+01, 5.47265625E+00));
    _r0184.y = float(_TMP75);
    _TMP75 = dot(vec3(float(_B11.x), float(_B11.y), float(_B11.z)), vec3( 1.43593750E+01, 2.81718750E+01, 5.47265625E+00));
    _r0184.z = float(_TMP75);
    _TMP75 = dot(vec3(float(_D0.x), float(_D0.y), float(_D0.z)), vec3( 1.43593750E+01, 2.81718750E+01, 5.47265625E+00));
    _r0184.w = float(_TMP75);
    _h5 = vec4(float(_r0184.x), float(_r0184.y), float(_r0184.z), float(_r0184.w));
    _fx = vec4( 1.00000000E+00, -1.00000000E+00, -1.00000000E+00, 1.00000000E+00)*_fp.y + vec4( 1.00000000E+00, 1.00000000E+00, -1.00000000E+00, -1.00000000E+00)*_fp.x;
    _fx_left = vec4( 1.00000000E+00, -1.00000000E+00, -1.00000000E+00, 1.00000000E+00)*_fp.y + vec4( 5.00000000E-01, 2.00000000E+00, -5.00000000E-01, -2.00000000E+00)*_fp.x;
    _fx_up = vec4( 1.00000000E+00, -1.00000000E+00, -1.00000000E+00, 1.00000000E+00)*_fp.y + vec4( 2.00000000E+00, 5.00000000E-01, -2.00000000E+00, -5.00000000E-01)*_fp.x;
    _a0198 = _b1.wxyz - _b1;
    _TMP195 = abs(_a0198);
    _TMP27 = bvec4(_TMP195.x < 1.50000000E+01, _TMP195.y < 1.50000000E+01, _TMP195.z < 1.50000000E+01, _TMP195.w < 1.50000000E+01);
    _a0204 = _b1.wxyz - _c3;
    _TMP201 = abs(_a0204);
    _TMP28 = bvec4(_TMP201.x < 1.50000000E+01, _TMP201.y < 1.50000000E+01, _TMP201.z < 1.50000000E+01, _TMP201.w < 1.50000000E+01);
    _a0210 = _b1.zwxy - _b1.yzwx;
    _TMP207 = abs(_a0210);
    _TMP29 = bvec4(_TMP207.x < 1.50000000E+01, _TMP207.y < 1.50000000E+01, _TMP207.z < 1.50000000E+01, _TMP207.w < 1.50000000E+01);
    _a0216 = _b1.zwxy - _c3.zwxy;
    _TMP213 = abs(_a0216);
    _TMP30 = bvec4(_TMP213.x < 1.50000000E+01, _TMP213.y < 1.50000000E+01, _TMP213.z < 1.50000000E+01, _TMP213.w < 1.50000000E+01);
    _a0222 = _e1 - _c3.wxyz;
    _TMP219 = abs(_a0222);
    _TMP31 = bvec4(_TMP219.x < 1.50000000E+01, _TMP219.y < 1.50000000E+01, _TMP219.z < 1.50000000E+01, _TMP219.w < 1.50000000E+01);
    _a0228 = _b1.wxyz - _h5.yzwx;
    _TMP225 = abs(_a0228);
    _TMP32 = bvec4(_TMP225.x < 1.50000000E+01, _TMP225.y < 1.50000000E+01, _TMP225.z < 1.50000000E+01, _TMP225.w < 1.50000000E+01);
    _a0234 = _b1.wxyz - _i4;
    _TMP231 = abs(_a0234);
    _TMP33 = bvec4(_TMP231.x < 1.50000000E+01, _TMP231.y < 1.50000000E+01, _TMP231.z < 1.50000000E+01, _TMP231.w < 1.50000000E+01);
    _a0240 = _b1.zwxy - _h5;
    _TMP237 = abs(_a0240);
    _TMP34 = bvec4(_TMP237.x < 1.50000000E+01, _TMP237.y < 1.50000000E+01, _TMP237.z < 1.50000000E+01, _TMP237.w < 1.50000000E+01);
    _a0246 = _b1.zwxy - _i5;
    _TMP243 = abs(_a0246);
    _TMP35 = bvec4(_TMP243.x < 1.50000000E+01, _TMP243.y < 1.50000000E+01, _TMP243.z < 1.50000000E+01, _TMP243.w < 1.50000000E+01);
    _a0252 = _e1 - _c3.zwxy;
    _TMP249 = abs(_a0252);
    _TMP36 = bvec4(_TMP249.x < 1.50000000E+01, _TMP249.y < 1.50000000E+01, _TMP249.z < 1.50000000E+01, _TMP249.w < 1.50000000E+01);
    _a0258 = _e1 - _c3;
    _TMP255 = abs(_a0258);
    _TMP37 = bvec4(_TMP255.x < 1.50000000E+01, _TMP255.y < 1.50000000E+01, _TMP255.z < 1.50000000E+01, _TMP255.w < 1.50000000E+01);
    _interp_restriction_lv1 = bvec4(_e1.x != _b1.w && _e1.x != _b1.z && (!_TMP27.x && !_TMP28.x || !_TMP29.x && !_TMP30.x || _TMP31.x && (!_TMP32.x && !_TMP33.x || !_TMP34.x && !_TMP35.x) || _TMP36.x || _TMP37.x), _e1.y != _b1.x && _e1.y != _b1.w && (!_TMP27.y && !_TMP28.y || !_TMP29.y && !_TMP30.y || _TMP31.y && (!_TMP32.y && !_TMP33.y || !_TMP34.y && !_TMP35.y) || _TMP36.y || _TMP37.y), _e1.z != _b1.y && _e1.z != _b1.x && (!_TMP27.z && !_TMP28.z || !_TMP29.z && !_TMP30.z || _TMP31.z && (!_TMP32.z && !_TMP33.z || !_TMP34.z && !_TMP35.z) || _TMP36.z || _TMP37.z), _e1.w != _b1.z && _e1.w != _b1.y && (!_TMP27.w && !_TMP28.w || !_TMP29.w && !_TMP30.w || _TMP31.w && (!_TMP32.w && !_TMP33.w || !_TMP34.w && !_TMP35.w) || _TMP36.w || _TMP37.w));
    _interp_restriction_lv2_left = bvec4(_e1.x != _c3.z && _b1.y != _c3.z, _e1.y != _c3.w && _b1.z != _c3.w, _e1.z != _c3.x && _b1.w != _c3.x, _e1.w != _c3.y && _b1.x != _c3.y);
    _interp_restriction_lv2_up = bvec4(_e1.x != _c3.x && _b1.x != _c3.x, _e1.y != _c3.y && _b1.y != _c3.y, _e1.z != _c3.z && _b1.z != _c3.z, _e1.w != _c3.w && _b1.w != _c3.w);
    _x0262 = _fx - vec4( 1.00000000E+00, 0.00000000E+00, -1.00000000E+00, 0.00000000E+00);
    _TMP77 = min(vec4( 1.00000000E+00, 1.00000000E+00, 1.00000000E+00, 1.00000000E+00), _x0262);
    _TMP263 = max(vec4( 0.00000000E+00, 0.00000000E+00, 0.00000000E+00, 0.00000000E+00), _TMP77);
    _fx45 = _TMP263*_TMP263*(3.00000000E+00 - 2.00000000E+00*_TMP263);
    _x0272 = _fx_left - vec4( 5.00000000E-01, 5.00000000E-01, -1.00000000E+00, -5.00000000E-01);
    _TMP77 = min(vec4( 1.00000000E+00, 1.00000000E+00, 1.00000000E+00, 1.00000000E+00), _x0272);
    _TMP273 = max(vec4( 0.00000000E+00, 0.00000000E+00, 0.00000000E+00, 0.00000000E+00), _TMP77);
    _fx30 = _TMP273*_TMP273*(3.00000000E+00 - 2.00000000E+00*_TMP273);
    _x0282 = _fx_up - vec4( 1.50000000E+00, -5.00000000E-01, -1.50000000E+00, 0.00000000E+00);
    _TMP77 = min(vec4( 1.00000000E+00, 1.00000000E+00, 1.00000000E+00, 1.00000000E+00), _x0282);
    _TMP283 = max(vec4( 0.00000000E+00, 0.00000000E+00, 0.00000000E+00, 0.00000000E+00), _TMP77);
    _fx60 = _TMP283*_TMP283*(3.00000000E+00 - 2.00000000E+00*_TMP283);
    _a0294 = _e1 - _c3;
    _TMP291 = abs(_a0294);
    _a0298 = _e1 - _c3.zwxy;
    _TMP295 = abs(_a0298);
    _a0302 = _c3.wxyz - _h5;
    _TMP299 = abs(_a0302);
    _a0306 = _c3.wxyz - _h5.yzwx;
    _TMP303 = abs(_a0306);
    _a0310 = _b1.zwxy - _b1.wxyz;
    _TMP307 = abs(_a0310);
    _TMP38 = _TMP291 + _TMP295 + _TMP299 + _TMP303 + 4.00000000E+00*_TMP307;
    _a0316 = _b1.zwxy - _b1.yzwx;
    _TMP313 = abs(_a0316);
    _a0320 = _b1.zwxy - _i5;
    _TMP317 = abs(_a0320);
    _a0324 = _b1.wxyz - _i4;
    _TMP321 = abs(_a0324);
    _a0328 = _b1.wxyz - _b1;
    _TMP325 = abs(_a0328);
    _a0332 = _e1 - _c3.wxyz;
    _TMP329 = abs(_a0332);
    _TMP39 = _TMP313 + _TMP317 + _TMP321 + _TMP325 + 4.00000000E+00*_TMP329;
    _edr = bvec4((_TMP38 + 3.50000000E+00).x < _TMP39.x && _interp_restriction_lv1.x, (_TMP38 + 3.50000000E+00).y < _TMP39.y && _interp_restriction_lv1.y, (_TMP38 + 3.50000000E+00).z < _TMP39.z && _interp_restriction_lv1.z, (_TMP38 + 3.50000000E+00).w < _TMP39.w && _interp_restriction_lv1.w);
    _a0336 = _b1.wxyz - _c3.zwxy;
    _TMP333 = abs(_a0336);
    _a0340 = _b1.zwxy - _c3;
    _TMP337 = abs(_a0340);
    _edr_left = bvec4((2.00000000E+00*_TMP333).x <= _TMP337.x && _interp_restriction_lv2_left.x, (2.00000000E+00*_TMP333).y <= _TMP337.y && _interp_restriction_lv2_left.y, (2.00000000E+00*_TMP333).z <= _TMP337.z && _interp_restriction_lv2_left.z, (2.00000000E+00*_TMP333).w <= _TMP337.w && _interp_restriction_lv2_left.w);
    _a0344 = _b1.wxyz - _c3.zwxy;
    _TMP341 = abs(_a0344);
    _a0348 = _b1.zwxy - _c3;
    _TMP345 = abs(_a0348);
    _edr_up = bvec4(_TMP341.x >= (2.00000000E+00*_TMP345).x && _interp_restriction_lv2_up.x, _TMP341.y >= (2.00000000E+00*_TMP345).y && _interp_restriction_lv2_up.y, _TMP341.z >= (2.00000000E+00*_TMP345).z && _interp_restriction_lv2_up.z, _TMP341.w >= (2.00000000E+00*_TMP345).w && _interp_restriction_lv2_up.w);
    _nc45 = bvec4(_edr.x && bool(_fx45.x), _edr.y && bool(_fx45.y), _edr.z && bool(_fx45.z), _edr.w && bool(_fx45.w));
    _nc30 = bvec4(_edr.x && _edr_left.x && bool(_fx30.x), _edr.y && _edr_left.y && bool(_fx30.y), _edr.z && _edr_left.z && bool(_fx30.z), _edr.w && _edr_left.w && bool(_fx30.w));
    _nc60 = bvec4(_edr.x && _edr_up.x && bool(_fx60.x), _edr.y && _edr_up.y && bool(_fx60.y), _edr.z && _edr_up.z && bool(_fx60.z), _edr.w && _edr_up.w && bool(_fx60.w));
    _a0352 = _e1 - _b1.wxyz;
    _TMP349 = abs(_a0352);
    _a0356 = _e1 - _b1.zwxy;
    _TMP353 = abs(_a0356);
    _px = bvec4(_TMP349.x <= _TMP353.x, _TMP349.y <= _TMP353.y, _TMP349.z <= _TMP353.z, _TMP349.w <= _TMP353.w);
    _n1 = vec3(float(_B11.x), float(_B11.y), float(_B11.z));
    _n2 = vec3(float(_B2.x), float(_B2.y), float(_B2.z));
    _s = vec3(float(_E.x), float(_E.y), float(_E.z));
    _n3 = vec3(float(_H.x), float(_H.y), float(_H.z));
    _n4 = vec3(float(_H5.x), float(_H5.y), float(_H5.z));
    _aa = _n2 - _n1;
    _bb = _s - _n2;
    _cc = _n3 - _s;
    _dd = _n4 - _n3;
    _t = (7.00000000E+00*(_bb + _cc) - 3.00000000E+00*(_aa + _dd))/1.60000000E+01;
    _m = vec3(_s.x < 5.00000000E-01 ? (2.00000000E+00*_s).x : (2.00000000E+00*(1.00000000E+00 - _s)).x, _s.y < 5.00000000E-01 ? (2.00000000E+00*_s).y : (2.00000000E+00*(1.00000000E+00 - _s)).y, _s.z < 5.00000000E-01 ? (2.00000000E+00*_s).z : (2.00000000E+00*(1.00000000E+00 - _s)).z);
    _TMP46 = abs(_bb);
    _b0360 = 6.49999976E-01*_TMP46;
    _m = min(_m, _b0360);
    _TMP47 = abs(_cc);
    _b0364 = 6.49999976E-01*_TMP47;
    _m = min(_m, _b0364);
    _a0366 = -_m;
    _TMP74 = min(_m, _t);
    _TMP365 = max(_a0366, _TMP74);
    _s1 = (2.00000000E+00*_fp.y - 1.00000000E+00)*_TMP365 + _s;
    _n1 = vec3(float(_D0.x), float(_D0.y), float(_D0.z));
    _n2 = vec3(float(_D.x), float(_D.y), float(_D.z));
    _n3 = vec3(float(_F.x), float(_F.y), float(_F.z));
    _n4 = vec3(float(_F4.x), float(_F4.y), float(_F4.z));
    _aa = _n2 - _n1;
    _bb = _s1 - _n2;
    _cc = _n3 - _s1;
    _dd = _n4 - _n3;
    _t = (7.00000000E+00*(_bb + _cc) - 3.00000000E+00*(_aa + _dd))/1.60000000E+01;
    _m = vec3(_s1.x < 5.00000000E-01 ? (2.00000000E+00*_s1).x : (2.00000000E+00*(1.00000000E+00 - _s1)).x, _s1.y < 5.00000000E-01 ? (2.00000000E+00*_s1).y : (2.00000000E+00*(1.00000000E+00 - _s1)).y, _s1.z < 5.00000000E-01 ? (2.00000000E+00*_s1).z : (2.00000000E+00*(1.00000000E+00 - _s1)).z);
    _TMP48 = abs(_bb);
    _b0374 = 6.49999976E-01*_TMP48;
    _m = min(_m, _b0374);
    _TMP49 = abs(_cc);
    _b0378 = 6.49999976E-01*_TMP49;
    _m = min(_m, _b0378);
    _a0380 = -_m;
    _TMP74 = min(_m, _t);
    _TMP379 = max(_a0380, _TMP74);
    _s0 = (2.00000000E+00*_fp.x - 1.00000000E+00)*_TMP379 + _s1;
    _nc = bvec4(_nc30.x || _nc60.x || _nc45.x, _nc30.y || _nc60.y || _nc45.y, _nc30.z || _nc60.z || _nc45.z, _nc30.w || _nc60.w || _nc45.w);
    _blend2 = 0.00000000E+00;
    _blend1 = 0.00000000E+00;
    _a0386 = vec4(float(_nc45.x), float(_nc45.y), float(_nc45.z), float(_nc45.w));
    _TMP51 = dot(_a0386, _fx45);
    _a0388 = vec4(float(_nc30.x), float(_nc30.y), float(_nc30.z), float(_nc30.w));
    _TMP52 = dot(_a0388, _fx30);
    _a0390 = vec4(float(_nc60.x), float(_nc60.y), float(_nc60.z), float(_nc60.w));
    _TMP53 = dot(_a0390, _fx60);
    _TMP54 = max(vec4(_TMP52, _TMP52, _TMP52, _TMP52), vec4(_TMP53, _TMP53, _TMP53, _TMP53));
    _maximo = max(_TMP54, vec4(_TMP51, _TMP51, _TMP51, _TMP51));
    if (_nc.x) { 
        if (_px.x) { 
            _TMP55 = _F;
        } else {
            _TMP55 = _H;
        } 
        _pix1 = _TMP55;
        _blend1 = _maximo.x;
    } else {
        if (_nc.y) { 
            if (_px.y) { 
                _TMP56 = _B2;
            } else {
                _TMP56 = _F;
            } 
            _pix1 = _TMP56;
            _blend1 = _maximo.y;
        } else {
            if (_nc.z) { 
                if (_px.z) { 
                    _TMP57 = _D;
                } else {
                    _TMP57 = _B2;
                } 
                _pix1 = _TMP57;
                _blend1 = _maximo.z;
            } else {
                if (_nc.w) { 
                    if (_px.w) { 
                        _TMP58 = _H;
                    } else {
                        _TMP58 = _D;
                    } 
                    _pix1 = _TMP58;
                    _blend1 = _maximo.w;
                } 
            } 
        } 
    } 
    if (_nc.w) { 
        if (_px.w) { 
            _TMP59 = _H;
        } else {
            _TMP59 = _D;
        } 
        _pix2 = _TMP59;
        _blend2 = _maximo.w;
    } else {
        if (_nc.z) { 
            if (_px.z) { 
                _TMP60 = _D;
            } else {
                _TMP60 = _B2;
            } 
            _pix2 = _TMP60;
            _blend2 = _maximo.z;
        } else {
            if (_nc.y) { 
                if (_px.y) { 
                    _TMP61 = _B2;
                } else {
                    _TMP61 = _F;
                } 
                _pix2 = _TMP61;
                _blend2 = _maximo.y;
            } else {
                if (_nc.x) { 
                    if (_px.x) { 
                        _TMP62 = _F;
                    } else {
                        _TMP62 = _H;
                    } 
                    _pix2 = _TMP62;
                    _blend2 = _maximo.x;
                } 
            } 
        } 
    } 
    _b0396 = vec3(float(_pix1.x), float(_pix1.y), float(_pix1.z));
    _TMP63 = _s0 + _blend1*(_b0396 - _s0);
    _res1 = vec3(float(_TMP63.x), float(_TMP63.y), float(_TMP63.z));
    _b0398 = vec3(float(_pix2.x), float(_pix2.y), float(_pix2.z));
    _TMP64 = _s0 + _blend2*(_b0398 - _s0);
    _res2 = vec3(float(_TMP64.x), float(_TMP64.y), float(_TMP64.z));
    _a0402 = _E - _res1;
    _TMP76 = abs(vec3(float(_a0402.x), float(_a0402.y), float(_a0402.z)));
    _df0400 = vec3(float(_TMP76.x), float(_TMP76.y), float(_TMP76.z));
    _TMP65 = _df0400.x + _df0400.y + _df0400.z;
    _a0406 = _E - _res2;
    _TMP76 = abs(vec3(float(_a0406.x), float(_a0406.y), float(_a0406.z)));
    _df0404 = vec3(float(_TMP76.x), float(_TMP76.y), float(_TMP76.z));
    _TMP66 = _df0404.x + _df0404.y + _df0404.z;
    _TMP67 = float((_TMP66 >= _TMP65));
    _res = _res1 + _TMP67*(_res2 - _res1);
    _ret_0 = vec4(_res.x, _res.y, _res.z, 1.00000000E+00);
    FragColor = vec4(float(_ret_0.x), float(_ret_0.y), float(_ret_0.z), float(_ret_0.w));
    return;
} 
#endif
