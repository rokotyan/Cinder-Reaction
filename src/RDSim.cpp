//
//  RDSim.cpp
//  Reaction
//
//  Created by Nikita Rokotyan on 26.09.14.
//
//

#include "RDSim.h"

RDSim::RDSim() {
    mPerturbHW  = 20;
    mParameterF = 0.05;//.03f;
    mParameterK = 0.06f;//.056f;
    
    mParameterU = 0.000181;
    mParameterV = 0.00004;
    mSimSteps = Vec2i(128 , 128);
    mSize = Rectf( 0, 0, 6.5, 6.5 );
    
//    dx = .027;
    dx = .025;
    
    // Initial sate
    for ( size_t col = 0; col < mSimSteps.x; ++col ) {
        mU.push_back( vector< vector< float > >::value_type() );
        mV.push_back( vector< vector< float > >::value_type() );
        dU.push_back( vector< vector< float > >::value_type() );
        dV.push_back( vector< vector< float > >::value_type() );
        for ( size_t row = 0; row < mSimSteps.y; ++row ) {
            mU[col].push_back( 1.0f );
            mV[col].push_back( 0.0f );
            dU[col].push_back( 0.0f );
            dV[col].push_back( 0.0f );
            
            mMeshVertices.push_back( Vec3f( (float)col/64.f - 1, 0.0f, (float)row/64.f - 1) );
            mMeshVerticesColor.push_back( Vec3f( mU[col][row], 0.5f, mV[col][row] ) );
        }
    }
    
    // Perturbation
    for ( size_t row = mU.size()/2 - mPerturbHW; row < mU.size()/2 + mPerturbHW; ++row ) {
        for ( size_t col = mU[row].size()/2 - mPerturbHW; col < mU[row].size()/2 + mPerturbHW; ++col ) {
            mU[col][row] = 0.5f;
            mV[col][row] = 0.25f;
        }

    }
    
    // Add random noise
    for ( size_t row = 0; row < mU.size(); ++row ) {
        for ( size_t col = 0; col < mU[row].size(); ++col ) {
            mU[col][row] += 0.16f*rand()/(float)RAND_MAX;
            mV[col][row] += 0.16f*rand()/(float)RAND_MAX;
        }
    }
    
    // Create Mesh with Normals
    for ( size_t row = 0; row < mU.size()-1; ++row ) {
        for ( size_t col = 0; col < mU[row].size()-1; ++col ) {
            mMeshIndices.push_back( row * mU[row].size() + col );
            mMeshIndices.push_back( row * mU[row].size() + col+1 );
            mMeshIndices.push_back( (row+1) * mU[row].size() + col );
            
            Vec3f v0 = mMeshVertices.at( row * mU[row].size() + col );
            Vec3f v1 = mMeshVertices.at( row * mU[row].size() + col+1 );
            Vec3f v2 = mMeshVertices.at( (row+1) * mU[row].size() + col );
            Vec3f e0 = (v2 - v0).normalized();
            Vec3f e1 = (v2 - v1).normalized();
            Vec3f n = e0.cross(e1).normalized();
            n.normalize();
            mMeshNormals.push_back( n );
            mMeshNormals.push_back( n );
            mMeshNormals.push_back( n );
            
            
            mMeshIndices.push_back( (row+1) * mU[row].size() + col );
            mMeshIndices.push_back( row * mU[row].size() + col+1 );
            mMeshIndices.push_back( (row+1) * mU[row].size() + col+1 );
            
            v0 = mMeshVertices.at( (row+1) * mU[row].size() + col );
            v1 = mMeshVertices.at( row * mU[row].size() + col+1 );
            v2 = mMeshVertices.at( (row+1) * mU[row].size() + col+1 );
            e0 = (v2 - v0).normalized();
            e1 = (v2 - v1).normalized();
            n = e0.cross(e1).normalized();
            n.normalize();
            mMeshNormals.push_back( n );
            mMeshNormals.push_back( n );
            mMeshNormals.push_back( n );
            
        }
    }
    
    
    // Calculate
    for ( size_t i = 0; i < 1; ++i ){
        update();
    }
}

