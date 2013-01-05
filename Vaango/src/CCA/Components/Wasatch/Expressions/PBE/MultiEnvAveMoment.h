/*
 * The MIT License
 *
 * Copyright (c) 2012 The University of Utah
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

#ifndef MultiEnvAveMoment_Expr_h
#define MultiEnvAveMoment_Expr_h
#include <spatialops/structured/FVStaggeredFieldTypes.h>
#include <spatialops/structured/FVStaggeredOperatorTypes.h>

#include <expression/Expression.h>

/**
 *  \ingroup WasatchExpressions
 *  \class MultiEnvAveMoment
 *  \author Alex Abboud
 *  \date June 2012
 *  \brief Calculates the averaged moment at each grid point
 *  \f$ \left<\phi_\alpha\right> = \sum_i^3 w_i \phi_{\alpha,i} \f$
 *  requires the initial moment value set s the moments of 1st and 3rd envrinment
 *  requires new moment (state_none) to keep average up to date
 *  and the tag list of weights and derivatives of weights
 *  this is essentially a postprocess expression
 */
template< typename FieldT >
class MultiEnvAveMoment
: public Expr::Expression<FieldT>
{
  typedef std::vector<const FieldT*> FieldTVec;
  FieldTVec weightsAndDerivs_;
  const Expr::TagList weightAndDerivativeTags_; //this tag list has wieghts and derivatives [w0 dw0/dt w1 dw1/dt w2 dw2/dt]
  const Expr::Tag phiTag_;                      //tag for this moment in 2nd env
  const FieldT* phi_;
  const double initialMoment_;

  MultiEnvAveMoment( const Expr::TagList weightAndDerivativeTags,
                     const Expr::Tag phiTag,
                     const double initialMoment);

public:
  class Builder : public Expr::ExpressionBuilder
  {
  public:
    Builder( const Expr::Tag& result,
             const Expr::TagList& weightAndDerivativeTags,
             const Expr::Tag& phiTag,
             const double initialMoment)
    : ExpressionBuilder(result),
    weightandderivtaglist_(weightAndDerivativeTags),
    phit_(phiTag),
    initialmoment_(initialMoment)
    {}

    ~Builder(){}

    Expr::ExpressionBase* build() const
    {
      return new MultiEnvAveMoment<FieldT>( weightandderivtaglist_, phit_, initialmoment_ );
    }

  private:
    const Expr::TagList weightandderivtaglist_;
    const Expr::Tag phit_;
    const double initialmoment_;
  };

  ~MultiEnvAveMoment();

  void advertise_dependents( Expr::ExprDeps& exprDeps );
  void bind_fields( const Expr::FieldManagerList& fml );
  void bind_operators( const SpatialOps::OperatorDatabase& opDB );
  void evaluate();
};



// ###################################################################
//
//                          Implementation
//
// ###################################################################

template< typename FieldT >
MultiEnvAveMoment<FieldT>::
MultiEnvAveMoment( const Expr::TagList weightAndDerivativeTags,
               const Expr::Tag phiTag,
               const double initialMoment)
: Expr::Expression<FieldT>(),
weightAndDerivativeTags_(weightAndDerivativeTags),
phiTag_(phiTag),
initialMoment_(initialMoment)
{}

//--------------------------------------------------------------------

template< typename FieldT >
MultiEnvAveMoment<FieldT>::
~MultiEnvAveMoment()
{}

//--------------------------------------------------------------------

template< typename FieldT >
void
MultiEnvAveMoment<FieldT>::
advertise_dependents( Expr::ExprDeps& exprDeps )
{
  exprDeps.requires_expression( weightAndDerivativeTags_ );
  exprDeps.requires_expression( phiTag_ );
}

//--------------------------------------------------------------------

template< typename FieldT >
void
MultiEnvAveMoment<FieldT>::
bind_fields( const Expr::FieldManagerList& fml )
{
  const typename Expr::FieldMgrSelector<FieldT>::type& fm = fml.field_manager<FieldT>();
  weightsAndDerivs_.clear();
  for( Expr::TagList::const_iterator iW=weightAndDerivativeTags_.begin();
      iW!=weightAndDerivativeTags_.end();
      ++iW ){
    weightsAndDerivs_.push_back( &fm.field_ref(*iW) );
  }
  phi_ = &fm.field_ref( phiTag_ );
}

//--------------------------------------------------------------------

template< typename FieldT >
void
MultiEnvAveMoment<FieldT>::
bind_operators( const SpatialOps::OperatorDatabase& opDB )
{}

//--------------------------------------------------------------------

template< typename FieldT >
void
MultiEnvAveMoment<FieldT>::
evaluate()
{
  using SpatialOps::operator<<=;
  FieldT& result = this->value();

  const int wdSize = 6;
  const FieldT* sampleField = weightsAndDerivs_[0];
  typename FieldT::const_interior_iterator sampleIterator = sampleField->interior_begin();
  typename FieldT::const_interior_iterator phiIter = phi_->interior_begin();
  typename FieldT::interior_iterator resultsIter = result.interior_begin();

  std::vector<typename FieldT::const_interior_iterator> weightsAndDerivsIters;
  for (int i = 0; i<wdSize; i++) {
    typename FieldT::const_interior_iterator thisIterator = weightsAndDerivs_[i]->interior_begin();
    weightsAndDerivsIters.push_back(thisIterator);
  }

  while (sampleIterator!=sampleField->interior_end() ) {
    *resultsIter = ( *weightsAndDerivsIters[0] + *weightsAndDerivsIters[4] ) * initialMoment_ + *weightsAndDerivsIters[2] * *phiIter;
    //increment iterators
    for (int i = 0; i< wdSize; i++) {
      weightsAndDerivsIters[i] += 1;
    }
    ++phiIter;
    ++resultsIter;
    ++sampleIterator;
  }

}

#endif
