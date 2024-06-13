//
// Created by Nadrino on 01/09/2020.
//

#ifndef CPP_GENERIC_TOOLBOX_ROOT_H
#define CPP_GENERIC_TOOLBOX_ROOT_H

// GenericToolbox
#include "GenericToolbox.Vector.h"
#include "GenericToolbox.Utils.h"
#include "GenericToolbox.Fs.h"

// ROOT Headers
#include "TMatrixDSymEigen.h"
#include "TTreeFormula.h"
#include "TPaletteAxis.h"
#include "TMatrixDSym.h"
#include "TDecompChol.h"
#include "TRandom3.h"
#include "TMatrixD.h"
#include "TFormula.h"
#include "TCanvas.h"
#include "TSpline.h"
#include "TGlobal.h"
#include "TGraph.h"
#include "TStyle.h"
#include "TFrame.h"
#include "TMath.h"
#include "TTree.h"
#include "TROOT.h"
#include "TFile.h"
#include "TAxis.h"
#include "TH2D.h"
#include "TH1D.h"
#include "TEnv.h"

// STD Headers
#include <string>
#include <map>

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-function"
#pragma GCC diagnostic ignored "-Wunused-variable"



//! User Parameters
//! (CAVEAT: only set for a given source file)
namespace GenericToolbox{
  namespace Parameters{
    static int _verboseLevel_ = 0;
  }
}

namespace GenericToolbox{

  const std::vector<Color_t> defaultColorWheel = {
      kGreen-3, kTeal+3, kAzure+7,
      kCyan-2, kBlue-7, kBlue+2,
      kOrange-3, kOrange+9, kRed+2,
      kPink+9, kViolet, kGreen-8,
      kCyan+1, kOrange-4, kOrange+6,
      kMagenta-10, kCyan-9, kGreen-10
  };

}

namespace GenericToolbox{

  //! Conversion Tools
  inline TH1D* convertToTH1D(const TVectorD *yValuesPtr_, const std::string &histTitle_ = "", const std::string &yTitle_ = "", const std::string &xTitle_ = "Entry #", TVectorD *yErrorsPtr_ = nullptr);
  inline TH1D* convertToTH1D(const std::vector<double> &Y_values_, const std::string &histTitle_ = "", const std::string &Y_title_ = "", const std::string &X_title_ = "Entry #", TVectorD *Y_errors_ = nullptr);
  inline TH2D* convertToTH2D(const TMatrixD *XY_values_, std::string graph_title_ = "", const std::string &Z_title_ = "", const std::string &Y_title_ = "Row #", const std::string &X_title_ = "Col #");
  inline TH2D* convertToTH2D(const TMatrixDSym *XY_values_, const std::string& graph_title_ = "", const std::string &Z_title_ = "", const std::string &Y_title_ = "Row #", const std::string &X_title_ = "Col #");
  template<typename T> inline TVectorT<T>* convertToTVector(const std::vector<T>& vector_);

  // Deprecated calls (kept for compatibility):
  inline TH1D* convertTVectorDtoTH1D(const TVectorD *yValuesPtr_, const std::string &histTitle_ = "", const std::string &yTitle_ = "", const std::string &xTitle_ = "Entry #", TVectorD *yErrorsPtr_ = nullptr);
  inline TH1D* convertTVectorDtoTH1D(const std::vector<double> &Y_values_, const std::string &histTitle_ = "", const std::string &Y_title_ = "", const std::string &X_title_ = "Entry #", TVectorD *Y_errors_ = nullptr);
  inline TH2D* convertTMatrixDtoTH2D(const TMatrixD *XY_values_, std::string graph_title_ = "", const std::string &Z_title_ = "", const std::string &Y_title_ = "Row #", const std::string &X_title_ = "Col #");
  inline TH2D* convertTMatrixDtoTH2D(const TMatrixDSym *XY_values_, std::string graph_title_ = "", const std::string &Z_title_ = "", const std::string &Y_title_ = "Row #", const std::string &X_title_ = "Col #");
  inline TVectorD* convertStdVectorToTVectorD(const std::vector<double> &vect_);
  inline TMatrixDSym* convertToSymmetricMatrix(TMatrixD* matrix_);
  inline TMatrixDSym* convertToSymmetricMatrix(const TMatrixD* matrix_);
  inline TMatrixD* convertToCorrelationMatrix(TMatrixD* covarianceMatrix_);

  //! Formula Tools
  inline TFormula* convertToFormula(TTreeFormula* treeFormula_);
  inline std::vector<std::string> getFormulaEffectiveParameterNameList(TFormula* formula_);
  inline std::vector<std::vector<int>> fetchParameterIndexes(TFormula* formula_);
  inline TTreeFormula* createTreeFormulaWithoutTree(const std::string& formulaStr_, std::vector<std::string> expectedLeafNames_);
  inline bool doesEntryPassCut(TTreeFormula* treeFormula_);
  inline void enableSelectedBranches(TTree* tree_, TTreeFormula* formula_);

  //! Files Tools
  inline TFile* openExistingTFile(const std::string &inputFilePath_, const std::vector<std::string>& objectListToCheck_ = {});
  inline bool doesTFileIsValid(const std::string &inputFilePath_, const std::vector<std::string>& objectListToCheck_ = {});
  inline bool doesTFileIsValid(TFile* tfileCandidatePtr_, bool check_if_writable_ = false);
  inline std::vector<TFile*> getListOfOpenedTFiles();
  inline TDirectory* mkdirTFile(TDirectory* baseDir_, const std::string &dirName_);
  inline TDirectory* mkdirTFile(TFile* outputFile_, const std::string &dirName_);
  inline TDirectory* getCurrentTDirectory();
  inline void writeInTFile(TDirectory* dir_, const TObject* objToSave_, std::string saveName_ = "", bool forceWriteFile_=false);
  inline void writeInTFile(TDirectory* dir_, const TObject& objToSave_, std::string saveName_ = "", bool forceWriteFile_=false);
  inline void triggerTFileWrite(TDirectory* dir_);

  inline std::vector<std::string> lsTDirectory(TDirectory* directory_, const std::string& className_ = "");
  inline std::vector<std::string> lsSubDirTDirectory(TDirectory* directory_);
  inline std::vector<TObject*> getListOfObjectFromTDirectory(TDirectory* directory_, const std::string &className_ = "", bool cloneObj_=false);
  template<typename T> std::vector<T*> getObjectList(TDirectory* directory_, bool cloneObj_ = false);

  //! Trees Tools
  inline void disableUnhookedBranches(TTree* tree_);
  inline std::vector<TLeaf*> getEnabledLeavesList(TTree* tree_, bool includeArrayLeaves_ = true);
  inline TVectorD* generateMeanVectorOfTree(TTree* tree_, bool showProgressBar_ = false);
  inline TMatrixD* generateCovarianceMatrixOfTree(TTree* tree_, bool showProgressBar_ = false, TVectorD* meanValueLeafList_ = nullptr);
  inline std::string generateCleanBranchName(const std::string& name_);
  inline void generateCompareTree(TTree* tree1_, TTree* tree2_, TDirectory* outDir_);

  //! Matrix Tools
  inline std::map<std::string, TMatrixD*> invertMatrixSVD(TMatrixD *matrix_, const std::string &outputContent_= "inverse_covariance_matrix:regularized_eigen_values");
  inline std::vector<double> getEigenValues(TMatrixD *matrix_);
  inline TMatrixD* getCholeskyMatrix(TMatrixD* covMatrix_);
  inline TMatrixD* getCholeskyMatrix(TMatrixDSym* covMatrix_);
  inline std::vector<double> throwCorrelatedParameters(TMatrixD* choleskyCovMatrix_);
  inline void throwCorrelatedParameters(TMatrixD* choleskyCovMatrix_, std::vector<double>& thrownParListOut_);
//  inline TMatrixD* computeSqrt(TMatrixD* inputMatrix_);
  inline TMatrixD* getOuterProduct(TVectorD* v_, TVectorD* w_ = nullptr);
  template<typename T> inline void transformMatrix(TMatrixT<T>* m_, std::function<void(TMatrixT<T>*, int, int)> transformFunction_);
  template<typename T> inline TMatrixT<T>* makeIdentityMatrix(int dim_);
  template<typename T> inline TMatrixT<T>* makeDiagonalMatrix(TVectorT<T>* v_);

  template<typename T> inline TVectorT<T>* getMatrixDiagonal(TMatrixT<T>* m_);
  template<typename T> inline TVectorT<T>* getMatrixDiagonal(TMatrixTSym<T>* m_);
  template<typename T> inline TVectorT<T>* getMatrixLine(TMatrixT<T>* m_, int line_);
  template<typename T> inline TVectorT<T>* getMatrixColumn(TMatrixT<T>* m_, int col_);

  //! Histogram Tools
  inline void drawHistHorizontalBars(TH1D* hist_);
  inline void resetHistogram(TH1D* hist_);
  inline void rescalePerBinWidth(TH1D* hist_, double globalScaler_ = 1);
  inline void transformBinContent(TH1D* hist_, const std::function<void(TH1D*, int)>& transformFunction_, bool processOverflowBins_ = false);
  inline std::pair<double, double> fetchYRange(TH1* h_, bool withError_ = true, const std::pair<double, double>& caps_ = {0., std::nan("unset")});
  inline std::pair<double, double> getYBounds(TH1* h_, const std::pair<double, double>& margins_ = {0.1, 0.25});
  inline std::pair<double, double> getYBounds(const std::vector<TH1*>& h_, const std::pair<double, double>& margins_ = {0.1, 0.25});
  inline std::vector<double> getLogBinning(int n_bins_, double X_min_, double X_max_);
  inline std::vector<double> getLinearBinning(int n_bins_, double X_min_, double X_max_);
  inline TH1D* getTH1DlogBinning(const std::string &name_, const std::string &title_, int n_bins_, double X_min_, double X_max_);
  inline TH2D* getTH2DlogBinning(const std::string &name_, const std::string &title_, int nb_X_bins_, double X_min_, double X_max_,
                                 int nb_Y_bins_, double Y_min_, double Y_max_, std::string log_axis_= "XY");
  template <class T> inline double getHistogramFwhm(TH1* hist_, int binMin_ = 1, int binMax_ = -1);


  //! Spline / Graph tools
  inline bool isFlatAndOne(const TGraph* graph_);
  inline bool isFlatAndOne(const TSpline3* spline_);
  inline bool hasUniformlySpacedKnots(const TGraph* graph_, double tolerance_ = 1E-6);

  //! Canvas Tools
  inline void setDefaultPalette();
  inline void setBlueRedPalette();
  inline void setT2kPalette();
  inline void setOrangePalette();
  inline void fixTH2display(TH2 *histogram_);
  inline void setXaxisOfAllPads(TCanvas* canvas_, double Xmin_, double Xmax_);


  //! ROOT Internals
  inline void muteRoot();
  inline void unmuteRoot();

  inline char findOriginalVariableType(const AnyType& obj_);
  inline AnyType leafToAnyType(const std::string& leafTypeName_);
  inline AnyType leafToAnyType(const TLeaf* leaf_);
  inline void leafToAnyType(const TLeaf* leaf_, AnyType& out_);
  inline void leafToAnyType(const std::string& leafTypeName_, AnyType& out_);

  // LinkDef does not enable function declaration by itself.
  // A proxy class can be used to trigger the function declaration on CINT startup
  struct Enabler{};

}



//! Conversion Tools
namespace GenericToolbox {

