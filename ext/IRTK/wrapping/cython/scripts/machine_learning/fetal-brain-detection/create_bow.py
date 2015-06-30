#!/usr/bin/python

import numpy as np
import cv2
from sklearn import cluster# as skcluster
import os
import csv
import SimpleITK as sitk
from glob import glob
import argparse

from scipy.stats.mstats import mquantiles
from joblib import Parallel, delayed

# Removing duplicate rows
# http://mail.scipy.org/pipermail/scipy-user/2011-December/031193.html

######################################################################

def process_file( raw_file, NEW_SAMPLING, step=1, DEBUG=False ):
    print raw_file
    whole = sitk.ReadImage( raw_file )

    ## Resample
    resample = sitk.ResampleImageFilter()
    resample.SetOutputDirection(whole.GetDirection())
    resample.SetOutputOrigin(whole.GetOrigin())
    resample.SetOutputSpacing([NEW_SAMPLING,NEW_SAMPLING,whole.GetSpacing()[2]])
    resample.SetSize([int(whole.GetSize()[0]*whole.GetSpacing()[0]/NEW_SAMPLING),
                      int(whole.GetSize()[1]*whole.GetSpacing()[1]/NEW_SAMPLING),
                      whole.GetSize()[2]])
    whole = resample.Execute(whole)
    
    data = sitk.GetArrayFromImage( whole ).astype('float')

    ## Contrast-stretch with saturation
    q = mquantiles(data.flatten(),[0.01,0.99])
    data[data<q[0]] = q[0]
    data[data>q[1]] = q[1]
    data -= data.min()
    data /= data.max()
    data *= 255
    data = data.astype('uint8')

    detector = cv2.SIFT( nfeatures=0,
              nOctaveLayers=3,
              contrastThreshold=0.04,
              edgeThreshold=10,
              sigma=0.8)
    descriptorExtractor = cv2.DescriptorExtractor_create("SIFT")

    points = []
    for z in range(0,data.shape[0],step):
        keypoints = detector.detect(data[z,:,:])
        if keypoints is None or len(keypoints) == 0:
            continue
        (keypoints, descriptors) = descriptorExtractor.compute(data[z,:,:],keypoints)       
        unique_index= np.unique( descriptors.dot(np.random.rand(128)),
                                 return_index=True)[1]
        points.extend(descriptors[unique_index])

        ## For debugging purpose:
        if DEBUG:
            img_color = cv2.cvtColor( data[z,:,:].astype('uint8'), cv2.cv.CV_GRAY2RGB )
            for y,x in F.transpose():
                cv2.circle( img_color,
                            (int(x),int(y)),
                            2,
                            (0,0,255),
                            -1)
            cv2.imwrite( "/tmp/"
                         + os.path.basename(raw_file.rstrip('.nii'))
                         + "_" + str(z)
                         +".png", img_color )

    points = np.array(points)
    unique_index= np.unique( points.dot(np.random.rand(128)),
                                 return_index=True)[1]
    return points[unique_index]

######################################################################


parser = argparse.ArgumentParser(
    description='Learn SIFT words for BOW classification.' )
parser.add_argument( '--training_patients' )
parser.add_argument( '--new_sampling', type=float )
parser.add_argument( '--original_folder' )
parser.add_argument( '--step', type=int )
parser.add_argument( '--output' )
parser.add_argument( '--debug', action="store_true", default=False )

args = parser.parse_args()
    
f = open( args.training_patients, "r" )
patients = []
for p in f:
    patients.append(p.rstrip())
f.close()
      
originals = glob(args.original_folder + "/*")

training_files = []
for f in originals:
    patient_id = os.path.basename(f).split('_')[0]
    if patient_id in patients:
        training_files.append( f )

training_files = training_files[::args.step]

print "Will now process " + str(len(training_files)) + " files from " + str(len(patients)) + " patients..."
        
bulk_points = Parallel(n_jobs=-1)(delayed(process_file)(raw_file,
                                                        args.new_sampling,
                                                        args.step) for raw_file in training_files)

points = []
for D in bulk_points:
    points.extend(D)

print len(points)
points = np.array(points,dtype='float')

print points, points.shape

print "Starting K-means clustering..."

kmeans = cluster.MiniBatchKMeans( n_clusters=400,
                                  max_iter=1000,
                                  compute_labels=False )
kmeans.fit(points)

print kmeans.cluster_centers_[:,-10:]

vocabulary = open(args.output, 'wb')
np.save(vocabulary, kmeans.cluster_centers_ )

print "Done"
