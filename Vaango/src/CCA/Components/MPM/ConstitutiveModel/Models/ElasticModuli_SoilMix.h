/*
 * The MIT License
 *
 * Copyright (c) 2015-2106 Parresia Research Limited, New Zealand
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

#ifndef ___ELASTIC_MODULI_SOIL_MIXTURE_MODEL_H__
#define ___ELASTIC_MODULI_SOIL_MIXTURE_MODEL_H__


#include <CCA/Components/MPM/ConstitutiveModel/Models/ElasticModuliModel.h>
#include <CCA/Components/MPM/ConstitutiveModel/Models/ModelStateBase.h>
#include <CCA/Components/MPM/ConstitutiveModel/Models/Pressure_Air.h>
#include <CCA/Components/MPM/ConstitutiveModel/Models/Pressure_Water.h>
#include <CCA/Components/MPM/ConstitutiveModel/Models/Pressure_Granite.h>
#include <Core/ProblemSpec/ProblemSpecP.h>

#include <limits>

namespace Vaango {

  /*! \class ElasticModuli_SoilMix
   *  \brief Elasticity model for Arenisca3PartSat micture of soils
   *  \author Biswajit Banerjee, 
   *          Caveat below by Michael Homel.
   *
   *  Purpose: 
   *  Compute the nonlinear elastic tangent stiffness as a function of the pressure
   *  plastic strain, and fluid parameters for a mixture of soils.
   *
   * Caveat:
   *  To be thermodynamically consistent, the shear modulus in an isotropic model
   *  must be constant, but the bulk modulus can depend on pressure.  However, this
   *  leads to a Poisson's ratio that approaches 0.5 at high pressures, which is
   *  inconsistent with experimental data for the Poisson's ratio, inferred from the 
   *  Young's modulus.  Induced anisotropy is likely the cause of the discrepency, 
   *  but it may be better to allow the shear modulus to vary so the Poisson's ratio
   *  remains reasonable.
   *
   *  If the user has specified a nonzero value of nu1 and nu2, the shear modulus will
   *  vary with pressure so the drained Poisson's ratio transitions from nu1 to nu1+nu2 as 
   *  the bulk modulus varies from b0 to b0+b1.  The fluid model further affects the 
   *  bulk modulus, but does not alter the shear modulus, so the pore fluid does
   *  increase the Poisson's ratio.  
   *
   */
  class ElasticModuli_SoilMix : public ElasticModuliModel {

  private:
    
    /* Tangent bulk modulus parameters */
    struct BulkModulusParameters {
      double b0;
      double b1;
      double b2;
      double b3;
      double b4;
    };

    /* Tangent shear modulus parameters */
    struct ShearModulusParameters {
      double G0;
      double nu1;
      double nu2;
    };

    /* Volume fractions */
    /* TODO: These volume fractions should come from the deformed volumes 
             in the master code rather than from a local copy */
    double d_volfrac[2];

    BulkModulusParameters d_bulk[2];
    ShearModulusParameters d_shear[2];

    /* Tangent bulk modulus models for air, water, granite */
    Pressure_Air     d_air;
    Pressure_Water   d_water;
    Pressure_Granite d_granite;

    void checkInputParameters();

    void computeDrainedModuli(const double& I1_eff_bar, 
                              const double& ev_p_bar,
                              double& KK,
                              double& GG); // not const: modifies d_bulk in PressureModel

    void computePartialSaturatedModuli(const double& I1_eff_bar, 
                                       const double& pw_bar,
                                       const double& ev_p_bar,
                                       const double& phi,
                                       const double& S_w,
                                       double& KK,
                                       double& GG); // not const: modifies d_bulk in PressureModel

    void computeDrainedModuli(int phase,
                              const double& I1_eff_bar, 
                              const double& ev_p_bar,
                              double& KK,
                              double& GG); // not const: modifies d_bulk in PressureModel

    void computePartialSaturatedModuli(int phase,
                                       const double& I1_eff_bar, 
                                       const double& pw_bar,
                                       const double& ev_p_bar,
                                       const double& phi,
                                       const double& S_w,
                                       double& KK,
                                       double& GG); // not const: modifies d_bulk in PressureModel

    ElasticModuli_SoilMix& operator=(const ElasticModuli_SoilMix &smm);

  public:
         
    /*! Construct a constant elasticity model. */
    ElasticModuli_SoilMix(Uintah::ProblemSpecP& ps);

    /*! Construct a copy of constant elasticity model. */
    ElasticModuli_SoilMix(const ElasticModuli_SoilMix* smm);

    /*! Destructor of constant elasticity model.   */
    virtual ~ElasticModuli_SoilMix();
         
    virtual void outputProblemSpec(Uintah::ProblemSpecP& ps);

    /*! Get parameters */
    std::map<std::string, double> getParameters() const {
      std::map<std::string, double> params;
      params["b0.phase1"] = d_bulk[0].b0;
      params["b1.phase1"] = d_bulk[0].b1;
      params["b2.phase1"] = d_bulk[0].b2;
      params["b3.phase1"] = d_bulk[0].b3;
      params["b4.phase1"] = d_bulk[0].b4;
      params["G0.phase1"] = d_shear[0].G0;
      params["nu1.phase1"] = d_shear[0].nu1;
      params["nu2.phase1"] = d_shear[0].nu2;
      params["b0.phase2"] = d_bulk[1].b0;
      params["b1.phase2"] = d_bulk[1].b1;
      params["b2.phase2"] = d_bulk[1].b2;
      params["b3.phase2"] = d_bulk[1].b3;
      params["b4.phase2"] = d_bulk[1].b4;
      params["G0.phase2"] = d_shear[1].G0;
      params["nu1.phase2"] = d_shear[1].nu1;
      params["nu2.phase2"] = d_shear[1].nu2;
      return params;
    }

    /*! Compute the elasticity */
    ElasticModuli getInitialElasticModuli() const;
    ElasticModuli getCurrentElasticModuli(const ModelStateBase* state);

    ElasticModuli getElasticModuliLowerBound() const {return getInitialElasticModuli();}
    ElasticModuli getElasticModuliUpperBound() const {
      return ElasticModuli(std::numeric_limits<double>::max(), 
                           std::numeric_limits<double>::max());
    }

  };
} // End namespace Uintah
      
#endif  // __ELASTIC_MODULI_SOIL_MIXTURE_MODEL_H__

