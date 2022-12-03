#ifndef SCENE_H
#define SCENE_H

#include <vector>
#include <string>
#include "Mesh.h"
#include "Sphere.h"
#include "Square.h"

#include <GL/glut.h>

enum LightType
{
    LightType_Spherical,
    LightType_Quad
};

struct Light
{
    Vec3 material;
    bool isInCamSpace;
    LightType type;

    Vec3 pos;
    float radius;

    Mesh quad;

    float powerCorrection;

    Light() : powerCorrection(1.0) {}
};

struct RaySceneIntersection
{
    bool intersectionExists;
    unsigned int typeOfIntersectedObject;
    unsigned int objectIndex;
    float t;
    RayTriangleIntersection rayMeshIntersection;
    RaySphereIntersection raySphereIntersection;
    RaySquareIntersection raySquareIntersection;
    RaySceneIntersection() : intersectionExists(false), t(FLT_MAX) {}
};

class Scene
{
    std::vector<Mesh> meshes;
    std::vector<Sphere> spheres;
    std::vector<Square> squares;
    std::vector<Light> lights;

public:
    Scene()
    {
    }

    void draw()
    {
        // iterer sur l'ensemble des objets, et faire leur rendu :
        for (unsigned int It = 0; It < meshes.size(); ++It)
        {
            Mesh const &mesh = meshes[It];
            mesh.draw();
        }
        for (unsigned int It = 0; It < spheres.size(); ++It)
        {
            Sphere const &sphere = spheres[It];
            sphere.draw();
        }
        for (unsigned int It = 0; It < squares.size(); ++It)
        {
            Square const &square = squares[It];
            square.draw();
        }
    }

    RaySceneIntersection computeIntersection(Ray const &ray)
    {
        RaySceneIntersection result;
        // TODO calculer les intersections avec les objets de la scene et garder la plus proche
        result.t = FLT_MAX;
        float z_near = 5.0;
        for (unsigned int i = 0; i < spheres.size(); i++)
        {
            RaySphereIntersection intersphere = spheres[i].intersect(ray);
            if (intersphere.intersectionExists && intersphere.t <= result.t)
            {
                result.raySphereIntersection = intersphere;
                result.intersectionExists = true;
                result.t = intersphere.t;
                result.objectIndex = i;
                result.typeOfIntersectedObject = 0;
            }
        }

        for (unsigned int j = 0; j < squares.size(); j++)
        {
            RaySquareIntersection intersquare = squares[j].intersect(ray);
            if (intersquare.intersectionExists && intersquare.t <= result.t && intersquare.t > z_near)
            {
                result.raySquareIntersection = intersquare;
                result.t = intersquare.t;
                result.intersectionExists = true;
                result.objectIndex = j;
                result.typeOfIntersectedObject = 1;
            }
        }

        z_near = 3.0;
        for (unsigned int k = 0; k < meshes.size(); k++)
        {
            RayTriangleIntersection intermesh = meshes[k].intersect(ray);
            if (intermesh.intersectionExists && intermesh.t <= result.t && intermesh.t > z_near)
            {
                result.rayMeshIntersection = intermesh;
                result.t = intermesh.t;
                result.intersectionExists = true;
                result.objectIndex = k;
                result.typeOfIntersectedObject = 2;
            }
        }

        return result;
    }

    /***
     * Here, a simple display
     */
    Vec3 rayTraceRecursive(Ray ray, int NRemainingBounces)
    {

        // TODO RaySceneIntersection raySceneIntersection = computeIntersection(ray);
        Vec3 color;
        RaySceneIntersection raySceneIntersection = computeIntersection(ray);
        if (raySceneIntersection.intersectionExists)
        {

            if (raySceneIntersection.typeOfIntersectedObject == 0)
            {
                color += spheres[raySceneIntersection.objectIndex].material.diffuse_material;
            }

            else if (raySceneIntersection.typeOfIntersectedObject == 1)
            {
                color += squares[raySceneIntersection.objectIndex].material.diffuse_material;
            }

            else if (raySceneIntersection.typeOfIntersectedObject == 2)
            {
                color += meshes[raySceneIntersection.objectIndex].material.diffuse_material;
            }
        }
        else
        {
            color += Vec3(0, 0, 0);
        }
        return color;
    }

