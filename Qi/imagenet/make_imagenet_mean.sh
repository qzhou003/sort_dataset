#!/usr/bin/env sh
# Compute the mean image from the imagenet training lmdb
# N.B. this is available in data/ilsvrc12

EXAMPLE=/home/admobilize/Qi/git/sort_HK_dataset/data
DATA=data
TOOLS=/home/admobilize/Qi/git/caffe/build/tools

$TOOLS/compute_image_mean $EXAMPLE/ilsvrc12_train_lmdb \
  $DATA/HK_mean.binaryproto

echo "Done."
