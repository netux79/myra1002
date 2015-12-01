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
vec4 _r0011;
vec4 _v0011;
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
    _v0011 = vec4(float(VertexCoord.x), float(VertexCoord.y), float(VertexCoord.z), float(VertexCoord.w));
    _r0011 = _v0011.x*MVPMatrix[0];
    _r0011 = _r0011 + _v0011.y*MVPMatrix[1];
    _r0011 = _r0011 + _v0011.z*MVPMatrix[2];
    _r0011 = _r0011 + _v0011.w*MVPMatrix[3];
    _OUT._position1 = vec4(float(_r0011.x), float(_r0011.y), float(_r0011.z), float(_r0011.w));
    _ps = vec2(1.00000000E+00/TextureSize.x, 1.00000000E+00/TextureSize.y);
    _OUT.VARt1 = TexCoord.xxxy + vec4(float(float(-_ps.x)), 0.00000000E+00, float(float(_ps.x)), float(float((-2.00000000E+00*_ps.y))));
    _OUT.VARt2 = TexCoord.xxxy + vec4(float(float(-_ps.x)), 0.00000000E+00, float(float(_ps.x)), float(float(-_ps.y)));
    _OUT.VARt3 = TexCoord.xxxy + vec4(float(float(-_ps.x)), 0.00000000E+00, float(float(_ps.x)), 0.00000000E+00);
    _OUT.VARt4 = TexCoord.xxxy + vec4(float(float(-_ps.x)), 0.00000000E+00, float(float(_ps.x)), float(float(_ps.y)));
    _OUT.VARt5 = TexCoord.xxxy + vec4(float(float(-_ps.x)), 0.00000000E+00, float(float(_ps.x)), float(float((2.00000000E+00*_ps.y))));
    _OUT.VARt6 = TexCoord.xyyy + vec4(float(float((-2.00000000E+00*_ps.x))), float(float(-_ps.y)), 0.00000000E+00, float(float(_ps.y)));
    _OUT.VARt7 = TexCoord.xyyy + vec4(float(float((2.00000000E+00*_ps.x))), float(float(-_ps.y)), 0.00000000E+00, float(float(_ps.y)));
    _ret_0._position1 = _OUT._position1;
    _ret_0._color1 = COLOR;
    _ret_0._texCoord1 = TexCoord.xy;
    VARt1 = _OUT.VARt1;
    VARt2 = _OUT.VARt2;
    VARt3 = _OUT.VARt3;
    VARt4 = _OUT.VARt4;
    VARt5 = _OUT.VARt5;
    VARt6 = _OUT.VARt6;
    VARt7 = _OUT.VARt7;
    gl_Position = _OUT._position1;
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
float _TMP60;
vec3 _TMP52;
vec3 _TMP50;
vec3 _TMP48;
vec3 _TMP46;
vec3 _TMP51;
vec3 _TMP49;
vec3 _TMP47;
vec3 _TMP45;
vec4 _TMP44;
vec4 _TMP37;
vec4 _TMP36;
vec4 _TMP61;
bvec4 _TMP35;
bvec4 _TMP34;
bvec4 _TMP33;
bvec4 _TMP32;
bvec4 _TMP31;
bvec4 _TMP30;
bvec4 _TMP29;
bvec4 _TMP28;
bvec4 _TMP27;
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
vec2 _x0076;
vec4 _r0120;
vec4 _r0130;
vec4 _r0140;
vec4 _r0150;
vec4 _r0160;
vec4 _r0170;
vec4 _TMP181;
vec4 _a0184;
vec4 _TMP187;
vec4 _a0190;
vec4 _TMP193;
vec4 _a0196;
vec4 _TMP199;
vec4 _a0202;
vec4 _TMP205;
vec4 _a0208;
vec4 _TMP211;
vec4 _a0214;
vec4 _TMP217;
vec4 _a0220;
vec4 _TMP223;
vec4 _a0226;
vec4 _TMP229;
vec4 _a0232;
vec4 _x0234;
vec4 _TMP235;
vec4 _x0242;
vec4 _TMP243;
vec4 _x0250;
vec4 _TMP251;
vec4 _TMP259;
vec4 _a0262;
vec4 _TMP263;
vec4 _a0266;
vec4 _TMP267;
vec4 _a0270;
vec4 _TMP271;
vec4 _a0274;
vec4 _TMP275;
vec4 _a0278;
vec4 _TMP281;
vec4 _a0284;
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
float _t0330;
float _t0332;
float _t0334;
float _t0336;
float _t0338;
float _t0340;
float _t0342;
float _t0344;
vec4 _r0346;
vec4 _TMP355;
vec4 _a0358;
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
    vec4 _fx;
    vec4 _fx_left;
    vec4 _fx_up;
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
    vec4 _c1;
    vec4 _e1;
    vec4 _i4;
    vec4 _i5;
    vec4 _h5;
    vec4 _fx45;
    vec4 _fx30;
    vec4 _fx60;
    vec4 _maximo;
    vec4 _pixel;
    vec3 _res;
    float _mx;
    _x0076 = TEX0.xy*TextureSize;
    _fp = fract(_x0076);
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
    _TMP60 = dot(vec3(float(_B2.x), float(_B2.y), float(_B2.z)), vec3( 1.43593750E+01, 2.81718750E+01, 5.47265625E+00));
    _r0120.x = float(_TMP60);
    _TMP60 = dot(vec3(float(_D.x), float(_D.y), float(_D.z)), vec3( 1.43593750E+01, 2.81718750E+01, 5.47265625E+00));
    _r0120.y = float(_TMP60);
    _TMP60 = dot(vec3(float(_H.x), float(_H.y), float(_H.z)), vec3( 1.43593750E+01, 2.81718750E+01, 5.47265625E+00));
    _r0120.z = float(_TMP60);
    _TMP60 = dot(vec3(float(_F.x), float(_F.y), float(_F.z)), vec3( 1.43593750E+01, 2.81718750E+01, 5.47265625E+00));
    _r0120.w = float(_TMP60);
    _b1 = vec4(float(_r0120.x), float(_r0120.y), float(_r0120.z), float(_r0120.w));
    _TMP60 = dot(vec3(float(_C.x), float(_C.y), float(_C.z)), vec3( 1.43593750E+01, 2.81718750E+01, 5.47265625E+00));
    _r0130.x = float(_TMP60);
    _TMP60 = dot(vec3(float(_A2.x), float(_A2.y), float(_A2.z)), vec3( 1.43593750E+01, 2.81718750E+01, 5.47265625E+00));
    _r0130.y = float(_TMP60);
    _TMP60 = dot(vec3(float(_G.x), float(_G.y), float(_G.z)), vec3( 1.43593750E+01, 2.81718750E+01, 5.47265625E+00));
    _r0130.z = float(_TMP60);
    _TMP60 = dot(vec3(float(_I.x), float(_I.y), float(_I.z)), vec3( 1.43593750E+01, 2.81718750E+01, 5.47265625E+00));
    _r0130.w = float(_TMP60);
    _c1 = vec4(float(_r0130.x), float(_r0130.y), float(_r0130.z), float(_r0130.w));
    _TMP60 = dot(vec3(float(_E.x), float(_E.y), float(_E.z)), vec3( 1.43593750E+01, 2.81718750E+01, 5.47265625E+00));
    _r0140.x = float(_TMP60);
    _TMP60 = dot(vec3(float(_E.x), float(_E.y), float(_E.z)), vec3( 1.43593750E+01, 2.81718750E+01, 5.47265625E+00));
    _r0140.y = float(_TMP60);
    _TMP60 = dot(vec3(float(_E.x), float(_E.y), float(_E.z)), vec3( 1.43593750E+01, 2.81718750E+01, 5.47265625E+00));
    _r0140.z = float(_TMP60);
    _TMP60 = dot(vec3(float(_E.x), float(_E.y), float(_E.z)), vec3( 1.43593750E+01, 2.81718750E+01, 5.47265625E+00));
    _r0140.w = float(_TMP60);
    _e1 = vec4(float(_r0140.x), float(_r0140.y), float(_r0140.z), float(_r0140.w));
    _TMP60 = dot(vec3(float(_I4.x), float(_I4.y), float(_I4.z)), vec3( 1.43593750E+01, 2.81718750E+01, 5.47265625E+00));
    _r0150.x = float(_TMP60);
    _TMP60 = dot(vec3(float(_C1.x), float(_C1.y), float(_C1.z)), vec3( 1.43593750E+01, 2.81718750E+01, 5.47265625E+00));
    _r0150.y = float(_TMP60);
    _TMP60 = dot(vec3(float(_A0.x), float(_A0.y), float(_A0.z)), vec3( 1.43593750E+01, 2.81718750E+01, 5.47265625E+00));
    _r0150.z = float(_TMP60);
    _TMP60 = dot(vec3(float(_G5.x), float(_G5.y), float(_G5.z)), vec3( 1.43593750E+01, 2.81718750E+01, 5.47265625E+00));
    _r0150.w = float(_TMP60);
    _i4 = vec4(float(_r0150.x), float(_r0150.y), float(_r0150.z), float(_r0150.w));
    _TMP60 = dot(vec3(float(_I5.x), float(_I5.y), float(_I5.z)), vec3( 1.43593750E+01, 2.81718750E+01, 5.47265625E+00));
    _r0160.x = float(_TMP60);
    _TMP60 = dot(vec3(float(_C4.x), float(_C4.y), float(_C4.z)), vec3( 1.43593750E+01, 2.81718750E+01, 5.47265625E+00));
    _r0160.y = float(_TMP60);
    _TMP60 = dot(vec3(float(_A11.x), float(_A11.y), float(_A11.z)), vec3( 1.43593750E+01, 2.81718750E+01, 5.47265625E+00));
    _r0160.z = float(_TMP60);
    _TMP60 = dot(vec3(float(_G0.x), float(_G0.y), float(_G0.z)), vec3( 1.43593750E+01, 2.81718750E+01, 5.47265625E+00));
    _r0160.w = float(_TMP60);
    _i5 = vec4(float(_r0160.x), float(_r0160.y), float(_r0160.z), float(_r0160.w));
    _TMP60 = dot(vec3(float(_H5.x), float(_H5.y), float(_H5.z)), vec3( 1.43593750E+01, 2.81718750E+01, 5.47265625E+00));
    _r0170.x = float(_TMP60);
    _TMP60 = dot(vec3(float(_F4.x), float(_F4.y), float(_F4.z)), vec3( 1.43593750E+01, 2.81718750E+01, 5.47265625E+00));
    _r0170.y = float(_TMP60);
    _TMP60 = dot(vec3(float(_B11.x), float(_B11.y), float(_B11.z)), vec3( 1.43593750E+01, 2.81718750E+01, 5.47265625E+00));
    _r0170.z = float(_TMP60);
    _TMP60 = dot(vec3(float(_D0.x), float(_D0.y), float(_D0.z)), vec3( 1.43593750E+01, 2.81718750E+01, 5.47265625E+00));
    _r0170.w = float(_TMP60);
    _h5 = vec4(float(_r0170.x), float(_r0170.y), float(_r0170.z), float(_r0170.w));
    _fx = vec4( 1.00000000E+00, -1.00000000E+00, -1.00000000E+00, 1.00000000E+00)*_fp.y + vec4( 1.00000000E+00, 1.00000000E+00, -1.00000000E+00, -1.00000000E+00)*_fp.x;
    _fx_left = vec4( 1.00000000E+00, -1.00000000E+00, -1.00000000E+00, 1.00000000E+00)*_fp.y + vec4( 5.00000000E-01, 2.00000000E+00, -5.00000000E-01, -2.00000000E+00)*_fp.x;
    _fx_up = vec4( 1.00000000E+00, -1.00000000E+00, -1.00000000E+00, 1.00000000E+00)*_fp.y + vec4( 2.00000000E+00, 5.00000000E-01, -2.00000000E+00, -5.00000000E-01)*_fp.x;
    _a0184 = _b1.wxyz - _b1;
    _TMP181 = abs(_a0184);
    _TMP27 = bvec4(_TMP181.x < 1.50000000E+01, _TMP181.y < 1.50000000E+01, _TMP181.z < 1.50000000E+01, _TMP181.w < 1.50000000E+01);
    _a0190 = _b1.zwxy - _b1.yzwx;
    _TMP187 = abs(_a0190);
    _TMP28 = bvec4(_TMP187.x < 1.50000000E+01, _TMP187.y < 1.50000000E+01, _TMP187.z < 1.50000000E+01, _TMP187.w < 1.50000000E+01);
    _a0196 = _e1 - _c1.wxyz;
    _TMP193 = abs(_a0196);
    _TMP29 = bvec4(_TMP193.x < 1.50000000E+01, _TMP193.y < 1.50000000E+01, _TMP193.z < 1.50000000E+01, _TMP193.w < 1.50000000E+01);
    _a0202 = _b1.wxyz - _i4;
    _TMP199 = abs(_a0202);
    _TMP30 = bvec4(_TMP199.x < 1.50000000E+01, _TMP199.y < 1.50000000E+01, _TMP199.z < 1.50000000E+01, _TMP199.w < 1.50000000E+01);
    _a0208 = _b1.zwxy - _i5;
    _TMP205 = abs(_a0208);
    _TMP31 = bvec4(_TMP205.x < 1.50000000E+01, _TMP205.y < 1.50000000E+01, _TMP205.z < 1.50000000E+01, _TMP205.w < 1.50000000E+01);
    _a0214 = _e1 - _c1.zwxy;
    _TMP211 = abs(_a0214);
    _TMP32 = bvec4(_TMP211.x < 1.50000000E+01, _TMP211.y < 1.50000000E+01, _TMP211.z < 1.50000000E+01, _TMP211.w < 1.50000000E+01);
    _a0220 = _e1 - _c1;
    _TMP217 = abs(_a0220);
    _TMP33 = bvec4(_TMP217.x < 1.50000000E+01, _TMP217.y < 1.50000000E+01, _TMP217.z < 1.50000000E+01, _TMP217.w < 1.50000000E+01);
    _a0226 = _b1 - _i4.yzwx;
    _TMP223 = abs(_a0226);
    _TMP34 = bvec4(_TMP223.x < 1.50000000E+01, _TMP223.y < 1.50000000E+01, _TMP223.z < 1.50000000E+01, _TMP223.w < 1.50000000E+01);
    _a0232 = _b1.yzwx - _i5.wxyz;
    _TMP229 = abs(_a0232);
    _TMP35 = bvec4(_TMP229.x < 1.50000000E+01, _TMP229.y < 1.50000000E+01, _TMP229.z < 1.50000000E+01, _TMP229.w < 1.50000000E+01);
    _interp_restriction_lv1 = bvec4(_e1.x != _b1.w && _e1.x != _b1.z && (!_TMP27.x && !_TMP28.x || _TMP29.x && !_TMP30.x && !_TMP31.x || _TMP32.x || _TMP33.x) && (_b1.w != _h5.y && _b1.w != _c1.w || _b1.z != _h5.x && _b1.z != _c1.w || _b1.z != _c1.z || _b1.w != _c1.x || _TMP34.x && _TMP35.x), _e1.y != _b1.x && _e1.y != _b1.w && (!_TMP27.y && !_TMP28.y || _TMP29.y && !_TMP30.y && !_TMP31.y || _TMP32.y || _TMP33.y) && (_b1.x != _h5.z && _b1.x != _c1.x || _b1.w != _h5.y && _b1.w != _c1.x || _b1.w != _c1.w || _b1.x != _c1.y || _TMP34.y && _TMP35.y), _e1.z != _b1.y && _e1.z != _b1.x && (!_TMP27.z && !_TMP28.z || _TMP29.z && !_TMP30.z && !_TMP31.z || _TMP32.z || _TMP33.z) && (_b1.y != _h5.w && _b1.y != _c1.y || _b1.x != _h5.z && _b1.x != _c1.y || _b1.x != _c1.x || _b1.y != _c1.z || _TMP34.z && _TMP35.z), _e1.w != _b1.z && _e1.w != _b1.y && (!_TMP27.w && !_TMP28.w || _TMP29.w && !_TMP30.w && !_TMP31.w || _TMP32.w || _TMP33.w) && (_b1.z != _h5.x && _b1.z != _c1.z || _b1.y != _h5.w && _b1.y != _c1.z || _b1.y != _c1.y || _b1.z != _c1.w || _TMP34.w && _TMP35.w));
    _interp_restriction_lv2_left = bvec4(_e1.x != _c1.z && _b1.y != _c1.z, _e1.y != _c1.w && _b1.z != _c1.w, _e1.z != _c1.x && _b1.w != _c1.x, _e1.w != _c1.y && _b1.x != _c1.y);
    _interp_restriction_lv2_up = bvec4(_e1.x != _c1.x && _b1.x != _c1.x, _e1.y != _c1.y && _b1.y != _c1.y, _e1.z != _c1.z && _b1.z != _c1.z, _e1.w != _c1.w && _b1.w != _c1.w);
    _x0234 = ((_fx + vec4( 2.00000003E-01, 2.00000003E-01, 2.00000003E-01, 2.00000003E-01)) - vec4( 1.50000000E+00, 5.00000000E-01, -5.00000000E-01, 5.00000000E-01))/vec4( 4.00000006E-01, 4.00000006E-01, 4.00000006E-01, 4.00000006E-01);
    _TMP61 = min(vec4( 1.00000000E+00, 1.00000000E+00, 1.00000000E+00, 1.00000000E+00), _x0234);
    _TMP235 = max(vec4( 0.00000000E+00, 0.00000000E+00, 0.00000000E+00, 0.00000000E+00), _TMP61);
    _x0242 = ((_fx_left + vec4( 1.00000001E-01, 2.00000003E-01, 1.00000001E-01, 2.00000003E-01)) - vec4( 1.00000000E+00, 1.00000000E+00, -5.00000000E-01, 0.00000000E+00))/vec4( 2.00000003E-01, 4.00000006E-01, 2.00000003E-01, 4.00000006E-01);
    _TMP61 = min(vec4( 1.00000000E+00, 1.00000000E+00, 1.00000000E+00, 1.00000000E+00), _x0242);
    _TMP243 = max(vec4( 0.00000000E+00, 0.00000000E+00, 0.00000000E+00, 0.00000000E+00), _TMP61);
    _x0250 = ((_fx_up + vec4( 2.00000003E-01, 1.00000001E-01, 2.00000003E-01, 1.00000001E-01)) - vec4( 2.00000000E+00, 0.00000000E+00, -1.00000000E+00, 5.00000000E-01))/vec4( 4.00000006E-01, 2.00000003E-01, 4.00000006E-01, 2.00000003E-01);
    _TMP61 = min(vec4( 1.00000000E+00, 1.00000000E+00, 1.00000000E+00, 1.00000000E+00), _x0250);
    _TMP251 = max(vec4( 0.00000000E+00, 0.00000000E+00, 0.00000000E+00, 0.00000000E+00), _TMP61);
    _a0262 = _e1 - _c1;
    _TMP259 = abs(_a0262);
    _a0266 = _e1 - _c1.zwxy;
    _TMP263 = abs(_a0266);
    _a0270 = _c1.wxyz - _h5;
    _TMP267 = abs(_a0270);
    _a0274 = _c1.wxyz - _h5.yzwx;
    _TMP271 = abs(_a0274);
    _a0278 = _b1.zwxy - _b1.wxyz;
    _TMP275 = abs(_a0278);
    _TMP36 = _TMP259 + _TMP263 + _TMP267 + _TMP271 + 4.00000000E+00*_TMP275;
    _a0284 = _b1.zwxy - _b1.yzwx;
    _TMP281 = abs(_a0284);
    _a0288 = _b1.zwxy - _i5;
    _TMP285 = abs(_a0288);
    _a0292 = _b1.wxyz - _i4;
    _TMP289 = abs(_a0292);
    _a0296 = _b1.wxyz - _b1;
    _TMP293 = abs(_a0296);
    _a0300 = _e1 - _c1.wxyz;
    _TMP297 = abs(_a0300);
    _TMP37 = _TMP281 + _TMP285 + _TMP289 + _TMP293 + 4.00000000E+00*_TMP297;
    _edr = bvec4(_TMP36.x < _TMP37.x && _interp_restriction_lv1.x, _TMP36.y < _TMP37.y && _interp_restriction_lv1.y, _TMP36.z < _TMP37.z && _interp_restriction_lv1.z, _TMP36.w < _TMP37.w && _interp_restriction_lv1.w);
    _a0304 = _b1.wxyz - _c1.zwxy;
    _TMP301 = abs(_a0304);
    _a0308 = _b1.zwxy - _c1;
    _TMP305 = abs(_a0308);
    _edr_left = bvec4((2.00000000E+00*_TMP301).x <= _TMP305.x && _interp_restriction_lv2_left.x && _edr.x, (2.00000000E+00*_TMP301).y <= _TMP305.y && _interp_restriction_lv2_left.y && _edr.y, (2.00000000E+00*_TMP301).z <= _TMP305.z && _interp_restriction_lv2_left.z && _edr.z, (2.00000000E+00*_TMP301).w <= _TMP305.w && _interp_restriction_lv2_left.w && _edr.w);
    _a0312 = _b1.wxyz - _c1.zwxy;
    _TMP309 = abs(_a0312);
    _a0316 = _b1.zwxy - _c1;
    _TMP313 = abs(_a0316);
    _edr_up = bvec4(_TMP309.x >= (2.00000000E+00*_TMP313).x && _interp_restriction_lv2_up.x && _edr.x, _TMP309.y >= (2.00000000E+00*_TMP313).y && _interp_restriction_lv2_up.y && _edr.y, _TMP309.z >= (2.00000000E+00*_TMP313).z && _interp_restriction_lv2_up.z && _edr.z, _TMP309.w >= (2.00000000E+00*_TMP313).w && _interp_restriction_lv2_up.w && _edr.w);
    _fx45 = vec4(float(_edr.x), float(_edr.y), float(_edr.z), float(_edr.w))*_TMP235;
    _fx30 = vec4(float(_edr_left.x), float(_edr_left.y), float(_edr_left.z), float(_edr_left.w))*_TMP243;
    _fx60 = vec4(float(_edr_up.x), float(_edr_up.y), float(_edr_up.z), float(_edr_up.w))*_TMP251;
    _a0320 = _e1 - _b1.wxyz;
    _TMP317 = abs(_a0320);
    _a0324 = _e1 - _b1.zwxy;
    _TMP321 = abs(_a0324);
    _px = bvec4(_TMP317.x <= _TMP321.x, _TMP317.y <= _TMP321.y, _TMP317.z <= _TMP321.z, _TMP317.w <= _TMP321.w);
    _TMP44 = max(_fx30, _fx60);
    _maximo = max(_TMP44, _fx45);
    _t0330 = float(_px.x);
    _TMP45 = _H + _t0330*(_F - _H);
    _t0332 = float(_maximo.x);
    _TMP46 = _E + _t0332*(_TMP45 - _E);
    _t0334 = float(_px.y);
    _TMP47 = _F + _t0334*(_B2 - _F);
    _t0336 = float(_maximo.y);
    _TMP48 = _E + _t0336*(_TMP47 - _E);
    _t0338 = float(_px.z);
    _TMP49 = _B2 + _t0338*(_D - _B2);
    _t0340 = float(_maximo.z);
    _TMP50 = _E + _t0340*(_TMP49 - _E);
    _t0342 = float(_px.w);
    _TMP51 = _D + _t0342*(_H - _D);
    _t0344 = float(_maximo.w);
    _TMP52 = _E + _t0344*(_TMP51 - _E);
    _TMP60 = dot(vec3(float(_TMP46.x), float(_TMP46.y), float(_TMP46.z)), vec3( 1.43593750E+01, 2.81718750E+01, 5.47265625E+00));
    _r0346.x = float(_TMP60);
    _TMP60 = dot(vec3(float(_TMP48.x), float(_TMP48.y), float(_TMP48.z)), vec3( 1.43593750E+01, 2.81718750E+01, 5.47265625E+00));
    _r0346.y = float(_TMP60);
    _TMP60 = dot(vec3(float(_TMP50.x), float(_TMP50.y), float(_TMP50.z)), vec3( 1.43593750E+01, 2.81718750E+01, 5.47265625E+00));
    _r0346.z = float(_TMP60);
    _TMP60 = dot(vec3(float(_TMP52.x), float(_TMP52.y), float(_TMP52.z)), vec3( 1.43593750E+01, 2.81718750E+01, 5.47265625E+00));
    _r0346.w = float(_TMP60);
    _pixel = vec4(float(_r0346.x), float(_r0346.y), float(_r0346.z), float(_r0346.w));
    _a0358 = _pixel - _e1;
    _TMP355 = abs(_a0358);
    _res = _TMP46;
    _mx = _TMP355.x;
    if (_TMP355.y > _TMP355.x) { 
        _res = _TMP48;
        _mx = _TMP355.y;
    } 
    if (_TMP355.z > _mx) { 
        _res = _TMP50;
        _mx = _TMP355.z;
    } 
    if (_TMP355.w > _mx) { 
        _res = _TMP52;
    } 
    _ret_0 = vec4(float(_res.x), float(_res.y), float(_res.z), 1.00000000E+00);
    FragColor = _ret_0;
    return;
} 
#endif
