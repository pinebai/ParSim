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

#ifndef SCI_project_CCTensorField_h
#define SCI_project_CCTensorField_h 1

#include <Core/Datatypes/TensorField.h>
#include <Core/Grid/Variables/CCVariable.h>
#include <Core/Grid/Grid.h>
#include <Core/Grid/GridP.h>
#include <Core/Grid/LevelP.h>

#include <Core/Geometry/Point.h>
#include <Core/Geometry/IntVector.h>

#include <vector>

namespace Uintah {

  using namespace SCIRun;
  
class CCTensorField: public TensorField {
public:
  int nx;
  int ny;
  int nz;
  CCTensorField();
  CCTensorField(const CCTensorField&);
  CCTensorField(GridP grid, LevelP level, std::string var, int mat,
		const std::vector< CCVariable<Matrix3> >& vars);
  virtual ~CCTensorField() {}
  virtual TensorField* clone();

  virtual void compute_bounds();
  virtual void get_boundary_lines(Array1<Point>& lines);
  void computeHighLowIndices();


  Matrix3 grid(int i, int j, int k);
  void SetGrid( GridP g ){ _grid = g; }
  void SetLevel( LevelP l){ _level = l; computeHighLowIndices(); }
  const LevelP GetLevel() { return _level; }
  void SetName( std::string vname ) { _varname = vname; }
  void SetMaterial( int index) { _matIndex = index; }
  void AddVar( const CCVariable<Matrix3>& var);
private:
  std::vector< CCVariable<Matrix3> > _vars;
  GridP _grid;
  LevelP _level;
  std::string _varname;
  int _matIndex;
  IntVector low;
  IntVector high;
};
} // End namespace Uintah

#endif
