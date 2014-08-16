#ifndef Vaango_Peridynamics_H
#define Vaango_Peridynamics_H

#include <CCA/Components/Peridynamics/PeridynamicsLabel.h>
#include <CCA/Components/Peridynamics/PeridynamicsFlags.h>
#include <CCA/Components/Peridynamics/GradientComputer/PeridynamicsDefGradComputer.h>
#include <CCA/Components/Peridynamics/InternalForceComputer/BondInternalForceComputer.h>
#include <CCA/Components/Peridynamics/InternalForceComputer/ParticleInternalForceComputer.h>
#include <CCA/Components/Peridynamics/FamilyComputer/FamilyComputer.h>

#include <CCA/Components/MPM/Contact/Contact.h>

#include <Core/Grid/SimulationStateP.h>
#include <CCA/Ports/SimulationInterface.h>

#include <Core/Parallel/UintahParallelComponent.h>

#include <Core/Grid/GridP.h>
#include <Core/Grid/LevelP.h>
#include <Core/Grid/Variables/ComputeSet.h>
#include <Core/Grid/Variables/ParticleVariable.h>
#include <Core/Grid/ParticleInterpolator.h>

#include <Core/Geometry/Vector.h>

#include <Core/ProblemSpec/ProblemSpecP.h>
#include <Core/Math/Matrix3.h>
#include <Core/Math/Short27.h>

#include <CCA/Ports/DataWarehouseP.h>
#include <CCA/Ports/Output.h>


namespace Vaango {

  /////////////////////////////////////////////////////////////////////////////
  /*!
    \class   Peridynamics
    \brief   Parallel version of bond-based/state-based peridynamics
    \author  Biswajit Banerjee 
    \warning 
  */
  /////////////////////////////////////////////////////////////////////////////

