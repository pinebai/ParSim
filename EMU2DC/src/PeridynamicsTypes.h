#ifndef EMU2DC_PERIDYNAMIC_TYPES_H
#define EMU2DC_PERIDYNAMIC_TYPES_H

#include <NodeP.h>
#include <BondP.h>
#include <map>
#include <vector>

namespace Emu2DC {
  typedef std::multimap<NodeP,NodeP> NodeFamily;
  typedef NodeFamily::const_iterator constNodeFamilyIterator;
  typedef NodeFamily::iterator NodeFamilyIterator;

  typedef std::multimap<NodeP,BondP> BondFamily;
  typedef BondFamily::const_iterator constBondFamilyIterator;
  typedef BondFamily::iterator BondFamilyIterator;
  typedef std::pair<NodeP,BondP> NodeBondPair;

  typedef std::vector<BondP> BondArray;
  typedef std::vector<BondP>::iterator BondIterator;
}

#endif