    std::vector<Light> Arealight(int nbEchantillon)
    {

        std::vector<Light> point_light;
        point_light.resize(nbEchantillon);
        for (size_t i = 0; i < nbEchantillon; i++)
        {
            Light l;
            float val1 = ((float)(rand()) / (float)(RAND_MAX)) * 2;
            float val2 = ((float)(rand()) / (float)(RAND_MAX)) * 2;
            l.pos = lights[0].pos;
            l.pos[0] += val1; // offset x
            l.pos[2] += val2; // offset z
            point_light[i] = l;
            /*Light l;
            l.material = lights[0].material;
            int val1 = rand();
            int val2 = rand();
            float theta = 2 * M_PI * val1;
            float phi = cos(2 * val2 - 1);
            float x = lights[0].pos[0] + (lights[i].radius * sin(phi) * cos(theta));
            float y = lights[0].pos[1] + (lights[i].radius * sin(phi) * sin(theta));
            float z = lights[0].pos[2] + (lights[i].radius * cos(phi));
            // Light l;
            l.pos = Vec3(x, y, z);

            // std::cout<<i<<std::endl;
            */
        }

        return point_light;
    }

    Vec3 rayTraceRecursive1(Ray ray, int NRemainingBounces)
    {
        Vec3 color = Vec3(0, 0, 0);
        RaySceneIntersection raySceneIntersection = computeIntersection(ray);
        if (raySceneIntersection.intersectionExists)
        {
            if (raySceneIntersection.typeOfIntersectedObject == 0)
            {
                unsigned int index = raySceneIntersection.objectIndex;
                Vec3 normal_sphere = raySceneIntersection.raySphereIntersection.normal;
                Vec3 light_diffuse;
                Vec3 light_specular;
                Vec3 light_ambient;

                Vec3 lum = lights[0].pos - raySceneIntersection.raySphereIntersection.intersection;
                lum.normalize();
                Vec3 reflexion = (2 * std::max(0.f, Vec3::dot(normal_sphere, lum))) * normal_sphere - lum;
                reflexion.normalize();
                Vec3 view = (-1) * ray.direction();
                view.normalize();
                light_ambient = spheres[index].material.ambient_material;
                light_diffuse = Vec3::compProduct(spheres[index].material.diffuse_material * std::max(0.f, Vec3::dot(lum, normal_sphere)), lights[0].material);
                light_specular = Vec3::compProduct(spheres[index].material.specular_material * pow(std::max(0.f, Vec3::dot(reflexion, view)), spheres[index].material.shininess), lights[0].material);
                color += light_diffuse + light_specular + light_ambient;
            }
            else if (raySceneIntersection.typeOfIntersectedObject == 1)
            {
                unsigned int index = raySceneIntersection.objectIndex;
                Vec3 normal_square = raySceneIntersection.raySquareIntersection.normal;
                Vec3 light_diffuse;
                Vec3 light_specular;
                Vec3 light_ambient;
                Vec3 lum;
                lum = lights[0].pos - raySceneIntersection.raySquareIntersection.intersection;
                lum.normalize();
                Vec3 reflexion = (2 * std::max((float)0.0, Vec3::dot(normal_square, lum))) * normal_square - lum;
                reflexion.normalize();
                Vec3 view = (-1) * ray.direction();
                view.normalize();
                light_ambient += squares[index].material.ambient_material;
                light_diffuse += Vec3::compProduct(squares[index].material.diffuse_material * std::max((float)0.0, Vec3::dot(lum, normal_square)), lights[0].material);
                light_specular += Vec3::compProduct(squares[index].material.specular_material * pow(std::max((float)0.0, Vec3::dot(reflexion, view)), squares[index].material.shininess), lights[0].material);
                color += (light_diffuse + light_specular + light_ambient);

                for (unsigned int i = 0; i < lights.size(); i++)
                {

                    Ray new_ray = Ray(raySceneIntersection.raySquareIntersection.intersection, lum);
                    RaySceneIntersection new_raySceneIntersection = computeIntersection(new_ray);
                    if (new_raySceneIntersection.intersectionExists && new_raySceneIntersection.typeOfIntersectedObject == 0 && new_raySceneIntersection.t < lum.length())
                    {
                        return Vec3(0, 0, 0);
                    }
                }
            }

            else if (raySceneIntersection.typeOfIntersectedObject == 2)
            {
                unsigned int index = raySceneIntersection.objectIndex;
                Vec3 normal_mesh = raySceneIntersection.rayMeshIntersection.normal;
                Vec3 light_diffuse;
                Vec3 light_specular;
                Vec3 light_ambient;
                Vec3 lum;
                lum = lights[0].pos - raySceneIntersection.rayMeshIntersection.intersection;
                lum.normalize();
                Vec3 reflexion = (2 * std::max((float)0.0, Vec3::dot(normal_mesh, lum))) * normal_mesh - lum;
                reflexion.normalize();
                Vec3 view = (-1) * ray.direction();
                view.normalize();
                light_ambient += meshes[index].material.ambient_material;
                light_diffuse += Vec3::compProduct(meshes[index].material.diffuse_material * std::max((float)0.0, Vec3::dot(lum, normal_mesh)), lights[0].material);
                light_specular += Vec3::compProduct(meshes[index].material.specular_material * pow(std::max((float)0.0, Vec3::dot(reflexion, view)), meshes[index].material.shininess), lights[0].material);
                color += (light_diffuse + light_specular + light_ambient);

                for (unsigned int i = 0; i < lights.size(); i++)
                {
                    Ray new_ray = Ray(raySceneIntersection.rayMeshIntersection.intersection, lum);
                    RaySceneIntersection new_raySceneIntersection = computeIntersection(new_ray);
                    if (new_raySceneIntersection.intersectionExists && new_raySceneIntersection.typeOfIntersectedObject == 2 && new_raySceneIntersection.t < lum.length())
                    {
                        return Vec3(0, 0, 0);
                    }
                }
            }
        }

        return color;
    }