  inline TH1D* convertTVectorDtoTH1D(const TVectorD* yValuesPtr_, const std::string &histTitle_, const std::string &yTitle_,
                                     const std::string &xTitle_, TVectorD* yErrorsPtr_){

    auto* th1_histogram = new TH1D(histTitle_.c_str(), histTitle_.c_str(),
                                   yValuesPtr_->GetNrows(), -0.5, yValuesPtr_->GetNrows() - 0.5);

    for(int i_row = 0; i_row < yValuesPtr_->GetNrows(); i_row++)
    {
      th1_histogram->SetBinContent(i_row + 1, (*yValuesPtr_)[i_row]);
      if(yErrorsPtr_ != nullptr)
        th1_histogram->SetBinError(i_row + 1, (*yErrorsPtr_)[i_row]);
    }

    th1_histogram->SetLineWidth(2);
    th1_histogram->SetLineColor(kBlue);
    th1_histogram->GetXaxis()->SetTitle(xTitle_.c_str());
    th1_histogram->GetYaxis()->SetTitle(yTitle_.c_str());

    return th1_histogram;
  }
  inline TH1D* convertTVectorDtoTH1D(const std::vector<double> &Y_values_, const std::string &histTitle_, const std::string &Y_title_, const std::string &X_title_, TVectorD *Y_errors_){
    TH1D* out = nullptr;
    auto* tVectorHandler = new TVectorD(int(Y_values_.size()), &Y_values_[0]);
    out = convertTVectorDtoTH1D(tVectorHandler, histTitle_, Y_title_, X_title_, Y_errors_);
    delete tVectorHandler;
    return out;
  }
  inline TH2D* convertTMatrixDtoTH2D(const TMatrixD* XY_values_, std::string graph_title_, const std::string &Z_title_,
                                     const std::string &Y_title_, const std::string &X_title_){

    if(graph_title_.empty()){
      graph_title_ = XY_values_->GetTitle();
    }

    auto* th2_histogram = new TH2D(graph_title_.c_str(), graph_title_.c_str(),
                                   XY_values_->GetNrows(), -0.5, XY_values_->GetNrows() - 0.5,
                                   XY_values_->GetNcols(), -0.5, XY_values_->GetNcols() - 0.5);

    for(int i_col = 0; i_col < XY_values_->GetNcols(); i_col++)
    {
      for(int j_row = 0; j_row < XY_values_->GetNrows(); j_row++)
      {
        th2_histogram->SetBinContent(i_col + 1, j_row + 1, (*XY_values_)[i_col][j_row]);
      }
    }

    th2_histogram->GetXaxis()->SetTitle(X_title_.c_str());
    th2_histogram->GetYaxis()->SetTitle(Y_title_.c_str());
    th2_histogram->GetZaxis()->SetTitle(Z_title_.c_str());

    return th2_histogram;
  }
  inline TH2D* convertTMatrixDtoTH2D(const TMatrixDSym* XY_values_, std::string graph_title_, const std::string &Z_title_,
                                     const std::string &Y_title_, const std::string &X_title_){
    return convertTMatrixDtoTH2D((TMatrixD*) XY_values_, std::move(graph_title_), Z_title_, Y_title_, X_title_);
  }
  inline TVectorD *convertStdVectorToTVectorD(const std::vector<double> &vect_){

    auto *output = new TVectorD(int(vect_.size()));
    for(int i = 0 ; i < int(vect_.size()) ; i++){
      (*output)[i] = vect_[i];
    }
    return output;

  }
  inline TMatrixDSym *convertToSymmetricMatrix(const TMatrixD *matrix_) {

    auto *symmetric_matrix = (TMatrixD *) matrix_->Clone();
    auto *transposed_symmetric_matrix = new TMatrixD(*matrix_);

    transposed_symmetric_matrix->Transpose(*matrix_);
    *symmetric_matrix += *transposed_symmetric_matrix;
    for (int i_col = 0; i_col < matrix_->GetNcols(); i_col++) {
      for (int i_row = 0; i_row < matrix_->GetNrows(); i_row++) {
        (*symmetric_matrix)[i_row][i_col] /= 2.;
      }
    }

    auto *result = (TMatrixDSym *) symmetric_matrix->Clone(); // Convert to TMatrixDSym

    delete transposed_symmetric_matrix;
    delete symmetric_matrix;

    return result;
  }
  inline TMatrixDSym *convertToSymmetricMatrix(TMatrixD *matrix_){
    return convertToSymmetricMatrix((const TMatrixD*) matrix_);
  }
  inline TMatrixD* convertToCorrelationMatrix(TMatrixD* covarianceMatrix_){
    if(covarianceMatrix_ == nullptr) return nullptr;
    if(covarianceMatrix_->GetNrows() != covarianceMatrix_->GetNcols()) return nullptr;

    auto* correlationMatrix = (TMatrixD*) covarianceMatrix_->Clone();

    for(int iRow = 0 ; iRow < covarianceMatrix_->GetNrows() ; iRow++){
      for(int iCol = 0 ; iCol < covarianceMatrix_->GetNcols() ; iCol++){

        if(   (*covarianceMatrix_)[iRow][iRow] == 0
              or (*covarianceMatrix_)[iCol][iCol] == 0 ){
          (*correlationMatrix)[iRow][iCol] = 0;
        }
        else{
          (*correlationMatrix)[iRow][iCol] /=
              TMath::Sqrt((*covarianceMatrix_)[iRow][iRow]*(*covarianceMatrix_)[iCol][iCol]);
        }

      }
    }

    return correlationMatrix;
  }

}

//! Formula Tools
namespace GenericToolbox {

  inline TFormula* convertToFormula(TTreeFormula* treeFormula_){
    if( treeFormula_ == nullptr ) return nullptr;

    // Grab the appearing leaf names
    std::vector<std::string> leafNameList;
    for( int iLeaf = 0 ; iLeaf < treeFormula_->GetNcodes() ; iLeaf++ ){
      if( not isIn(treeFormula_->GetLeaf(iLeaf)->GetName(), leafNameList)){
        leafNameList.emplace_back(treeFormula_->GetLeaf(iLeaf)->GetName());
      }
    }

    // Make sure the longest leaves appear in the list first
    std::sort(leafNameList.begin(), leafNameList.end(), []
        (const std::string& first, const std::string& second){
      return first.size() > second.size();
    });

    std::vector<std::string> expressionBrokenDown;
    std::vector<bool> isReplacedElement;
    expressionBrokenDown.emplace_back(treeFormula_->GetExpFormula().Data());
    isReplacedElement.push_back(false);

    // Replace in the expression
    for( const auto& leafName : leafNameList ){

      // Defining sub pieces
      std::vector<std::vector<std::string>> expressionBreakDownUpdate(expressionBrokenDown.size(), std::vector<std::string>());
      std::vector<std::vector<bool>> isReplacedElementUpdate(isReplacedElement.size(), std::vector<bool>());

      int nExpr = int(expressionBrokenDown.size());
      for( int iExpr = nExpr-1 ; iExpr >= 0 ; iExpr-- ){

        if( isReplacedElement[iExpr] ){
          // Already processed
          continue;
        }

        if( not GenericToolbox::hasSubStr(expressionBrokenDown[iExpr], leafName) ){
          // Leaf is not present in this chunk
          continue;
        }
        // Here, we know the leaf appear at least once

        // Adding update pieces
        expressionBreakDownUpdate.at(iExpr) = splitString(expressionBrokenDown[iExpr], leafName);
        isReplacedElementUpdate.at(iExpr) = std::vector<bool>(expressionBreakDownUpdate.at(iExpr).size(), false);

        // Look for leaves called as arrays
        int nSubExpr = int(expressionBreakDownUpdate.at(iExpr).size());
        for( int iSubExpr = nSubExpr-1 ; iSubExpr >= 1 ; iSubExpr-- ){

          std::string leafExprToReplace = leafName;

          // Look for an opening "["
          if( expressionBreakDownUpdate.at(iExpr)[iSubExpr][0] == '[' ){
            // It is an array call!
            size_t iChar;
            for( iChar = 0 ; iChar < expressionBreakDownUpdate.at(iExpr)[iSubExpr].size() ; iChar++ ){
              leafExprToReplace += expressionBreakDownUpdate.at(iExpr)[iSubExpr][iChar];
              if( expressionBreakDownUpdate.at(iExpr)[iSubExpr][iChar] == ']' ){
                if( iChar+1 == expressionBreakDownUpdate.at(iExpr)[iSubExpr].size() or expressionBreakDownUpdate.at(iExpr)[iSubExpr][iChar+1] != '[' ){
                  // Ok, it's the end of the array
                  break;
                }
              }
            }

            std::string untouchedSubExpr;
            iChar++;
            for( ; iChar < expressionBreakDownUpdate.at(iExpr)[iSubExpr].size() ; iChar++ ){
              untouchedSubExpr += expressionBreakDownUpdate.at(iExpr)[iSubExpr][iChar];
            }
            expressionBreakDownUpdate.at(iExpr)[iSubExpr] = untouchedSubExpr;

            replaceSubstringInsideInputString(leafExprToReplace, "[", "(");
            replaceSubstringInsideInputString(leafExprToReplace, "]", ")");
          }
          else{
            // Not an array! We are good
          }

          insertInVector(expressionBreakDownUpdate.at(iExpr), "[" + leafExprToReplace + "]", iSubExpr);
          insertInVector(isReplacedElementUpdate.at(iExpr), true, iSubExpr);

        } // iSubExpr

        // Stripping empty elements
        for( int iSubExpr = nSubExpr-1 ; iSubExpr >= 0 ; iSubExpr-- ){
          if( expressionBreakDownUpdate.at(iExpr).at(iSubExpr).empty() ){
            expressionBreakDownUpdate.at(iExpr).erase(expressionBreakDownUpdate.at(iExpr).begin() + iSubExpr);
            isReplacedElementUpdate.at(iExpr).erase(isReplacedElementUpdate.at(iExpr).begin() + iSubExpr);
          }
        } // iSubExpr

        expressionBrokenDown.erase(expressionBrokenDown.begin() + iExpr);
        isReplacedElement.erase(isReplacedElement.begin() + iExpr);

        insertInVector(expressionBrokenDown, expressionBreakDownUpdate.at(iExpr), iExpr);
        insertInVector(isReplacedElement, isReplacedElementUpdate.at(iExpr), iExpr);

      } // iExpr

    } // Leaf

    std::string formulaStr = joinVectorString(expressionBrokenDown, "");

    return new TFormula(formulaStr.c_str(), formulaStr.c_str());

  }
  inline std::vector<std::string> getFormulaEffectiveParameterNameList(TFormula* formula_){
    std::vector<std::string> output;
    if( formula_ == nullptr ) return output;

    for( int iPar = 0 ; iPar < formula_->GetNpar() ; iPar++ ){
      output.emplace_back(splitString(formula_->GetParName(iPar), "(")[0]);
    }
    return output;
  }
  inline std::vector<std::vector<int>> fetchParameterIndexes(TFormula* formula_){
    std::vector<std::vector<int>> output;
    if(formula_ == nullptr) return output;

    for( int iPar = 0 ; iPar < formula_->GetNpar() ; iPar++ ){
      output.emplace_back();
      auto parCandidateSplit = splitString(formula_->GetParName(iPar), "(");

      if( parCandidateSplit.size() == 1 ){
        continue; // no index
      }
      else{
        // need to fetch the indices
        for( size_t iIndex = 1 ; iIndex < parCandidateSplit.size() ; iIndex++ ){
          std::string indexStr;
          for( char c : parCandidateSplit.at(iIndex) ){
            if( c == ')' ) break;
            indexStr += c;
          }
          output.back().emplace_back(std::stoi(indexStr));
        }
      }

    } // iPar

    return output;
  }
  inline TTreeFormula* createTreeFormulaWithoutTree(const std::string& formulaStr_, std::vector<std::string> expectedLeafNames_){
    auto* cwd = getCurrentTDirectory();
    ROOT::GetROOT()->cd();
    std::vector<Int_t> varObjList(expectedLeafNames_.size(),0);
    auto* fakeTree = new TTree("fakeTree", "fakeTree");
    for( size_t iVar = 0 ; iVar < expectedLeafNames_.size() ; iVar++ ){
      fakeTree->Branch(expectedLeafNames_.at(iVar).c_str(), &varObjList[iVar]);
    }
    fakeTree->Fill();
    auto* output = new TTreeFormula(formulaStr_.c_str(), formulaStr_.c_str(), fakeTree);
    output->SetTree(nullptr);
    delete fakeTree;
    cwd->cd();
    return output;
  }
  inline bool doesEntryPassCut(TTreeFormula* treeFormula_){
    // instances are distinct expressions which are separated with ";", for example: "var1 == 4; var2 == var3"
    // In practice, we never use multiple instance. In case we do, this algo will understand the ";" as "&&"
    for(int jInstance = 0; jInstance < treeFormula_->GetNdata(); jInstance++) {
      if ( treeFormula_->EvalInstance(jInstance) == 0 ) {
        return false;
        break;
      }
    }
    return true;
  }
  inline void enableSelectedBranches(TTree* tree_, TTreeFormula* formula_){
    for( int iLeaf = 0 ; iLeaf < formula_->GetNcodes() ; iLeaf++ ){
      if( formula_->GetLeaf(iLeaf) == nullptr ) continue; // for "Entry$" like dummy leaves
      tree_->SetBranchStatus(formula_->GetLeaf(iLeaf)->GetBranch()->GetName(), true);
    }
  }

}

