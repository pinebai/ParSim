/*
 * The MIT License
 *
 * Copyright (c) 1997-2012 The University of Utah
 * Copyright (c) 2013-2014 Callaghan Innovation, New Zealand
 * Copyright (c) 2015-     Parresia Research Limited, New Zealand
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

#ifndef VAANGO_CCA_COMPONENTS_SCHEDULERS_DWDatabase_H
#define VAANGO_CCA_COMPONENTS_SCHEDULERS_DWDatabase_H

#include <CCA/Components/Schedulers/MemoryLog.h>
#include <CCA/Components/Schedulers/KeyDatabase.h>

#include <Core/Grid/UnknownVariable.h>
#include <Core/Grid/Variables/VarLabel.h>
#include <CCA/Components/Schedulers/MemoryLog.h>
#include <CCA/Components/Schedulers/DetailedTasks.h>
#include <Core/Grid/Variables/VarLabelMatl.h>
#include <Core/Grid/Variables/ScrubItem.h>
#include <Core/Grid/Variables/ReductionVariableBase.h>

#include <Core/Parallel/Parallel.h>

#include <Core/Containers/FastHashTable.h>
#include <Core/Exceptions/InternalError.h>
#include <Core/Malloc/Allocator.h>
#include <Core/Util/FancyAssert.h>

#include <map>
#include <vector>
#include <iosfwd>
#include <list>
#include <sstream>

#include <sci_hash_map.h>
namespace Uintah {

  using std::vector;
  using std::iostream;
  using std::ostringstream;
  using Uintah::FastHashTable;

  /**************************************
     
     CLASS
       DWDatabase
      
       Short Description...
      
     GENERAL INFORMATION
      
       DWDatabase.h
      
       Steven G. Parker
       Department of Computer Science
       University of Utah
      
       Center for the Simulation of Accidental Fires and Explosions (C-SAFE)
      
             
     KEYWORDS
       DWDatabase
      
     DESCRIPTION
       Long description...
      
     WARNING
      
  ****************************************/

  template<class DomainType>
  class DWDatabase {
  public:
    DWDatabase();
    ~DWDatabase();

    void clear();
   
    void doReserve(KeyDatabase<DomainType>* keydb);

    bool exists(const VarLabel* label, 
                int matlIndex, 
                const DomainType* dom) const;

    void put(const VarLabel* label, 
             int matlindex, 
             const DomainType* dom,
             Variable* var, 
             bool init,
             bool replace);

    void putReduce(const VarLabel* label,
                   int matlindex,
                   const DomainType* dom,
                   ReductionVariableBase* var,
                   bool init);

    void putForeign(const VarLabel* label, 
                    int matlindex, 
                    const DomainType* dom,
                    Variable* var,
                    bool init);

    void get(const VarLabel* label, 
             int matlindex, 
             const DomainType* dom,
             Variable& var) const;

    void getlist( const VarLabel* label, 
                  int matlIndex, 
                  const DomainType* dom,
                  vector<Variable*>& varlist ) const;

    inline Variable* get(const VarLabel* label, 
                         int matlindex,
                         const DomainType* dom) const;

    void print(ostream&, int rank) const;

    void cleanForeign();

    // Scrub counter manipulator functions -- when the scrub count goes to
    // zero, the data is scrubbed.  Return remaining count

    // How Scrubbing works:  If a variable is in the OldDW, at the beginning of the timestep
    // initializeScrubs will be called for each of those variables.  For each variable computed 
    // or copied via MPI, setScrubCount will be called on it, based on the scrubCountTable in
    // DetailedTasks.  Then, when the variable is used, decrementScrubCount is called on it
    // and if the count reaches zero, it is scrubbed.
    int decrementScrubCount(const VarLabel* label, 
                            int matlindex,
                            const DomainType* dom);

    void setScrubCount(const VarLabel* label, 
                       int matlindex,
                       const DomainType* dom, 
                       int count);

    void scrub(const VarLabel* label, 
               int matlindex, 
               const DomainType* dom);
   
    // add means increment the scrub count instead of setting it.  This is for when a DW
    // can act as a CoarseOldDW as well as an OldDW
    void initializeScrubs(int dwid, 
                          const FastHashTable<ScrubItem>* scrubcounts, 
                          bool add);

    void logMemoryUse(ostream& out, 
                      unsigned long& total,
                      const std::string& tag, 
                      int dwid);

    void getVarLabelMatlTriples(vector<VarLabelMatl<DomainType> >& vars) const;


  private:
    struct DataItem {
      DataItem() : var(0), next(0) {}

      ~DataItem()
        {
          if (next)
            delete next;
          ASSERT(var);
          delete var;
        }
      
      Variable* var;
      struct DataItem *next;
    };
  
    DataItem* getDataItem(const VarLabel* label, 
                          int matlindex,
                          const DomainType* dom) const;
    
    KeyDatabase<DomainType>* keys;
    typedef vector<DataItem*> varDBtype;
    varDBtype vars;
    typedef std::vector<int> scrubDBtype;
    scrubDBtype scrubs;

    DWDatabase(const DWDatabase&);
    DWDatabase& operator=(const DWDatabase&);      
  };

  template<class DomainType>
  DWDatabase<DomainType>::DWDatabase()
  {
  }

  template<class DomainType>
  DWDatabase<DomainType>::~DWDatabase()
  {
    clear();
  }

  template<class DomainType>
  void DWDatabase<DomainType>::clear()
  {
    for(auto iter = vars.begin(); iter != vars.end(); iter++){
      if (*iter) delete *iter;
      *iter=0;
    }
    vars.clear();
  }

  template<class DomainType>
  void
  DWDatabase<DomainType>::cleanForeign()
  {
    for (typename varDBtype::iterator iter = vars.begin(); iter != vars.end(); ++iter) {
      if (*iter && (*iter)->var->isForeign()) {
        delete (*iter);
        (*iter) = 0;
      }
    }
  }

  template<class DomainType>
  int DWDatabase<DomainType>::
  decrementScrubCount(const VarLabel* label, int matlIndex, const DomainType* dom)
  {
    // Dav's conjectures on how this works:
    //   setScrubCount is called the first time with "count" set to some X.  
    //   This X represents the number of tasks that will use the var.  Later,
    //   after a task has used the var, it will call decrementScrubCount
    //   If scrubCount then is equal to 0, the var is scrubbed.
 
    ASSERT(matlIndex >= -1);
    int idx = keys->lookup(label, matlIndex, dom);
    if (idx == -1) {
      return 0;
    }
    if (!vars[idx]) {
      return 0;
    }
    int rt = __sync_sub_and_fetch(&(scrubs[idx]), 1);
    if (rt == 0) {
      delete vars[idx];
      vars[idx] = 0;
    }
    return rt;
  }

  template<class DomainType>
  void DWDatabase<DomainType>::
  setScrubCount(const VarLabel* label, int matlIndex, const DomainType* dom, int count)
  {
    int idx = keys->lookup(label, matlIndex, dom);
    if (idx == -1) {
      SCI_THROW(UnknownVariable(label->getName(), -99, dom, matlIndex, "DWDatabase::setScrubCount", __FILE__, __LINE__));
    }
    scrubs[idx] = count;
  }

  template<class DomainType>
  void
  DWDatabase<DomainType>::scrub(const VarLabel* label, int matlIndex, const DomainType* dom)
  {
    ASSERT(matlIndex >= -1);
    int idx = keys->lookup(label, matlIndex, dom);
    // VarLabelMatl<DomainType> v(label, matlIndex, getRealDomain(dom));
    if (idx != -1 && vars[idx]) {
      delete vars[idx];
      vars[idx] = 0;
    }
  }

  template<class DomainType>
  void
  DWDatabase<DomainType>::initializeScrubs(int dwid, const FastHashTable<ScrubItem>* scrubcounts, bool add)
  {
    // loop over each variable, probing the scrubcount map. Set the
    // scrubcount appropriately.  if the variable has no entry in
    // the scrubcount map, delete it
    for (auto keyiter = keys->keys.begin(); keyiter != keys->keys.end();) {
      if (vars[keyiter->second]) {
        VarLabelMatl<DomainType> vlm = keyiter->first;
        // See if it is in the scrubcounts map.
        ScrubItem key(vlm.label_, vlm.matlIndex_, vlm.domain_, dwid);
        ScrubItem* result = scrubcounts->lookup(&key);
        if (!result && !add) {
          delete vars[keyiter->second];
          vars[keyiter->second] = 0;

          // TODO do we need this - APH 03/20/15
          //leave a hole in the map instead of erase, read only operation 
          //vars.erase(variter++);
        }
        else {
          if (result) {
            if (add)
              __sync_add_and_fetch(&(scrubs[keyiter->second]), result->count);
            else {
              if (!__sync_bool_compare_and_swap(&(scrubs[keyiter->second]), 0, result->count)) {
                SCI_THROW(InternalError("initializing non-zero scrub counter", __FILE__, __LINE__));
              }
            }
          }
          keyiter++;
        }
      }
      else {
        keyiter++;
      }
    }
  }