    Vec3 rayTraceRecursiveSoftShadow(Ray ray, int NRemainingBounces)
    {
        Vec3 color = Vec3(0, 0, 0);
        RaySceneIntersection raySceneIntersection = computeIntersection(ray);
        std::vector<Light> area_light;
        area_light = Arealight(20);
        unsigned int index = raySceneIntersection.objectIndex;

        if (raySceneIntersection.intersectionExists)
        {
            if (raySceneIntersection.typeOfIntersectedObject == 0)
            {
                Vec3 normal_sphere = raySceneIntersection.raySphereIntersection.normal;
                Vec3 light_diffuse;
                Vec3 light_specular;
                Vec3 light_ambient;
                Vec3 lum = lights[0].pos - raySceneIntersection.raySphereIntersection.intersection;
                lum.normalize();
                Vec3 reflexion = (2 * std::max(0.f, Vec3::dot(normal_sphere, lum))) * normal_sphere - lum;
                reflexion.normalize();
                Vec3 view = (-1) * ray.direction();
                view.normalize();
                light_ambient = spheres[index].material.ambient_material;
                light_diffuse = Vec3::compProduct(spheres[index].material.diffuse_material * std::max(0.f, Vec3::dot(lum, normal_sphere)), lights[0].material);
                light_specular = Vec3::compProduct(spheres[index].material.specular_material * pow(std::max(0.f, Vec3::dot(reflexion, view)), spheres[index].material.shininess), lights[0].material);
                color += light_diffuse + light_specular + light_ambient;
            }

            else if (raySceneIntersection.typeOfIntersectedObject == 1)
            {
                //unsigned int index = raySceneIntersection.objectIndex;
                Vec3 normal_square = raySceneIntersection.raySquareIntersection.normal;
                Vec3 light_diffuse;
                Vec3 light_specular;
                Vec3 light_ambient;
                Vec3 lum;
                lum = lights[0].pos - raySceneIntersection.raySquareIntersection.intersection;
                lum.normalize();
                Vec3 reflexion = (2 * std::max((float)0.0, Vec3::dot(normal_square, lum))) * normal_square - lum;
                reflexion.normalize();
                Vec3 view = (-1) * ray.direction();
                view.normalize();
                light_ambient += squares[index].material.ambient_material;
                light_diffuse += Vec3::compProduct(squares[index].material.diffuse_material * std::max((float)0.0, Vec3::dot(lum, normal_square)), lights[0].material);
                light_specular += Vec3::compProduct(squares[index].material.specular_material * pow(std::max((float)0.0, Vec3::dot(reflexion, view)), squares[index].material.shininess), lights[0].material);
                color += (light_diffuse + light_specular + light_ambient);

                float v = 0.f;
                for (unsigned int i = 0; i < area_light.size(); i++)
                {
                    lum = area_light[i].pos - raySceneIntersection.raySquareIntersection.intersection;
                    lum.normalize();
                    // Vec3 intersection_point = ray.origin() + raySceneIntersection.raySquareIntersection.t * ray.direction();
                    Ray new_ray = Ray(raySceneIntersection.raySquareIntersection.intersection, lum);
                    RaySceneIntersection new_raySceneIntersection = computeIntersection(new_ray);
                    if (new_raySceneIntersection.intersectionExists && new_raySceneIntersection.typeOfIntersectedObject == 0 && new_raySceneIntersection.t < lum.length())
                    {
                        // v++;
                        color += Vec3(0, 0, 0);
                    }
                    else
                    {
                        v++;
                    }
                }
                v /= area_light.size();
                // v = 1 - v;
                color = v * color;
            }

            else if (raySceneIntersection.typeOfIntersectedObject == 2)
            {
                //unsigned int index = raySceneIntersection.objectIndex;
                Vec3 normal_square = raySceneIntersection.rayMeshIntersection.normal;
                Vec3 light_diffuse;
                Vec3 light_specular;
                Vec3 light_ambient;
                Vec3 lum;
                lum = lights[0].pos - raySceneIntersection.rayMeshIntersection.intersection;
                lum.normalize();
                Vec3 reflexion = (2 * std::max((float)0.0, Vec3::dot(normal_square, lum))) * normal_square - lum;
                reflexion.normalize();
                Vec3 view = (-1) * ray.direction();
                view.normalize();
                light_ambient += meshes[index].material.ambient_material;
                light_diffuse += Vec3::compProduct(meshes[index].material.diffuse_material * std::max((float)0.0, Vec3::dot(lum, normal_square)), lights[0].material);
                light_specular += Vec3::compProduct(meshes[index].material.specular_material * pow(std::max((float)0.0, Vec3::dot(reflexion, view)), meshes[index].material.shininess), lights[0].material);
                color += (light_diffuse + light_specular + light_ambient);

                float v = 0.f;
                for (unsigned int i = 0; i < area_light.size(); i++)
                {
                    lum = area_light[i].pos - raySceneIntersection.rayMeshIntersection.intersection;
                    lum.normalize();
                    // Vec3 intersection_point = ray.origin() + raySceneIntersection.raySquareIntersection.t * ray.direction();
                    Ray new_ray = Ray(raySceneIntersection.rayMeshIntersection.intersection, lum);
                    RaySceneIntersection new_raySceneIntersection = computeIntersection(new_ray);
                    if (new_raySceneIntersection.intersectionExists && new_raySceneIntersection.typeOfIntersectedObject == 0 && new_raySceneIntersection.t < lum.length())
                    {
                        // v++;
                        color += Vec3(0, 0, 0);
                    }
                    else
                    {
                        v++;
                    }
                }
                v /= area_light.size();
                // v = 1 - v;
                color = v * color;
            }

            if (NRemainingBounces > 0 && raySceneIntersection.typeOfIntersectedObject == 0 && spheres[index].material.type == Material_Mirror){
                color += rayTraceRecursiveSoftShadow(Reflection(ray, raySceneIntersection.raySphereIntersection.normal, raySceneIntersection.raySphereIntersection.intersection),NRemainingBounces - 1);
            }
        }

        return color;
    }