//! Files Tools
namespace GenericToolbox {

  inline TFile* openExistingTFile(const std::string &inputFilePath_, const std::vector<std::string>& objectListToCheck_){
    TFile* outPtr{nullptr};

    if( not isFile(inputFilePath_) ){
      throw std::runtime_error("Could not find file: \"" + inputFilePath_ + "\"");
    }
    auto old_verbosity = gErrorIgnoreLevel;
    gErrorIgnoreLevel  = kFatal;
    gEnv->SetValue("TFile.Recover", 0);
    outPtr = TFile::Open(inputFilePath_.c_str(), "READ");
    gErrorIgnoreLevel = old_verbosity;

    if( not doesTFileIsValid(outPtr) ){
      throw std::runtime_error("Invalid TFile: \"" + inputFilePath_ + "\"");
    }

    for( const auto& objectPath : objectListToCheck_ ){
      if( outPtr->Get(objectPath.c_str()) == nullptr ){
        throw std::runtime_error("Could not find: \"" + objectPath + "\" in \"" + inputFilePath_ + "\"");
      }
    }

    return outPtr;
  }
  inline bool doesTFileIsValid(const std::string &inputFilePath_, const std::vector<std::string>& objectListToCheck_){
    bool fileIsValid = false;
    gEnv->SetValue("TFile.Recover", 0);
    if(isFile(inputFilePath_)) {
      auto old_verbosity = gErrorIgnoreLevel;
      gErrorIgnoreLevel  = kFatal;
      auto* tfileCandidatePtr  = TFile::Open(inputFilePath_.c_str(), "READ");
      if(doesTFileIsValid(tfileCandidatePtr)) {
        fileIsValid = true;
        for( const auto& objectPath : objectListToCheck_ ){
          if(tfileCandidatePtr->Get(objectPath.c_str()) == nullptr ){
            fileIsValid = false;
            break;
          }
        }
        tfileCandidatePtr->Close();
      }
      delete tfileCandidatePtr;
      gErrorIgnoreLevel = old_verbosity;
    }
    return fileIsValid;
  }
  inline bool doesTFileIsValid(TFile* tfileCandidatePtr_, bool check_if_writable_){

    if(tfileCandidatePtr_ == nullptr){
      if(Parameters::_verboseLevel_ >= 1)
        std::cout << "tfileCandidatePtr_ is a nullptr" << std::endl;
      return false;
    }

    if(not tfileCandidatePtr_->IsOpen()){
      if(Parameters::_verboseLevel_ >= 1)
        std::cout << "tfileCandidatePtr_ = " << tfileCandidatePtr_->GetName() << " is not opened."
                  << std::endl;
      if(Parameters::_verboseLevel_ >= 1)
        std::cout << "tfileCandidatePtr_->IsOpen() = " << tfileCandidatePtr_->IsOpen()
                  << std::endl;
      return false;
    }

    if( tfileCandidatePtr_->IsZombie() ){
      if(Parameters::_verboseLevel_ >= 1){
        std::cout << GET_VAR_NAME_VALUE(tfileCandidatePtr_->IsZombie()) << std::endl;
      }
      return false;
    }

    if( tfileCandidatePtr_->TestBit(TFile::kRecovered) ){
      if(Parameters::_verboseLevel_ >= 1){
        std::cout << GET_VAR_NAME_VALUE(tfileCandidatePtr_->TestBit(TFile::kRecovered)) << std::endl;
      }
      return false;
    }

    if(check_if_writable_ and not tfileCandidatePtr_->IsWritable()){
      if(Parameters::_verboseLevel_ >= 1)
        std::cout << "tfileCandidatePtr_ = " << tfileCandidatePtr_->GetName()
                  << " is not writable." << std::endl;
      if(Parameters::_verboseLevel_ >= 1)
        std::cout << "tfileCandidatePtr_->IsWritable() = " << tfileCandidatePtr_->IsWritable()
                  << std::endl;
      return false;
    }

    return true;
  }
  template<typename T> inline std::vector<T*> getObjectList(TDirectory* directory_, bool cloneObj_){
    if( directory_ == nullptr ) return {};

    TClass* templateClass = TClass::GetClass<T>();
    if( templateClass == nullptr ){
      throw std::runtime_error("invalid template class in getObjectList. Not TObject castable?");
    }

    auto ls = lsTDirectory(directory_, templateClass->GetName());
    std::vector<T*> output; output.reserve(ls.size());
    for( auto& entry : ls ){
      output.template emplace_back( directory_->Get<T>(entry.c_str()) );
      if( cloneObj_ ){ output.back() = (T*) output.back()->Clone(); }
    }

    delete templateClass;
    return output;
  }
  inline std::vector<TObject *> getListOfObjectFromTDirectory(TDirectory *directory_, const std::string &className_, bool cloneObj_) {
    return getObjectList<TObject>(directory_, cloneObj_);
  }
  inline std::vector<std::string> lsTDirectory(TDirectory* directory_, const std::string& className_){
    std::vector<std::string> output{};
    if( directory_ == nullptr ) return output;

    for (int iEntry = 0; iEntry < directory_->GetListOfKeys()->GetSize(); iEntry++) {
      std::string entryName{directory_->GetListOfKeys()->At(iEntry)->GetName()};
      if( className_.empty() or directory_->Get(entryName.c_str())->ClassName() == className_ ){
        output.emplace_back(entryName);
      }
    }

    return output;
  }
  inline std::vector<std::string> lsSubDirTDirectory(TDirectory* directory_){
    return lsTDirectory(directory_, "TDirectoryFile");
  }
  inline TDirectory* mkdirTFile(TDirectory* baseDir_, const std::string &dirName_){
    if( baseDir_ == nullptr ) return nullptr;
    if(baseDir_->GetDirectory(dirName_.c_str()) == nullptr){
      baseDir_->mkdir(dirName_.c_str());
    }
    return baseDir_->GetDirectory(dirName_.c_str());
  }
  inline TDirectory* mkdirTFile(TFile* outputFile_, const std::string &dirName_){
    return mkdirTFile(outputFile_->GetDirectory(""), dirName_);
  }
  inline std::vector<TFile *> getListOfOpenedTFiles() {
    std::vector<TFile *> output;
    // TIter next_iter(gROOT->GetListOfGlobals());
    auto *global_obj_list = (TList *) gROOT->GetListOfGlobals();
    TGlobal *global;
    for (int i_obj = 0; i_obj < global_obj_list->GetEntries(); i_obj++) {
      global = (TGlobal *) global_obj_list->At(i_obj);
      TString type = global->GetTypeName();
      if (type == "TFile") {
        auto *file = (TFile *) gInterpreter->Calc(global->GetName());
        if (file && file->IsOpen()) {
          // printf("%s: %s\n", global->GetName(),file->GetName());
          output.emplace_back(file);
        }
      }
    }
    // while ((global=(TGlobal*)next_iter())) {

    // }
    return output;
  }
  inline TDirectory* getCurrentTDirectory(){
#if ROOT_VERSION_CODE >= ROOT_VERSION(6,28,04)
    return gDirectory.fValue->load();
#elif ROOT_VERSION_CODE >= ROOT_VERSION(6,23,02)
    return gDirectory.fValue.load();
#else
    // Prior releases of ROOT, gDirectory was returning a TDirectory*
    // Implementation has been made on the 16 Dec 2020:
    // https://github.com/root-project/root/commit/085e9c182b9f639d5921c75de284ae7f20168b6e
    return gDirectory;
#endif
  }
  inline void writeInTFile(TDirectory* dir_, const TObject* objToSave_, std::string saveName_, bool forceWriteFile_){
    if( dir_ == nullptr or objToSave_ == nullptr ) return;

    // Object name:
    if( saveName_.empty() ) saveName_ = objToSave_->GetName();

    // Cleaning up object name
    saveName_ = generateCleanBranchName(saveName_);

    // Building custom extension:
    std::string className = objToSave_->ClassName();
    replaceSubstringInsideInputString(className, "<", "_");
    replaceSubstringInsideInputString(className, ">", "");
    if( className == "TMatrixT_double" ) className = "TMatrixD";
    else if( className == "TMatrixTSym_double" ) className = "TMatrixDSym";

    if( endsWith( saveName_, className ) ){
      // extension already included in the obj name
      dir_->WriteObject(objToSave_, Form("%s", saveName_.c_str()), "overwrite");
    }
    else{
      // add extension
      dir_->WriteObject(objToSave_, Form("%s_%s", saveName_.c_str(), className.c_str()), "overwrite");
    }


    // Force TFile Write?
    if( forceWriteFile_ ) triggerTFileWrite(dir_);
  }
  inline void writeInTFile(TDirectory* dir_, const TObject& objToSave_, std::string saveName_, bool forceWriteFile_){
    writeInTFile(dir_, &objToSave_, std::move(saveName_), forceWriteFile_);
  }
  inline void triggerTFileWrite(TDirectory* dir_){
    if( dir_->GetFile() != nullptr ) dir_->GetFile()->Write();
  }


}

//! Trees Tools
namespace GenericToolbox {

  inline void disableUnhookedBranches(TTree* tree_){
    if(tree_ == nullptr){
      std::cout << "ERROR in " << __METHOD_NAME__ << ": " << GET_VAR_NAME_VALUE(tree_) << std::endl;
      return;
    }
    tree_->SetBranchStatus("*", false);
    auto* branchList = tree_->GetListOfBranches();
    for( int iBranch = 0 ; iBranch < branchList->GetEntries() ; iBranch++ ){
      if( tree_->GetBranch( branchList->At(iBranch)->GetName() )->GetAddress() != nullptr ){
        tree_->SetBranchStatus( branchList->At(iBranch)->GetName(), true );
      }
    } // iBranch
  }
  inline std::vector<TLeaf*> getEnabledLeavesList(TTree* tree_, bool includeArrayLeaves_){
    std::vector<TLeaf*> leafList;

    leafList.reserve( tree_->GetListOfLeaves()->GetEntries() );
    for( int iBranch = 0 ; iBranch < tree_->GetListOfBranches()->GetEntries() ; iBranch++ ){

      auto* br{dynamic_cast<TBranch*>(tree_->GetListOfBranches()->At(iBranch))};
      if( br == nullptr or tree_->GetBranchStatus( br->GetName() ) != 1 ){ continue; }

      for( int iLeaf = 0 ; iLeaf < br->GetListOfLeaves()->GetEntries() ; iLeaf++ ){

        auto* lf{dynamic_cast<TLeaf*>( br->GetListOfLeaves()->At(iLeaf) )};
        if( lf == nullptr or lf->GetNdata() == 0 ){ continue; }

        if( includeArrayLeaves_ or lf->GetNdata() == 1 ){
          leafList.emplace_back( lf );
        }
        else{
          // DON'T SUPPORT ARRAYS AT THE MOMENT
          std::cout << __METHOD_NAME__
                    << ": " << tree_->GetListOfLeaves()->At(iLeaf)->GetName()
                    << " -> array leaves are not supported yet." << std::endl;
        }
      } // iLeaf
    } // iBranch

    return leafList;
  }
  inline TVectorD* generateMeanVectorOfTree(TTree* tree_, bool showProgressBar_){
    TVectorD* outMeanVector;
    std::vector<TLeaf*> leafList = getEnabledLeavesList(tree_, false);

    outMeanVector = new TVectorD(int(leafList.size()));
    for(int iLeaf = 0 ; iLeaf < outMeanVector->GetNrows() ; iLeaf++){ (*outMeanVector)[iLeaf] = 0; }

    Long64_t nEntries = tree_->GetEntries();
    for(Long64_t iEntry = 0 ; iEntry < nEntries ; iEntry++){
      if( showProgressBar_ ) displayProgressBar(iEntry, nEntries, "Compute mean of every variable");
      tree_->GetEntry(iEntry);
      for(int iLeaf = 0 ; iLeaf < outMeanVector->GetNrows() ; iLeaf++){
        (*outMeanVector)[iLeaf] += leafList[iLeaf]->GetValue(0);
      }
    }
    for(int iLeaf = 0 ; iLeaf < outMeanVector->GetNrows() ; iLeaf++){
      (*outMeanVector)[iLeaf] /= double(nEntries);
    }

    return outMeanVector;
  }
  inline TMatrixD* generateCovarianceMatrixOfTree(TTree* tree_, bool showProgressBar_, TVectorD* meanValueLeafList_){

    TMatrixD* outCovMatrix;

    // Generate covariance matrix of all ENABLED branches of the input tree
    std::vector<TLeaf*> leafList = getEnabledLeavesList(tree_, false);

    // Initializing the matrix
    outCovMatrix = new TMatrixD(int(leafList.size()), int(leafList.size()));
    for(int iCol = 0 ; iCol < leafList.size() ; iCol++){
      for(int iRow = 0 ; iRow < leafList.size() ; iRow++){
        (*outCovMatrix)[iCol][iRow] = 0;
      }
    }

    // Compute mean of every variable
    if( meanValueLeafList_ == nullptr ){
      meanValueLeafList_ = generateMeanVectorOfTree(tree_, showProgressBar_);
    }

    // Compute covariance
    Long64_t nEntries = tree_->GetEntries();
    for(Long64_t iEntry = 0 ; iEntry < nEntries ; iEntry++){
      if( showProgressBar_ ) displayProgressBar(iEntry, nEntries, "Compute covariance");
      tree_->GetEntry(iEntry);
      for(int iCol = 0 ; iCol < leafList.size() ; iCol++){
        for(int iRow = 0 ; iRow < leafList.size() ; iRow++){
          (*outCovMatrix)[iCol][iRow] +=
              (leafList[iCol]->GetValue(0) - (*meanValueLeafList_)[iCol])
              *(leafList[iRow]->GetValue(0) - (*meanValueLeafList_)[iRow]);
        } // iRow
      } // iCol
    } // iEntry
    for(int iCol = 0 ; iCol < leafList.size() ; iCol++){
      for(int iRow = 0 ; iRow < leafList.size() ; iRow++){
        (*outCovMatrix)[iCol][iRow] /= double(nEntries);
      }
    }

    return outCovMatrix;

  }
  inline std::string generateCleanBranchName(const std::string& name_){
    std::string out{name_};

    replaceSubstringInsideInputString(out, " ", "_");
    replaceSubstringInsideInputString(out, "-", "_");
    replaceSubstringInsideInputString(out, "/", "_");
    replaceSubstringInsideInputString(out, "<", "_");

    replaceSubstringInsideInputString(out, ">", "");
    replaceSubstringInsideInputString(out, "(", "");
    replaceSubstringInsideInputString(out, ")", "");
    replaceSubstringInsideInputString(out, "{", "");
    replaceSubstringInsideInputString(out, "}", "");
    replaceSubstringInsideInputString(out, "[", "");
    replaceSubstringInsideInputString(out, "]", "");
    replaceSubstringInsideInputString(out, "#", "");

    return out;
  }

