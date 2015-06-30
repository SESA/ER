/*=========================================================================

  Library   : Image Registration Toolkit (IRTK)
  Module    : $Id$
  Copyright : Imperial College, Department of Computing
              Visual Information Processing (VIP), 2009 onwards
  Date      : $Date$
  Version   : $Revision$
  Changes   : $Author$

=========================================================================*/

#include <irtkImage.h>
#include <vector>
#include <algorithm>
#include <set>

#include "irtkImageMedianFilter.h"

template<typename VoxelType> irtkImageMedianFilter<VoxelType>::irtkImageMedianFilter()
{
  // Default window size
  _rx = 1;
  _ry = 1;
  _rz = 1;
}

template<typename VoxelType> irtkImageMedianFilter<VoxelType>::~irtkImageMedianFilter()
{
}

template<typename VoxelType> const char *irtkImageMedianFilter<VoxelType>::NameOfClass()
{
  return "irtkImageMedianFilter";
}

template<typename VoxelType> void irtkImageMedianFilter<VoxelType>::Initialize()
{
  // Do the initial set up
  this->irtkImageToImage<VoxelType>::Initialize();

  // Image dimension
  _X = this->_input->GetX();
  _Y = this->_input->GetY();
  _Z = this->_input->GetZ();
  _T = this->_input->GetT();
}

template<typename VoxelType> void irtkImageMedianFilter<VoxelType>::SetWindowSize(int rx, int ry, int rz)
{
  _rx = rx;
  _ry = ry;
  _rz = rz;
}

template<typename VoxelType> const void irtkImageMedianFilter<VoxelType>::GetWindowSize(int &rx, int &ry, int &rz)
{
  rx = _rx;
  ry = _ry;
  rz = _rz;
}

template<typename VoxelType> bool irtkImageMedianFilter<VoxelType>::RequiresBuffering()
{
  return true;
}

template<typename VoxelType> void irtkImageMedianFilter<VoxelType>::Run()
{
  // Do the initial set up
  this->Initialize();

  for(int t=0; t<_T; t++){
    for(int z=0; z<_Z; z++){
      for(int y=0; y<_Y; y++){
	for(int x=0; x<_X; x++){
	  vector<VoxelType> voxels;

	  for(int dz=-_rz; dz<=_rz; dz++){
	    for(int dy=-_ry; dy<=_ry; dy++){
	      for(int dx=-_rx; dx<=_rx; dx++){
		int x2 = x + dx;
		int y2 = y + dy;
		int z2 = z + dz;
			  
		// Duplicate at the boundary in case the voxel is outside the image region
		x2 = (x2 >= 0)? x2 : 0;
		x2 = (x2 < _X)? x2 : (_X-1);
		y2 = (y2 >= 0)? y2 : 0;
		y2 = (y2 < _Y)? y2 : (_Y-1);
		z2 = (z2 >= 0)? z2 : 0;
		z2 = (z2 < _Z)? z2 : (_Z-1);
		
		voxels.push_back(this->_input->Get(x2, y2, z2, t));
	      }
	    }
	  }

	  // Sort the voxel values and record the median value
	  sort(voxels.begin(), voxels.end());
	  VoxelType median = voxels[(int)(voxels.size() / 2)];
	  this->_output->Put(x, y, z, t, median);
        }
      }
    }
  }
	  
  // Do the final clean-up
  this->Finalize();
}

template class irtkImageMedianFilter<irtkBytePixel>;
template class irtkImageMedianFilter<irtkGreyPixel>;
template class irtkImageMedianFilter<irtkRealPixel>;