  class Peridynamics : public Uintah::SimulationInterface, 
                       public Uintah::UintahParallelComponent 
  {
  public:

    /*! Constructor/Destructor */
    Peridynamics(const Uintah::ProcessorGroup* myworld);
    virtual ~Peridynamics();

    
    /*! Problem spec reader and particle creation */
    virtual void problemSetup(const Uintah::ProblemSpecP& params, 
                              const Uintah::ProblemSpecP& restart_prob_spec, 
                              Uintah::GridP& grid,
                              Uintah::SimulationStateP& state);

    /*! Output writer for problem spec */
    virtual void outputProblemSpec(Uintah::ProblemSpecP& ps);

    /*! Schedule tasks */
    virtual void scheduleInitialize(const Uintah::LevelP& level, 
                                    Uintah::SchedulerP& sched);
    virtual void scheduleComputeStableTimestep(const Uintah::LevelP& level, 
                                               Uintah::SchedulerP& sched);
    virtual void scheduleTimeAdvance(const Uintah::LevelP& level, 
                                     Uintah::SchedulerP& sched);

    /*! For restarts */
    virtual void restartInitialize();

  protected:
 
    /*! Register the materials */
    void materialProblemSetup(const Uintah::ProblemSpecP& prob_spec);

    /*! Schedule initialization of particle load BCs */
    void scheduleInitializeParticleLoadBCs(const Uintah::LevelP& level,
                                           Uintah::SchedulerP& sched);
 
    /*! Actual initialization */
    void actuallyInitialize(const Uintah::ProcessorGroup*,
                                    const Uintah::PatchSubset* patches,
                                    const Uintah::MaterialSubset* matls,
                                    Uintah::DataWarehouse* old_dw,
                                    Uintah::DataWarehouse* new_dw);

    /*!  Calculate the number of material points per load curve */
    void countSurfaceParticlesPerLoadCurve(const Uintah::ProcessorGroup*,
                                           const Uintah::PatchSubset* patches,
                                           const Uintah::MaterialSubset*,
                                           Uintah::DataWarehouse* ,
                                           Uintah::DataWarehouse* new_dw);

    /*!  Use the type of LoadBC to find the initial external force at each particle */
    void initializeParticleLoadBC(const Uintah::ProcessorGroup*,
                                  const Uintah::PatchSubset* patches,
                                  const Uintah::MaterialSubset*,
                                  Uintah::DataWarehouse* ,
                                  Uintah::DataWarehouse* new_dw);

    /*! Find list of neighbors */
    void findNeighborsInHorizon(const Uintah::ProcessorGroup*,
                                const Uintah::PatchSubset* patches,
                                const Uintah::MaterialSubset* matls,
                                Uintah::DataWarehouse*,
                                Uintah::DataWarehouse* new_dw);

    /*! Actual stable timestep computation */
    void actuallyComputeStableTimestep(const Uintah::ProcessorGroup*,
                                       const Uintah::PatchSubset* patches,
                                       const Uintah::MaterialSubset* matls,
                                       Uintah::DataWarehouse* old_dw,
                                       Uintah::DataWarehouse* new_dw);

    /*! Apply external loads to bodies inside the domain */
    void scheduleApplyExternalLoads(Uintah::SchedulerP& sched, 
                                    const Uintah::PatchSet* patches,
                                    const Uintah::MaterialSet* matls);
    void applyExternalLoads(const Uintah::ProcessorGroup*,
                            const Uintah::PatchSubset* patches,
                            const Uintah::MaterialSubset* ,
                            Uintah::DataWarehouse* old_dw,
                            Uintah::DataWarehouse* new_dw);

    /*! Interpolation of particles to grid for contact detection */
    void scheduleInterpolateParticlesToGrid(Uintah::SchedulerP& sched, 
                                            const Uintah::PatchSet* patches,
                                            const Uintah::MaterialSet* matls);
    void interpolateParticlesToGrid(const Uintah::ProcessorGroup*,
                                    const Uintah::PatchSubset* patches,
                                    const Uintah::MaterialSubset* matls,
                                    Uintah::DataWarehouse* old_dw,
                                    Uintah::DataWarehouse* new_dw);

    /*! Apply contact forces */
    void scheduleApplyContactLoads(Uintah::SchedulerP& sched, 
                                   const Uintah::PatchSet* patches,
                                   const Uintah::MaterialSet* matls);

    /*! Computation of deformation gradient */
    void scheduleComputeDeformationGradient(Uintah::SchedulerP& sched, 
                                            const Uintah::PatchSet* patches,
                                            const Uintah::MaterialSet* matls);
    void computeDeformationGradient(const Uintah::ProcessorGroup*,
                                    const Uintah::PatchSubset* patches,
                                    const Uintah::MaterialSubset* matls,
                                    Uintah::DataWarehouse* old_dw,
                                    Uintah::DataWarehouse* new_dw);

    /*! Computation of stress tensor */
    void scheduleComputeStressTensor(Uintah::SchedulerP& sched, 
                                     const Uintah::PatchSet* patches,
                                     const Uintah::MaterialSet* matls);
    void computeStressTensor(const Uintah::ProcessorGroup*,
                             const Uintah::PatchSubset* patches,
                             const Uintah::MaterialSubset* matls,
                             Uintah::DataWarehouse* old_dw,
                             Uintah::DataWarehouse* new_dw);

    /*! Computation of internal force */
    /*! Computation of bond internal force */
    /*! Computation of particle internal force */
    void scheduleComputeInternalForce(Uintah::SchedulerP& sched, 
                                      const Uintah::PatchSet* patches,
                                      const Uintah::MaterialSet* matls);
    void computeBondInternalForce(const Uintah::ProcessorGroup*,
                                  const Uintah::PatchSubset* patches,
                                  const Uintah::MaterialSubset* matls,
                                  Uintah::DataWarehouse* old_dw,
                                  Uintah::DataWarehouse* new_dw);
    void computeInternalForce(const Uintah::ProcessorGroup*,
                              const Uintah::PatchSubset* patches,
                              const Uintah::MaterialSubset* matls,
                              Uintah::DataWarehouse* old_dw,
                              Uintah::DataWarehouse* new_dw);


    /*! Integration of acceleration */
    void scheduleComputeAndIntegrateAcceleration(Uintah::SchedulerP& sched,
                                                 const Uintah::PatchSet* patches,
                                                 const Uintah::MaterialSet* matls);
    void computeAndIntegrateAcceleration(const Uintah::ProcessorGroup*,
                                         const Uintah::PatchSubset* patches,
                                         const Uintah::MaterialSubset* matls,
                                         Uintah::DataWarehouse* old_dw,
                                         Uintah::DataWarehouse* new_dw);

    /*! Project particle acceleration and velocity to grid */
    void scheduleProjectParticleAccelerationToGrid(Uintah::SchedulerP& sched,
                                                   const Uintah::PatchSet* patches,
                                                   const Uintah::MaterialSet* matls);
    void projectParticleAccelerationToGrid(const Uintah::ProcessorGroup*,
                                           const Uintah::PatchSubset* patches,
                                           const Uintah::MaterialSubset* matls,
                                           Uintah::DataWarehouse* old_dw,
                                           Uintah::DataWarehouse* new_dw);


    /*! Correct contact forces */
    void scheduleCorrectContactLoads(Uintah::SchedulerP& sched, 
                                     const Uintah::PatchSet* patches,
                                     const Uintah::MaterialSet* matls);

    /*! Grid boundary condition set up */
    void scheduleSetGridBoundaryConditions(Uintah::SchedulerP& sched, 
                                           const Uintah::PatchSet* patches,
                                           const Uintah::MaterialSet* matls);
    void setGridBoundaryConditions(const Uintah::ProcessorGroup*,
                                   const Uintah::PatchSubset* patches,
                                   const Uintah::MaterialSubset* ,
                                   Uintah::DataWarehouse* old_dw,
                                   Uintah::DataWarehouse* new_dw);

                                                 
    /*! Update particle velocities and displacements from grid data */
    void scheduleUpdateParticleKinematics(Uintah::SchedulerP& sched,
                                          const Uintah::PatchSet* patches,
                                          const Uintah::MaterialSet* matls);
    void updateParticleKinematics(const Uintah::ProcessorGroup*,
                                  const Uintah::PatchSubset* patches,
                                  const Uintah::MaterialSubset* matls,
                                  Uintah::DataWarehouse* old_dw,
                                  Uintah::DataWarehouse* new_dw);

    /*! Compute damage and remove bonds */
    void scheduleComputeDamage(Uintah::SchedulerP& sched, 
                               const Uintah::PatchSet* patches,
                               const Uintah::MaterialSet* matls);
    void computeDamage(const Uintah::ProcessorGroup*,
                       const Uintah::PatchSubset* patches,
                       const Uintah::MaterialSubset* ,
                       Uintah::DataWarehouse* old_dw,
                       Uintah::DataWarehouse* new_dw);

    /*! Finalize the particle state to get ready for the next time step */
    void scheduleFinalizeParticleState(Uintah::SchedulerP& sched,
                                       const Uintah::PatchSet* patches,
                                       const Uintah::MaterialSet* matls);
    void finalizeParticleState(const Uintah::ProcessorGroup*,
                               const Uintah::PatchSubset* patches,
                               const Uintah::MaterialSubset* matls,
                               Uintah::DataWarehouse* old_dw,
                               Uintah::DataWarehouse* new_dw);

    /*! Need taskgraph recompile ? */  
    bool needRecompile(double time, double dt, const Uintah::GridP& grid);


    template<typename T>
      void setParticleDefault(Uintah::ParticleVariable<T>& pvar,
                              const Uintah::VarLabel* label, 
                              Uintah::ParticleSubset* pset,
                              Uintah::DataWarehouse* new_dw,
                              const T& val)
    {
      new_dw->allocateAndPut(pvar, label, pset);
      Uintah::ParticleSubset::iterator iter = pset->begin();
      for (; iter != pset->end(); iter++) {
        pvar[*iter] = val;
      }
    }

  protected:
  
    Uintah::SimulationStateP d_sharedState;

    PeridynamicsLabel* d_labels;
    PeridynamicsFlags* d_flags;
    PeridynamicsDefGradComputer* d_defGradComputer;
    BondInternalForceComputer* d_bondIntForceComputer;
    ParticleInternalForceComputer* d_intForceComputer;
    FamilyComputer* d_familyComputer;

    Uintah::ParticleInterpolator* d_interpolator;
    Uintah::Output* d_dataArchiver;
    Uintah::Contact* d_contactModel;


    int  d_numGhostNodes;      // Number of ghost nodes needed
    int  d_numGhostParticles;  // Number of ghost particles needed
    bool d_recompile;
    Uintah::MaterialSubset*  d_loadCurveIndex;
  
  private:

    /*! Don't allow copying */
    Peridynamics(const Peridynamics&);
    Peridynamics& operator=(const Peridynamics&);
};
      
} // end namespace Vaango

#endif