  inline void generateCompareTree(TTree* tree1_, TTree* tree2_, TDirectory* outDir_){
//    if( tree1_ == nullptr ) throw std::runtime_error("tree1_ is nullptr");
//    if( tree2_ == nullptr ) throw std::runtime_error("tree2_ is nullptr");
//    if( outDir_ == nullptr ) throw std::runtime_error("outDir_ is nullptr");
//
////    TTree* outCompTree{nullptr};
//
//    auto* prevDir = getCurrentTDirectory();
//    outDir_->cd();
//
////    outCompTree = new TTree("outCompTree", "outCompTree");
//
//    ((TBranch*) tree1_->GetListOfBranches()->At(0))->GetAddress();
//
//    prevDir->cd();
  }

}

//! Matrix Tools
namespace GenericToolbox {

  inline std::map<std::string, TMatrixD *> invertMatrixSVD(TMatrixD *matrix_, const std::string &outputContent_) {
    std::map<std::string, TMatrixD *> results_handler;

    auto content_names = splitString(outputContent_, ":");

    if (std::find(content_names.begin(), content_names.end(), "inverse_covariance_matrix") != content_names.end()) {
      results_handler["inverse_covariance_matrix"]
          = new TMatrixD(matrix_->GetNrows(), matrix_->GetNcols());
    }
    if (std::find(content_names.begin(), content_names.end(), "regularized_covariance_matrix") != content_names.end()) {
      results_handler["regularized_covariance_matrix"]
          = new TMatrixD(matrix_->GetNrows(), matrix_->GetNcols());
    }
    if (std::find(content_names.begin(), content_names.end(), "projector") != content_names.end()) {
      results_handler["projector"]
          = new TMatrixD(matrix_->GetNrows(), matrix_->GetNcols());
    }
    if (std::find(content_names.begin(), content_names.end(), "regularized_eigen_values") != content_names.end()) {
      results_handler["regularized_eigen_values"]
          = new TMatrixD(matrix_->GetNrows(), 1);
    }

    // make sure all are 0
    for (const auto &matrix_handler : results_handler) {
      for (int i_dof = 0; i_dof < matrix_handler.second->GetNrows(); i_dof++) {
        for (int j_dof = 0; j_dof < matrix_handler.second->GetNcols(); j_dof++) {
          (*matrix_handler.second)[i_dof][j_dof] = 0.;
        }
      }
    }


    // Covariance matrices are symetric :
    auto *symmetric_matrix = convertToSymmetricMatrix(matrix_);
    auto *Eigen_matrix_decomposer = new TMatrixDSymEigen(*symmetric_matrix);
    auto *Eigen_values = &(Eigen_matrix_decomposer->GetEigenValues());
    auto *Eigen_vectors = &(Eigen_matrix_decomposer->GetEigenVectors());

    double max_eigen_value = (*Eigen_values)[0];
    for (int i_eigen_value = 0; i_eigen_value < matrix_->GetNcols(); i_eigen_value++) {
      if (max_eigen_value < (*Eigen_values)[i_eigen_value]) {
        max_eigen_value = (*Eigen_values)[i_eigen_value];
      }
    }

    for (int i_eigen_value = 0; i_eigen_value < matrix_->GetNcols(); i_eigen_value++) {
      if ((*Eigen_values)[i_eigen_value] > max_eigen_value * 1E-5) {
        if (results_handler.find("regularized_eigen_values") != results_handler.end()) {
          (*results_handler["regularized_eigen_values"])[i_eigen_value][0]
              = (*Eigen_values)[i_eigen_value];
        }
        for (int i_dof = 0; i_dof < matrix_->GetNrows(); i_dof++) {
          for (int j_dof = 0; j_dof < matrix_->GetNrows(); j_dof++) {
            if (results_handler.find("inverse_covariance_matrix") != results_handler.end()) {
              (*results_handler["inverse_covariance_matrix"])[i_dof][j_dof]
                  += (1. / (*Eigen_values)[i_eigen_value])
                     * (*Eigen_vectors)[i_dof][i_eigen_value]
                     * (*Eigen_vectors)[j_dof][i_eigen_value];
            }
            if (results_handler.find("projector") != results_handler.end()) {
              (*results_handler["projector"])[i_dof][j_dof]
                  += (*Eigen_vectors)[i_dof][i_eigen_value]
                     * (*Eigen_vectors)[j_dof][i_eigen_value];
            }
            if (results_handler.find("regularized_covariance_matrix") != results_handler.end()) {
              (*results_handler["regularized_covariance_matrix"])[i_dof][j_dof]
                  += (*Eigen_values)[i_eigen_value]
                     * (*Eigen_vectors)[i_dof][i_eigen_value]
                     * (*Eigen_vectors)[j_dof][i_eigen_value];
            }

          }
        }
      } else {
//            std::cout << ALERT << "Skipping i_eigen_value = " << (*Eigen_values)[i_eigen_value]
//                      << std::endl;
      }
    }

    // No memory leak ? : CHECKED
    delete Eigen_matrix_decomposer;
    delete symmetric_matrix;

    return results_handler;
  }
  inline std::vector<double> getEigenValues(TMatrixD *matrix_) {
    auto *symmetric_matrix = convertToSymmetricMatrix(matrix_);
    auto *Eigen_matrix_decomposer = new TMatrixDSymEigen(*symmetric_matrix);
    auto *Eigen_values = &(Eigen_matrix_decomposer->GetEigenValues());

    std::vector<double> output;
    for (int i_dim = 0; i_dim < matrix_->GetNcols(); i_dim++) {
      output.emplace_back((*Eigen_values)[i_dim]);
    }
    std::sort(output.begin(), output.end(), std::greater<double>());
    return output;
  }
  inline TMatrixD* getCholeskyMatrix(TMatrixD* covMatrix_){
    if(covMatrix_ == nullptr) return nullptr;
    auto* covMatrixSym = convertToSymmetricMatrix(covMatrix_);
    auto* out = getCholeskyMatrix(covMatrixSym);
    delete covMatrixSym;
    return out;
  }
  inline TMatrixD* getCholeskyMatrix(TMatrixDSym* covMatrix_){
    if(covMatrix_ == nullptr) return nullptr;
    auto* choleskyDecomposer = new TDecompChol((*covMatrix_));
    if( not choleskyDecomposer->Decompose() ){ return nullptr; }
    auto* output = (TMatrixD *)(((TMatrixD *)(choleskyDecomposer->GetU()).Clone())->T()).Clone();
    delete choleskyDecomposer;
    return output;
  }
  inline std::vector<double> throwCorrelatedParameters(TMatrixD* choleskyCovMatrix_){
    std::vector<double> out;
    throwCorrelatedParameters(choleskyCovMatrix_, out);
    return out;
  }
  inline void throwCorrelatedParameters(TMatrixD* choleskyCovMatrix_, std::vector<double>& thrownParListOut_){
    if( choleskyCovMatrix_ == nullptr ) return;
    if( thrownParListOut_.size() != choleskyCovMatrix_->GetNcols() ){
      thrownParListOut_.resize(choleskyCovMatrix_->GetNcols(), 0);
    }
    TVectorD thrownParVec(choleskyCovMatrix_->GetNcols());
    for( int iPar = 0 ; iPar < choleskyCovMatrix_->GetNcols() ; iPar++ ){
      thrownParVec[iPar] = gRandom->Gaus();
    }
    thrownParVec *= (*choleskyCovMatrix_);
    for( int iPar = 0 ; iPar < choleskyCovMatrix_->GetNcols() ; iPar++ ){
      thrownParListOut_.at(iPar) = thrownParVec[iPar];
    }
  }
  inline TMatrixD* getOuterProduct(TVectorD* v_, TVectorD* w_ ){
    if( v_ == nullptr ) return nullptr;
    if( w_ == nullptr ) w_ = v_;
    auto* out = new TMatrixD(v_->GetNrows(), w_->GetNrows());
    for( int iX = 0 ; iX < v_->GetNrows() ; iX++ ){
      for( int jX = 0 ; jX < w_->GetNrows() ; jX++ ){
        (*out)[iX][jX] = (*v_)[iX] * (*w_)[jX];
      }
    }
    return out;
  }
  template<typename T> inline void transformMatrix(TMatrixT<T>* m_, std::function<void(TMatrixT<T>*, int, int)> transformFunction_){
    if( m_ == nullptr ) return;
    for( int iRow = 0 ; iRow < m_->GetNrows() ; iRow++ ){
      for( int iCol = 0 ; iCol < m_->GetNcols() ; iCol++ ){
        transformFunction_(m_, iRow, iCol);
      }
    }
  }
  template<typename T> inline auto makeIdentityMatrix(int dim_) -> TMatrixT<T>* {
    auto* out = new TMatrixT<T>(dim_, dim_);
    for( int iDiag = 0 ; iDiag < out->GetNrows() ; iDiag++ ){
      (*out)[iDiag][iDiag] = 1;
    }
    return out;
  }
  template<typename T> inline TMatrixT<T>* makeDiagonalMatrix(TVectorT<T>* v_){
    if( v_ == nullptr ) return nullptr;
    auto* out = new TMatrixT<T>(v_->GetNrows(), v_->GetNrows());
    for( int iDiag = 0 ; iDiag < out->GetNrows() ; iDiag++ ){
      (*out)[iDiag][iDiag] = (*v_)[iDiag];
    }
    return out;
  }
  template<typename T> inline TVectorT<T>* getMatrixDiagonal(TMatrixT<T>* m_){
    if( m_ == nullptr ) return nullptr;
    auto* out = new TVectorT<T>(std::min(m_->GetNcols(), m_->GetNrows()));
    for( int iDiag = 0 ; iDiag < out->GetNrows() ; iDiag++ ){
      (*out)[iDiag] = (*m_)[iDiag][iDiag];
    }
    return out;
  }
  template<typename T> inline TVectorT<T>* getMatrixDiagonal(TMatrixTSym<T>* m_){
    return getMatrixDiagonal((TMatrixT<T>*) m_);
  }
  template<typename T> inline TVectorT<T>* getMatrixLine(TMatrixT<T>* m_, int line_){
    if( m_ == nullptr ) return nullptr;
    if( line_ < 0 or line_ >= m_->GetNrows() ) throw std::runtime_error("invalid matrix line: " + std::to_string(line_));
    auto* out = new TVectorT<T>(m_->GetNcols());
    for( int iCol = 0 ; iCol < out->GetNrows() ; iCol++ ){ (*out)[iCol] = (*m_)[line_][iCol]; }
    return out;
  }
  template<typename T> inline TVectorT<T>* getMatrixColumn(TMatrixT<T>* m_, int col_){
    if( m_ == nullptr ) return nullptr;
    if( col_ < 0 or col_ >= m_->GetNcols() ) throw std::runtime_error("invalid matrix line: " + std::to_string(col_));
    auto* out = new TVectorT<T>(m_->GetNrows());
    for( int iRow = 0 ; iRow < out->GetNrows() ; iRow++ ){ (*out)[iRow] = (*m_)[iRow][col_]; }
    return out;
  }
}