void RDSim::update() {
//    float dx = 1;( mSize.x2 - mSize.x1 ) / (float)( mSimSteps.x - 1 );
//    float dy = 1( mSize.y2 - mSize.y1 ) / (float)( mSimSteps.y - 1 );
    
    for ( size_t i = 0; i < 20; ++i ){
        calcLaplacian( mU, &dU );
        calcLaplacian( mV, &dV );
        
        // Update Center of the matrix
        for ( size_t row = 0; row < mU.size(); ++row ) {
            for ( size_t col = 0; col < mU[row].size(); ++col ) {
                float uvv = mU[row][col] * mV[row][col] * mV[row][col];
                mU[row][col] += 0.15*(mParameterU * dU[row][col]/dx/dx - uvv +   mParameterF * ( 1 - mU[row][col] ) );
                mV[row][col] += 0.15*(mParameterV * dV[row][col]/dx/dx + uvv -  (mParameterF + mParameterK) * mV[row][col]   );
                
                mMeshVertices[ row*mU[row].size() + col ].y       = mU[row][col]/3.f;
//                mMeshVerticesColor[ row * mU[row].size() + col].x = 1-mU[row][col];
//                mMeshVerticesColor[ row * mU[row].size() + col].z = 1-mU[row][col];
//                mMeshVerticesColor[ row * mU[row].size() + col].y = 1-mU[row][col];
                
//                if ( mU[row][col] > 0.5 )
//                    mMeshVerticesColor[ row * mU[row].size() + col] = Vec3f(1-mU[row][col],1,1);
//                else
                    mMeshVerticesColor[ row * mU[row].size() + col] = Vec3f(1,1,1);
            }
        }
    }
    
    // Update Normals
    for ( size_t row = 0; row < mU.size()-1; ++row ) {
        for ( size_t col = 0; col < mU[row].size()-1; ++col ) {
            
            Vec3f v0 = mMeshVertices.at( row * mU[row].size() + col );
            Vec3f v1 = mMeshVertices.at( row * mU[row].size() + col+1 );
            Vec3f v2 = mMeshVertices.at( (row+1) * mU[row].size() + col );
            
            Vec3f e0 = (v2 - v0).normalized();
            Vec3f e1 = (v2 - v1).normalized();
            Vec3f n = e0.cross(e1).normalized();
            n.normalize();
            mMeshNormals.at( row * mU[row].size() + col ) = n;
            mMeshNormals.at( row * mU[row].size() + col+1 ) = n;
            mMeshNormals.at( (row+1) * mU[row].size() + col ) = n;
            
            
            v0 = mMeshVertices.at( (row+1) * mU[row].size() + col );
            v1 = mMeshVertices.at( row * mU[row].size() + col+1 );
            v2 = mMeshVertices.at( (row+1) * mU[row].size() + col+1 );
            e0 = (v2 - v0).normalized();
            e1 = (v2 - v1).normalized();
            n = e0.cross(e1).normalized();
            n.normalize();
            mMeshNormals.at( (row+1) * mU[row].size() + col ) = n;
            mMeshNormals.at( row * mU[row].size() + col+1 ) = n;
            mMeshNormals.at( (row+1) * mU[row].size() + col+1 ) = n;
        }
    }
 
}

void RDSim::draw() {
//    for ( size_t row = 0; row < mU.size(); ++row ) {
//        for ( size_t col = 0; col < mU[row].size(); ++col ) {
//            gl::color( mU[row][col], mV[row][col], 0.5f );
//            gl::drawSolidCircle( Vec2i( 2*row, 2*col ), 4, 4 );
//        }
//    }
    
    glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );
    glPointSize( 10.0f );
    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_COLOR_ARRAY);
    glEnableClientState(GL_NORMAL_ARRAY);
//
    glVertexPointer( 3, GL_FLOAT, 0, &(mMeshVertices[0].x)  );
    glColorPointer(3, GL_FLOAT, 0, &(mMeshVerticesColor[0].x) );
    glNormalPointer(GL_FLOAT, 0, &(mMeshNormals[0].x) );
