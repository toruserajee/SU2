/*!
 * \file CFEMStandardQuadAdjacentPrismSol.cpp
 * \brief Functions for the class CFEMStandardQuadAdjacentPrismSol.
 * \author E. van der Weide
 * \version 7.0.8 "Blackbird"
 *
 * SU2 Project Website: https://su2code.github.io
 *
 * The SU2 Project is maintained by the SU2 Foundation
 * (http://su2foundation.org)
 *
 * Copyright 2012-2020, SU2 Contributors (cf. AUTHORS.md)
 *
 * SU2 is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * SU2 is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with SU2. If not, see <http://www.gnu.org/licenses/>.
 */

#include "../../include/fem/CFEMStandardQuadAdjacentPrismSol.hpp"

/*----------------------------------------------------------------------------------*/
/*            Public member functions of CFEMStandardQuadAdjacentPrismSol.          */
/*----------------------------------------------------------------------------------*/

CFEMStandardQuadAdjacentPrismSol::CFEMStandardQuadAdjacentPrismSol(const unsigned short val_nPoly,
                                                                   const unsigned short val_orderExact,
                                                                   const unsigned short val_faceID_Elem,
                                                                   const unsigned short val_orientation,
                                                                   CGemmBase           *val_gemm_1,
                                                                   CGemmBase           *val_gemm_2)
  : CFEMStandardPrismBase(),
    CFEMStandardQuadBase(val_nPoly, val_orderExact) {

  /*--- Convert the pointers for the gemm functionalities. ---*/
  gemmDOFs2Int = dynamic_cast<CGemmStandard *> (val_gemm_1);
  gemmInt2DOFs = dynamic_cast<CGemmStandard *> (val_gemm_2);
  if(!gemmDOFs2Int || !gemmInt2DOFs)
    SU2_MPI::Error(string("Dynamic cast failure. This should not happen"), CURRENT_FUNCTION);

  /*--- Convert the 2D parametric coordinates of the integration points of the
        quadrilateral face to the 3D parametric coordinates of the adjacent prism. ---*/
  vector<passivedouble> rInt, sInt, tInt;
  ConvertCoor2DQuadFaceTo3DPrism(rLineInt, val_faceID_Elem, val_orientation, rInt, sInt, tInt);

  /*--- Allocate the memory for the Legendre basis functions and its
        1st derivatives in the integration points. ---*/
  nDOFs = nDOFs1D*nDOFs1D*(nDOFs1D+1)/2;
  legBasisInt.resize(nIntegrationPad, nDOFs); legBasisInt.setConstant(0.0);

  derLegBasisInt.resize(3);
  derLegBasisInt[0].resize(nIntegrationPad, nDOFs); derLegBasisInt[0].setConstant(0.0);
  derLegBasisInt[1].resize(nIntegrationPad, nDOFs); derLegBasisInt[1].setConstant(0.0);
  derLegBasisInt[2].resize(nIntegrationPad, nDOFs); derLegBasisInt[2].setConstant(0.0);

  /*--- Compute the Legendre basis functions and its first
        derivatives in the integration points. ---*/
  VandermondePrism(nPoly, rInt, sInt, tInt, legBasisInt);
  GradVandermondePrism(nPoly, rInt, sInt, tInt, derLegBasisInt[0],
                       derLegBasisInt[1], derLegBasisInt[2]);
}