//! Histogram Tools
namespace GenericToolbox {

  inline void drawHistHorizontalBars(TH1D* hist_){
    // Incompatible with zoom-in
    if(hist_ == nullptr) return;
    TLine *l;
    int n = hist_->GetNbinsX();
    Double_t x1,x2,y;
    for (int i=1; i<=n; i++) {
      y = hist_->GetBinContent(i);
      x1= hist_->GetBinLowEdge(i);
      x2 = hist_->GetBinWidth(i)+x1;
      l= new TLine(x1,y,x2,y);
      l->SetLineColor(hist_->GetLineColor());
//      l->Paint();
      l->Draw();
    }
  }
  inline void resetHistogram(TH1D* hist_){
    hist_->Reset("ICESM");
    transformBinContent(hist_, [](TH1D* h_, int iBin_){
      h_->SetBinContent(iBin_, 0);
      h_->SetBinError(iBin_, 0);
    }, true);
  }
  inline void rescalePerBinWidth(TH1D* hist_, double globalScaler_){
    transformBinContent(hist_, [&](TH1D* h_, int iBin_){
      h_->SetBinContent( iBin_,globalScaler_ * h_->GetBinContent(iBin_)/hist_->GetBinWidth(iBin_) );
    }, true);
  }
  inline void transformBinContent(TH1D* hist_, const std::function<void(TH1D*, int)>& transformFunction_, bool processOverflowBins_){
    int firstBin = processOverflowBins_ ? 0 : 1;
    int lastBin = processOverflowBins_ ? hist_->GetNbinsX() + 1 : hist_->GetNbinsX();
    for( int iBin = firstBin ; iBin <= lastBin ; iBin++ ){
      transformFunction_(hist_, iBin);
    }
  }
  inline std::pair<double, double> fetchYRange(TH1* h_, bool withError_, const std::pair<double, double>& caps_){
    std::pair<double, double> out{std::nan(""), std::nan("")};
    if( h_ == nullptr ) return out;
    for( int iBin = 1 ; iBin <= h_->GetNbinsX() ; iBin++ ){
      double minValCandidate = h_->GetBinContent(iBin);
      double maxValCandidate = h_->GetBinContent(iBin);

      if( withError_ ){
        minValCandidate -= h_->GetBinError(iBin);
        maxValCandidate += h_->GetBinError(iBin);
      }

      if( std::isnan(caps_.first) or minValCandidate > caps_.first ){
        out.first = std::min(minValCandidate, out.first); // std::nan on right
      }
      if( std::isnan(caps_.second) or maxValCandidate < caps_.second ){
        out.second = std::max(maxValCandidate, out.second);
      }
    }
    // try to avoid returning nan:
    if( std::isnan(out.first) ) out.first = caps_.first;
    if( std::isnan(out.second) ) out.second = caps_.second;
    return out;
  }
  inline std::pair<double, double> getYBounds(TH1* h_, const std::pair<double, double>& margins_){
    std::pair<double, double> out{std::nan("unset"), std::nan("unset")};
    if( h_ == nullptr ) return out;
    for( int iBin = 1 ; iBin <= h_->GetNbinsX() ; iBin++ ){
      double minValCandidate = h_->GetBinContent(iBin) - h_->GetBinError(iBin);
      double maxValCandidate = h_->GetBinContent(iBin) + h_->GetBinError(iBin);
      out.first = std::min(minValCandidate, out.first); // std::nan on right
      out.second = std::max(maxValCandidate, out.second);
    }
    double yRange = (out.second-out.first);
    out.first -= margins_.first * yRange;
    out.second += margins_.second * yRange;
    return out;
  }
  inline std::pair<double, double> getYBounds(const std::vector<TH1*>& h_, const std::pair<double, double>& margins_){
    std::pair<double, double> out{std::nan("unset"), std::nan("unset")};
    for( auto& hist : h_ ){
      if( hist == nullptr ) continue;
      auto bounds = getYBounds(hist, margins_);
      if( out.first != out.first ) out.first = bounds.first;
      if( out.second != out.second ) out.second = bounds.second;
      out.first = std::min(out.first, bounds.first);
      out.second = std::max(out.second, bounds.second);
    }
    return out;
  }
  inline std::vector<double> getLogBinning(int n_bins_, double X_min_, double X_max_) {
    std::vector<double> output(n_bins_ + 1); // add one extra bin for the boundary
    double xlogmin = TMath::Log10(X_min_);
    double xlogmax = TMath::Log10(X_max_);
    double dlogx = (xlogmax - xlogmin) / ((double) n_bins_);
    for (int i_bin = 0; i_bin <= n_bins_; i_bin++) {
      double xlog = xlogmin + i_bin * dlogx;
      output[i_bin] = TMath::Exp(TMath::Log(10) * xlog);
    }
    return output;
  }
  inline std::vector<double> getLinearBinning(int n_bins_, double X_min_, double X_max_) {
    std::vector<double> output(n_bins_ + 1); // add one extra bin for the boundary
    double dx = (X_max_ - X_min_) / ((double) n_bins_);
    for (int i_bin = 0; i_bin <= n_bins_; i_bin++) {
      double x = X_min_ + i_bin * dx;
      output[i_bin] = x;
    }
    return output;
  }
  inline TH1D *getTH1DlogBinning(const std::string &name_, const std::string &title_, int n_bins_, double X_min_, double X_max_) {

    TH1D *output = nullptr;
    std::vector<double> xbins = getLogBinning(n_bins_, X_min_, X_max_);
    output = new TH1D(name_.c_str(), title_.c_str(), xbins.size() - 1, &xbins[0]);
    return output;

  }
  inline TH2D *getTH2DlogBinning(const std::string &name_, const std::string &title_, int nb_X_bins_, double X_min_, double X_max_,
                                 int nb_Y_bins_, double Y_min_, double Y_max_, std::string log_axis_) {

    TH2D *output = nullptr;
    std::vector<double> xbins;
    std::vector<double> ybins;
    if( hasSubStr(log_axis_, "X") ){
      xbins = getLogBinning(nb_X_bins_, X_min_, X_max_);
    } else {
      xbins = getLinearBinning(nb_X_bins_, X_min_, X_max_);
    }
    if( hasSubStr(log_axis_, "Y") ){
      ybins = getLogBinning(nb_Y_bins_, Y_min_, Y_max_);
    } else {
      ybins = getLinearBinning(nb_Y_bins_, Y_min_, Y_max_);
    }

    output = new TH2D(name_.c_str(), title_.c_str(), xbins.size() - 1, &xbins[0], ybins.size() - 1, &ybins[0]);
    return output;

  }

  template<class T> inline double getHistogramFwhm(TH1* hist_, int binMin_, int binMax_) {
    /**
     * @param hist_ histogram to extract FWHM
     * @param binMin_ first bin to evaluate FWHM
     * @param binMax_ last bin to evaluate FWHM
     * @return full width half maximum of the histogram
     */

    // Sanity check
    if( hist_ == nullptr ) throw std::logic_error("hist_ is nullptr");

    // TH2 are castable to TH1 but are not allowed arg
    // GetBinLowEdge() for TH2 return NaN. Test it.
    double test = hist_->GetBinLowEdge(1);
    if( test != test ) throw std::logic_error("Argument is not 1D histo");

    if( binMax_ == -1 ) binMax_ = hist_->GetNbinsX();
    double maxValue = hist_->GetMaximum();

    double lowFwhmBound{0};
    // First, start from the lower bound
    int iBin;
    for( iBin = binMin_; iBin <= binMax_; iBin++ ) {
      if( hist_->GetBinContent(iBin) > maxValue/2. ) { lowFwhmBound = hist_->GetBinLowEdge(iBin); break; }
    }

    // Continue until the high bound has been found
    double highFwhmBound = lowFwhmBound;
    for( ; iBin <= binMax_; iBin++ ) {
      if( hist_->GetBinContent(iBin) < maxValue / 2) { highFwhmBound = hist_->GetBinLowEdge(iBin) + hist_->GetBinWidth(iBin); break; }
    }

    return highFwhmBound - lowFwhmBound;
  }

}

// Misc
namespace GenericToolbox{

  inline bool isFlatAndOne(const TGraph* graph_){
    if( graph_->GetN() < 1 ){ return true; }
    return not std::any_of(&(graph_->GetY()[0]), &(graph_->GetY()[graph_->GetN()]), [](Double_t val_){ return val_ != 1.; });
  }
  inline bool isFlatAndOne(const TSpline3* spline_){
    if( spline_->GetNp() < 1 ){ return true; }
    bool isFlatOne{true};
    for( int iKnot = 0 ; iKnot < int(spline_->GetNp()) ; iKnot++ ){
      double xBuff{}, yBuff{}; spline_->GetKnot(iKnot, xBuff, yBuff);
      if( yBuff != 1. ){
        isFlatOne = false;
        break;
      }
    }
    return isFlatOne;
  }
  inline bool hasUniformlySpacedKnots(const TGraph* graph_, double tolerance_){
    // Check if the spline has uniformly spaced knots.  There is a flag for
    // this is TSpline3, but it's not uniformly (or ever) filled correctly.
    bool uniform{true};
    for (int i = 1; i < graph_->GetN()-1; ++i) {
      double d1 = graph_->GetX()[i-1];
      d1 = graph_->GetX()[i] - d1;
      double d2 = graph_->GetX()[i];
      d2 = graph_->GetX()[i+1] - d2;
      if (std::abs((d1-d2)/(d1+d2)) > tolerance_) {
        uniform = false;
        break;
      }
    }
    return uniform;
  }

}

//! Canvas Tools
namespace GenericToolbox {

  inline void setDefaultPalette(){
    gStyle->SetPalette(kBird);
  }
  inline void setBlueRedPalette(){
    gStyle->SetPalette(kBlackBody);
    TColor::InvertPalette();
  }
  inline void setT2kPalette(){
    int NRGBs = 3;
    int NCont = 255;

    std::vector<double> stops{0.00, 0.50, 1.000};
    std::vector<double> red{0.00, 1.00, 1.00};
    std::vector<double> green{0.00, 1.00, 0.00};
    std::vector<double> blue{1.00, 1.00, 0.00};

    TColor::CreateGradientColorTable(NRGBs,&stops[0],&red[0],&green[0],&blue[0],NCont);
    gStyle->SetNumberContours(NCont+1);
  }
  inline void setOrangePalette(){
    gStyle->SetPalette(kDarkBodyRadiator);
  }
  inline void fixTH2display(TH2 *histogram_){
    if( histogram_ == nullptr ) return;

    if( gPad != nullptr ) gPad->SetRightMargin(0.15);
    histogram_->GetZaxis()->SetTitleOffset(0.8);
    auto* pal = (TPaletteAxis*) histogram_->GetListOfFunctions()->FindObject("palette");
    // TPaletteAxis* pal = (TPaletteAxis*) histogram_->GetListOfFunctions()->At(0);
    if(pal != nullptr){
      pal->SetX1NDC(1 - 0.15 + 0.01);
      pal->SetX2NDC(1 - 0.15 + 0.05);
      pal->GetAxis()->SetMaxDigits(2);
      pal->Draw();
    }

  }
  inline void setXaxisOfAllPads(TCanvas* canvas_, double Xmin_, double Xmax_){

    for( int iPad = 0 ; iPad < canvas_->GetListOfPrimitives()->GetSize() ; iPad++ ){

      auto* pad = (TPad*) canvas_->GetListOfPrimitives()->At(iPad);
      auto* list = (TList*) pad->GetListOfPrimitives();

      TIter next(list);
      TObject *obj;

      while( (obj = next()) ){
        if( obj->InheritsFrom( TH1::Class() ) ) {
          auto* histTemp = (TH1*) obj;
          histTemp->GetXaxis()->SetRangeUser(Xmin_, Xmax_);
        }
        else if( obj->InheritsFrom( TFrame::Class() ) ){
          auto* frameTemp = (TFrame*) obj;
          frameTemp->SetX1(Xmin_);
          frameTemp->SetX2(Xmax_);
        }
      }

      pad->Update();

    }
    canvas_->Update();
    // canvas_->Draw() is needed to propagate changes
  }

}

