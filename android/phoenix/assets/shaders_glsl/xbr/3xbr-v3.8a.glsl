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
float _TMP50;
vec3 _TMP43;
vec3 _TMP41;
vec3 _TMP39;
vec3 _TMP37;
vec3 _TMP42;
vec3 _TMP40;
vec3 _TMP38;
vec3 _TMP36;
vec4 _TMP35;
vec4 _TMP28;
vec4 _TMP27;
vec4 _TMP51;
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
vec2 _x0066;
vec4 _r0110;
vec4 _r0120;
vec4 _r0130;
vec4 _r0140;
vec4 _r0150;
vec4 _r0160;
vec4 _x0170;
vec4 _TMP171;
vec4 _x0178;
vec4 _TMP179;
vec4 _x0186;
vec4 _TMP187;
vec4 _TMP195;
vec4 _a0198;
vec4 _TMP199;
vec4 _a0202;
vec4 _TMP203;
vec4 _a0206;
vec4 _TMP207;
vec4 _a0210;
vec4 _TMP211;
vec4 _a0214;
vec4 _TMP217;
vec4 _a0220;
vec4 _TMP221;
vec4 _a0224;
vec4 _TMP225;
vec4 _a0228;
vec4 _TMP229;
vec4 _a0232;
vec4 _TMP233;
vec4 _a0236;
vec4 _TMP237;
vec4 _a0240;
vec4 _TMP241;
vec4 _a0244;
vec4 _TMP245;
vec4 _a0248;
vec4 _TMP249;
vec4 _a0252;
vec4 _TMP253;
vec4 _a0256;
vec4 _TMP257;
vec4 _a0260;
float _t0266;
float _t0268;
float _t0270;
float _t0272;
float _t0274;
float _t0276;
float _t0278;
float _t0280;
vec4 _r0282;
vec4 _TMP291;
vec4 _a0294;
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
    vec3 _A1;
    vec3 _B1;
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
    _x0066 = TEX0.xy*TextureSize;
    _fp = fract(_x0066);
    _TMP0 = COMPAT_TEXTURE(Texture, VARt1.xw);
    _A1 = vec3(float(_TMP0.x), float(_TMP0.y), float(_TMP0.z));
    _TMP1 = COMPAT_TEXTURE(Texture, VARt1.yw);
    _B1 = vec3(float(_TMP1.x), float(_TMP1.y), float(_TMP1.z));
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
    _TMP50 = dot(vec3(float(_B2.x), float(_B2.y), float(_B2.z)), vec3( 1.43593750E+01, 2.81718750E+01, 5.47265625E+00));
    _r0110.x = float(_TMP50);
    _TMP50 = dot(vec3(float(_D.x), float(_D.y), float(_D.z)), vec3( 1.43593750E+01, 2.81718750E+01, 5.47265625E+00));
    _r0110.y = float(_TMP50);
    _TMP50 = dot(vec3(float(_H.x), float(_H.y), float(_H.z)), vec3( 1.43593750E+01, 2.81718750E+01, 5.47265625E+00));
    _r0110.z = float(_TMP50);
    _TMP50 = dot(vec3(float(_F.x), float(_F.y), float(_F.z)), vec3( 1.43593750E+01, 2.81718750E+01, 5.47265625E+00));
    _r0110.w = float(_TMP50);
    _b1 = vec4(float(_r0110.x), float(_r0110.y), float(_r0110.z), float(_r0110.w));
    _TMP50 = dot(vec3(float(_C.x), float(_C.y), float(_C.z)), vec3( 1.43593750E+01, 2.81718750E+01, 5.47265625E+00));
    _r0120.x = float(_TMP50);
    _TMP50 = dot(vec3(float(_A2.x), float(_A2.y), float(_A2.z)), vec3( 1.43593750E+01, 2.81718750E+01, 5.47265625E+00));
    _r0120.y = float(_TMP50);
    _TMP50 = dot(vec3(float(_G.x), float(_G.y), float(_G.z)), vec3( 1.43593750E+01, 2.81718750E+01, 5.47265625E+00));
    _r0120.z = float(_TMP50);
    _TMP50 = dot(vec3(float(_I.x), float(_I.y), float(_I.z)), vec3( 1.43593750E+01, 2.81718750E+01, 5.47265625E+00));
    _r0120.w = float(_TMP50);
    _c1 = vec4(float(_r0120.x), float(_r0120.y), float(_r0120.z), float(_r0120.w));
    _TMP50 = dot(vec3(float(_E.x), float(_E.y), float(_E.z)), vec3( 1.43593750E+01, 2.81718750E+01, 5.47265625E+00));
    _r0130.x = float(_TMP50);
    _TMP50 = dot(vec3(float(_E.x), float(_E.y), float(_E.z)), vec3( 1.43593750E+01, 2.81718750E+01, 5.47265625E+00));
    _r0130.y = float(_TMP50);
    _TMP50 = dot(vec3(float(_E.x), float(_E.y), float(_E.z)), vec3( 1.43593750E+01, 2.81718750E+01, 5.47265625E+00));
    _r0130.z = float(_TMP50);
    _TMP50 = dot(vec3(float(_E.x), float(_E.y), float(_E.z)), vec3( 1.43593750E+01, 2.81718750E+01, 5.47265625E+00));
    _r0130.w = float(_TMP50);
    _e1 = vec4(float(_r0130.x), float(_r0130.y), float(_r0130.z), float(_r0130.w));
    _TMP50 = dot(vec3(float(_I4.x), float(_I4.y), float(_I4.z)), vec3( 1.43593750E+01, 2.81718750E+01, 5.47265625E+00));
    _r0140.x = float(_TMP50);
    _TMP50 = dot(vec3(float(_C1.x), float(_C1.y), float(_C1.z)), vec3( 1.43593750E+01, 2.81718750E+01, 5.47265625E+00));
    _r0140.y = float(_TMP50);
    _TMP50 = dot(vec3(float(_A0.x), float(_A0.y), float(_A0.z)), vec3( 1.43593750E+01, 2.81718750E+01, 5.47265625E+00));
    _r0140.z = float(_TMP50);
    _TMP50 = dot(vec3(float(_G5.x), float(_G5.y), float(_G5.z)), vec3( 1.43593750E+01, 2.81718750E+01, 5.47265625E+00));
    _r0140.w = float(_TMP50);
    _i4 = vec4(float(_r0140.x), float(_r0140.y), float(_r0140.z), float(_r0140.w));
    _TMP50 = dot(vec3(float(_I5.x), float(_I5.y), float(_I5.z)), vec3( 1.43593750E+01, 2.81718750E+01, 5.47265625E+00));
    _r0150.x = float(_TMP50);
    _TMP50 = dot(vec3(float(_C4.x), float(_C4.y), float(_C4.z)), vec3( 1.43593750E+01, 2.81718750E+01, 5.47265625E+00));
    _r0150.y = float(_TMP50);
    _TMP50 = dot(vec3(float(_A1.x), float(_A1.y), float(_A1.z)), vec3( 1.43593750E+01, 2.81718750E+01, 5.47265625E+00));
    _r0150.z = float(_TMP50);
    _TMP50 = dot(vec3(float(_G0.x), float(_G0.y), float(_G0.z)), vec3( 1.43593750E+01, 2.81718750E+01, 5.47265625E+00));
    _r0150.w = float(_TMP50);
    _i5 = vec4(float(_r0150.x), float(_r0150.y), float(_r0150.z), float(_r0150.w));
    _TMP50 = dot(vec3(float(_H5.x), float(_H5.y), float(_H5.z)), vec3( 1.43593750E+01, 2.81718750E+01, 5.47265625E+00));
    _r0160.x = float(_TMP50);
    _TMP50 = dot(vec3(float(_F4.x), float(_F4.y), float(_F4.z)), vec3( 1.43593750E+01, 2.81718750E+01, 5.47265625E+00));
    _r0160.y = float(_TMP50);
    _TMP50 = dot(vec3(float(_B1.x), float(_B1.y), float(_B1.z)), vec3( 1.43593750E+01, 2.81718750E+01, 5.47265625E+00));
    _r0160.z = float(_TMP50);
    _TMP50 = dot(vec3(float(_D0.x), float(_D0.y), float(_D0.z)), vec3( 1.43593750E+01, 2.81718750E+01, 5.47265625E+00));
    _r0160.w = float(_TMP50);
    _h5 = vec4(float(_r0160.x), float(_r0160.y), float(_r0160.z), float(_r0160.w));
    _fx = vec4( 1.00000000E+00, -1.00000000E+00, -1.00000000E+00, 1.00000000E+00)*_fp.y + vec4( 1.00000000E+00, 1.00000000E+00, -1.00000000E+00, -1.00000000E+00)*_fp.x;
    _fx_left = vec4( 1.00000000E+00, -1.00000000E+00, -1.00000000E+00, 1.00000000E+00)*_fp.y + vec4( 5.00000000E-01, 2.00000000E+00, -5.00000000E-01, -2.00000000E+00)*_fp.x;
    _fx_up = vec4( 1.00000000E+00, -1.00000000E+00, -1.00000000E+00, 1.00000000E+00)*_fp.y + vec4( 2.00000000E+00, 5.00000000E-01, -2.00000000E+00, -5.00000000E-01)*_fp.x;
    _interp_restriction_lv1 = bvec4(_e1.x != _b1.w && _e1.x != _b1.z, _e1.y != _b1.x && _e1.y != _b1.w, _e1.z != _b1.y && _e1.z != _b1.x, _e1.w != _b1.z && _e1.w != _b1.y);
    _interp_restriction_lv2_left = bvec4(_e1.x != _c1.z && _b1.y != _c1.z, _e1.y != _c1.w && _b1.z != _c1.w, _e1.z != _c1.x && _b1.w != _c1.x, _e1.w != _c1.y && _b1.x != _c1.y);
    _interp_restriction_lv2_up = bvec4(_e1.x != _c1.x && _b1.x != _c1.x, _e1.y != _c1.y && _b1.y != _c1.y, _e1.z != _c1.z && _b1.z != _c1.z, _e1.w != _c1.w && _b1.w != _c1.w);
    _x0170 = ((_fx + vec4( 3.33333343E-01, 3.33333343E-01, 3.33333343E-01, 3.33333343E-01)) - vec4( 1.50000000E+00, 5.00000000E-01, -5.00000000E-01, 5.00000000E-01))/vec4( 6.66666687E-01, 6.66666687E-01, 6.66666687E-01, 6.66666687E-01);
    _TMP51 = min(vec4( 1.00000000E+00, 1.00000000E+00, 1.00000000E+00, 1.00000000E+00), _x0170);
    _TMP171 = max(vec4( 0.00000000E+00, 0.00000000E+00, 0.00000000E+00, 0.00000000E+00), _TMP51);
    _x0178 = ((_fx_left + vec4( 1.66666672E-01, 3.33333343E-01, 1.66666672E-01, 3.33333343E-01)) - vec4( 1.00000000E+00, 1.00000000E+00, -5.00000000E-01, 0.00000000E+00))/vec4( 3.33333343E-01, 6.66666687E-01, 3.33333343E-01, 6.66666687E-01);
    _TMP51 = min(vec4( 1.00000000E+00, 1.00000000E+00, 1.00000000E+00, 1.00000000E+00), _x0178);
    _TMP179 = max(vec4( 0.00000000E+00, 0.00000000E+00, 0.00000000E+00, 0.00000000E+00), _TMP51);
    _x0186 = ((_fx_up + vec4( 3.33333343E-01, 1.66666672E-01, 3.33333343E-01, 1.66666672E-01)) - vec4( 2.00000000E+00, 0.00000000E+00, -1.00000000E+00, 5.00000000E-01))/vec4( 6.66666687E-01, 3.33333343E-01, 6.66666687E-01, 3.33333343E-01);
    _TMP51 = min(vec4( 1.00000000E+00, 1.00000000E+00, 1.00000000E+00, 1.00000000E+00), _x0186);
    _TMP187 = max(vec4( 0.00000000E+00, 0.00000000E+00, 0.00000000E+00, 0.00000000E+00), _TMP51);
    _a0198 = _e1 - _c1;
    _TMP195 = abs(_a0198);
    _a0202 = _e1 - _c1.zwxy;
    _TMP199 = abs(_a0202);
    _a0206 = _c1.wxyz - _h5;
    _TMP203 = abs(_a0206);
    _a0210 = _c1.wxyz - _h5.yzwx;
    _TMP207 = abs(_a0210);
    _a0214 = _b1.zwxy - _b1.wxyz;
    _TMP211 = abs(_a0214);
    _TMP27 = _TMP195 + _TMP199 + _TMP203 + _TMP207 + 4.00000000E+00*_TMP211;
    _a0220 = _b1.zwxy - _b1.yzwx;
    _TMP217 = abs(_a0220);
    _a0224 = _b1.zwxy - _i5;
    _TMP221 = abs(_a0224);
    _a0228 = _b1.wxyz - _i4;
    _TMP225 = abs(_a0228);
    _a0232 = _b1.wxyz - _b1;
    _TMP229 = abs(_a0232);
    _a0236 = _e1 - _c1.wxyz;
    _TMP233 = abs(_a0236);
    _TMP28 = _TMP217 + _TMP221 + _TMP225 + _TMP229 + 4.00000000E+00*_TMP233;
    _edr = bvec4(_TMP27.x < _TMP28.x && _interp_restriction_lv1.x, _TMP27.y < _TMP28.y && _interp_restriction_lv1.y, _TMP27.z < _TMP28.z && _interp_restriction_lv1.z, _TMP27.w < _TMP28.w && _interp_restriction_lv1.w);
    _a0240 = _b1.wxyz - _c1.zwxy;
    _TMP237 = abs(_a0240);
    _a0244 = _b1.zwxy - _c1;
    _TMP241 = abs(_a0244);
    _edr_left = bvec4((2.00000000E+00*_TMP237).x <= _TMP241.x && _interp_restriction_lv2_left.x && _edr.x, (2.00000000E+00*_TMP237).y <= _TMP241.y && _interp_restriction_lv2_left.y && _edr.y, (2.00000000E+00*_TMP237).z <= _TMP241.z && _interp_restriction_lv2_left.z && _edr.z, (2.00000000E+00*_TMP237).w <= _TMP241.w && _interp_restriction_lv2_left.w && _edr.w);
    _a0248 = _b1.wxyz - _c1.zwxy;
    _TMP245 = abs(_a0248);
    _a0252 = _b1.zwxy - _c1;
    _TMP249 = abs(_a0252);
    _edr_up = bvec4(_TMP245.x >= (2.00000000E+00*_TMP249).x && _interp_restriction_lv2_up.x && _edr.x, _TMP245.y >= (2.00000000E+00*_TMP249).y && _interp_restriction_lv2_up.y && _edr.y, _TMP245.z >= (2.00000000E+00*_TMP249).z && _interp_restriction_lv2_up.z && _edr.z, _TMP245.w >= (2.00000000E+00*_TMP249).w && _interp_restriction_lv2_up.w && _edr.w);
    _fx45 = vec4(float(_edr.x), float(_edr.y), float(_edr.z), float(_edr.w))*_TMP171;
    _fx30 = vec4(float(_edr_left.x), float(_edr_left.y), float(_edr_left.z), float(_edr_left.w))*_TMP179;
    _fx60 = vec4(float(_edr_up.x), float(_edr_up.y), float(_edr_up.z), float(_edr_up.w))*_TMP187;
    _a0256 = _e1 - _b1.wxyz;
    _TMP253 = abs(_a0256);
    _a0260 = _e1 - _b1.zwxy;
    _TMP257 = abs(_a0260);
    _px = bvec4(_TMP253.x <= _TMP257.x, _TMP253.y <= _TMP257.y, _TMP253.z <= _TMP257.z, _TMP253.w <= _TMP257.w);
    _TMP35 = max(_fx30, _fx60);
    _maximo = max(_TMP35, _fx45);
    _t0266 = float(_px.x);
    _TMP36 = _H + _t0266*(_F - _H);
    _t0268 = float(_maximo.x);
    _TMP37 = _E + _t0268*(_TMP36 - _E);
    _t0270 = float(_px.y);
    _TMP38 = _F + _t0270*(_B2 - _F);
    _t0272 = float(_maximo.y);
    _TMP39 = _E + _t0272*(_TMP38 - _E);
    _t0274 = float(_px.z);
    _TMP40 = _B2 + _t0274*(_D - _B2);
    _t0276 = float(_maximo.z);
    _TMP41 = _E + _t0276*(_TMP40 - _E);
    _t0278 = float(_px.w);
    _TMP42 = _D + _t0278*(_H - _D);
    _t0280 = float(_maximo.w);
    _TMP43 = _E + _t0280*(_TMP42 - _E);
    _TMP50 = dot(vec3(float(_TMP37.x), float(_TMP37.y), float(_TMP37.z)), vec3( 1.43593750E+01, 2.81718750E+01, 5.47265625E+00));
    _r0282.x = float(_TMP50);
    _TMP50 = dot(vec3(float(_TMP39.x), float(_TMP39.y), float(_TMP39.z)), vec3( 1.43593750E+01, 2.81718750E+01, 5.47265625E+00));
    _r0282.y = float(_TMP50);
    _TMP50 = dot(vec3(float(_TMP41.x), float(_TMP41.y), float(_TMP41.z)), vec3( 1.43593750E+01, 2.81718750E+01, 5.47265625E+00));
    _r0282.z = float(_TMP50);
    _TMP50 = dot(vec3(float(_TMP43.x), float(_TMP43.y), float(_TMP43.z)), vec3( 1.43593750E+01, 2.81718750E+01, 5.47265625E+00));
    _r0282.w = float(_TMP50);
    _pixel = vec4(float(_r0282.x), float(_r0282.y), float(_r0282.z), float(_r0282.w));
    _a0294 = _pixel - _e1;
    _TMP291 = abs(_a0294);
    _res = _TMP37;
    _mx = _TMP291.x;
    if (_TMP291.y > _TMP291.x) { 
        _res = _TMP39;
        _mx = _TMP291.y;
    } 
    if (_TMP291.z > _mx) { 
        _res = _TMP41;
        _mx = _TMP291.z;
    } 
    if (_TMP291.w > _mx) { 
        _res = _TMP43;
    } 
    _ret_0 = vec4(float(_res.x), float(_res.y), float(_res.z), 1.00000000E+00);
    FragColor = _ret_0;
    return;
} 
#endif