//______________________________________________________________________
//
  template<class DomainType>
  void DWDatabase<DomainType>::doReserve(KeyDatabase<DomainType>* keydb)
  {
    keys = keydb;
    vars.resize(keys->keycount + 1, (DataItem*)0);
    scrubs.resize(keys->keycount + 1, 0);
  }

  template<class DomainType>
  bool DWDatabase<DomainType>::exists(const VarLabel* label, int matlIndex, const DomainType* dom) const
  {
    int idx = keys->lookup(label, matlIndex, dom);
    if (idx == -1) {
      return false;
    }
    if (vars[idx] == 0) {
      return false;
    }
    return true;
  }

  template<class DomainType>
  void
  DWDatabase<DomainType>::put( const VarLabel* label, int matlIndex,const DomainType* dom,
                               Variable* var, bool init, bool replace )
  {
    ASSERT(matlIndex >= -1);

    if (init) {
      keys->insert(label, matlIndex, dom);
      this->doReserve(keys);
    }
    int idx = keys->lookup(label, matlIndex, dom);

    if (idx == -1) {
      SCI_THROW(UnknownVariable(label->getName(), -1, dom, matlIndex, "check task computes", __FILE__, __LINE__));
    }
    if (vars[idx]) {
      if (vars[idx]->next) {
        SCI_THROW(InternalError("More than one vars on this label", __FILE__, __LINE__));
      }
      if (!replace) {
        SCI_THROW(InternalError("Put replacing old vars", __FILE__, __LINE__));
      }
      ASSERT(vars[idx]->var != var);
      delete vars[idx];
    }
    DataItem* newdi = new DataItem();
    newdi->var = var;
    vars[idx] = newdi;
  }

