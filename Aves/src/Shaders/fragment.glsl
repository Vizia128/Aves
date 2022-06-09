#version 460

#define MAX_STEPS 256
#define MAX_DIST 200.
#define SURF_DIST .01
#define PI 3.14159265358979323846
#define TWO_PI 6.28318530717958647692

out vec4 FragColor;
flat in vec3 ourColor;

//------------------------------------------------------------------
//------------------- 3D GEOMETRIC ALGEBRA -------------------------

// From jeremyong.com/klein

// p0 -> (e0, e1, e2, e3)
// p1 -> (1, e23, e31, e12)
// p2 -> (e0123, e01, e02, e03)
// p3 -> (e123, e032, e013, e021)

struct kln_plane
{
    vec4 p0;
};

struct kln_line
{
    vec4 p1;
    vec4 p2;
};

// If integrating this library with other code, remember that the point layout
// here has the homogeneous component in p3[0] and not p3[3]. The swizzle to
// get the vec3 Cartesian representation is p3.yzw
struct kln_point
{
    vec4 p3;
};

struct kln_rotor
{
    vec4 p1;
};

struct kln_translator
{
    vec4 p2;
};

struct kln_motor
{
    vec4 p1;
    vec4 p2;
};

kln_rotor kln_mul(in kln_rotor a, in kln_rotor b)
{
    kln_rotor c;
    c.p1 = a.p1.x * b.p1;
    c.p1 -= a.p1.yzwy * b.p1.ywyz;

    vec4 t = a.p1.zyzw * b.p1.zxxx;
    t += a.p1.wwyz * b.p1.wzwy;
    t.x = -t.x;

    c.p1 += t;
    return c;
}

kln_translator kln_mul(in kln_translator a, in kln_translator b)
{
    // (1 + a.p2) * (1 + b.p2) = 1 + a.p2 + b.p2
    kln_translator c;
    c.p2 = a.p2 + b.p2;
    return c;
}

kln_motor kln_mul(in kln_motor a, in kln_motor b)
{
    kln_motor c;
    vec4 a_zyzw = a.p1.zyzw;
    vec4 a_ywyz = a.p1.ywyz;
    vec4 a_wzwy = a.p1.wzwy;
    vec4 c_wwyz = b.p1.wwyz;
    vec4 c_yzwy = b.p1.yzwy;

    c.p1 = a.p1.x * b.p1;
    vec4 t = a_ywyz * c_yzwy;
    t += a_zyzw * b.p1.zxxx;
    t.x = -t.x;
    c.p1 += t;
    c.p1 -= a_wzwy * c_wwyz;

    c.p2 = a.p1.x * b.p2;
    c.p2 += a.p2 * b.p1.x;
    c.p2 += a_ywyz * b.p2.yzwy;
    c.p2 += a.p2.ywyz * c_yzwy;
    t = a_zyzw * b.p2.zxxx;
    t += a_wzwy * b.p2.wwyz;
    t += a.p2.zxxx * b.p1.zyzw;
    t += a.p2.wzwy * c_wwyz;
    t.x = -t.x;
    c.p2 -= t;
    return c;
}

kln_plane kln_apply(in kln_rotor r, in kln_plane p)
{
    vec4 dc_scale = vec4(1.0, 2.0, 2.0, 2.0);
    vec4 neg_low = vec4(-1.0, 1.0, 1.0, 1.0);

    vec4 t1 = r.p1.zxxx * r.p1.zwyz;
    t1 += r.p1.yzwy * r.p1.yyzw;
    t1 *= dc_scale;

    vec4 t2 = r.p1 * r.p1.xwyz;
    t2 -= (r.p1.wxxx * r.p1.wzwy) * neg_low;
    t2 *= dc_scale;

    vec4 t3 = r.p1 * r.p1;
    t3 -= r.p1.xwyz * r.p1.xwyz;
    t3 += r.p1.xxxx * r.p1.xxxx;
    t3 -= r.p1.xzwy * r.p1.xzwy;

    // TODO: provide variadic rotor-plane application
    kln_plane q;
    q.p0 = t1 * p.p0.xzwy;
    q.p0 += t2 * p.p0.xwyz;
    q.p0 += t3 * p.p0;
    return q;
}

