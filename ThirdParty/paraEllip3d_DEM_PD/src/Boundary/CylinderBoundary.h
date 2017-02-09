#ifndef CYLINDER_BOUNDARY_H
#define CYLINDER_BOUNDARY_H

#include <Boundary/Boundary.h>
#include <Boundary/BoundaryTangent.h>
#include <Boundary/Containers.h>
#include <Core/Geometry/Cylinder.h>
#include <Core/Math/Vec.h>
#include <Core/Types/realtypes.h>
#include <boost/serialization/base_object.hpp>
#include <boost/serialization/vector.hpp>
#include <iostream>

namespace dem {

class Particle; // forward declaration, only use pointer to class Particle

///////////////////////////////////////
class CylinderBoundary : public Boundary
{
private:
  Vec direc;
  Vec point;
  Vec prevPoint;
  Vec veloc;
  Vec prevVeloc;
  REAL radius;

private:
  friend class boost::serialization::access;
  template <class Archive>
  void serialize(Archive& ar, const unsigned int version)
  {
    ar& boost::serialization::base_object<Boundary>(*this);
    ar& direc;
    ar& point;
    ar& radius;
  }

public:
  CylinderBoundary()
    : Boundary()
    , direc(0)
    , point(0)
    , prevPoint(0)
    , veloc(0)
    , prevVeloc(0)
    , radius(0)
  {
  }

  CylinderBoundary(std::size_t type, std::ifstream& ifs);

  Vec getDirec() const { return direc; }
  Vec getPoint() const { return point; }
  Vec getVeloc() const { return veloc; }
  Vec getPrevPoint() const { return prevPoint; }
  Vec getPrevVeloc() const { return prevVeloc; }
  REAL getRadius() const { return radius; }

  void setDirec(Vec dir) { direc = dir; }
  void setPoint(Vec pnt) { point = pnt; }
  void setVeloc(Vec vel) { veloc = vel; }

  void print(std::ostream& os)
  {
    Boundary::print(os);
    os << std::setw(OWID) << direc.getX() << std::setw(OWID) << direc.getY()
       << std::setw(OWID) << direc.getZ() << std::setw(OWID) << point.getX()
       << std::setw(OWID) << point.getY() << std::setw(OWID) << point.getZ()
       << std::setw(OWID) << radius << std::endl
       << std::endl;
  }

  void findBdryContact(ParticlePArray& ptcls);
  void boundaryForce(BoundaryTangentArrayMap& boundaryTgtMap);
};

} // namespace dem ends

#endif