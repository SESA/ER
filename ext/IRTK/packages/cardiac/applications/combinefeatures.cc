/*=========================================================================

Library   : Image Registration Toolkit (IRTK)
Module    : $Id$
Copyright : Imperial College, Department of Computing
Visual Information Processing (VIP), 2008 onwards
Date      : $Date$
Version   : $Revision$
Changes   : $Author$

=========================================================================*/

#include <irtkImage.h>

#include <irtkGradientImageFilter.h>

#include <irtkGaussianBlurring.h>

#ifdef HAS_CONTRIB

#include <irtkEuclideanDistanceTransform.h>

char *reference_name = NULL, *intensity_name = NULL, *spectral_name = NULL, *output_name = NULL;

void usage()
{
	cerr << "Usage: combinefeatures [referenceimage] [intensity] [spectral] [outputfeatures]" << endl;
	cerr << "Combine the intensity and spectral features, normalize spectral feature according to referenceimage's intensity range." << endl;
	exit(1);
}

int main(int argc, char **argv)
{
	irtkRealImage reference, intensity, spectral;

	if (argc < 5) {
		usage();
	}

	reference_name  = argv[1];
	argc--;
	argv++;
	intensity_name = argv[1];
	argc--;
	argv++;
	spectral_name  = argv[1];
	argc--;
	argv++;
	output_name = argv[1];
	argc--;
	argv++;

	// Read input
	reference.Read(reference_name);
	intensity.Read(intensity_name);
	spectral.Read(spectral_name);

	double min,max,smin,smax;
	reference.GetMinMax(&min, &max);
	
	double scale;
	scale = (max - min) / 255;
	
	irtkRealImage output;
	irtkImageAttributes atr = reference.GetImageAttributes();
	atr._t = intensity.GetT() + spectral.GetT();

	output.Initialize(atr);
	for(int t = 0; t < intensity.GetT(); t++){
		for(int k = 0; k < intensity.GetZ(); k++){
			for(int j = 0; j < intensity.GetY(); j++){
				for(int i = 0; i < intensity.GetX(); i++){
					output.Put(i,j,k,t,intensity.Get(i,j,k,t));
				}
			}
		}
	}

	for(int t = intensity.GetT(); t < intensity.GetT() + spectral.GetT(); t++){
		for(int k = 0; k < spectral.GetZ(); k++){
			for(int j = 0; j < spectral.GetY(); j++){
				for(int i = 0; i < spectral.GetX(); i++){
					output.Put(i,j,k,t,spectral.Get(i,j,k,t-intensity.GetT()) * scale + min);
				}
			}
		}
	}

	// Write image
	output.Write(output_name);
}

#else

int main(int argc, char **argv)
{
	cerr << "Needs to be compiled with HAS_CONTRIB" << endl;
	return 0;
}

#endif