kln_plane kln_apply(in kln_motor m, in kln_plane p)
{
    vec4 dc_scale = vec4(1.0, 2.0, 2.0, 2.0);
    vec4 neg_low = vec4(-1.0, 1.0, 1.0, 1.0);

    vec4 t1 = m.p1.zxxx * m.p1.zwyz;
    t1 += m.p1.yzwy * m.p1.yyzw;
    t1 *= dc_scale;

    vec4 t2 = m.p1 * m.p1.xwyz;
    t2 -= (m.p1.wxxx * m.p1.wzwy) * neg_low;
    t2 *= dc_scale;

    vec4 t3 = m.p1 * m.p1;
    t3 -= m.p1.xwyz * m.p1.xwyz;
    t3 += m.p1.xxxx * m.p1.xxxx;
    t3 -= m.p1.xzwy * m.p1.xzwy;

    vec4 t4 = m.p1.x * m.p2;
    t4 += m.p1.xzwy * m.p2.xwyz;
    t4 += m.p1 * m.p2.x;
    t4 -= m.p1.xwyz * m.p2.xzwy;
    t4 *= vec4(0.0, 2.0, 2.0, 2.0);

    // TODO: provide variadic motor-plane application
    kln_plane q;
    q.p0 = t1 * p.p0.xzwy;
    q.p0 += t2 * p.p0.xwyz;
    q.p0 += t3 * p.p0;
    q.p0 += vec4(dot(t4, p.p0), 0.0, 0.0, 0.0);
    return q;
}

kln_point kln_apply(in kln_rotor r, in kln_point p)
{
    vec4 scale = vec4(0, 2, 2, 2);

    vec4 t1 = r.p1 * r.p1.xwyz;
    t1 -= r.p1.x * r.p1.xzwy;
    t1 *= scale;

    vec4 t2 = r.p1.x * r.p1.xwyz;
    t2 += r.p1.xzwy * r.p1;
    t2 *= scale;

    vec4 t3 = r.p1 * r.p1;
    t3 += r.p1.yxxx * r.p1.yxxx;
    vec4 t4 = r.p1.zwyz * r.p1.zwyz;
    t4 += r.p1.wzwy * r.p1.wzwy;
    t3 -= t4 * vec4(-1.0, 1.0, 1.0, 1.0);

    // TODO: provide variadic rotor-point application
    kln_point q;
    q.p3 = t1 * p.p3.xwyz;
    q.p3 += t2 * p.p3.xzwy;
    q.p3 += t3 * p.p3;
    return  q;
}

kln_point kln_apply(in kln_motor m, in kln_point p)
{
    vec4 scale = vec4(0, 2, 2, 2);

    vec4 t1 = m.p1 * m.p1.xwyz;
    t1 -= m.p1.x * m.p1.xzwy;
    t1 *= scale;

    vec4 t2 = m.p1.x * m.p1.xwyz;
    t2 += m.p1.xzwy * m.p1;
    t2 *= scale;

    vec4 t3 = m.p1 * m.p1;
    t3 += m.p1.yxxx * m.p1.yxxx;
    vec4 t4 = m.p1.zwyz * m.p1.zwyz;
    t4 += m.p1.wzwy * m.p1.wzwy;
    t3 -= t4 * vec4(-1.0, 1.0, 1.0, 1.0);

    t4 = m.p1.xzwy * m.p2.xwyz;
    t4 -= m.p1.x * m.p2;
    t4 -= m.p1.xwyz * m.p2.xzwy;
    t4 -= m.p1 * m.p2.x;
    t4 *= scale;

    // TODO: provide variadic motor-point application
    kln_point q;
    q.p3 = t1 * p.p3.xwyz;
    q.p3 += t2 * p.p3.xzwy;
    q.p3 += t3 * p.p3;
    q.p3 += t4 * p.p3.x;
    return  q;
}

// If no entity is provided as the second argument, the motor is
// applied to the origin.
// NOTE: The motor MUST be normalized for the result of this operation to be
// well defined.
kln_point kln_apply(in kln_motor m)
{
    kln_point p;
    p.p3 = m.p1 * m.p2.x;
    p.p3 += m.p1.x * m.p2;
    p.p3 += m.p1.xwyz * m.p2.xzwy;
    p.p3 = m.p1.xzwy * m.p2.xwyz - p.p3;
    p.p3 *= vec4(0.0, 2.0, 2.0, 2.0);
    p.p3.x = 1.0;
    return p;
}

