/*=========================================================================

  Library   : Image Registration Toolkit (IRTK)
  Module    : $Id$
  Copyright : Imperial College, Department of Computing
              Visual Information Processing (VIP), 2011 onwards
  Date      : $Date$
  Version   : $Revision$
  Changes   : $Author$

=========================================================================*/


#include <stdio.h>
#include <iostream>
#include <cmath>

#include "irtkImage.h"

#include "graph.h"
#include "GCoptimization.h"

#define FOREGROUND_LABEL 1
#define BACKGROUND_LABEL 2

typedef irtkRealPixel pixel_t;

typedef short LabelID;
typedef int SiteID;

typedef int captype;
typedef int tcaptype;
typedef long long flowtype;

class irtkSimpleGraphcut : public irtkObject
{
private:
    irtkGenericImage<pixel_t> &_img;
    irtkGenericImage<LabelID> &_labels;
    double _sigma;
    double _sigmaZ;

    size_t index( size_t i, size_t j, size_t k );
    
public:

    /// Constructor
    irtkSimpleGraphcut( irtkGenericImage<pixel_t> &img,
                        irtkGenericImage<LabelID> &labels );

    /// Destructor
    ~irtkSimpleGraphcut();
    
    void SetSigma( double sigma );
    void SetSigmaZ( double sigmaZ );

    void Run();
};

inline size_t irtkSimpleGraphcut::index( size_t z, size_t y, size_t x ) {
    return x + _img.GetX()*( y + _img.GetY()*z );
}

inline void irtkSimpleGraphcut::SetSigma( double sigma ) {
    _sigma = sigma;
}

inline void irtkSimpleGraphcut::SetSigmaZ( double sigmaZ ) {
    _sigmaZ = sigmaZ;
}
