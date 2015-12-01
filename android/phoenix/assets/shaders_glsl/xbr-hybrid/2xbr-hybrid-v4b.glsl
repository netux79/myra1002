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
float _TMP63;
float _TMP62;
float _TMP61;
vec3 _TMP72;
vec3 _TMP60;
vec3 _TMP59;
vec3 _TMP58;
vec3 _TMP57;
vec3 _TMP56;
vec3 _TMP55;
vec3 _TMP54;
vec3 _TMP53;
vec3 _TMP52;
vec3 _TMP51;
vec4 _TMP50;
float _TMP49;
float _TMP48;
float _TMP47;
vec3 _TMP70;
vec3 _TMP45;
vec3 _TMP44;
vec3 _TMP43;
vec3 _TMP42;
vec4 _TMP35;
vec4 _TMP34;
vec4 _TMP73;
bvec4 _TMP33;
bvec4 _TMP32;
bvec4 _TMP31;
bvec4 _TMP30;
bvec4 _TMP29;
bvec4 _TMP28;
bvec4 _TMP27;
float _TMP71;
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
vec2 _x0086;
vec4 _r0130;
vec4 _r0140;
vec4 _r0150;
vec4 _r0160;
vec4 _r0170;
vec4 _r0180;
vec4 _TMP191;
vec4 _a0194;
vec4 _TMP197;
vec4 _a0200;
vec4 _TMP203;
vec4 _a0206;
vec4 _TMP209;
vec4 _a0212;
vec4 _TMP215;
vec4 _a0218;
vec4 _TMP221;
vec4 _a0224;
vec4 _TMP227;
vec4 _a0230;
vec4 _x0234;
vec4 _TMP235;
vec4 _x0244;
vec4 _TMP245;
vec4 _x0254;
vec4 _TMP255;
vec4 _TMP263;
vec4 _a0266;
vec4 _TMP267;
vec4 _a0270;
vec4 _TMP271;
vec4 _a0274;
vec4 _TMP275;
vec4 _a0278;
vec4 _TMP279;
vec4 _a0282;
vec4 _TMP285;
vec4 _a0288;
vec4 _TMP289;
vec4 _a0292;
vec4 _TMP293;
vec4 _a0296;
vec4 _TMP297;
vec4 _a0300;
vec4 _TMP301;
vec4 _a0304;
vec4 _TMP305;
vec4 _a0308;
vec4 _TMP309;
vec4 _a0312;
vec4 _TMP313;
vec4 _a0316;
vec4 _TMP317;
vec4 _a0320;
vec4 _TMP321;
vec4 _a0324;
vec4 _TMP325;
vec4 _a0328;
vec3 _b0332;
vec3 _b0336;
vec3 _TMP337;
vec3 _a0338;
vec3 _b0346;
vec3 _b0350;
vec3 _TMP351;
vec3 _a0352;
vec4 _a0358;
vec4 _a0360;
vec4 _a0362;
vec3 _b0368;
vec3 _b0370;
vec3 _df0372;
vec3 _a0374;
vec3 _df0376;
vec3 _a0378;
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
    _x0086 = TEX0.xy*TextureSize;
    _fp = fract(_x0086);
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
    _TMP71 = dot(vec3(float(_B2.x), float(_B2.y), float(_B2.z)), vec3( 1.43593750E+01, 2.81718750E+01, 5.47265625E+00));
    _r0130.x = float(_TMP71);
    _TMP71 = dot(vec3(float(_D.x), float(_D.y), float(_D.z)), vec3( 1.43593750E+01, 2.81718750E+01, 5.47265625E+00));
    _r0130.y = float(_TMP71);
    _TMP71 = dot(vec3(float(_H.x), float(_H.y), float(_H.z)), vec3( 1.43593750E+01, 2.81718750E+01, 5.47265625E+00));
    _r0130.z = float(_TMP71);
    _TMP71 = dot(vec3(float(_F.x), float(_F.y), float(_F.z)), vec3( 1.43593750E+01, 2.81718750E+01, 5.47265625E+00));
    _r0130.w = float(_TMP71);
    _b1 = vec4(float(_r0130.x), float(_r0130.y), float(_r0130.z), float(_r0130.w));
    _TMP71 = dot(vec3(float(_C.x), float(_C.y), float(_C.z)), vec3( 1.43593750E+01, 2.81718750E+01, 5.47265625E+00));
    _r0140.x = float(_TMP71);
    _TMP71 = dot(vec3(float(_A2.x), float(_A2.y), float(_A2.z)), vec3( 1.43593750E+01, 2.81718750E+01, 5.47265625E+00));
    _r0140.y = float(_TMP71);
    _TMP71 = dot(vec3(float(_G.x), float(_G.y), float(_G.z)), vec3( 1.43593750E+01, 2.81718750E+01, 5.47265625E+00));
    _r0140.z = float(_TMP71);
    _TMP71 = dot(vec3(float(_I.x), float(_I.y), float(_I.z)), vec3( 1.43593750E+01, 2.81718750E+01, 5.47265625E+00));
    _r0140.w = float(_TMP71);
    _c3 = vec4(float(_r0140.x), float(_r0140.y), float(_r0140.z), float(_r0140.w));
    _TMP71 = dot(vec3(float(_E.x), float(_E.y), float(_E.z)), vec3( 1.43593750E+01, 2.81718750E+01, 5.47265625E+00));
    _r0150.x = float(_TMP71);
    _TMP71 = dot(vec3(float(_E.x), float(_E.y), float(_E.z)), vec3( 1.43593750E+01, 2.81718750E+01, 5.47265625E+00));
    _r0150.y = float(_TMP71);
    _TMP71 = dot(vec3(float(_E.x), float(_E.y), float(_E.z)), vec3( 1.43593750E+01, 2.81718750E+01, 5.47265625E+00));
    _r0150.z = float(_TMP71);
    _TMP71 = dot(vec3(float(_E.x), float(_E.y), float(_E.z)), vec3( 1.43593750E+01, 2.81718750E+01, 5.47265625E+00));
    _r0150.w = float(_TMP71);
    _e1 = vec4(float(_r0150.x), float(_r0150.y), float(_r0150.z), float(_r0150.w));
    _TMP71 = dot(vec3(float(_I4.x), float(_I4.y), float(_I4.z)), vec3( 1.43593750E+01, 2.81718750E+01, 5.47265625E+00));
    _r0160.x = float(_TMP71);
    _TMP71 = dot(vec3(float(_C1.x), float(_C1.y), float(_C1.z)), vec3( 1.43593750E+01, 2.81718750E+01, 5.47265625E+00));
    _r0160.y = float(_TMP71);
    _TMP71 = dot(vec3(float(_A0.x), float(_A0.y), float(_A0.z)), vec3( 1.43593750E+01, 2.81718750E+01, 5.47265625E+00));
    _r0160.z = float(_TMP71);
    _TMP71 = dot(vec3(float(_G5.x), float(_G5.y), float(_G5.z)), vec3( 1.43593750E+01, 2.81718750E+01, 5.47265625E+00));
    _r0160.w = float(_TMP71);
    _i4 = vec4(float(_r0160.x), float(_r0160.y), float(_r0160.z), float(_r0160.w));
    _TMP71 = dot(vec3(float(_I5.x), float(_I5.y), float(_I5.z)), vec3( 1.43593750E+01, 2.81718750E+01, 5.47265625E+00));
    _r0170.x = float(_TMP71);
    _TMP71 = dot(vec3(float(_C4.x), float(_C4.y), float(_C4.z)), vec3( 1.43593750E+01, 2.81718750E+01, 5.47265625E+00));
    _r0170.y = float(_TMP71);
    _TMP71 = dot(vec3(float(_A11.x), float(_A11.y), float(_A11.z)), vec3( 1.43593750E+01, 2.81718750E+01, 5.47265625E+00));
    _r0170.z = float(_TMP71);
    _TMP71 = dot(vec3(float(_G0.x), float(_G0.y), float(_G0.z)), vec3( 1.43593750E+01, 2.81718750E+01, 5.47265625E+00));
    _r0170.w = float(_TMP71);
    _i5 = vec4(float(_r0170.x), float(_r0170.y), float(_r0170.z), float(_r0170.w));
    _TMP71 = dot(vec3(float(_H5.x), float(_H5.y), float(_H5.z)), vec3( 1.43593750E+01, 2.81718750E+01, 5.47265625E+00));
    _r0180.x = float(_TMP71);
    _TMP71 = dot(vec3(float(_F4.x), float(_F4.y), float(_F4.z)), vec3( 1.43593750E+01, 2.81718750E+01, 5.47265625E+00));
    _r0180.y = float(_TMP71);
    _TMP71 = dot(vec3(float(_B11.x), float(_B11.y), float(_B11.z)), vec3( 1.43593750E+01, 2.81718750E+01, 5.47265625E+00));
    _r0180.z = float(_TMP71);
    _TMP71 = dot(vec3(float(_D0.x), float(_D0.y), float(_D0.z)), vec3( 1.43593750E+01, 2.81718750E+01, 5.47265625E+00));
    _r0180.w = float(_TMP71);
    _h5 = vec4(float(_r0180.x), float(_r0180.y), float(_r0180.z), float(_r0180.w));
    _fx = vec4( 1.00000000E+00, -1.00000000E+00, -1.00000000E+00, 1.00000000E+00)*_fp.y + vec4( 1.00000000E+00, 1.00000000E+00, -1.00000000E+00, -1.00000000E+00)*_fp.x;
    _fx_left = vec4( 1.00000000E+00, -1.00000000E+00, -1.00000000E+00, 1.00000000E+00)*_fp.y + vec4( 5.00000000E-01, 2.00000000E+00, -5.00000000E-01, -2.00000000E+00)*_fp.x;
    _fx_up = vec4( 1.00000000E+00, -1.00000000E+00, -1.00000000E+00, 1.00000000E+00)*_fp.y + vec4( 2.00000000E+00, 5.00000000E-01, -2.00000000E+00, -5.00000000E-01)*_fp.x;
    _a0194 = _b1.wxyz - _b1;
    _TMP191 = abs(_a0194);
    _TMP27 = bvec4(_TMP191.x < 1.50000000E+01, _TMP191.y < 1.50000000E+01, _TMP191.z < 1.50000000E+01, _TMP191.w < 1.50000000E+01);
    _a0200 = _b1.zwxy - _b1.yzwx;
    _TMP197 = abs(_a0200);
    _TMP28 = bvec4(_TMP197.x < 1.50000000E+01, _TMP197.y < 1.50000000E+01, _TMP197.z < 1.50000000E+01, _TMP197.w < 1.50000000E+01);
    _a0206 = _e1 - _c3.wxyz;
    _TMP203 = abs(_a0206);
    _TMP29 = bvec4(_TMP203.x < 1.50000000E+01, _TMP203.y < 1.50000000E+01, _TMP203.z < 1.50000000E+01, _TMP203.w < 1.50000000E+01);
    _a0212 = _b1.wxyz - _i4;
    _TMP209 = abs(_a0212);
    _TMP30 = bvec4(_TMP209.x < 1.50000000E+01, _TMP209.y < 1.50000000E+01, _TMP209.z < 1.50000000E+01, _TMP209.w < 1.50000000E+01);
    _a0218 = _b1.zwxy - _i5;
    _TMP215 = abs(_a0218);
    _TMP31 = bvec4(_TMP215.x < 1.50000000E+01, _TMP215.y < 1.50000000E+01, _TMP215.z < 1.50000000E+01, _TMP215.w < 1.50000000E+01);
    _a0224 = _e1 - _c3.zwxy;
    _TMP221 = abs(_a0224);
    _TMP32 = bvec4(_TMP221.x < 1.50000000E+01, _TMP221.y < 1.50000000E+01, _TMP221.z < 1.50000000E+01, _TMP221.w < 1.50000000E+01);
    _a0230 = _e1 - _c3;
    _TMP227 = abs(_a0230);
    _TMP33 = bvec4(_TMP227.x < 1.50000000E+01, _TMP227.y < 1.50000000E+01, _TMP227.z < 1.50000000E+01, _TMP227.w < 1.50000000E+01);
    _interp_restriction_lv1 = bvec4(_e1.x != _b1.w && _e1.x != _b1.z && (!_TMP27.x && !_TMP28.x || _TMP29.x && !_TMP30.x && !_TMP31.x || _TMP32.x || _TMP33.x), _e1.y != _b1.x && _e1.y != _b1.w && (!_TMP27.y && !_TMP28.y || _TMP29.y && !_TMP30.y && !_TMP31.y || _TMP32.y || _TMP33.y), _e1.z != _b1.y && _e1.z != _b1.x && (!_TMP27.z && !_TMP28.z || _TMP29.z && !_TMP30.z && !_TMP31.z || _TMP32.z || _TMP33.z), _e1.w != _b1.z && _e1.w != _b1.y && (!_TMP27.w && !_TMP28.w || _TMP29.w && !_TMP30.w && !_TMP31.w || _TMP32.w || _TMP33.w));
    _interp_restriction_lv2_left = bvec4(_e1.x != _c3.z && _b1.y != _c3.z, _e1.y != _c3.w && _b1.z != _c3.w, _e1.z != _c3.x && _b1.w != _c3.x, _e1.w != _c3.y && _b1.x != _c3.y);
    _interp_restriction_lv2_up = bvec4(_e1.x != _c3.x && _b1.x != _c3.x, _e1.y != _c3.y && _b1.y != _c3.y, _e1.z != _c3.z && _b1.z != _c3.z, _e1.w != _c3.w && _b1.w != _c3.w);
    _x0234 = _fx - vec4( 1.00000000E+00, 0.00000000E+00, -1.00000000E+00, 0.00000000E+00);
    _TMP73 = min(vec4( 1.00000000E+00, 1.00000000E+00, 1.00000000E+00, 1.00000000E+00), _x0234);
    _TMP235 = max(vec4( 0.00000000E+00, 0.00000000E+00, 0.00000000E+00, 0.00000000E+00), _TMP73);
    _fx45 = _TMP235*_TMP235*(3.00000000E+00 - 2.00000000E+00*_TMP235);
    _x0244 = _fx_left - vec4( 5.00000000E-01, 5.00000000E-01, -1.00000000E+00, -5.00000000E-01);
    _TMP73 = min(vec4( 1.00000000E+00, 1.00000000E+00, 1.00000000E+00, 1.00000000E+00), _x0244);
    _TMP245 = max(vec4( 0.00000000E+00, 0.00000000E+00, 0.00000000E+00, 0.00000000E+00), _TMP73);
    _fx30 = _TMP245*_TMP245*(3.00000000E+00 - 2.00000000E+00*_TMP245);
    _x0254 = _fx_up - vec4( 1.50000000E+00, -5.00000000E-01, -1.50000000E+00, 0.00000000E+00);
    _TMP73 = min(vec4( 1.00000000E+00, 1.00000000E+00, 1.00000000E+00, 1.00000000E+00), _x0254);
    _TMP255 = max(vec4( 0.00000000E+00, 0.00000000E+00, 0.00000000E+00, 0.00000000E+00), _TMP73);
    _fx60 = _TMP255*_TMP255*(3.00000000E+00 - 2.00000000E+00*_TMP255);
    _a0266 = _e1 - _c3;
    _TMP263 = abs(_a0266);
    _a0270 = _e1 - _c3.zwxy;
    _TMP267 = abs(_a0270);
    _a0274 = _c3.wxyz - _h5;
    _TMP271 = abs(_a0274);
    _a0278 = _c3.wxyz - _h5.yzwx;
    _TMP275 = abs(_a0278);
    _a0282 = _b1.zwxy - _b1.wxyz;
    _TMP279 = abs(_a0282);
    _TMP34 = _TMP263 + _TMP267 + _TMP271 + _TMP275 + 4.00000000E+00*_TMP279;
    _a0288 = _b1.zwxy - _b1.yzwx;
    _TMP285 = abs(_a0288);
    _a0292 = _b1.zwxy - _i5;
    _TMP289 = abs(_a0292);
    _a0296 = _b1.wxyz - _i4;
    _TMP293 = abs(_a0296);
    _a0300 = _b1.wxyz - _b1;
    _TMP297 = abs(_a0300);
    _a0304 = _e1 - _c3.wxyz;
    _TMP301 = abs(_a0304);
    _TMP35 = _TMP285 + _TMP289 + _TMP293 + _TMP297 + 4.00000000E+00*_TMP301;
    _edr = bvec4((_TMP34 + 3.50000000E+00).x < _TMP35.x && _interp_restriction_lv1.x, (_TMP34 + 3.50000000E+00).y < _TMP35.y && _interp_restriction_lv1.y, (_TMP34 + 3.50000000E+00).z < _TMP35.z && _interp_restriction_lv1.z, (_TMP34 + 3.50000000E+00).w < _TMP35.w && _interp_restriction_lv1.w);
    _a0308 = _b1.wxyz - _c3.zwxy;
    _TMP305 = abs(_a0308);
    _a0312 = _b1.zwxy - _c3;
    _TMP309 = abs(_a0312);
    _edr_left = bvec4((2.00000000E+00*_TMP305).x <= _TMP309.x && _interp_restriction_lv2_left.x, (2.00000000E+00*_TMP305).y <= _TMP309.y && _interp_restriction_lv2_left.y, (2.00000000E+00*_TMP305).z <= _TMP309.z && _interp_restriction_lv2_left.z, (2.00000000E+00*_TMP305).w <= _TMP309.w && _interp_restriction_lv2_left.w);
    _a0316 = _b1.wxyz - _c3.zwxy;
    _TMP313 = abs(_a0316);
    _a0320 = _b1.zwxy - _c3;
    _TMP317 = abs(_a0320);
    _edr_up = bvec4(_TMP313.x >= (2.00000000E+00*_TMP317).x && _interp_restriction_lv2_up.x, _TMP313.y >= (2.00000000E+00*_TMP317).y && _interp_restriction_lv2_up.y, _TMP313.z >= (2.00000000E+00*_TMP317).z && _interp_restriction_lv2_up.z, _TMP313.w >= (2.00000000E+00*_TMP317).w && _interp_restriction_lv2_up.w);
    _nc45 = bvec4(_edr.x && bool(_fx45.x), _edr.y && bool(_fx45.y), _edr.z && bool(_fx45.z), _edr.w && bool(_fx45.w));
    _nc30 = bvec4(_edr.x && _edr_left.x && bool(_fx30.x), _edr.y && _edr_left.y && bool(_fx30.y), _edr.z && _edr_left.z && bool(_fx30.z), _edr.w && _edr_left.w && bool(_fx30.w));
    _nc60 = bvec4(_edr.x && _edr_up.x && bool(_fx60.x), _edr.y && _edr_up.y && bool(_fx60.y), _edr.z && _edr_up.z && bool(_fx60.z), _edr.w && _edr_up.w && bool(_fx60.w));
    _a0324 = _e1 - _b1.wxyz;
    _TMP321 = abs(_a0324);
    _a0328 = _e1 - _b1.zwxy;
    _TMP325 = abs(_a0328);
    _px = bvec4(_TMP321.x <= _TMP325.x, _TMP321.y <= _TMP325.y, _TMP321.z <= _TMP325.z, _TMP321.w <= _TMP325.w);
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
    _TMP42 = abs(_bb);
    _b0332 = 6.49999976E-01*_TMP42;
    _m = min(_m, _b0332);
    _TMP43 = abs(_cc);
    _b0336 = 6.49999976E-01*_TMP43;
    _m = min(_m, _b0336);
    _a0338 = -_m;
    _TMP70 = min(_m, _t);
    _TMP337 = max(_a0338, _TMP70);
    _s1 = (2.00000000E+00*_fp.y - 1.00000000E+00)*_TMP337 + _s;
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
    _TMP44 = abs(_bb);
    _b0346 = 6.49999976E-01*_TMP44;
    _m = min(_m, _b0346);
    _TMP45 = abs(_cc);
    _b0350 = 6.49999976E-01*_TMP45;
    _m = min(_m, _b0350);
    _a0352 = -_m;
    _TMP70 = min(_m, _t);
    _TMP351 = max(_a0352, _TMP70);
    _s0 = (2.00000000E+00*_fp.x - 1.00000000E+00)*_TMP351 + _s1;
    _nc = bvec4(_nc30.x || _nc60.x || _nc45.x, _nc30.y || _nc60.y || _nc45.y, _nc30.z || _nc60.z || _nc45.z, _nc30.w || _nc60.w || _nc45.w);
    _blend2 = 0.00000000E+00;
    _blend1 = 0.00000000E+00;
    _a0358 = vec4(float(_nc45.x), float(_nc45.y), float(_nc45.z), float(_nc45.w));
    _TMP47 = dot(_a0358, _fx45);
    _a0360 = vec4(float(_nc30.x), float(_nc30.y), float(_nc30.z), float(_nc30.w));
    _TMP48 = dot(_a0360, _fx30);
    _a0362 = vec4(float(_nc60.x), float(_nc60.y), float(_nc60.z), float(_nc60.w));
    _TMP49 = dot(_a0362, _fx60);
    _TMP50 = max(vec4(_TMP48, _TMP48, _TMP48, _TMP48), vec4(_TMP49, _TMP49, _TMP49, _TMP49));
    _maximo = max(_TMP50, vec4(_TMP47, _TMP47, _TMP47, _TMP47));
    if (_nc.x) { 
        if (_px.x) { 
            _TMP51 = _F;
        } else {
            _TMP51 = _H;
        } 
        _pix1 = _TMP51;
        _blend1 = _maximo.x;
    } else {
        if (_nc.y) { 
            if (_px.y) { 
                _TMP52 = _B2;
            } else {
                _TMP52 = _F;
            } 
            _pix1 = _TMP52;
            _blend1 = _maximo.y;
        } else {
            if (_nc.z) { 
                if (_px.z) { 
                    _TMP53 = _D;
                } else {
                    _TMP53 = _B2;
                } 
                _pix1 = _TMP53;
                _blend1 = _maximo.z;
            } else {
                if (_nc.w) { 
                    if (_px.w) { 
                        _TMP54 = _H;
                    } else {
                        _TMP54 = _D;
                    } 
                    _pix1 = _TMP54;
                    _blend1 = _maximo.w;
                } 
            } 
        } 
    } 
    if (_nc.w) { 
        if (_px.w) { 
            _TMP55 = _H;
        } else {
            _TMP55 = _D;
        } 
        _pix2 = _TMP55;
        _blend2 = _maximo.w;
    } else {
        if (_nc.z) { 
            if (_px.z) { 
                _TMP56 = _D;
            } else {
                _TMP56 = _B2;
            } 
            _pix2 = _TMP56;
            _blend2 = _maximo.z;
        } else {
            if (_nc.y) { 
                if (_px.y) { 
                    _TMP57 = _B2;
                } else {
                    _TMP57 = _F;
                } 
                _pix2 = _TMP57;
                _blend2 = _maximo.y;
            } else {
                if (_nc.x) { 
                    if (_px.x) { 
                        _TMP58 = _F;
                    } else {
                        _TMP58 = _H;
                    } 
                    _pix2 = _TMP58;
                    _blend2 = _maximo.x;
                } 
            } 
        } 
    } 
    _b0368 = vec3(float(_pix1.x), float(_pix1.y), float(_pix1.z));
    _TMP59 = _s0 + _blend1*(_b0368 - _s0);
    _res1 = vec3(float(_TMP59.x), float(_TMP59.y), float(_TMP59.z));
    _b0370 = vec3(float(_pix2.x), float(_pix2.y), float(_pix2.z));
    _TMP60 = _s0 + _blend2*(_b0370 - _s0);
    _res2 = vec3(float(_TMP60.x), float(_TMP60.y), float(_TMP60.z));
    _a0374 = _E - _res1;
    _TMP72 = abs(vec3(float(_a0374.x), float(_a0374.y), float(_a0374.z)));
    _df0372 = vec3(float(_TMP72.x), float(_TMP72.y), float(_TMP72.z));
    _TMP61 = _df0372.x + _df0372.y + _df0372.z;
    _a0378 = _E - _res2;
    _TMP72 = abs(vec3(float(_a0378.x), float(_a0378.y), float(_a0378.z)));
    _df0376 = vec3(float(_TMP72.x), float(_TMP72.y), float(_TMP72.z));
    _TMP62 = _df0376.x + _df0376.y + _df0376.z;
    _TMP63 = float((_TMP62 >= _TMP61));
    _res = _res1 + _TMP63*(_res2 - _res1);
    _ret_0 = vec4(_res.x, _res.y, _res.z, 1.00000000E+00);
    FragColor = vec4(float(_ret_0.x), float(_ret_0.y), float(_ret_0.z), float(_ret_0.w));
    return;
} 
#endif