//------------------------------------------------------------------
//------------- PRIMATIVE SIGNED DISTANCE FUNCTIONS ----------------


//------------------------- SPHERE ---------------------------------

float sdSphere( vec3 pnt, vec3 sphereCenter, float radius ) 
{
	return length(pnt - sphereCenter.xyz) - radius;
}

float sdSphere( vec3 pnt, float radius ) {
	return length(pnt) - radius;
}

float sdSphere( kln_point pnt, kln_point center, float radius) 
{
    return sdSphere(pnt.p3.yzw, center.p3.yzw, radius);
}

float sdSphere( kln_point pnt, kln_motor invPose, float radius) 
{
    return sdSphere(kln_apply(invPose, pnt).p3.yzw, radius);
}


//--------------------------- BOX ---------------------------------

float sdBox( vec3 pnt, vec3 boxDim ) 
{
  vec3 q = abs(pnt) - boxDim;
  return length(max(q,0.0)) + min(max(q.x,max(q.y,q.z)),0.0);
}

float sdBox( kln_point pnt, kln_motor invPose, vec3 boxDim) 
{
    //return sdBox(pnt.p3.yzw, boxDim);
    return sdBox(kln_apply(invPose, pnt).p3.yzw, boxDim);
}


//------------------------------------------------------------------
//-------------------------- CAMERA --------------------------------

layout (std140) uniform Camera 
{
    kln_motor camPose;
    //kln_translator camTran;
    //kln_rotor camRotor;
    vec2 windowRes;
    float fov;
    float time;

    kln_motor tempCube;
};

//------------------------------------------------------------------
//------------- ENVIRONMENT 1 : INSIDE OF A SPHERE -----------------

float sdEnv1(vec3 pnt) //, vec3 sphereCenter, float radius)
{ 
	return -(length(pnt - vec3(0.0)) - 64.);
}

//------------------------------------------------------------------
//-------------------------- OBJECT 1 ------------------------------
#define N 2
layout (std140) uniform Object1 
{
    vec3 pos;           // position
    vec4 rot;           // quaternion rotation
    float size;         // scaling
    float boundRad;     // bounding sphere radius

    vec3 color;         // rgb
    float radiance;     // light source
    float transparency; // transparency
    float refIndex;     // refractive index

    float k_s;          // specular reflection constant
    float k_d;          // diffuse reflection constant
    float k_a;          // ambient reflection constant
    float alpha;        // shininess constant
    int isActive;       // isActive
}object1[N];

bool object1Active[N];

float sdPntObject1(vec3 pnt, int ID) 
{ 
    return sdBox(pnt, vec3(1.,2.,3.));
}

float sdRayObject1Bound(vec3 rayOrigin, vec3 rayDir, int ID) 
{
    vec3 objPos = object1[ID].pos;
    float h = max(0.0, dot(objPos - rayOrigin, rayDir));  //rayDir must be normalized
    vec3 pnt = rayOrigin + rayDir * h;
    return sdSphere(pnt, objPos, object1[ID].boundRad);
}

//------------------------------------------------------------------
//---------------------- RENDERING FUNCTIONS -----------------------

float calcDist(vec3 pnt) 
{
    vec4 s = vec4(0, 1, 6, 1);
    
    float sphereDist =  length(pnt-s.xyz)-s.w;
    //float cubeDist = sdBox(pnt-vec3(3, 2, 4), vec3(0.5,1.0,1.5));

    kln_point KLN_PNT;
    KLN_PNT.p3 = vec4(1, pnt);
    float cubeDist = sdBox(KLN_PNT, tempCube, vec3(0.5,1.0,1.5));
    
    float planeDist = pnt.y;
    
    float d = min(sphereDist,min( planeDist, cubeDist));
    return d;
}