//! ROOT Internals
namespace GenericToolbox{

  static Int_t oldVerbosity = -1;

  inline void muteRoot(){
    oldVerbosity      = gErrorIgnoreLevel;
    gErrorIgnoreLevel = kFatal;
  }
  inline void unmuteRoot(){
    gErrorIgnoreLevel = oldVerbosity;
    oldVerbosity      = -1;
  }

  inline char findOriginalVariableType(const AnyType& obj_){
    if( obj_.getType() == typeid(Bool_t) ){ return 'O'; }
    if( obj_.getType() == typeid(Char_t) ){ return 'B'; }
    if( obj_.getType() == typeid(UChar_t) ){ return 'b'; }
    if( obj_.getType() == typeid(Short_t) ){ return 'S'; }
    if( obj_.getType() == typeid(UShort_t) ){ return 's'; }
    if( obj_.getType() == typeid(Int_t) ){ return 'I'; }
    if( obj_.getType() == typeid(UInt_t) ){ return 'i'; }
    if( obj_.getType() == typeid(Float_t) ){ return 'F'; }    // `F` : a 32 bit floating point (`Float_t`)
    if( obj_.getType() == typeid(Float16_t) ){ return 'f'; }  // `f` : a 24 bit floating point with truncated mantissa
    if( obj_.getType() == typeid(Double_t) ){ return 'D'; }   // `D` : a 64 bit floating point (`Double_t`)
    if( obj_.getType() == typeid(Double32_t) ){ return 'd'; } // `d` : a 24 bit truncated floating point (`Double32_t`)
    if( obj_.getType() == typeid(Long64_t) ){ return 'L'; }
    if( obj_.getType() == typeid(ULong64_t) ){ return 'l'; }
    if( obj_.getType() == typeid(Long_t) ){ return 'G'; } // `G` : a long signed integer, stored as 64 bit (`Long_t`)
    if( obj_.getType() == typeid(ULong_t) ){ return 'g'; } // `g` : a long unsigned integer, stored as 64 bit (`ULong_t`)
    return char(0xFF); // OTHER??
  }
  inline AnyType leafToAnyType(const std::string& leafTypeName_){
    AnyType out;
    leafToAnyType(leafTypeName_, out);
    return out;
  }
  inline AnyType leafToAnyType(const TLeaf* leaf_){
    AnyType out;
    leafToAnyType(leaf_, out);
    return out;
  }
  inline void leafToAnyType(const TLeaf* leaf_, AnyType& out_){
    if( leaf_ == nullptr ){ throw std::runtime_error("leaf_ is nullptr"); }
    leafToAnyType(leaf_->GetTypeName(), out_);
  }
  inline void leafToAnyType(const std::string& leafTypeName_, AnyType& out_){
    if( leafTypeName_.empty() ){ throw std::runtime_error("empty leafTypeName_ provided."); }

      // Int like variables
    else if( leafTypeName_ == "Bool_t" )      { out_ = Bool_t(); }
    else if( leafTypeName_ == "Char_t" )      { out_ = Char_t(); }
    else if( leafTypeName_ == "UChar_t" )     { out_ = UChar_t(); }
    else if( leafTypeName_ == "Short_t" )     { out_ = Short_t(); }
    else if( leafTypeName_ == "UShort_t" )    { out_ = UShort_t(); }
    else if( leafTypeName_ == "Int_t" )       { out_ = Int_t(); }
    else if( leafTypeName_ == "UInt_t" )      { out_ = UInt_t(); }
    else if( leafTypeName_ == "Long_t" )      { out_ = Long_t(); }
    else if( leafTypeName_ == "ULong_t" )     { out_ = ULong_t(); }
    else if( leafTypeName_ == "Long64_t" )    { out_ = Long64_t(); }
    else if( leafTypeName_ == "ULong64_t" )   { out_ = ULong64_t(); }

      // Floating Variables
    else if( leafTypeName_ == "Float16_t" )   { out_ = Float16_t(); }
    else if( leafTypeName_ == "Float_t" )     { out_ = Float_t(); }
    else if( leafTypeName_ == "Double32_t" )  { out_ = Double32_t(); }
    else if( leafTypeName_ == "Double_t" )    { out_ = Double_t(); }

      // TObjects (can't be loaded as objects)
    else if( leafTypeName_ == "TGraph" )      { out_ = (TGraph*)(nullptr); }
    else if( leafTypeName_ == "TSpline3" )    { out_ = (TSpline3*)(nullptr); }
    else if( leafTypeName_ == "TClonesArray" ){ out_ = (TClonesArray*)(nullptr); }

      // Others
    else{
      std::cout << "leafToAnyType: WARNING: leafType = \"" << leafTypeName_ << "\" not set. Assuming ptr object..." << std::endl;
      out_ = (void*)(nullptr);
    }
  }
}


// Classes

// CorrelatedVariablesSampler
namespace GenericToolbox {
  class CorrelatedVariablesSampler : public InitBaseClass {

  public:
    CorrelatedVariablesSampler() = default;
    ~CorrelatedVariablesSampler() override = default;

    inline void setCovarianceMatrixPtr(TMatrixDSym *covarianceMatrixPtr_){ _covarianceMatrixPtr_ = covarianceMatrixPtr_; }

    inline void throwCorrelatedVariables(TVectorD& output_);

  protected:
    inline void initializeImpl() override;

  private:
    // inputs:
    TMatrixDSym * _covarianceMatrixPtr_{nullptr};

    // optionals
    TRandom* _prng_{gRandom}; // using TRandom3 by default

    // internals
    std::shared_ptr<TDecompChol> _choleskyDecomposer_{nullptr};
    std::shared_ptr<TMatrixD> _sqrtMatrix_{nullptr};
    std::shared_ptr<TVectorD> _throwBuffer_{nullptr};

  };

  inline void CorrelatedVariablesSampler::initializeImpl() {
    if( _covarianceMatrixPtr_ == nullptr ){
      throw std::runtime_error("Can't init while _covarianceMatrixPtr_ is not set");
    }


    // https://root.cern.ch/doc/master/classTDecompChol.html
    _choleskyDecomposer_ = std::make_shared<TDecompChol>(*_covarianceMatrixPtr_ );
    if( not _choleskyDecomposer_->Decompose() ){
      throw std::runtime_error("Can't decompose covariance matrix.");
    }

    // Decompose a symmetric, positive definite matrix: A = U^T * U
    _sqrtMatrix_ = std::make_shared<TMatrixD>(_choleskyDecomposer_->GetU());

    _sqrtMatrix_->T(); // Transpose it to get the left side one
  }
  inline void CorrelatedVariablesSampler::throwCorrelatedVariables(TVectorD& output_){
    // https://math.stackexchange.com/questions/446093/generate-correlated-normal-random-variables
    this->throwIfNotInitialized(__METHOD_NAME__);
    if(output_.GetNrows() != _covarianceMatrixPtr_->GetNrows()){
      std::stringstream ss;
      ss << __METHOD_NAME__ << ": ";
      ss << "Provided output TVector does not have the same dimension as the cov matrix";
      ss << " -> cov=" << _covarianceMatrixPtr_->GetNrows() << " != vector=" << output_.GetNrows();
      throw std::runtime_error( ss.str() );
    }

    for( int iVar = 0 ; iVar < output_.GetNrows() ; iVar++ ){
      output_[iVar] = _prng_->Gaus(0, 1);
    }

    output_ *= (*_sqrtMatrix_);
  }
}

// TObjNotifier
namespace GenericToolbox{

  class TObjNotifier : public TObject {

  public:
    inline TObjNotifier() = default;
    inline ~TObjNotifier() override = default;

    inline void setOnNotifyFct(const std::function<void()>& onNotifyFct_){ _onNotifyFct_ = onNotifyFct_; }
    inline Bool_t Notify() override { if( _onNotifyFct_ ){ _onNotifyFct_(); return true; } return false; }

    static constexpr Version_t Class_Version() { return 1; }

  private:
    std::function<void()> _onNotifyFct_{};

  };
}

// BranchBuffer
namespace GenericToolbox{
  class BranchBuffer{

  public:
    inline BranchBuffer() = default;
    inline virtual ~BranchBuffer() = default;

    // setters
    inline void setBranchPtr(TBranch* branchPtr_){ _branchPtr_ = branchPtr_; _branchName_ = _branchPtr_->GetFullName(); }

    // getters
    [[nodiscard]] inline const std::string& getBranchName() const { return _branchName_; }
    [[nodiscard]] inline const TBranch* getBranchPtr() const { return _branchPtr_; }
    [[nodiscard]] inline const std::vector<unsigned char>& getByteBuffer() const { return _byteBuffer_; }

    inline void buildBuffer();
    inline void hookBuffer();

    [[nodiscard]] inline std::string getSummary() const;

  private:
    TBranch* _branchPtr_{nullptr};
    std::string _branchName_{}; // used for ptr update
    std::vector<unsigned char> _byteBuffer_{};

  };

  inline void BranchBuffer::buildBuffer(){
    if( not _byteBuffer_.empty() ){ throw std::logic_error(__METHOD_NAME__ + ": buffer already set."); }
    if( _branchPtr_ == nullptr ){ throw std::runtime_error(__METHOD_NAME__ + ": branch not set."); }

    // Calculating the requested buffer size
    size_t bufferSize{0};
    auto* leavesList = _branchPtr_->GetListOfLeaves();
    int nLeaves = leavesList->GetEntries();
    for( int iLeaf = 0 ; iLeaf < nLeaves ; iLeaf++ ){
      auto* l = (TLeaf*) leavesList->At(iLeaf);
      if( l->GetNdata() != 0 ){
        // primary type leaf (int, double, long, etc...)
        bufferSize += l->GetNdata() * l->GetLenType();
      }
      else{
        // pointer-like obj (TGraph, TClonesArray...)
        bufferSize += 2 * l->GetLenType(); // pointer-like obj: ROOT didn't update the ptr size from 32 to 64 bits??
      }
    }

    if( bufferSize == 0 ){
      throw std::runtime_error(__METHOD_NAME__ + ": empty buffer size for branch: " + _branchPtr_->GetName());
    }

    _byteBuffer_.resize( bufferSize, 0 );
    if( _byteBuffer_.empty() ){ throw std::runtime_error(__METHOD_NAME__ + ": empty byte buffer"); }
  }
  inline void BranchBuffer::hookBuffer(){
    if( _byteBuffer_.empty() ){ throw std::runtime_error(__METHOD_NAME__ + ": empty byte buffer"); }
    _branchPtr_->SetStatus( true ); // on notify, ttree might have started back to all disabled state
    _branchPtr_->SetAddress( (void*) &_byteBuffer_[0] );
  }
  inline std::string BranchBuffer::getSummary() const {
    std::stringstream ss;
    if( _branchPtr_ != nullptr ){
      ss << _branchPtr_->GetName() << ": addr{" << (void*) _branchPtr_->GetAddress() << "}, size{" << _byteBuffer_.size() << "}";
    }
    else{
      ss << "branch not set";
    }
    return ss.str();
  }
}

// LeafForm
namespace GenericToolbox{
  class LeafForm{

  public:
    inline LeafForm() = default;
    inline virtual ~LeafForm() = default;

    inline void setIndex(int index_){ _index_ = index_; }
    inline void setNestedFormPtr(LeafForm* nestedLeafFormPtr_){ _nestedLeafFormPtr_ = nestedLeafFormPtr_; }
    inline void setPrimaryExprStr(const std::string &primaryExprStr) { _primaryExprStr_ = primaryExprStr; }
    inline void setPrimaryLeafPtr(TLeaf* primaryLeafPtr_) { _primaryLeafPtr_ = primaryLeafPtr_; _primaryLeafFullName_ = _primaryLeafPtr_->GetFullName(); }
    inline void setTreeFormulaPtr(const std::shared_ptr<TTreeFormula>& treeFormulaPtr) { _treeFormulaPtr_ = treeFormulaPtr; }

