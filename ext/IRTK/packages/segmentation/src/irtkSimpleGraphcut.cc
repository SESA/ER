/*=========================================================================

  Library   : Image Registration Toolkit (IRTK)
  Module    : $Id$
  Copyright : Imperial College, Department of Computing
              Visual Information Processing (VIP), 2011 onwards
  Date      : $Date$
  Version   : $Revision$
  Changes   : $Author$

=========================================================================*/


#include "irtkSimpleGraphcut.h"

irtkSimpleGraphcut::irtkSimpleGraphcut( irtkGenericImage<pixel_t> &img,
                             irtkGenericImage<LabelID> &labels )
 : _img(img), _labels(labels)
{
    _sigma = 0;
    _sigmaZ = 0;
}

irtkSimpleGraphcut::~irtkSimpleGraphcut()
{ }

void irtkSimpleGraphcut::Run()
{
    try {
         typedef Graph<captype,tcaptype,flowtype> GraphType;
        
        SiteID nb_pixels = _img.GetX() * _img.GetY() * _img.GetZ();
        std::cout << "will create graph for " << nb_pixels  << " pixels\n";
        
        GraphType G( nb_pixels, /*estimated # of nodes*/
                     nb_pixels*(26+1) ); /*estimated #	of edges*/
        G.add_node( nb_pixels );

        std::cout << "building graph...\n";

        int x0, y0, z0;
        SiteID id, id0;
        double w, std2;
        if ( this->_sigma == 0 )
            this->_sigma = _img.GetSD();
        if ( this->_sigmaZ == 0 )
            this->_sigmaZ = this->_sigma;
        for ( int z = 0; z < _img.GetZ(); z++ )
            for ( int y = 0; y < _img.GetY(); y++ )
                for ( int x = 0; x < _img.GetX(); x++ ) 
                    for ( int a = -1; a <= 1; a++ )
                        for ( int b = -1; b <= 1; b++ )
                            for ( int c = -1; c <= 1; c++ ) {
                                if ( abs(a)+abs(b)+abs(c) == 0 )
                                    continue;
                                z0 = z+a;
                                y0 = y+b;
                                x0 = x+c;
                                id = index(z,y,x);
                                id0 = index(z0,y0,x0);
                                if ( 0 <= z0 && z0 < _img.GetZ() &&
                                     0 <= y0 && y0 < _img.GetY() &&
                                     0 <= x0 && x0 < _img.GetX() ) { 
                                    double dist = sqrt( pow(double(a)*_img.GetZSize(),2.0) +
                                                        pow(double(b)*_img.GetYSize(),2.0) +
                                                        pow(double(c)*_img.GetXSize(),2.0) );
                                    if ( _img(x,y,z) < _img(x0,y0,z0) )
                                        w = 1.0/dist;
                                    else {
                                        if (a==0)
                                            std2 = 2.0 * pow(this->_sigma,2);
                                        else
                                            std2 = 2.0 * pow(this->_sigmaZ,2);
                                        w = exp( -pow( double(_img(x,y,z) -
                                                              _img(x0,y0,z0)), 2.0)
                                                 / std2 )
                                            / dist;
                                    }
                                    G.add_edge( id, id0, w*1000, 0 );
                                }
                            }

        std::cout << "linking to source and sink...\n";

        for ( int z = 0; z < _img.GetZ(); z++ )
            for ( int y = 0; y < _img.GetY(); y++ )
                for ( int x = 0; x < _img.GetX(); x++ ) {        
                    id = index(z,y,x);
                    if ( _labels(x,y,z) == FOREGROUND_LABEL )
                        G.add_tweights(id,100000,0);
                    else if ( _labels(x,y,z) == BACKGROUND_LABEL ) 
                        G.add_tweights(id,0,100000);
                }

        std::cout << "computing maxflow...\n";
        std::cout << G.maxflow() << "\n";
        
        std::cout <<  "transcription of segmentation...\n";        

        int l;
        for ( int z = 0; z < _img.GetZ(); z++ )
            for ( int y = 0; y < _img.GetY(); y++ )
                for ( int x = 0; x < _img.GetX(); x++ ) {
                    SiteID id = index(z,y,x);
                    if ( G.what_segment(id) == 0 )
                        _labels(x,y,z) = 1;
                    else
                        _labels(x,y,z) = 0;
                }

    }
    catch (GCException e){
        e.Report();
        exit(1);
    }    
    
    return;
}
