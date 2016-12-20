/*
 * The MIT License
 *
 * Copyright (c) 2013-2014 Callaghan Innovation, New Zealand
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to
 * deal in the Software without restriction, including without limitation the
 * rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
 * sell copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
 * IN THE SOFTWARE.
 */

/*
 * The MIT License
 *
 * Copyright (c) 1997-2012 The University of Utah
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to
 * deal in the Software without restriction, including without limitation the
 * rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
 * sell copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
 * IN THE SOFTWARE.
 */
//  
//    File   : PrismLinearLgn.h
//    Author : Martin Cole, Frank B. Sachse
//    Date   : Dec 04 2004

#if !defined(PrismLinearLgn_h)
#define PrismLinearLgn_h

#include <Core/Basis/CrvLinearLgn.h>
#include <Core/Basis/TriLinearLgn.h>
#include <Core/Basis/QuadBilinearLgn.h>

#include <Core/Basis/share.h>

namespace Uintah {

//! Class for describing unit geometry of PrismLinearLgn 
class PrismLinearLgnUnitElement {
public:
  //! Parametric coordinates of vertices of unit edge
  static SCISHARE double unit_vertices[6][3];
  //! References to vertices of unit edge 
  static SCISHARE int unit_edges[9][2]; 
  //! References to vertices of unit face
  static SCISHARE int unit_faces[5][4]; 
  //! Normals of unit face
  static SCISHARE double unit_face_normals[5][3];
  //! Precalculated area of faces
  static SCISHARE double unit_face_areas[5];

  PrismLinearLgnUnitElement() {};
  virtual ~PrismLinearLgnUnitElement() {}

  //! return dimension of domain 
  static int domain_dimension() { return 3; } 
  //! return number of vertices
  static int number_of_vertices() { return 6; } 
  //! return number of vertices in mesh
  static int number_of_mesh_vertices() { return 6; }
  //! return number of edges 
  static int number_of_edges() { return 9; } 
  //! return degrees of freedom
  static int dofs() { return 6; } 
  //! return number of vertices per face 
  static int vertices_of_face() { return 3; } 
  //! return number of faces per cell 
  static int faces_of_cell() { return 5; } 

  static inline double length(int edge) { //!< return length
    const double *v0 = unit_vertices[unit_edges[edge][0]];
    const double *v1 = unit_vertices[unit_edges[edge][1]];
    const double dx = v1[0] - v0[0];
    const double dy = v1[1] - v0[1];
    const double dz = v1[2] - v0[2];
    return sqrt(dx*dx+dy*dy+dz*dz);
  } 
  static double area(int face) { return unit_face_areas[face]; } //!< return area
  static double volume() { return .5; } //!< return volume
};

//! Class for creating geometrical approximations of Prism meshes
class PrismApprox {  
public:

  PrismApprox() {}
  virtual ~PrismApprox() {}
  
  //! Approximate edge for element by piecewise linear segments
  //! return: coords gives parametric coordinates of the approximation.
  //! Use interpolate with coordinates to get the world coordinates.
  virtual void approx_edge(const unsigned edge,
			   const unsigned div_per_unit, 
			   std::vector<std::vector<double> > &coords) const
  {
    coords.resize(div_per_unit + 1);

    const double *v0 = PrismLinearLgnUnitElement::unit_vertices[PrismLinearLgnUnitElement::unit_edges[edge][0]];
    const double *v1 = PrismLinearLgnUnitElement::unit_vertices[PrismLinearLgnUnitElement::unit_edges[edge][1]];

    const double &p1x = v0[0];
    const double &p1y = v0[1];
    const double &p1z = v0[2];
    const double dx = v1[0] - p1x;
    const double dy = v1[1] - p1y;
    const double dz = v1[2] - p1z;

    for(unsigned int i = 0; i <= div_per_unit; i++) {
      const double d = (double)i / (double)div_per_unit;
      std::vector<double> &tmp = coords[i];
      tmp.resize(3);
      tmp[0] = p1x + d * dx;
      tmp[1] = p1y + d * dy;
      tmp[2] = p1z + d * dz;
    } 	
  }
    