//______________________________________________________________________
//
  template<class DomainType>
  void
  DWDatabase<DomainType>::putReduce( const VarLabel* label, int matlIndex,const DomainType* dom,
                                     ReductionVariableBase* var, bool init)
  {
    ASSERT(matlIndex >= -1);

    if (init) {
      keys->insert(label, matlIndex, dom);
      this->doReserve(keys);
    }
    int idx = keys->lookup(label, matlIndex, dom);

    if (idx == -1) {
      SCI_THROW(UnknownVariable(label->getName(), -1, dom, matlIndex, "check task computes", __FILE__, __LINE__));
    }
    DataItem* newdi = new DataItem();
    newdi->var = var;
    do {
      DataItem* olddi = __sync_lock_test_and_set(&vars[idx], 0);
      if (olddi == 0) {
        olddi = newdi;
      }
      else {
        ReductionVariableBase* oldvar = dynamic_cast<ReductionVariableBase*>(olddi->var);
        ReductionVariableBase* newvar = dynamic_cast<ReductionVariableBase*>(newdi->var);
        oldvar->reduce(*newvar);
        delete newdi;
      }
      newdi = __sync_lock_test_and_set(&vars[idx], olddi);
    }
    while (newdi != 0);
  }

  template<class DomainType>
  void
  DWDatabase<DomainType>::putForeign( const VarLabel* label, int matlIndex,const DomainType* dom,
                                      Variable* var, bool init)
  {
    ASSERT(matlIndex >= -1);
    if (init) {
      keys->insert(label, matlIndex, dom);
      this->doReserve(keys);
    }
    int idx = keys->lookup(label, matlIndex, dom);

    DataItem* newdi = new DataItem();
    newdi->var = var;
    if (idx == -1) {
      SCI_THROW(UnknownVariable(label->getName(), -1, dom, matlIndex, "check task computes", __FILE__, __LINE__));
    }
    do {
      newdi->next = vars[idx];
    }
    while (!__sync_bool_compare_and_swap(&vars[idx], newdi->next, newdi));  // vars[iter->second] = newdi;
  }