    inline const LeafForm* getNestedFormPtr() const { return _nestedLeafFormPtr_; }

    inline TLeaf *getPrimaryLeafPtr() const{ return _primaryLeafPtr_; }
    [[nodiscard]] inline const std::string &getPrimaryExprStr() const { return _primaryExprStr_; }
    [[nodiscard]] inline const std::string &getPrimaryLeafFullName() const { return _primaryLeafFullName_; }
    [[nodiscard]] inline const std::shared_ptr<TTreeFormula> &getTreeFormulaPtr() const { return _treeFormulaPtr_; }

    inline void initialize();

    [[nodiscard]] inline void* getDataAddress() const;
    [[nodiscard]] inline size_t getDataSize() const;
    [[nodiscard]] inline std::string getLeafTypeName() const;

    inline double evalAsDouble() const;
    inline void fillLocalBuffer() const;
    inline void dropToAny(GenericToolbox::AnyType& any_) const;
    [[nodiscard]] inline std::string getSummary() const;

    inline void cacheDataSize();
    inline void cacheDataAddr();

  protected:
    template<typename T> inline const T& eval() const; // Use ONLY if the type is known

  private:
    TLeaf* _primaryLeafPtr_{nullptr};     // volatile ptr for TChains
    std::string _primaryExprStr_{};       // keep track of the expression it handles
    std::string _primaryLeafFullName_{};  // used to keep track of the leaf ptr

    size_t _index_{0};
    LeafForm* _nestedLeafFormPtr_{nullptr};
    std::shared_ptr<TTreeFormula> _treeFormulaPtr_{nullptr};

    // buffers
    size_t _dataSize_{0};
    void* _dataAddress_{nullptr};
    mutable double _localBuffer_{}; // for TTreeFormula
    mutable std::shared_ptr<GenericToolbox::AnyType> _anyTypeContainer_{nullptr};

  };

  inline void LeafForm::initialize(){
    this->cacheDataSize();
    this->cacheDataAddr();
  }
  inline void* LeafForm::getDataAddress() const{
    if( _nestedLeafFormPtr_ != nullptr ){
      return ((char*) _dataAddress_) + int(_nestedLeafFormPtr_->evalAsDouble()) * this->getDataSize(); // offset
    }
    return _dataAddress_;
  }
  inline size_t LeafForm::getDataSize() const{ return _dataSize_; }
  inline std::string LeafForm::getLeafTypeName() const{
    if     ( _primaryLeafPtr_ != nullptr ){ return _primaryLeafPtr_->GetTypeName(); }
    else if( _treeFormulaPtr_ != nullptr ){ return "Double_t"; }
    return {};
  }
  inline void LeafForm::fillLocalBuffer() const {
    if( _treeFormulaPtr_ == nullptr ){
      memcpy(&_localBuffer_, this->getDataAddress(), std::min(this->getDataSize(), sizeof(double)));
    }
    else{
      _localBuffer_ = _treeFormulaPtr_->EvalInstance(0);
    }
  }
  inline double LeafForm::evalAsDouble() const {
    if( _treeFormulaPtr_ != nullptr ){
      this->fillLocalBuffer();
      return _localBuffer_; // double by default
    }
    else if( this->getLeafTypeName() == "Double_t" ){
      return this->eval<double>();
    }
    else{
      if( _anyTypeContainer_ == nullptr ){
        _anyTypeContainer_ = std::make_shared<AnyType>( GenericToolbox::leafToAnyType( this->getLeafTypeName() ) );
      }
      this->dropToAny(*_anyTypeContainer_);
      return _anyTypeContainer_->getValueAsDouble();
    }
  }
  inline void LeafForm::dropToAny(GenericToolbox::AnyType& any_) const{
    if( _treeFormulaPtr_ != nullptr ){ this->fillLocalBuffer(); }
    memcpy(any_.getPlaceHolderPtr()->getVariableAddress(), this->getDataAddress(), this->getDataSize());
  }
  inline std::string LeafForm::getSummary() const{
    std::stringstream ss;
    if     ( _primaryLeafPtr_ != nullptr ){
      ss << _primaryExprStr_ << ": br{ " << _primaryLeafFullName_ << " }";
      if     ( _index_ != 0 )                  { ss << "[" << _index_ << "]"; }
      else if( _nestedLeafFormPtr_ != nullptr ){
        ss << "[" << _nestedLeafFormPtr_->getPrimaryExprStr() << " -> " << int(_nestedLeafFormPtr_->evalAsDouble()) << "]";
      }
    }
    else if( _treeFormulaPtr_ != nullptr ){
      ss << "formula{ \"" << this->getTreeFormulaPtr()->GetName() << "\" }";
    }
    ss << ", addr{ " << this->getDataAddress() << " }, size{ " << this->getDataSize() << " }";
    if( this->getDataAddress() != nullptr and this->getDataSize() != 0 ){
      if( _treeFormulaPtr_ == nullptr ){
        ss << ", data{ 0x" << GenericToolbox::toHex(this->getDataAddress(), this->getDataSize()) << " }";
      }
      else{
        this->fillLocalBuffer();
        ss << ", eval{ " << _localBuffer_ << " }";
      }

    }
    return ss.str();
  }
  inline void LeafForm::cacheDataSize(){
    // buffer data size
    _dataSize_ = 0; // reset
    if     ( _primaryLeafPtr_ != nullptr ){
      if( _primaryLeafPtr_->GetNdata() != 0 ){
        // primary type leaf (int, double, long, etc...)
        _dataSize_ += _primaryLeafPtr_->GetLenType();
      }
      else{
        // pointer-like obj (TGraph, TClonesArray...)
        _dataSize_ += 2 * _primaryLeafPtr_->GetLenType(); // pointer-like obj: ROOT didn't update the ptr size from 32 to 64 bits??
      }
    }
    else if( _treeFormulaPtr_ != nullptr ){
      _dataSize_ = 8; // double
    }
    else{
      throw std::runtime_error(__METHOD_NAME__ + ": no data defined -> " + this->getSummary());
    }
  }
  inline void LeafForm::cacheDataAddr(){
    // buffer data address
    _dataAddress_ = nullptr;
    if     ( _primaryLeafPtr_ != nullptr ){
      _dataAddress_ = _primaryLeafPtr_->GetBranch()->GetAddress() + _primaryLeafPtr_->GetOffset();
      if( _index_ != 0 ){
        _dataAddress_ = (char*) _dataAddress_ + _index_ * this->getDataSize();
      }
    }
    else if( _treeFormulaPtr_ != nullptr ){
      _dataAddress_ = (void*) &_localBuffer_;
    }
  }
  template<typename T> inline const T& LeafForm::eval() const {
    // Use ONLY if the type is known
    if( _treeFormulaPtr_ != nullptr ){ this->fillLocalBuffer(); }
    auto* addr{(T*) this->getDataAddress()};
    if( addr == nullptr ){ throw std::runtime_error(__METHOD_NAME__ + ": invalid address: " + this->getSummary()); }
    return *addr;
  }
}

// LeafCollection
namespace GenericToolbox{
  class LeafCollection{

  public:
    inline LeafCollection() = default;
    inline virtual ~LeafCollection();

    // setters
    inline void setTreePtr(TTree* treePtr_){ _treePtr_ = treePtr_; }
    inline int addLeafExpression(const std::string& leafExpStr_);

    inline void initialize();

    // getters
    [[nodiscard]] inline const std::vector<LeafForm>& getLeafFormList() const{ return _leafFormList_; }

    // core
    inline void doNotify();
    [[nodiscard]] inline std::string getSummary() const;
    [[nodiscard]] inline int getLeafExpIndex(const std::string& leafExpression_) const;
    [[nodiscard]] inline const LeafForm* getLeafFormPtr(const std::string& leafExpression_) const;

  protected:
    inline void parseExpressions();
    inline void setupBranchBuffer(TLeaf* leaf_);

  private:
    // user
    TTree* _treePtr_{nullptr};
    std::vector<std::string> _leafExpressionList_{};

    // internals
    std::vector<BranchBuffer> _branchBufferList_{};
    std::vector<LeafForm> _leafFormList_{}; // handle the evaluation of each expression using the shared leaf buffers
    TObjNotifier _objNotifier_{};

  };

  inline LeafCollection::~LeafCollection(){
    if( _treePtr_ != nullptr and _treePtr_->GetNotify() == &_objNotifier_ ){
      // TTree will conflict the ownership as the _objNotifier_ is handled by us
      _treePtr_->SetNotify(nullptr);
    }
  }
  inline int LeafCollection::addLeafExpression(const std::string& leafExpStr_){
    auto iExp{this->getLeafExpIndex(leafExpStr_)};
    if( iExp != -1 ){ return iExp; }
    _leafExpressionList_.emplace_back( leafExpStr_ );
    return int(_leafExpressionList_.size())-1;
  }
  inline void LeafCollection::initialize() {
    if( _treePtr_ == nullptr ){ throw std::logic_error("_treePtr_ not set. Can't" + __METHOD_NAME__); }

    // make sure branch are available for TTreeFormula
    _treePtr_->SetBranchStatus("*", true);

    // read the expressions
    this->parseExpressions();

    // hook now? later?
    for( auto& branchBuf : _branchBufferList_ ){
      branchBuf.buildBuffer();
      branchBuf.hookBuffer();
    }

    // init
    for( auto& leafForm : _leafFormList_ ){ leafForm.initialize(); }

    // make sure the branch & leaf addr get updated
    _objNotifier_.setOnNotifyFct( [this](){ this->doNotify(); } );
    _treePtr_->SetNotify( &_objNotifier_ );
    this->doNotify();
  }
  void LeafCollection::doNotify(){
    // use for updating branches and leaves addresses
    // should be auto triggered by the TTree

    // reset all branch status to 0
    _treePtr_->SetBranchStatus("*", false);

    for( auto& br : _branchBufferList_ ){
      br.setBranchPtr( _treePtr_->GetBranch( br.getBranchName().c_str() ) );
      br.hookBuffer();
    }

    for( auto& lf : _leafFormList_ ){
      if( lf.getPrimaryLeafPtr() != nullptr ){
        lf.setPrimaryLeafPtr( _treePtr_->GetLeaf( lf.getPrimaryLeafFullName().c_str() ) );
      }
      if( lf.getTreeFormulaPtr() != nullptr ){
        lf.getTreeFormulaPtr()->Notify();
        GenericToolbox::enableSelectedBranches(_treePtr_, lf.getTreeFormulaPtr().get());
      }
      lf.cacheDataAddr();
    }
  }
  inline std::string LeafCollection::getSummary() const{
    std::stringstream ss;
    ss << "tree{ " << _treePtr_->GetName() << " }";
    ss << std::endl << "leavesExpr" << GenericToolbox::toString(_leafExpressionList_, true);
    ss << std::endl << "branchesBuf" << GenericToolbox::toString(_branchBufferList_, [](const BranchBuffer& b){ return b.getSummary(); });
    ss << std::endl << "leafForm" << GenericToolbox::toString(_leafFormList_, [](const LeafForm& l){ return l.getSummary(); } );
    return ss.str();
  }
  inline int LeafCollection::getLeafExpIndex(const std::string& leafExpression_) const {
    return GenericToolbox::findElementIndex( leafExpression_, _leafExpressionList_ );
  }
  inline const LeafForm* LeafCollection::getLeafFormPtr(const std::string& leafExpression_) const {
    auto idx{getLeafExpIndex(leafExpression_)};
    if( not _leafFormList_.empty() and idx != -1 ){ return &_leafFormList_[idx]; }
    return nullptr;
  }
  inline void LeafCollection::parseExpressions() {
    // avoid moving memory around
    _leafFormList_.reserve(_leafExpressionList_.size());

    // loop over the expressions. _leafExpressionList_ size might change within the loop -> USE INDICES (iExp) loop based
    for( size_t iExp = 0 ; iExp < _leafExpressionList_.size() ; iExp++ ){
      _leafFormList_.emplace_back();
      _leafFormList_.back().setPrimaryExprStr( _leafExpressionList_[iExp] );

      std::vector<std::string> argBuffer;
      auto strippedLeafExpr = GenericToolbox::stripBracket(_leafExpressionList_[iExp], '[', ']', false, &argBuffer);
      if( strippedLeafExpr.empty() ){ throw std::runtime_error(__METHOD_NAME__ + " Bad leaf form expression: " + _leafExpressionList_[iExp]); }

      // first, check if the remaining expr is a leaf
      auto* leafPtr = _treePtr_->GetLeaf(strippedLeafExpr.c_str());
      if( leafPtr == nullptr or argBuffer.size() > 1 ){
        // no leaf or multi-dim array -> use a complete TTreeFormula to eval the obj

        _leafFormList_.back().setTreeFormulaPtr( std::make_shared<TTreeFormula>(
            _leafExpressionList_[iExp].c_str(),
            _leafExpressionList_[iExp].c_str(),
            _treePtr_
        ) );

        // ROOT Hot fix: https://root-forum.cern.ch/t/ttreeformula-evalinstance-return-0-0/16366/10
        _leafFormList_.back().getTreeFormulaPtr()->GetNdata();

        if( _leafFormList_.back().getTreeFormulaPtr()->GetNdim() == 0 ){
          throw std::runtime_error(__METHOD_NAME__+": \"" + _leafExpressionList_[iExp] + "\" could not be parsed by the TTree");
        }
      }
      else{
        // leaf exists, create the associated branch buffer if not already set
        this->setupBranchBuffer( leafPtr );

        // set the leaf data will be extracted
        _leafFormList_.back().setPrimaryLeafPtr( leafPtr );

        // array-like
        if( not argBuffer.empty() ){
          try{
            int index = std::stoi(argBuffer[0]);
            _leafFormList_.back().setIndex( index );
          }
          catch(...){
            // nested? -> try
            size_t idx = this->addLeafExpression( argBuffer[0] ); // will be processed later
            _leafFormList_.back().setNestedFormPtr( (LeafForm*) idx ); // tweaking types while not all ptr are settled
          }
        }
      }
    }

    // refill up with the proper ptr addresses now _leafFormList_ size won't change
    for( auto& leafForm: _leafFormList_ ){
      if( leafForm.getNestedFormPtr() != nullptr ){
        leafForm.setNestedFormPtr( &_leafFormList_[(size_t) leafForm.getNestedFormPtr()] );
      }
    }

  }
  inline void LeafCollection::setupBranchBuffer(TLeaf* leaf_){
    auto* brPtr = leaf_->GetBranch();

    // leave if already set
    for( auto& branchBuffer : _branchBufferList_ ){
      if( branchBuffer.getBranchPtr() == brPtr ){ return; }
    }

    _branchBufferList_.emplace_back();
    _branchBufferList_.back().setBranchPtr( brPtr );
  }

}

