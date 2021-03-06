#ifndef DV2520_STRUCTS_H
#define DV2520_STRUCTS_H

#include <MathSimple.h>

struct Ray {
    Vec3F pos;
    float ref; // Reflective factor.

    Vec3F dir;
    float distMax;

    Vec3F light;
    int primID;
};

struct Intersection {
    float u;
    float v;
    float t;

    int primId;
    unsigned primVertexOffset;
    unsigned instanceIdx;

    float dist;
};

struct ObjInstance {
    Mat4F world;
    Mat4F worldInv;

    unsigned indexStart;
    unsigned indexCnt;
    unsigned vertexCnt;
    unsigned pad;
};

struct LightPoint {
    Vec3F pos;
    float dist; // range

    Vec4F ambient;
    Vec4F diffuse;
    Vec4F specular;

    Vec3F attenuation;
    float pad;
};

#endif // DV2520_STRUCTS_H
