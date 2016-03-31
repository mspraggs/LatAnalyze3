/*
 * XYSampleData.hpp, part of LatAnalyze 3
 *
 * Copyright (C) 2013 - 2016 Antonin Portelli
 *
 * LatAnalyze 3 is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * LatAnalyze 3 is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with LatAnalyze 3.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef Latan_XYSampleData_hpp_
#define Latan_XYSampleData_hpp_

#include <LatAnalyze/Global.hpp>
#include <LatAnalyze/FitInterface.hpp>
#include <LatAnalyze/Minimizer.hpp>
#include <LatAnalyze/MatSample.hpp>
#include <LatAnalyze/Model.hpp>
#include <LatAnalyze/XYStatData.hpp>

BEGIN_LATAN_NAMESPACE

/******************************************************************************
 *                            object for fit result                           *
 ******************************************************************************/
class SampleFitResult: public DMatSample
{
    friend class XYSampleData;
public:
    // constructors
    SampleFitResult(void) = default;
    EIGEN_EXPR_CTOR(SampleFitResult, SampleFitResult, DMatSample, ArrayExpr)
    // destructor
    virtual ~SampleFitResult(void) = default;
    // access
    double                       getChi2(const Index s = central) const;
    const DSample &              getChi2(const PlaceHolder ph) const;
    double                       getChi2PerDof(const Index s = central) const;
    DSample                      getChi2PerDof(const PlaceHolder ph) const;
    double                       getNDof(void) const;
    Index                        getNPar(void) const;
    double                       getPValue(const Index s = central) const;
    const DoubleFunction &       getModel(const Index s = central,
                                          const Index j = 0) const;
    const DoubleFunctionSample & getModel(const PlaceHolder ph,
                                          const Index j = 0) const;
    FitResult                    getFitResult(const Index s = central) const;
    // IO
    void print(const bool printXsi = false,
               std::ostream &out = std::cout) const;
private:
    DSample                           chi2_;
    Index                             nDof_{0}, nPar_{0};
    std::vector<DoubleFunctionSample> model_;
    std::vector<std::string>          parName_;
};

/******************************************************************************
 *                              XYSampleData                                  *
 ******************************************************************************/
class XYSampleData: public FitInterface
{
public:
    // constructor
    explicit XYSampleData(const Index nSample);
    // destructor
    virtual ~XYSampleData(void) = default;
    // data access
    DSample &       x(const Index r, const Index i = 0);
    const DSample & x(const Index r, const Index i = 0) const;
    DSample &       y(const Index k, const Index j = 0);
    const DSample & y(const Index k, const Index j = 0) const;
    const DMat &    getXXVar(const Index i1, const Index i2);
    const DMat &    getYYVar(const Index j1, const Index j2);
    const DMat &    getXYVar(const Index i, const Index j);
    DVec            getXError(const Index i);
    DVec            getYError(const Index j);
    // get total fit variance matrix and its pseudo-inverse
    const DMat & getFitVarMat(void);
    const DMat & getFitVarMatPInv(void);
    // set data to a particular sample
    void setDataToSample(const Index s);
    // get internal XYStatData
    const XYStatData & getData(void);
    // fit
    SampleFitResult fit(Minimizer &minimizer, const DVec &init,
                        const std::vector<const DoubleModel *> &v);
    template <typename... Mods>
    SampleFitResult fit(Minimizer &minimizer, const DVec &init,
                        const DoubleModel &model, const Mods... models);
private:
    // schedule data initilization from samples
    void scheduleDataInit(void);
    // variance matrix computation
    void scheduleComputeVarMat(void);
    void computeVarMat(void);
    // create data
    virtual void createXData(const std::string name, const Index nData);
    virtual void createYData(const std::string name);
private:
    std::vector<std::map<Index, DSample>> yData_;
    std::vector<std::vector<DSample>>     xData_;
    XYStatData                            data_;
    Index                                 nSample_, dataSample_{central};
    bool                                  initData_{true}, computeVarMat_{true};
};

/******************************************************************************
 *                    XYSampleData template implementation                    *
 ******************************************************************************/
template <typename... Ts>
SampleFitResult XYSampleData::fit(Minimizer &minimizer, const DVec &init,
                                  const DoubleModel &model, const Ts... models)
{
    static_assert(static_or<std::is_assignable<DoubleModel &, Ts>::value...>::value,
                  "model arguments are not compatible with DoubleModel &");
    
    std::vector<const DoubleModel *> modelVector{&model, &models...};
    
    return fit(minimizer, init, modelVector);
}

END_LATAN_NAMESPACE

#endif // Latan_XYSampleData_hpp_