    Ray Reflection(Ray ray, Vec3 normal, Vec3 intersection)
    {
        ray.direction().normalize();
        Vec3 dir = ray.direction() - (2.0 * (Vec3::dot(ray.direction(), normal) * normal));
        Vec3 orig = intersection;

        return Ray(orig, dir);
    }

    Vec3 rayTrace(Ray const &rayStart)
    {
        // TODO appeler la fonction recursive
        Vec3 color;
        color = rayTraceRecursive1(rayStart, 3);
        return color;
    }

    void setup_single_sphere()
    {
        meshes.clear();
        spheres.clear();
        squares.clear();
        lights.clear();

        {
            lights.resize(lights.size() + 1);
            Light &light = lights[lights.size() - 1];
            light.pos = Vec3(-5, 5, 5);
            light.radius = 2.5f;
            light.powerCorrection = 2.f;
            light.type = LightType_Spherical;
            light.material = Vec3(1, 1, 1);
            light.isInCamSpace = false;
        }
        {
            spheres.resize(spheres.size() + 1);
            Sphere &s = spheres[spheres.size() - 1];
            s.m_center = Vec3(0., 0., 0.);
            s.m_radius = 1.f;
            s.build_arrays();
            s.material.type = Material_Mirror;
            s.material.diffuse_material = Vec3(1., 0., 1.);
            s.material.specular_material = Vec3(0.2, 0.2, 0.2);
            s.material.shininess = 20;
        }
    }