  //! Approximate faces for element by piecewise linear elements
  //! return: coords gives parametric coordinates at the approximation point.
  //! Use interpolate with coordinates to get the world coordinates.
  virtual void approx_face(const unsigned face,
			   const unsigned div_per_unit, 
			   std::vector<std::vector<std::vector<double> > > &coords) const
  {	
    unsigned int fe = (PrismLinearLgnUnitElement::unit_faces[face][3] == -1 ? 1 : 2);
    coords.resize(fe * div_per_unit);
	
    for(unsigned int f = 0; f<fe; f++) {
      double *v0, *v1, *v2;

      if (f==0) {
	v0 = PrismLinearLgnUnitElement::unit_vertices[PrismLinearLgnUnitElement::unit_faces[face][0]];
	v1 = PrismLinearLgnUnitElement::unit_vertices[PrismLinearLgnUnitElement::unit_faces[face][1]];
	v2 = PrismLinearLgnUnitElement::unit_vertices[PrismLinearLgnUnitElement::unit_faces[face][2]];
      } else {
	v0 = PrismLinearLgnUnitElement::unit_vertices[PrismLinearLgnUnitElement::unit_faces[face][2]];
	v1 = PrismLinearLgnUnitElement::unit_vertices[PrismLinearLgnUnitElement::unit_faces[face][3]];
	v2 = PrismLinearLgnUnitElement::unit_vertices[PrismLinearLgnUnitElement::unit_faces[face][0]];
      }

      const double d = 1. / div_per_unit;
      for(unsigned int j = 0; j < div_per_unit; j++) {
	unsigned int k = j + f * div_per_unit;
	const double dj = (double)j / (double)div_per_unit;
	unsigned int e = 0;
	coords[k].resize((div_per_unit - j) * 2 + 1);
	std::vector<double> &tmp = coords[k][e++];
	tmp.resize(3);
	tmp.resize(3);
 	tmp[0] = v0[0] + dj * (v2[0] - v0[0]);
	tmp[1] = v0[1] + dj * (v2[1] - v0[1]);
	tmp[2] = v0[2] + dj * (v2[2] - v0[2]);

	for(unsigned int i = 0; i < div_per_unit - j; i++) {
	  const double di =  (double)i / (double)div_per_unit;
	  std::vector<double> &tmp1 = coords[j + f * div_per_unit][e++]; 
	  tmp1.resize(3);
	  tmp1[0] = v0[0] + (dj + d) * (v2[0] - v0[0]) + di * (v1[0] - v0[0]);
	  tmp1[1] = v0[1] + (dj + d) * (v2[1] - v0[1]) + di * (v1[1] - v0[1]);
	  tmp1[2] = v0[2] + (dj + d) * (v2[2] - v0[2]) + di * (v1[2] - v0[2]);
	  std::vector<double> &tmp2 = coords[j + f * div_per_unit][e++];
	  tmp2.resize(3);
	  tmp2[0] = v0[0] + dj * (v2[0] - v0[0]) + (di + d) * (v1[0] - v0[0]);
	  tmp2[1] = v0[1] + dj * (v2[1] - v0[1]) + (di + d) * (v1[1] - v0[1]);
	  tmp2[2] = v0[2] + dj * (v2[2] - v0[2]) + (di + d) * (v1[2] - v0[2]);
	}
      }
    }
  }
};

//! Class for searching of parametric coordinates related to a value in Prism meshes and fields
//! to do
template <class ElemBasis>
class PrismLocate : public Dim3Locate<ElemBasis> {
public:
  typedef typename ElemBasis::value_type T;
 
  PrismLocate() {}
  virtual ~PrismLocate() {}
 
  template <class ElemData>
  bool get_coords(const ElemBasis *pEB, std::vector<double> &coords, 
		  const T& value, const ElemData &cd) const  
  {      
    initial_guess(pEB, value, cd, coords);
    if (get_iterative(pEB, coords, value, cd))
      return check_coords(coords);
    return false;
  }

protected:
  inline bool check_coords(const std::vector<double> &x) const  
  {  
    if (x[0]>=-Dim3Locate<ElemBasis>::thresholdDist)
      if (x[1]>=-Dim3Locate<ElemBasis>::thresholdDist)
	if (x[2]>=-Dim3Locate<ElemBasis>::thresholdDist && 
	    x[2]<=Dim3Locate<ElemBasis>::thresholdDist1) 
	  if (x[0]+x[1]<=Dim3Locate<ElemBasis>::thresholdDist1)
	    return true;

    return false;
  };
  
