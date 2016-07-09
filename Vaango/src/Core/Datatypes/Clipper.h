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

//    File   : Clipper.h
//    Author : Michael Callahan
//    Date   : September 2001

#if !defined(Clipper_h)
#define Clipper_h

#include <Core/Containers/LockingHandle.h>
#include <Core/Datatypes/Datatype.h>
#include <Core/Datatypes/Mesh.h>
#include <Core/Geometry/Transform.h>
#include <Core/Containers/LockingHandle.h>

#include <Core/Datatypes/share.h>

namespace SCIRun {

class SCISHARE Clipper : public Datatype
{
public:
  virtual ~Clipper();

  virtual bool inside_p(const Point &p);
  virtual bool mesh_p() { return false; }

  static  PersistentTypeID type_id;
  void    io(Piostream &stream);
};


typedef LockingHandle<Clipper> ClipperHandle;



class SCISHARE IntersectionClipper : public Clipper
{
private:
  ClipperHandle clipper0_;
  ClipperHandle clipper1_;

public:
  IntersectionClipper(ClipperHandle c0, ClipperHandle c1);

  virtual bool inside_p(const Point &p);

  static  PersistentTypeID type_id;
  void    io(Piostream &stream);
};


class SCISHARE UnionClipper : public Clipper
{
private:
  ClipperHandle clipper0_;
  ClipperHandle clipper1_;

public:
  UnionClipper(ClipperHandle c0, ClipperHandle c1);

  virtual bool inside_p(const Point &p);

  static  PersistentTypeID type_id;
  void    io(Piostream &stream);
};


class SCISHARE InvertClipper : public Clipper
{
private:
  ClipperHandle clipper_;

public:
  InvertClipper(ClipperHandle clipper);

  virtual bool inside_p(const Point &p);

  static  PersistentTypeID type_id;
  void    io(Piostream &stream);
};

  
class SCISHARE BoxClipper : public Clipper
{
private:
  Transform trans_;

public:
  BoxClipper(Transform &t);

  virtual bool inside_p(const Point &p);

  static  PersistentTypeID type_id;
  void    io(Piostream &stream);
};



template <class MESH>
class MeshClipper : public Clipper
{
private:
  LockingHandle<MESH> mesh_;

public:
  MeshClipper(LockingHandle<MESH> mesh) : mesh_(mesh) { 
    mesh->synchronize(Mesh::LOCATE_E);
  }

  virtual bool inside_p(const Point &p)
  {
    typename MESH::Elem::index_type indx;
    return mesh_->locate(indx, p);
  }
  virtual bool mesh_p() { return true; }

  void    io(Piostream &stream) {}
};



} // end namespace SCIRun

#endif // Clipper_h

