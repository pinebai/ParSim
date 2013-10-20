#ifndef MATITI_VELOCITYBC_SP_H
#define MATITI_VELOCITYBC_SP_H

#include <memory>

namespace Matiti {
  
  // Forward declaration.  Make sure <VelocityBC.h> is included before using VelocityBCP.
  // using stdlib shared_ptr 
  class VelocityBC;
  typedef std::shared_ptr<VelocityBC> VelocityBCSP;
}

#endif