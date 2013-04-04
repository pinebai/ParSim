#ifndef EMU2DC_ELEMENT
#define EMU2DC_ELEMENT

#include <Node.h>

namespace Emu2DC {
    

  //   Node position on an element
  //    _______
  //  1|       |2 
  //   |       |
  //   |       |
  //  4|_______|3
  //      
  class Element {
      
    public:

      Element();
      ~Element();

      // This function is created to assign one element to another. 
      void Element_attribution(const Element* element);

      void computeGeometry(double& area, double& xlength, double& ylength) const;

    protected:

      int d_id;

      std::vector<Node*> d_elementnodes; // Array
    
      Node* d_node1;
      Node* d_node2;
      Node* d_node3;
      Node* d_node4;

      int d_depth;
      bool d_leath;    // tell us if this element is a leaf of the quadtree stucture
      bool d_root;    
      bool d_dif_level_refine; // flag that tell us if the element should be refined because 
                                     // a dif. level of refinment >2 situation happened
      bool d_strain_energy_refine;

      Element* d_child1;     // we can improve this by changing this pointer to point to a id number 
                           // instead of a data structure  
      Element* d_child2;     // this id number is from the global array of elements
      Element* d_child3;
      Element* d_child4;

      Element* d_father;
      Element* d_quad_element;

      int d_n_neighbors;
      int* d_neighborhood; // Array

    private:

      // Prevent copy construction and operator=
      Element(const Element& element);
      Element& operator=(const Element& element);

  };

} // end namespace

#endif

