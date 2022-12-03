#ifndef SQUARE_H
#define SQUARE_H
#include "Vec3.h"
#include <vector>
#include "Mesh.h"
#include <cmath>

struct RaySquareIntersection{
    bool intersectionExists;
    float t;
    float u,v;
    Vec3 intersection;
    Vec3 normal;
};


class Square : public Mesh {
public:
    Vec3 m_normal;
    Vec3 m_bottom_left;
    Vec3 m_right_vector;
    Vec3 m_up_vector;

    Square() : Mesh() {}
    Square(Vec3 const & bottomLeft , Vec3 const & rightVector , Vec3 const & upVector , float width=1. , float height=1. ,
           float uMin = 0.f , float uMax = 1.f , float vMin = 0.f , float vMax = 1.f) : Mesh() {
        setQuad(bottomLeft, rightVector, upVector, width, height, uMin, uMax, vMin, vMax);
    }


    void setQuad( Vec3 const & bottomLeft , Vec3 const & rightVector , Vec3 const & upVector , float width=1. , float height=1. ,
                  float uMin = 0.f , float uMax = 1.f , float vMin = 0.f , float vMax = 1.f) {
        m_right_vector = rightVector;
        m_up_vector = upVector;
        m_normal = Vec3::cross(rightVector , upVector);
        m_bottom_left = bottomLeft;

        m_normal.normalize();
        m_right_vector.normalize();
        m_up_vector.normalize();

        m_right_vector = m_right_vector*width;
        m_up_vector = m_up_vector*height;

        vertices.clear();
        vertices.resize(4);
        vertices[0].position = bottomLeft;                                      vertices[0].u = uMin; vertices[0].v = vMin;
        vertices[1].position = bottomLeft + m_right_vector;                     vertices[1].u = uMax; vertices[1].v = vMin;
        vertices[2].position = bottomLeft + m_right_vector + m_up_vector;       vertices[2].u = uMax; vertices[2].v = vMax;
        vertices[3].position = bottomLeft + m_up_vector;                        vertices[3].u = uMin; vertices[3].v = vMax;
        vertices[0].normal = vertices[1].normal = vertices[2].normal = vertices[3].normal = m_normal;
        triangles.clear();
        triangles.resize(2);
        triangles[0][0] = 0;
        triangles[0][1] = 1;
        triangles[0][2] = 2;
        triangles[1][0] = 0;
        triangles[1][1] = 2;
        triangles[1][2] = 3;
    }


    Vec3 bottomLeft() const { return this->vertices[0].position; }
    Vec3 bottomRight() const { return this->vertices[1].position; }
    Vec3 upRight() const { return this->vertices[2].position; }
    Vec3 upLeft() const { return this->vertices[3].position; }
    Vec3 normal() const { return Vec3::cross((bottomRight() - bottomLeft()) , (upLeft() - bottomLeft())); }
 

    RaySquareIntersection intersect(const Ray &ray) const {
        RaySquareIntersection intersection;
        float t;
        float denom = Vec3::dot(ray.direction(), normal());
        if (abs(denom) > 0.00001) {  
            //float d = Vec3::dot(bottomLeft(), normal());
            float num = Vec3::dot(normal(),bottomLeft() - ray.origin());
            t = num / denom;
            if (t > 0) {

                Vec3 pointInter = ray.origin() + t * ray.direction();
                intersection.intersectionExists = true;
                intersection.t = t;
                intersection.intersection = pointInter;
                intersection.normal = normal();
                intersection.normal.normalize();
            }
            else{
                intersection.intersectionExists = false;
            }
        }
        else {
            intersection.intersectionExists = false;
        }

        return intersection;
    }


    /* RaySquareIntersection intersect(const Ray &ray) const
    {
        RaySquareIntersection intersection;
        if (Vec3::dot(normal(), ray.direction()) == 0)
        {
            intersection.intersectionExists = false;
        }
        else
        {
            // TODO calculer l'intersection rayon quad

            float t = Vec3::dot(normal(),bottomLeft() - ray.origin()) / Vec3::dot(normal(), ray.direction());

            Vec3 pt = ray.origin() + t * ray.direction();

            if (t < 0)
            {
                intersection.intersectionExists = false;
            }
            else
            {

                Vec3 vectA_B=upLeft()-bottomLeft(); //axe Y du carré
                float normeA_ptProjAB=Vec3::dot(vectA_B,pt-bottomLeft())/sqrt(vectA_B.squareLength());

                Vec3 vectA_C=bottomRight()-bottomLeft(); //axe X du carré
                float normeA_ptProjAC=Vec3::dot(vectA_C,pt-bottomLeft())/sqrt(vectA_C.squareLength());

                if (normeA_ptProjAB<=vectA_B.norm() &&normeA_ptProjAB>=0  && normeA_ptProjAC<=vectA_C.norm() && normeA_ptProjAC>=0 )
                {
                    intersection.intersectionExists = true;
                    intersection.t = t;
                    intersection.normal=normal();
                    //intersection.normal.normalize();
                    intersection.intersection=pt;
                }
                else
                {
                    intersection.intersectionExists = false;
                }
            }
        }

        return intersection;
    } */

   

};
#endif // SQUARE_H
