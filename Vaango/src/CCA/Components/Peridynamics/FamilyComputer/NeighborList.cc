#include <CCA/Components/Peridynamics/NeighborList.h>
#include <Core/Util/Endian.h>
#include <Core/Util/TypeDescription.h>

const std::string& 
NeighborList::get_h_file_path()
{
  static const std::string path(SCIRun::TypeDescription::cc_to_h(__FILE__));
  return path;
}

// Added for compatibility with core types
namespace SCIRun {

  void 
  swapbytes(Vaango::NeighborList& family)
  {
    Uintah::ParticleID* ptr = (Uintah::ParticleID*) (&family);
    SWAP_8(*ptr);
    for (int ii = 1; ii < 216; ii++) {
      SWAP_8(*++ptr);
    }
  }

  template<> const std::string 
  find_type_name(Vaango::NeighborList*)
  {
    static const std::string name = "NeighborList";
    return name;
  }

  const TypeDescription* 
  get_type_description(Vaango::NeighborList*)
  {
    static TypeDescription* td = 0;
    if (!td) {
      td = scinew TypeDescription("NeighborList", 
                                  Vaango::NeighborList::get_h_file_path(),
                                  "Vaango");
    }
    return td;
  }

  void 
  Pio(Piostream& stream, Vaango::NeighborList& family)
  {
    stream.begin_cheap_delim();
    for (int ii = 0; ii < 216; ii++) {
      Pio(stream, family[ii]);
    }
    stream.end_cheap_delim();
  }

} // namespace SCIRun


#endif
