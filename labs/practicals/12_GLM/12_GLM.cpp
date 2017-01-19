#include <glm\glm.hpp>
#include <glm\gtc\constants.hpp>
#include <glm\gtc\matrix_transform.hpp>
#include <glm\gtc\quaternion.hpp>
#include <glm\gtx\euler_angles.hpp>
#include <glm\gtx\projection.hpp>
#include <iostream>

using namespace std;
using namespace glm;

int main() {

	vec2 a(1.0f, 0.0f);
	vec2 b(0.0f, 1.0f);

	vec3 c(1.0f, 0.0f, 0.0f);
	vec3 d(0.0f, 0.0f, 1.0f);

	vec4 e(0.0f, 1.0f, 0.0f, 0.0f);
	vec4 f(0.0f, 0.0f, 1.0f, 0.0f);
	//---------------------------------
	vec2 g(c);
	vec2 h(e);

	vec3 i(a, 0.0f);
	vec3 j(f);

	vec4 k(b, 0.0f, 0.0f);
	vec4 l(d, 0.0f);
	//---------------------------------
	vec2 m = a + b;
	vec2 n = g - h;

	vec3 o = c + d;
	vec3 p = i - j;

	vec4 q = e + f;
	vec4 r = k - l;
	//---------------------------------
	vec2 s = m * 2.0f;
	vec2 t = n / 2.0f;

	vec3 u = o * 3.0f;
	vec3 v = p / 3.0f;

	vec4 w = q * 4.0f;
	vec4 x = r / 4.0f;
	//---------------------------------
	float y = length(s);
	float z = length(u);
	float A = length(w);
	//---------------------------------
	vec2 B = normalize(t);

	vec3 C = normalize(v);

	vec4 D = normalize(x);
	//---------------------------------
	float E = dot(a, b);
	float F = dot(c, d);
	float G = dot(e, f);
	//---------------------------------
	vec2 H = proj(g, h);

	vec3 I = proj(i, j);

	vec4 J = proj(k, l);
	//---------------------------------
	vec3 K = cross(C, I);
	//---------------------------------
	mat2 L(1.0f);
	mat2 M(1.0f);

	mat3 N(1.0f);
	mat3 O(1.0f);

	mat4 P(1.0f);
	mat4 Q(1.0f);
	//---------------------------------
	mat2 R = L + M;
	mat2 S = L - M;

	mat3 T = N + O;
	mat3 U = N - O;

	mat4 V = P + Q;
	mat4 W = P - Q;
	//---------------------------------
	mat2 X = L * 2.0f;
	mat2 Y = M / 2.0f;

	mat3 Z = N * 3.0f;
	mat3 aa = O / 3.0f;

	mat4 bb = P * 4.0f;
	mat4 cc = Q / 4.0f;
	//---------------------------------
	mat2 dd = L * M;
	mat2 ee = X * Y;

	mat3 ff = N * O;
	mat3 gg = Z * aa;

	mat4 hh = P * Q;
	mat4 ii = bb * cc;
	//---------------------------------
	vec3 jj = vec3(bb * vec4(d, 1.0f));

	vec4 kk = vec4(cc * J);
	//---------------------------------
	mat4 ll = translate(mat4(1.0f), vec3(1.0f, 2.0f, 3.0f));
	//---------------------------------
	mat4 mm = rotate(mat4(1.0f), 3.14f, vec3(1.0f, 0.0f, 0.0f));
	vec3 nn = mm * e;
	//---------------------------------
	mat4 oo = scale(mat4(1.0f), vec3(3.0f, 2.0f, 1.0f));
	vec3 pp = oo * vec4(1.0f, 1.0f, 1.0f, 1.0f);
	//---------------------------------
	mat4 qq = ll * mm * oo;
	vec3 rr = qq * vec4(1.0f, 1.0f, 1.0f, 1.0f);
	//---------------------------------
	quat ss = rotate(quat(), 3.14f / 2.0f, vec3(0.0f, 0.0f, 1.0f));
	quat tt = rotate(quat(), 3.14f / 2.0f, vec3(1.0f, 0.0f, 0.0f));
	quat uu = rotate(quat(), 3.14f / 2.0f, vec3(0.0f, 1.0f, 0.0f));
	//---------------------------------
	quat vv = ss * tt * uu;
	//---------------------------------
	mat4 ww = mat4_cast(vv);
	vec3 xx = ww * vec4(1.0f, 1.0f, 0.0f, 1.0f);
}