//    glTexCoordPointer( 2, GL_FLOAT, us->stride, &(us->vertices[0].texCoord.x) );
//    glDrawElements( GL_TRIANGLES, us->i_size, GL_UNSIGNED_BYTE, &(us->shape->indices[0]) );
    glDrawElements( GL_TRIANGLES, mMeshIndices.size(), GL_UNSIGNED_INT, &(mMeshIndices[0]) );
//    glDrawArrays( GL_POINTS, 0, mMeshVertices.size() );
//
    glDisableClientState(GL_VERTEX_ARRAY);
    glDisableClientState(GL_COLOR_ARRAY);
}


void RDSim::calcLaplacian(const vector< vector< float > > &inMat, vector< vector< float > > *outMat) {
    size_t imax = inMat.size();
    size_t jmax = inMat[0].size();
    
    // Calculate the centner
    for ( size_t row = 1; row < inMat.size()-1; ++row )
        for ( size_t col = 1; col < inMat[row].size()-1; ++col )
            (*outMat)[row][col] = ( inMat[row-1][col] + inMat[row+1][col] + inMat[row][col-1] + inMat[row][col+1] ) - 4*inMat[row][col];
    
    // Calculate the borders borders
    for ( size_t row = 1; row < jmax-1; ++row ) {
        (*outMat)[row][0]      = ( inMat[row-1][0]      +inMat[row+1][0]      + inMat[row][jmax-1]   + inMat[row][1] )   - 4*inMat[row][0];
        (*outMat)[row][jmax-1] = ( inMat[row-1][jmax-1] +inMat[row+1][jmax-1] + inMat[row][jmax-1-1] + inMat[row][0] )   - 4*inMat[row][jmax-1];
    }
    
    for ( size_t col = 1; col < imax-1; ++col ) {
        (*outMat)[0][col]      = ( inMat[imax-1][col]   + inMat[1][col] + inMat[0][col-1]      + inMat[0][col+1] )      - 4*inMat[0][col];
        (*outMat)[imax-1][col] = ( inMat[imax-1-1][col] + inMat[0][col] + inMat[imax-1][col-1] + inMat[imax-1][col+1] ) - 4*inMat[imax-1][col];
    }
    
    (*outMat)[0][0] 		  = (inMat[imax-1][0]       +inMat[1][0]       +inMat[0][jmax-1]       +inMat[0][1] )        - 4*inMat[0][0];
    (*outMat)[imax-1][0]	  = (inMat[imax-1-1][0]     +inMat[0][0]       +inMat[imax-1][jmax-1]  +inMat[imax-1][1] )   - 4*inMat[imax-1][0];
    (*outMat)[0][jmax-1]      = (inMat[imax-1][jmax-1]  +inMat[1][jmax-1]  +inMat[0][jmax-1-1]     +inMat[0][0] )        - 4*inMat[0][jmax-1];
    (*outMat)[imax-1][jmax-1] = (inMat[imax-1-1][jmax-1]+inMat[0][jmax-1]  +inMat[imax-1][jmax-1-1]+inMat[imax-1][0] )   - 4*inMat[imax-1][jmax-1];
    
}

void RDSim::reset() {
    // Initial sate
    for ( size_t col = 0; col < mSimSteps.x; ++col ) {
        for ( size_t row = 0; row < mSimSteps.y; ++row ) {
            mU[row][col] = 1.0f;
            mV[row][col] = 0.0f;
            dU[row][col] = 0.0f;
            dV[row][col] = 0.0f;
        }
    }
    
//    // Perturbation
//    for ( size_t row = mU.size()/2 - mPerturbHW; row < mU.size()/2 + mPerturbHW; ++row ) {
//        for ( size_t col = mU[row].size()/2 - mPerturbHW; col < mU[row].size()/2 + mPerturbHW; ++col ) {
//            mU[col][row] = 0.5f;
//            mV[col][row] = 0.25f;
//        }
//        
//    }
//    
    // Add random noise
    for ( size_t row = 0; row < mU.size(); ++row ) {
        for ( size_t col = 0; col < mU[row].size(); ++col ) {
            mU[col][row] += 0.18f*(rand()/(float)RAND_MAX - 0);
            mV[col][row] += 0.18f*(rand()/(float)RAND_MAX - 0);
        }
    }
}