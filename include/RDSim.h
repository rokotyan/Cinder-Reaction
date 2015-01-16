//
//  RDSim.h
//  Reaction
//
//  Created by Nikita Rokotyan on 26.09.14.
//
//
#pragma once
#include "cinder/app/AppNative.h"
#include "cinder/gl/gl.h"

using namespace ci;
using namespace ci::app;
using namespace std;

class RDSim {
public:
    RDSim();
    void update();
    void draw();
    void reset();
    void calcLaplacian(const vector< vector< float > > &inMat, vector< vector< float > > *outMat);
    
    int mPerturbHW;
    float mParameterF, mParameterK, mParameterU, mParameterV;
    Vec2i mSimSteps;
    Rectf mSize;
    
    vector< vector< float > > mU, mV;
    vector< vector< float > > dU, dV;
    float dx;
    
    // Draw arrays
    vector< Vec3f >     mMeshVertices;
    vector< Vec3f >     mMeshNormals;
    vector< size_t >    mMeshIndices;
    vector< Vec3f >     mMeshVerticesColor;
    
};