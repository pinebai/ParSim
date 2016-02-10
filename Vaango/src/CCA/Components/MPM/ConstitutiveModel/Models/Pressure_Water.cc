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


#include <CCA/Components/MPM/ConstitutiveModel/Models/Pressure_Water.h>
#include <CCA/Components/MPM/ConstitutiveModel/Models/ModelState_MasonSand.h>
#include <Core/Exceptions/InternalError.h>
#include <Core/Exceptions/InvalidValue.h>
#include <cmath>

using namespace Uintah;
using namespace Vaango;

Pressure_Water::Pressure_Water()
{
  d_p0 = 0.0001;  // Hardcoded (SI units).  *TODO* Get as input with ProblemSpec later.
  d_K0 = 2.21e9;
  d_n = 6.029;
  d_bulk = d_K0;  
} 

Pressure_Water::Pressure_Water(Uintah::ProblemSpecP&)
{
  d_p0 = 0.0001;  // Hardcoded (SI units).  *TODO* Get as input with ProblemSpec later.
  d_K0 = 2.21e9;
  d_n = 6.029;
  d_bulk = d_K0;  
} 
         
Pressure_Water::Pressure_Water(const Pressure_Water* cm)
{
  d_p0 = cm->d_p0;
  d_K0 = cm->d_K0;
  d_n = cm->d_n;
  d_bulk = cm->d_bulk;
} 
         
Pressure_Water::~Pressure_Water()
{
}

void Pressure_Water::outputProblemSpec(Uintah::ProblemSpecP& ps)
{
  ProblemSpecP eos_ps = ps->appendChild("pressure_model");
  eos_ps->setAttribute("type","water");
}
         
//////////
// Calculate the pressure using the elastic constitutive equation
double 
Pressure_Water::computePressure(const Uintah::MPMMaterial* matl,
                                const ModelStateBase* state_input,
                                const Uintah::Matrix3& ,
                                const Uintah::Matrix3& rateOfDeformation,
                                const double& delT)
{
  const ModelState_MasonSand* state = dynamic_cast<const ModelState_MasonSand*>(state_input);
  if (!state) {
    std::ostringstream out;
    out << "**ERROR** The correct ModelState object has not been passed."
        << " Need ModelState_MasonSand.";
    throw SCIRun::InternalError(out.str(), __FILE__, __LINE__);
  }

  double rho_0 = matl->getInitialDensity();
  double rho = state->density;
  double p = computePressure(rho_0, rho);
  return p;
}

// Compute pressure (option 1)
double 
Pressure_Water::computePressure(const double& rho_orig,
                                const double& rho_cur)
{
  double J = rho_orig/rho_cur;
  double p = d_p0 + d_K0/d_n*(std::pow(J, -d_n) - 1);
  return p;
}

// Compute pressure (option 2)
void 
Pressure_Water::computePressure(const double& rho_orig,
                                const double& rho_cur,
                                double& pressure,
                                double& dp_drho,
                                double& csquared)
{
  double J = rho_orig/rho_cur;
  pressure = d_p0 + d_K0/d_n*(std::pow(J, -d_n) - 1);
  double dp_dJ = -d_K0*std::pow(J, -(d_n+1));
  dp_drho = d_K0/rho_cur*std::pow(J, -d_n);
  csquared = dp_dJ/rho_cur;
}

// Compute derivative of pressure 
double 
Pressure_Water::eval_dp_dJ(const Uintah::MPMMaterial* matl,
                           const double& detF, 
                           const ModelStateBase* state_input)
{
  const ModelState_MasonSand* state = dynamic_cast<const ModelState_MasonSand*>(state_input);
  if (!state) {
    std::ostringstream out;
    out << "**ERROR** The correct ModelState object has not been passed."
        << " Need ModelState_MasonSand.";
    throw SCIRun::InternalError(out.str(), __FILE__, __LINE__);
  }

  double J = detF;
  double dpdJ = -d_K0*std::pow(J, -(d_n+1));
  return dpdJ;
}

// Compute bulk modulus
double 
Pressure_Water::computeInitialBulkModulus()
{
  return d_K0;  
}

double 
Pressure_Water::computeBulkModulus(const double& pressure)
{
  d_bulk = d_K0 + d_n*(pressure - d_p0);
  return d_bulk;
}

double 
Pressure_Water::computeBulkModulus(const double& rho_orig,
                                   const double& rho_cur)
{
  double p = computePressure(rho_orig, rho_cur);
  d_bulk = computeBulkModulus(p);
  return d_bulk;
}

double 
Pressure_Water::computeBulkModulus(const ModelStateBase* state_input)
{
  const ModelState_MasonSand* state = dynamic_cast<const ModelState_MasonSand*>(state_input);
  if (!state) {
    std::ostringstream out;
    out << "**ERROR** The correct ModelState object has not been passed."
        << " Need ModelState_MasonSand.";
    throw SCIRun::InternalError(out.str(), __FILE__, __LINE__);
  }

  double p = -state->I1/3.0;
  d_bulk = computeBulkModulus(p);
  return d_bulk;
}

// Compute strain energy
double 
Pressure_Water::computeStrainEnergy(const double& rho_orig,
                                    const double& rho_cur)
{
  throw InternalError("ComputeStrainEnergy has not been implemented yet for Water.",
    __FILE__, __LINE__);
  return 0.0;
}

double 
Pressure_Water::computeStrainEnergy(const ModelStateBase* state)
{
  throw InternalError("ComputeStrainEnergy has not been implemented yet for Water.",
    __FILE__, __LINE__);
  return 0.0;
}


// Compute density given pressure (tension +ve)
double 
Pressure_Water::computeDensity(const double& rho_orig,
                               const double& pressure)
{
  throw InternalError("ComputeDensity has not been implemented yet for Water.",
    __FILE__, __LINE__);
  return 0.0;
}

//  Calculate the derivative of p with respect to epse_v
double 
Pressure_Water::computeDpDepse_v(const ModelStateBase* state_input) const
{
  const ModelState_MasonSand* state = dynamic_cast<const ModelState_MasonSand*>(state_input);
  if (!state) {
    std::ostringstream out;
    out << "**ERROR** The correct ModelState object has not been passed."
        << " Need ModelState_MasonSand.";
    throw SCIRun::InternalError(out.str(), __FILE__, __LINE__);
  }

  double p = -state->I1/3.0;
  double dp_depse_v = d_K0 + d_n*(p - d_p0);
  return dp_depse_v;
}