/*
 * The MIT License
 *
 * Copyright (c) 2015-2016 Parresia Research Limited, New Zealand
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

#ifndef __MODELS_AIR_EOS_MODEL_H__
#define __MODELS_AIR_EOS_MODEL_H__

#include <CCA/Components/MPM/ConstitutiveModel/Models/PressureModel.h> 
#include <CCA/Components/MPM/ConstitutiveModel/Models/ModelStateBase.h> 
#include <Core/ProblemSpec/ProblemSpecP.h>

namespace Vaango {

  ////////////////////////////////////////////////////////////////////////////
  /*! 
    \class Pressure_Air
    \brief Isentropic equation of state for air.  

    The equation of state is given by
    \f[
      p = p_0*(\exp(\gamma*\epsilon_v) - 1)
    \f]
    where \n
    \f$p\f$ = pressure\n
    \f$p_0\f$ = reference pressure\n
    \f$\gamma\f$ = parameter
    \f$n\epsilon_v = -\log(\rho/\rho_0)$ = volumetric strain
   
    \warning For use only with Arenisca3PartiallySaturated
  */
  ////////////////////////////////////////////////////////////////////////////

  class Pressure_Air : public PressureModel {

  private:

    double d_p0;
    double d_gamma;

    // Prevent copying of this class
    // copy constructor
    //Pressure_Air(const Pressure_Air &cm);
    Pressure_Air& operator=(const Pressure_Air &cm);

  public:

    // constructors
    Pressure_Air(); 
    Pressure_Air(Uintah::ProblemSpecP& ps); 
    Pressure_Air(const Pressure_Air* cm);
         
    // destructor 
    virtual ~Pressure_Air();

    virtual void outputProblemSpec(Uintah::ProblemSpecP& ps);
         
    /*! Get parameters */
    std::map<std::string, double> getParameters() const {
      std::map<std::string, double> params;
      params["p0"] = d_p0;
      params["gamma"] = d_gamma;
      params["Ka"] = d_bulkModulus;
      return params;
    }

    //////////
    // Calculate the pressure using a equation of state
    double computePressure(const Uintah::MPMMaterial* matl,
                           const ModelStateBase* state,
                           const Uintah::Matrix3& deformGrad,
                           const Uintah::Matrix3& rateOfDeformation,
                           const double& delT);
    double computePressure(const double& rho_orig,
                           const double& rho_cur);
    void computePressure(const double& rho_orig,
                         const double& rho_cur,
                         double& pressure,
                         double& dp_drho,
                         double& csquared);
  
    //////////
    // Calculate the derivative of the pressure
    double eval_dp_dJ(const Uintah::MPMMaterial* matl,
                      const double& detF,
                      const ModelStateBase* state);

    // Compute bulk modulus
    double computeInitialBulkModulus();
    double computeBulkModulus(const double& pressure);
    double computeBulkModulus(const double& rho_orig,
                              const double& rho_cur);
    double computeBulkModulus(const ModelStateBase* state);

    // Compute strain energy
    double computeStrainEnergy(const double& rho_orig,
                               const double& rho_cur);
    double computeStrainEnergy(const ModelStateBase* state);


    // Compute density given pressure
    double computeDensity(const double& rho_orig,
                          const double& pressure);

    ////////////////////////////////////////////////////////////////////////
    /*! Calculate the derivative of p with respect to epse_v
        where epse_v = tr(epse)
              epse = total elastic strain */
    ////////////////////////////////////////////////////////////////////////
    double computeDpDepse_v(const ModelStateBase* state) const;

    ////////////////////////////////////////////////////////////////////////
    /*! Calculate the derivative of p with respect to epse_s
        where epse_s = sqrt{2}{3} ||ee||
              ee = epse - 1/3 tr(epse) I
              epse = total elastic strain */
    ////////////////////////////////////////////////////////////////////////
    double computeDpDepse_s(const ModelStateBase* state) const
    {
      return 0.0;
    };
  };

} // End namespace Uintah

#endif  // __MODELS_AIR_EOS_MODEL_H__ 
