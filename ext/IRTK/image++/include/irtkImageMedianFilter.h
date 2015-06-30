/*=========================================================================

  Library   : Image Registration Toolkit (IRTK)
  Module    : $Id$
  Copyright : Imperial College, Department of Computing
              Visual Information Processing (VIP), 2009 onwards
  Date      : $Date$
  Version   : $Revision$
  Changes   : $Author$

=========================================================================*/

#ifndef _IRTKIMAGEMEDIAN_H

#define _IRTKIMAGEMEDIAN_H

#include <irtkImageToImage.h>

/*
 * Class for image median filter
 */

template<typename VoxelType> class irtkImageMedianFilter : public irtkImageToImage<VoxelType>
{
public:
  // Constructor
  irtkImageMedianFilter();

  // Destructor
  ~irtkImageMedianFilter();

  // Returns the name of the class
  virtual const char *NameOfClass();

  // Initialize the filter
  virtual void Initialize();

  // Run
  virtual void Run();

  // Set the window size
  void SetWindowSize(int, int, int);

  // Get the window size
  const void GetWindowSize(int &, int &, int &);

protected:
  // Returns whether the filter requires buffering
  virtual bool RequiresBuffering();

  // Image dimension
  int _X, _Y, _Z, _T;

  // Filter window size
  int _rx, _ry, _rz;
};

#endif
