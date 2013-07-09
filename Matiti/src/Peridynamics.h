#ifndef MATITI_PERIDYNAMICS_H
#define MATITI_PERIDYNAMICS_H

#include <SimulationState.h>
#include <Time.h>
#include <OutputVTK.h>
#include <Domain.h>
#include <MaterialSPArray.h>
#include <BodySPArray.h>
#include <HorizonComputer.h>

#include <PeridynamicsTypes.h>
#include <NodeP.h>
#include <NodePArray.h>
#include <BondP.h>
#include <BondPArray.h>

#include <Core/ProblemSpec/ProblemSpecP.h>

namespace Matiti {

  class Peridynamics {

  public:
    Peridynamics();
    ~Peridynamics();

    void problemSetup(Uintah::ProblemSpecP& ps);

    void run();

  protected:

    void applyInitialConditions();

    void computeInternalForce(const NodeP& node,
                              Vector3D& internalForce);

    void integrateNodalAcceleration(const NodeP& node,
                                    const Vector3D& acceleration,
                                    double delT,
                                    Vector3D& velNew);

    void integrateNodalVelocity(const NodeP& node,
                                const Vector3D& velocity,
                                double delT,
                                Vector3D& disp_new);

    double computeMicromodulus(const double& bondLengthInitial, 
		               const double& horizonRadius,
			       const double& youngsModulus);

    void breakBonds(const NodePArray& nodes);

    void checkMemoryUsage(double& resident_mem, double& shared_mem);

  private:

    Time d_time;
    OutputVTK d_output;
    SimulationState d_state;
    Domain d_domain;
    MaterialSPArray d_mat_list;
    BodySPArray d_body_list;

    int d_num_broken_bonds;

  }; // end class
} // end namespace

#endif

