#version 460

#define MAX_STEPS 256
#define MAX_DIST 200.
#define SURF_DIST .01
#define PI 3.14159265358979323846
#define TWO_PI 6.28318530717958647692

out vec4 FragColor;
flat in vec3 ourColor;


//------------- PRIMATIVE SIGNED DISTANCE FUNCTIONS ----------------

float sdSphere( vec3 pnt, vec3 sphereCenter, float radius ) {
	return length(pnt - sphereCenter.xyz) - radius;
}

float sdBox( vec3 pnt, vec3 boxDim ) {
  vec3 q = abs(pnt) - boxDim;
  return length(max(q,0.0)) + min(max(q.x,max(q.y,q.z)),0.0);
}


//------------------------ CAMERA -------------------------------

layout (std140) uniform Camera {
    vec3 cameraPos;
    vec3 cameraDir;    // 2x3x4 bytes
    vec2 windowRes;     // 2x3x4 + 2x2 bytes
    float fov;
    float time;        // 2x3x8 + 2x2 + 2 bytes
};


//----------- ENVIRONMENT 1 : INSIDE OF A SPHERE -----------------

float sdEnv1(vec3 pnt) { //, vec3 sphereCenter, float radius) {
	return -(length(pnt - vec3(0.0)) - 64.);
}


//------------------------ OBJECT 1 ------------------------------
#define N 2
layout (std140) uniform Object1 {
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

float sdPntObject1(vec3 pnt, int ID) { 
    return sdBox(pnt, vec3(1.,2.,3.));
}

float sdRayObject1Bound(vec3 rayOrigin, vec3 rayDir, int ID) {
    vec3 objPos = object1[ID].pos;
    float h = max(0.0, dot(objPos - rayOrigin, rayDir));  //rayDir must be normalized
    vec3 pnt = rayOrigin + rayDir * h;
    return sdSphere(pnt, objPos, object1[ID].boundRad);
}


//---------------------- RENDERING FUNCTIONS ---------------------

float calcDist(vec3 pnt) {
    vec4 s = vec4(0, 1, 6, 1);
    
    float sphereDist =  length(pnt-s.xyz)-s.w;
    float planeDist = pnt.y;
    
    float d = min(sphereDist, planeDist);
    return d;
}

vec2 rayMarch(vec3 rayOrigin, vec3 rayDir) {
    
    for (int i = 0; i<N; i++) {
        object1Active[i] = sdRayObject1Bound(rayOrigin, rayDir, i) <= object1[i].boundRad;
    }

    int stp = 0;
    float distTot = 0.0, distStp = 0.0;
    for (;stp<MAX_STEPS && distTot<MAX_DIST && distStp>SURF_DIST; stp++) {
        vec3 pnt = rayOrigin + rayDir*distStp;
        distStp = calcDist(pnt);
        distTot += distStp;
    }

    return vec2(distTot, stp);
}

vec2 RayMarch(vec3 ro, vec3 rd) {
	float dO=0.;

    int i =0;
    for(; i<MAX_STEPS; i++) {
    	vec3 p = ro + rd*dO;
        float dS = calcDist(p);
        dO += dS;
        if(dO>MAX_DIST || dS<SURF_DIST) break;
    }
    return vec2(dO, i);
}

vec3 calcNormal(vec3  pos, float dist) {
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

vec3 calcLighting(vec3 p) {
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

vec3 rayDirection() {
    vec2 xy = gl_FragCoord.xy - 0.5*windowRes - 0.5;
    float z = windowRes.y / tan(radians(fov) / 2.0);
    return normalize(vec3(xy, -z));
}

void main() {
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
	vec3 rayOrigin = cameraPos.xyz;
	vec3 rayDir = normalize(1.0 * normalize(cameraDir.xyz) + 16.0 * normalize(vec3(uv, 1.0)));


	vec2 dist_steps = RayMarch(rayOrigin, rayDir);

    if (dist_steps.x < MAX_DIST && dist_steps.y < MAX_STEPS) {
	    vec3 p = rayOrigin + rayDir * dist_steps.x;
 
        color = calcLighting(p);
    }
    else if (!(dist_steps.y < MAX_STEPS)){
        color = vec3(0.0, 0.8, 1.0);
    }
    else {
        color = vec3(0.8, 0.1, 0.5);
    }
    
    color = pow(color, vec3(.4545));	// gamma correction


	FragColor = vec4(color, 1.0f);
#endif


}