  //! find a reasonable initial guess 
  template <class ElemData>
  void initial_guess(const ElemBasis *pElem, const T &val, const ElemData &cd, 
		     std::vector<double> & guess) const
  {
    double dist = DBL_MAX;
	
    std::vector<double> coord(3);
    std::vector<T> derivs(3);
    guess.resize(3);

    const int end = 3;
    for (int x = 1; x < end; x++) {
      coord[0] = x / (double) end;
      for (int y = 1; y < end; y++) {
	coord[1] = y / (double) end;
	if (coord[0]+coord[1]>Dim3Locate<ElemBasis>::thresholdDist1)
	  break;
	for (int z = 1; z < end; z++) {
	  coord[2] = z / (double) end;
	  double cur_d;
	  if (compare_distance(pElem->interpolate(coord, cd), 
			       val, cur_d, dist)) {
	    pElem->derivate(coord, cd, derivs);
	    if (!check_zero(derivs)) {
	      dist = cur_d;
	      guess = coord;
	    }
	  }
	}
      }
    }
  } 
};

//! Class with weights and coordinates for 2nd order Gaussian integration
template <class T>
class PrismGaussian1 
{
public:
  static int GaussianNum;
  static T GaussianPoints[1][3];
  static T GaussianWeights[1];
};

#ifdef _WIN32
// force the instantiation of PrismGaussian1<double>
template class PrismGaussian1<double>;
#endif

template <class T>
int PrismGaussian1<T>::GaussianNum = 1;

template <class T>
T PrismGaussian1<T>::GaussianPoints[1][3] = {
  {1./3.,1./3., 0.5}};

template <class T>
T PrismGaussian1<T>::GaussianWeights[1] = 
  {1.0};

//! Class with weights and coordinates for 2nd order Gaussian integration
template <class T>
class PrismGaussian2 
{
public:
  static int GaussianNum;
  static T GaussianPoints[6][3];
  static T GaussianWeights[6];
};

#ifdef _WIN32
// force the instantiation of TetGaussian2<double>
template class PrismGaussian2<double>;
#endif

template <class T>
int PrismGaussian2<T>::GaussianNum = 6;

template <class T>
T PrismGaussian2<T>::GaussianPoints[6][3] = {
  {1./6.,1./6., 0.211324865405}, {2./3.,1./6., 0.211324865405}, 
  {1./6.,2./3., 0.211324865405}, {1./6.,1./6., 0.788675134595}, 
  {2./3.,1./6., 0.788675134595}, {1./6.,2./3., 0.788675134595}};

template <class T>
T PrismGaussian2<T>::GaussianWeights[6] = 
  {1./6., 1./6., 1./6., 1./6., 1./6., 1./6.};
  
//! Class with weights and coordinates for 3rd order Gaussian integration
//! to do

//! Class for handling of element of type prism with linear lagrangian interpolation
template <class T>
class PrismLinearLgn : public BasisSimple<T>, 
                       public PrismApprox, 
		       public PrismGaussian2<double>, 
		       public  PrismLinearLgnUnitElement
{
public:
  typedef T value_type;

  PrismLinearLgn() {}
  virtual ~PrismLinearLgn() {}

  static int polynomial_order() { return 1; }

  //! get weight factors at parametric coordinate 
  inline
  static void get_weights(const std::vector<double> &coords, double *w) 
  { 
    const double x = coords[0], y = coords[1], z = coords[2];  
    w[0] = (-1 + x + y) * (-1 + z);
    w[1] = - (x * (-1 + z));
    w[2] = - (y * (-1 + z));
    w[3] = - ((-1 + x + y) * z);
    w[4] = +x * z;
    w[5] = +y * z;
  }
  
  //! get value at parametric coordinate 
  template <class ElemData>
  T interpolate(const std::vector<double> &coords, const ElemData &cd) const
  {
    double w[6];
    get_weights(coords, w); 
 
    return (T)(w[0] * cd.node0() +
	       w[1] * cd.node1() +
	       w[2] * cd.node2() +
	       w[3] * cd.node3() +
	       w[4] * cd.node4() +
	       w[5] * cd.node5());
  }
  
 //! get derivative weight factors at parametric coordinate 
  inline
  static void get_derivate_weights(const std::vector<double> &coords, double *w) 
  {
    const double x=coords[0], y=coords[1], z=coords[2];  
    w[0]= (-1 + z);
    w[1]= (1 - z);
    w[2]= 0;
    w[3]= - z;
    w[4]= +z;
    w[5]= 0;
    w[6]= (-1 + z);
    w[7]= 0;
    w[8]= (1 - z);
    w[9]= - z;
    w[10]= 0;
    w[11]= z;
    w[12]= (-1 + x + y);
    w[13]= - x;
    w[14]= - y;
    w[15]= (1 - x - y);
    w[16]= x;
    w[17]= y;
  }

  //! get first derivative at parametric coordinate
  template <class ElemData>
  void derivate(const std::vector<double> &coords, const ElemData &cd, 
		std::vector<T> &derivs) const
  {
    const double x = coords[0], y = coords[1], z = coords[2]; 

    derivs.resize(3);
 
    derivs[0] = T((-1 + z) * cd.node0()
		  +(1 - z) * cd.node1()
		  - z * cd.node3()
		  +z * cd.node4());
      
    derivs[1] = T((-1 + z) * cd.node0()
		  + (1 - z) * cd.node2()
		  - z * cd.node3()
		  + z * cd.node5());
      
    derivs[2] = T((-1 + x + y) * cd.node0()
		  - x * cd.node1()
		  - y * cd.node2()
		  + (1 - x - y) * cd.node3()
		  + x * cd.node4()
		  + y * cd.node5());
  }

  //! get parametric coordinate for value within the element
  template <class ElemData>
  bool get_coords(std::vector<double> &coords, const T& value, 
		  const ElemData &cd) const  
  {
    PrismLocate< PrismLinearLgn<T> > CL;
    return CL.get_coords(this, coords, value, cd);
  }  

  //! get arc length for edge
  template <class ElemData>
  double get_arc_length(const unsigned edge, const ElemData &cd) const  
  {
    return get_arc3d_length<CrvGaussian1<double> >(this, edge, cd);
  }
 
  //! get area
  template <class ElemData>
    double get_area(const unsigned face, const ElemData &cd) const  
  {
    if (unit_faces[face][3]==-1) 
      return get_area3<TriGaussian2<double> >(this, face, cd);
    else
      return get_area3<QuadGaussian2<double> >(this, face, cd);
  }
 
  //! get volume
  template <class ElemData>
    double get_volume(const ElemData & cd) const  
  {
    return get_volume3(this, cd);
  }
  
  static  const std::string type_name(int n = -1);

  virtual void io (Piostream& str);
};


template <class T>
const FETypeDescription* get_type_description(PrismLinearLgn<T> *)
{
  static FETypeDescription* td = 0;
  if(!td){
    const FETypeDescription *sub = get_type_description((T*)0);
    FETypeDescription::td_vec *subs = scinew FETypeDescription::td_vec(1);
    (*subs)[0] = sub;
    td = scinew FETypeDescription("PrismLinearLgn", subs, 
				std::string(__FILE__),
				"Uintah", 
				FETypeDescription::BASIS_E);
  }
  return td;
}

template <class T>
const std::string
PrismLinearLgn<T>::type_name(int n)
{
  ASSERT((n >= -1) && n <= 1);
  if (n == -1)
  {
    static const std::string name = type_name(0) + FTNS + type_name(1) + FTNE;
    return name;
  }
  else if (n == 0)
  {
    static const std::string nm("PrismLinearLgn");
    return nm;
  } else {
    return find_type_name((T *)0);
  }
}


const int PRISMLINEARLGN_VERSION = 1;
template <class T>
void
PrismLinearLgn<T>::io(Piostream &stream)
{
  stream.begin_class(get_type_description(this)->get_name(),
                     PRISMLINEARLGN_VERSION);
  stream.end_class();
}

} //namespace Uintah


#endif // PrismLinearLgn_h