//______________________________________________________________________
//
  template<class DomainType>
  typename DWDatabase<DomainType>::DataItem*
  DWDatabase<DomainType>::getDataItem( const VarLabel* label, int matlIndex, const DomainType* dom ) const
  {
    ASSERT(matlIndex >= -1);
    int idx = keys->lookup(label, matlIndex, dom);
    if (idx == -1) {
      SCI_THROW(UnknownVariable(label->getName(), -99, dom, matlIndex, "DWDatabase::getDataItem", __FILE__, __LINE__));
    }
    return vars[idx];
  }

  template<class DomainType>
  inline
  Variable*
  DWDatabase<DomainType>::get( const VarLabel* label, int matlIndex, const DomainType* dom ) const
  {
    const DataItem* dataItem = getDataItem(label, matlIndex, dom);
    ASSERT(dataItem != 0);        // should have thrown an exception before
    ASSERT(dataItem->next == 0);  //should call getlist
    return dataItem->var;
  }

  template<class DomainType>
  void
  DWDatabase<DomainType>::get( const VarLabel* label,
                               int matlIndex,
                               const DomainType* dom,
                               Variable& var ) const
  {
    Variable* tmp = get(label, matlIndex, dom);
    var.copyPointer(*tmp);
  }

  template<class DomainType>
  void
  DWDatabase<DomainType>::getlist( const VarLabel* label,
                                   int matlIndex,
                                   const DomainType* dom,
                                   vector<Variable*>& varlist ) const
  {
    for (DataItem* dataItem = getDataItem(label, matlIndex, dom);dataItem!=0; dataItem=dataItem->next){
      varlist.push_back(dataItem->var);
    }
  }

  template<class DomainType>
  void DWDatabase<DomainType>::print(std::ostream& out, int rank) const
  {
    for (auto keyiter = keys->keys.begin(); keyiter != keys->keys.end(); keyiter++) {
      if (vars[keyiter->second]) {
        const VarLabelMatl<DomainType>& vlm = keyiter->first;
        const DomainType*  dom = vlm.domain_;
        if(dom){
          out << rank << " Name: " << vlm.label_->getName() << "  domain: " << *dom << "  matl:" << vlm.matlIndex_<< '\n';
        }else{
          out << rank << " Name: " << vlm.label_->getName() << "  domain: N/A  matl: " << vlm.matlIndex_<< '\n';
        }
      }
    }
  }

  template<class DomainType>
  void
  DWDatabase<DomainType>::logMemoryUse(ostream& out, unsigned long& total, const std::string& tag, int dwid)
  {
    for (auto keyiter = keys->keys.begin(); keyiter != keys->keys.end(); keyiter++) {
      if (vars[keyiter->second]) {
        Variable* var = vars[keyiter->second]->var;
        VarLabelMatl<DomainType> vlm = keyiter->first;
        const VarLabel* label = vlm.label_;
        string elems;
        unsigned long totsize;
        void* ptr;
        var->getSizeInfo(elems, totsize, ptr);
        const TypeDescription* td = label->typeDescription();
        logMemory(out, total, tag, label->getName(), (td?td->getName():"-"),
                  vlm.domain_, vlm.matlIndex_, elems, totsize, ptr, dwid);
      }
    }
  }

  template<class DomainType>
  void
  DWDatabase<DomainType>::getVarLabelMatlTriples( vector<VarLabelMatl<DomainType> >& v ) const
  {
    for (auto keyiter = keys->keys.begin(); keyiter != keys->keys.end(); keyiter++) {
      const VarLabelMatl<DomainType>& vlm = keyiter->first;
      if (vars[keyiter->second]) {
        v.push_back(vlm);
      }
    }
  }

} // End namespace Uintah

#endif
