#ifndef EMU2DC_BODY_H
#define EMU2DC_BODY_H

#include <Material.h>
#include <MaterialSPArray.h>
#include <NodePArray.h>
#include <ElementPArray.h>
#include <Core/ProblemSpec/ProblemSpecP.h>

#include <string>
#include <iostream>
#include <map>

namespace Emu2DC {

  class Body
  {
  public:  

    friend std::ostream& operator<<(std::ostream& out, const Emu2DC::Body& body);

  public:
   
    Body();
    virtual ~Body();

    void initialize(Uintah::ProblemSpecP& ps,
                    const MaterialSPArray& matList);

    inline int id() const {return d_id;}
    inline void id(const int& id) {d_id = id;}

    // **WARNING** One mat for now.  A body can have more than one material. Also the
    // materials can be PerMaterial, MPMMaterial, or RigidMaterial.
    inline int matID() const {return d_mat_id;}
    const NodePArray& nodes() const {return d_nodes;}
    const ElementPArray& elements() const {return d_elements;}

  protected:

    void readNodeFile(const std::string& fileName);
    void readElementFile(const std::string& fileName);

  private:

    int d_id;
    int d_mat_id;
    NodePArray d_nodes;
    ElementPArray d_elements;

    typedef std::map<int, NodeP> NodeIDMap;
    NodeIDMap d_id_ptr_map;

  };
} // end namespace

#endif