vec2 rayMarch(vec3 rayOrigin, vec3 rayDir) 
{
    
    for (int i = 0; i<N; i++) 
    {
        object1Active[i] = sdRayObject1Bound(rayOrigin, rayDir, i) <= object1[i].boundRad;
    }

    int stp = 0;
    float distTot = 0.0, distStp = 0.0;
    for (;stp<MAX_STEPS && distTot<MAX_DIST && distStp>SURF_DIST; stp++) 
    {
        vec3 pnt = rayOrigin + rayDir*distStp;
        distStp = calcDist(pnt);
        distTot += distStp;
    }

    return vec2(distTot, stp);
}

vec2 RayMarch(vec3 ro, vec3 rd) 
{
	float dO=0.;

    int i =0;
    for(; i<MAX_STEPS; i++) 
    {
    	vec3 p = ro + rd*dO;
        float dS = calcDist(p);
        dO += dS;
        if(dO>MAX_DIST || dS<SURF_DIST) break;
    }
    return vec2(dO, i);
}

vec3 calcNormal(vec3  pos, float dist) 
{
    const float h = 0.0001;      // replace by an appropriate value
    #define ZERO (min(int(windowRes.x),0)) // non-constant zero
    vec3 n = vec3(0.0);
    for( int i=ZERO; i<4; i++ )
    {
        vec3 e = 0.5773*(2.0*vec3((((i+3)>>1)&1),((i>>1)&1),(i&1))-1.0);
        n += e*calcDist(pos+e*h).x;
    }
    return normalize(n);
}

vec3 calcLighting(vec3 p) 
{
    vec3 lightPos = vec3(0, 5, 6);
    vec3 l = normalize(lightPos-p);
    vec3 n = calcNormal(p, 0.0001);
    
    float dif = clamp(dot(n, l), 0., 1.);
    //vec2 ds = rayMarch(p+n*SURF_DIST*2., l);
    vec2 ds = RayMarch(p+n*SURF_DIST*2., l);
    float d=ds.x;
    if(d<length(lightPos-p)) dif *= .1;
    
    return vec3(dif);
}

//vec3 rayDirection() 
//{
//    vec2 winSize = windowRes.xy;
//    vec2 uv = (gl_FragCoord.xy - 0.5*winSize - 0.5) / winSize.y;
//    return normalize(1.0 * normalize(camDir.p3.yzw) + 16.0 * normalize(vec3(uv, 1.0)));
//}


//------------------------------------------------------------------
//---------------------------- MAIN --------------------------------

void main() 
{
#if 0
    vec3 rayDir = rayDirection();
    vec3 rayOrigin = cameraPos;

    vec2 dist_steps = rayMarch(rayOrigin, rayDir);

    float dist = dist_steps.x;
    vec3 pntEnd = rayOrigin + rayDir*dist;

    vec3 color = calcLighting(pntEnd);
    color = pow(color, vec3(.4545));

    FragColor = vec4(color ,1.0);
#endif

#if 1
    vec2 winSize = windowRes.xy;
    vec2 uv = (gl_FragCoord.xy - 0.5*winSize - 0.5) / winSize.y;

	vec3 color = vec3(1., 0.4, 0.0);

    //kln_point klnRayOrigin;
    //klnRayOrigin.p3 = vec4(1,0,0,0);
    kln_point klnRayOrigin = kln_apply(camPose);
	vec3 rayOrigin = klnRayOrigin.p3.yzw / klnRayOrigin.p3.x;

    kln_point klnRayDir;
    klnRayDir.p3 = vec4(0, uv, fov);
    klnRayDir = kln_apply(camPose, klnRayDir);
    vec3 rayDir = normalize(klnRayDir.p3.yzw);
	//vec3 rayDir = normalize(normalize(camDir.p3.yzw) + normalize(vec3(uv, 1.0)));


	vec2 dist_steps = RayMarch(rayOrigin, rayDir);

    if (dist_steps.x < MAX_DIST && dist_steps.y < MAX_STEPS)
    {
	    vec3 p = rayOrigin + rayDir * dist_steps.x;
 
        color = calcLighting(p);
    }
    else if (!(dist_steps.y < MAX_STEPS))
    {
        color = vec3(0.0, 0.8, 1.0);
    }
    else
    {
        color = vec3(0.8, 0.1, 0.5);
    }
    
    color = pow(color, vec3(.4545));	// gamma correction


	FragColor = vec4(color, 1.0f);
#endif
}
