#!/bin/bash

cd vendor/samsung_slsi/ims/shannon-qns;
export version=`git log -1 | grep Change-Id | tr -d ' ' | cut -d ':' -f 2`;

cd -;

sed "s/package=\"com.shannon.qualifiednetworksservice\"/package=\"com.shannon.qualifiednetworksservice\"\n    android:versionName=\"${version##*( )}\"/g" $1 > $2 ;