// LeafHolder
namespace GenericToolbox{
  class LeafHolder{

  public:
    inline LeafHolder() = default;
    inline virtual ~LeafHolder() = default;

    inline void hook(TTree *tree_, TLeaf* leaf_);
    inline void hook(TTree *tree_, const std::string& leafName_);
    inline void hookDummyDouble(const std::string& leafName_);

    // const
    [[nodiscard]] inline size_t getArraySize() const;
    [[nodiscard]] inline size_t getLeafTypeSize() const;
    [[nodiscard]] inline const std::string &getLeafTypeName() const;
    [[nodiscard]] inline const std::string &getLeafFullName() const;
    [[nodiscard]] inline const std::vector<unsigned char> &getByteBuffer() const;

    // non-const
    inline std::vector<unsigned char> &getByteBuffer();

    // core
    [[nodiscard]] inline std::string getSummary() const;
    inline void dropToAny(std::vector<AnyType>& anyV_) const;
    inline void dropToAny(AnyType& any_, size_t slot_) const;

    // template
    template<typename T> inline T& getVariable(size_t arrayIndex_ = 0);
    template<typename T> inline const T& getVariable(size_t arrayIndex_ = 0) const;

    // Stream operator
    inline friend std::ostream& operator <<( std::ostream& o, const LeafHolder& v );

  private:
    size_t _leafTypeSize_{0};
    std::string _leafTypeName_{};
    std::string _leafFullName_{};
    std::vector<unsigned char> _byteBuffer_{};

  };

  inline void LeafHolder::hook(TTree *tree_, TLeaf* leaf_){
    _leafTypeName_ = leaf_->GetTypeName();
    _leafFullName_ = leaf_->GetFullName();

    // Setup buffer
    _byteBuffer_.clear();
    if(leaf_->GetNdata() != 0){
      _leafTypeSize_ = leaf_->GetLenType();
      _byteBuffer_.resize(leaf_->GetNdata() * _leafTypeSize_, 0);
    }
    else{
      // pointer-like obj
      _leafTypeSize_ = 2 * leaf_->GetLenType(); // pointer-like obj: ROOT didn't update the ptr size from 32 to 64 bits??
      _byteBuffer_.resize(_leafTypeSize_, 0);
    }
    if( _byteBuffer_.empty() ){ throw std::runtime_error("empty byte buffer"); }

    if( leaf_->GetBranch()->GetAddress() != nullptr ){
      throw std::runtime_error(leaf_->GetBranch()->GetName() + std::string(": branch address already set."));
    }

    tree_->SetBranchStatus(leaf_->GetBranch()->GetName(), true);
    tree_->SetBranchAddress(leaf_->GetBranch()->GetName(), (void*) &_byteBuffer_[0]);

    // THIS IS NOT WORKING WITH TCHAINS!!
//    leaf_->GetBranch()->SetAddress(&_byteBuffer_[0]);
//    leaf_->SetAddress(&_byteBuffer_[0]);

//    std::cout << getSummary() << std::endl;
  }
  inline void LeafHolder::hook(TTree *tree_, const std::string& leafName_){
    TLeaf* leafPtr = tree_->GetLeaf(leafName_.c_str());
    if(leafPtr == nullptr){ throw std::runtime_error("Could not get TLeaf: " + leafName_); }

    this->hook(tree_, leafPtr);
  }
  inline void LeafHolder::hookDummyDouble(const std::string& leafName_){
    _leafTypeName_ = "Double_t";
    _leafTypeSize_ = 8;

    _leafFullName_  = leafName_;
    _leafFullName_ += ".";
    _leafFullName_ += leafName_;

    // Setup buffer
    _byteBuffer_.clear();
    _byteBuffer_.resize(_leafTypeSize_, 0);

    double nanValue{std::nan("dummyLeaf")};
    memcpy(&_byteBuffer_[0], &nanValue, 8);
  }

  inline size_t LeafHolder::getArraySize() const{
    return _byteBuffer_.size()/_leafTypeSize_;
  }
  inline size_t LeafHolder::getLeafTypeSize() const {
    return _leafTypeSize_;
  }
  inline const std::string &LeafHolder::getLeafTypeName() const {
    return _leafTypeName_;
  }
  inline const std::string &LeafHolder::getLeafFullName() const {
    return _leafFullName_;
  }
  inline const std::vector<unsigned char> &LeafHolder::getByteBuffer() const {
    return _byteBuffer_;
  }
  inline std::string LeafHolder::getSummary() const {
    std::stringstream o;
    o << _leafFullName_ << "/" << _leafTypeName_ << " = ";
    if( not _byteBuffer_.empty() ){
      o << "{ ";
      auto aBuf = GenericToolbox::leafToAnyType(_leafTypeName_);
      for(int iSlot=0 ; iSlot < getArraySize() ; iSlot++ ){
        if( iSlot != 0 ) o << ", ";
        dropToAny(aBuf, iSlot);
        o << aBuf;
      }
      o << " } | ";
      o << GenericToolbox::stackToHex(_byteBuffer_, _leafTypeSize_);
      o << " / addr{" << GenericToolbox::toHex(&_byteBuffer_[0]) << "}";
    }
    else{
      o << "{ EMPTY }";
    }
    return o.str();
  }

  inline std::vector<unsigned char> &LeafHolder::getByteBuffer(){
    return _byteBuffer_;
  }
  template<typename T> inline T& LeafHolder::getVariable(size_t arrayIndex_){
    return *((T*) &_byteBuffer_[arrayIndex_ * sizeof(T)]);
  }

  template<typename T> inline const T& LeafHolder::getVariable(size_t arrayIndex_) const{
    return *((T*) &_byteBuffer_[arrayIndex_ * sizeof(T)]);
  }

  inline void LeafHolder::dropToAny(std::vector<AnyType>& anyV_) const{
    if(anyV_.empty()){ anyV_.resize(getArraySize(), GenericToolbox::leafToAnyType(_leafTypeName_)); }
    for( size_t iSlot = 0 ; iSlot < anyV_.size() ; iSlot++ ){ this->dropToAny(anyV_[iSlot], iSlot); }
  }
  inline void LeafHolder::dropToAny(AnyType& any_, size_t slot_) const{
    memcpy(any_.getPlaceHolderPtr()->getVariableAddress(), &_byteBuffer_[slot_ * _leafTypeSize_], _leafTypeSize_);
  }

  inline std::ostream& operator <<( std::ostream& o, const LeafHolder& v ){
    if( not v._byteBuffer_.empty() ){
      o << v.getSummary();
    }
    return o;
  }
}

// TreeEntryBuffer
namespace GenericToolbox{
  class TreeEntryBuffer {

  public:
    inline TreeEntryBuffer();
    inline virtual ~TreeEntryBuffer();

    inline void setLeafNameList(const std::vector<std::string> &leafNameList_);
    inline void setIsDummyLeaf(const std::string& leafName_, bool isDummy_);
    inline void hook(TTree* tree_);

    inline const std::vector<GenericToolbox::LeafHolder> &getLeafContentList() const;

    inline int fetchLeafIndex(const std::string& leafName_) const;
    inline const GenericToolbox::LeafHolder& getLeafContent(const std::string& leafName_) const;
    inline std::string getSummary() const;

  private:
    std::vector<std::string> _leafNameList_;
    std::vector<bool> _dummyLeafStateLeaf_;
    std::vector<GenericToolbox::LeafHolder> _leafContentList_;

  };

  inline TreeEntryBuffer::TreeEntryBuffer() = default;
  inline TreeEntryBuffer::~TreeEntryBuffer() = default;

  inline void TreeEntryBuffer::hook(TTree* tree_){
    _leafContentList_.clear();
    _leafContentList_.resize(_leafNameList_.size());
    for( size_t iLeaf{0} ; iLeaf < _leafNameList_.size() ; iLeaf++ ){
      if( _dummyLeafStateLeaf_[iLeaf] ){ _leafContentList_[iLeaf].hookDummyDouble( _leafNameList_[iLeaf] ); }
      else{ _leafContentList_[iLeaf].hook(tree_, _leafNameList_[iLeaf]); }
    }
  }

  inline void TreeEntryBuffer::setLeafNameList(const std::vector<std::string> &leafNameList_) {
    _leafNameList_ = leafNameList_;
    _dummyLeafStateLeaf_.resize(_leafNameList_.size(), false);
  }
  inline void TreeEntryBuffer::setIsDummyLeaf(const std::string& leafName_, bool isDummy_){
    int index = GenericToolbox::findElementIndex( leafName_, _leafNameList_ );
    if( index == -1 ){
      throw std::runtime_error(
          "TreeEntryBuffer::setIsDummyLeaf: \"" + leafName_ + "\" not found in leaf list: "
          + GenericToolbox::toString(_leafNameList_)
      );
    }
    _dummyLeafStateLeaf_[index] = isDummy_;
  }

  inline const std::vector<GenericToolbox::LeafHolder> &TreeEntryBuffer::getLeafContentList() const {
    return _leafContentList_;
  }

  inline int TreeEntryBuffer::fetchLeafIndex(const std::string& leafName_) const{
    return GenericToolbox::findElementIndex(leafName_, _leafNameList_);
  }
  inline const GenericToolbox::LeafHolder& TreeEntryBuffer::getLeafContent(const std::string& leafName_) const{
    int i = this->fetchLeafIndex(leafName_);
    if(i==-1){ throw std::runtime_error(leafName_ + ": not found -> " + GenericToolbox::toString(_leafNameList_)); }
    return _leafContentList_[i];
  }
  inline std::string TreeEntryBuffer::getSummary() const{
    std::stringstream ss;
    ss << "TreeEventBuffer:" << std::endl << "_leafContentList_ = {";
    if( not _leafNameList_.empty() ){
      for( size_t iLeaf = 0 ; iLeaf < _leafNameList_.size() ; iLeaf++ ){
        ss<< std::endl << "  " << _leafContentList_[iLeaf];
      }
      ss << std::endl << "}";
    }
    else{
      ss << "}";
    }
    return ss.str();
  }
}


#endif // CPP_GENERIC_TOOLBOX_ROOT_H
