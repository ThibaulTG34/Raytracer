#ifndef TRIANGLE_H
#define TRIANGLE_H
#include "Vec3.h"
#include "Ray.h"
#include "Plane.h"

struct RayTriangleIntersection
{
    bool intersectionExists;
    float t;
    float w0, w1, w2;
    unsigned int tIndex;
    Vec3 intersection;
    Vec3 normal;
};

class Triangle
{
private:
    Vec3 m_c[3], m_normal;
    float area;

public:
    Triangle() {}
    Triangle(Vec3 const &c0, Vec3 const &c1, Vec3 const &c2)
    {
        m_c[0] = c0;
        m_c[1] = c1;
        m_c[2] = c2;
        updateAreaAndNormal();
    }
    void updateAreaAndNormal()
    {
        Vec3 nNotNormalized = Vec3::cross(m_c[1] - m_c[0], m_c[2] - m_c[0]);
        float norm = nNotNormalized.length();
        m_normal = nNotNormalized / norm;
        area = norm / 2.f;
    }
    void setC0(Vec3 const &c0) { m_c[0] = c0; } // remember to update the area and normal afterwards!
    void setC1(Vec3 const &c1) { m_c[1] = c1; } // remember to update the area and normal afterwards!
    void setC2(Vec3 const &c2) { m_c[2] = c2; } // remember to update the area and normal afterwards!
    Vec3 const &normal() const { return m_normal; }

    Vec3 projectOnSupportPlane(Vec3 const &p) const
    {
        Vec3 result;
        // TODO completer
        Vec3 vect = m_c[1] - p;
        float scal = Vec3::dot(vect, normal()) / normal().norm();
        result = p - scal * normal();
        return result;
    }
    float squareDistanceToSupportPlane(Vec3 const &p) const
    {
        float result;
        // TODO completer
        result = projectOnSupportPlane(p).squareLength();
        return result;
    }
    float distanceToSupportPlane(Vec3 const &p) const { return sqrt(squareDistanceToSupportPlane(p)); }

    bool isParallelTo(Line const &L) const
    {
        bool result = false;
        // TODO completer
        float d = Vec3::dot(normal(), L.direction());
        if (fabs(d) < 0.00001f)
        {
            result = true;
        }
        return result;
    }
    Vec3 getIntersectionPointWithSupportPlane(Line const &L) const
    {
        // you should check first that the line is not parallel to the plane!
        Vec3 result;
        // TODO completer
        if (isParallelTo(L))
        {
            return result;
        }

        return result;
    }

    void computeBarycentricCoordinates(Vec3 const &p, float &u0, float &u1, float &u2) const
    {
        // TODO Complete
    }

    RayTriangleIntersection getIntersection(Ray const &ray) const
    {
        RayTriangleIntersection result;
        // 1) check that the ray is not parallel to the triangle:
        if (isParallelTo(ray))
        {
            result.intersectionExists = false;
            return result;
        }

        // 2) check that the triangle is "in front of" the ray:
        float NdotDir = Vec3::dot(normal(), ray.direction());
        float d = -Vec3::dot(normal(), m_c[0]);
        float t = -(Vec3::dot(normal(), ray.origin()) + d) / NdotDir;
        if (t < 0)
        {
            result.intersectionExists = false;
            return result;
        }

        // 3) check that the intersection point is inside the triangle:
        // CONVENTION: compute u,v such that p = w0*c0 + w1*c1 + w2*c2, check that 0 <= w0,w1,w2 <= 1
        Vec3 intersection = ray.origin() + t * ray.direction();

        Vec3 c1c0 = m_c[1] - m_c[0];
        Vec3 vect = intersection - m_c[0];
        Vec3 cross = Vec3::cross(c1c0, vect);
        if (Vec3::dot(normal(), cross) < 0)
        {
            result.intersectionExists = false;
            return result;
        }

        Vec3 c2c1 = m_c[2] - m_c[1];
        vect = intersection - m_c[1];
        cross = Vec3::cross(c2c1, vect);
        if (Vec3::dot(normal(), cross) < 0)
        {
            result.intersectionExists = false;
            return result;
        }

        Vec3 c0c2 = m_c[0] - m_c[2];
        vect = intersection - m_c[2];
        cross = Vec3::cross(c0c2, vect);
        if (Vec3::dot(normal(), cross) < 0)
        {
            result.intersectionExists = false;
            return result;
        }


        // 4) Finally, if all conditions were met, then there is an intersection! :
        result.t = t;
        result.intersection = intersection;
        result.intersectionExists = true;
        result.normal = normal();

        return result;
    }
};
#endif