    void setup_single_square()
    {
        meshes.clear();
        spheres.clear();
        squares.clear();
        lights.clear();

        {
            lights.resize(lights.size() + 1);
            Light &light = lights[lights.size() - 1];
            light.pos = Vec3(-5, 5, 5);
            light.radius = 2.5f;
            light.powerCorrection = 2.f;
            light.type = LightType_Spherical;
            light.material = Vec3(1, 1, 1);
            light.isInCamSpace = false;
        }

        {
            squares.resize(squares.size() + 1);
            Square &s = squares[squares.size() - 1];
            s.setQuad(Vec3(-1., -1., 0.), Vec3(1., 0, 0.), Vec3(0., 1, 0.), 2., 2.);
            s.build_arrays();
            s.material.diffuse_material = Vec3(0.8, 0.8, 0.8);
            s.material.specular_material = Vec3(0.8, 0.8, 0.8);
            s.material.shininess = 20;
        }
    }

    void setup_cornell_box()
    {
        meshes.clear();
        spheres.clear();
        squares.clear();
        lights.clear();

        {
            lights.resize(lights.size() + 1);
            Light &light = lights[lights.size() - 1];
            light.pos = Vec3(0.0, 1.5f, 0);
            light.radius = 2.5f;
            light.powerCorrection = 2.f;
            light.type = LightType_Spherical;
            light.material = Vec3(1, 1, 1);
            light.isInCamSpace = false;
        }

        { // Back Wall
            squares.resize(squares.size() + 1);
            Square &s = squares[squares.size() - 1];
            s.setQuad(Vec3(-1., -1., 0.), Vec3(1., 0, 0.), Vec3(0., 1, 0.), 2., 2.);
            s.scale(Vec3(2., 2., 1.));
            s.translate(Vec3(0., 0., -2.));
            s.build_arrays();
            s.material.diffuse_material = Vec3(0., 0., 1.);
            s.material.specular_material = Vec3(1., 1., 1.);
            s.material.shininess = 16;
        }

        { // Left Wall

            squares.resize(squares.size() + 1);
            Square &s = squares[squares.size() - 1];
            s.setQuad(Vec3(-1., -1., 0.), Vec3(1., 0, 0.), Vec3(0., 1, 0.), 2., 2.);
            s.scale(Vec3(2., 2., 1.));
            s.translate(Vec3(0., 0., -2.));
            s.rotate_y(90);
            s.build_arrays();
            s.material.diffuse_material = Vec3(1., 0., 0.);
            s.material.specular_material = Vec3(1., 0., 0.);
            s.material.shininess = 16;
        }

        { // Right Wall
            squares.resize(squares.size() + 1);
            Square &s = squares[squares.size() - 1];
            s.setQuad(Vec3(-1., -1., 0.), Vec3(1., 0, 0.), Vec3(0., 1, 0.), 2., 2.);
            s.translate(Vec3(0., 0., -2.));
            s.scale(Vec3(2., 2., 1.));
            s.rotate_y(-90);
            s.build_arrays();
            s.material.diffuse_material = Vec3(0., 1., 0.4);
            s.material.specular_material = Vec3(0.0, 1.0, 0.0);
            s.material.shininess = 16;
        }

        { // Floor
            squares.resize(squares.size() + 1);
            Square &s = squares[squares.size() - 1];
            s.setQuad(Vec3(-1., -1., 0.), Vec3(1., 0, 0.), Vec3(0., 1, 0.), 2., 2.);
            s.translate(Vec3(0., 0., -2.));
            s.scale(Vec3(2., 2., 1.));
            s.rotate_x(-90);
            s.build_arrays();
            s.material.diffuse_material = Vec3(1.0, 0.3, 0.0);
            s.material.specular_material = Vec3(0.0, 0.0, 0.0);
            s.material.shininess = 16;
        }

        { // Ceiling
            squares.resize(squares.size() + 1);
            Square &s = squares[squares.size() - 1];
            s.setQuad(Vec3(-1., -1., 0.), Vec3(1., 0, 0.), Vec3(0., 1, 0.), 2., 2.);
            s.translate(Vec3(0., 0., -2.));
            s.scale(Vec3(2., 2., 1.));
            s.rotate_x(90);
            s.build_arrays();
            s.material.diffuse_material = Vec3(1., 0.8, 1.);
            s.material.specular_material = Vec3(1., 1., 1.);
            s.material.shininess = 16;
        }

        { // Front Wall
            squares.resize(squares.size() + 1);
            Square &s = squares[squares.size() - 1];
            s.setQuad(Vec3(-1., -1., 0.), Vec3(1., 0, 0.), Vec3(0., 1, 0.), 2., 2.);
            s.translate(Vec3(0., 0., -2.));
            s.scale(Vec3(2., 2., 1.));
            s.rotate_y(180);
            s.build_arrays();
            s.material.diffuse_material = Vec3(0.2, 1.0, 0.6);
            s.material.specular_material = Vec3(1.0, 1.0, 1.0);
            s.material.shininess = 16;
        }

        { // GLASS Sphere

            spheres.resize(spheres.size() + 1);
            Sphere &s = spheres[spheres.size() - 1];
            s.m_center = Vec3(1.0, -1.25, 0.5);
            s.m_radius = 0.75f;
            s.build_arrays();
            s.material.type = Material_Mirror;
            s.material.diffuse_material = Vec3(1., 0., 0.);
            s.material.specular_material = Vec3(1., 0., 0.);
            s.material.shininess = 16;
            s.material.transparency = 1.0;
            s.material.index_medium = 1.4;
        }

        { // MIRRORED Sphere
            spheres.resize(spheres.size() + 1);
            Sphere &s = spheres[spheres.size() - 1];
            s.m_center = Vec3(-1.0, -1.25, -0.5);
            s.m_radius = 0.75f;
            s.build_arrays();
            s.material.type = Material_Glass;
            s.material.diffuse_material = Vec3(0., 1., 1.);
            s.material.specular_material = Vec3(1., 1., 1.);
            s.material.shininess = 16;
            s.material.transparency = 0.;
            s.material.index_medium = 0.;
        }
        { // MESH
            meshes.resize(spheres.size() + 1);
            Mesh &s = meshes[meshes.size() - 1];
            s.loadOFF("./data/epcot.off");
            s.scale(Vec3(0.5, 0.5, 0.5));
            s.translate(Vec3(-0.5, -1.5, +0.5));
            s.build_arrays();
            // s.material.type = Material_Mirror;
            s.material.diffuse_material = Vec3(1., 0., 1.);
            s.material.specular_material = Vec3(0.2, 0.2, 0.2);
            s.material.shininess = 20;
        }
    }

    void setup_mesh()
    {
        meshes.clear();
        spheres.clear();
        squares.clear();
        lights.clear();

        {
            lights.resize(lights.size() + 1);
            Light &light = lights[lights.size() - 1];
            light.pos = Vec3(-5, 5, 5);
            light.radius = 2.5f;
            light.powerCorrection = 2.f;
            light.type = LightType_Spherical;
            light.material = Vec3(1, 1, 1);
            light.isInCamSpace = false;
        }
        {
            meshes.resize(spheres.size() + 1);
            Mesh &s = meshes[meshes.size() - 1];
            s.loadOFF("./data/epcot.off");
            s.build_arrays();
            // s.material.type = Material_Mirror;
            s.material.diffuse_material = Vec3(1., 0., 1.);
            s.material.specular_material = Vec3(0.2, 0.2, 0.2);
            s.material.shininess = 20;
        }
    }
};

#endif
