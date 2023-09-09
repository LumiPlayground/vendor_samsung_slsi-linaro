#!/bin/bash
#
# Copyright (c) 2022 Samsung Electronics Co., Ltd All Rights Reserved
# PROPRIETARY/CONFIDENTIAL
#
# This software is the confidential and proprietary information of
# SAMSUNG ELECTRONICS ("Confidential Information").
#
# You shall not disclose such Confidential Information and shall use it
# only in accordance with the terms of the license agreement
# you entered into with SAMSUNG ELECTRONICS.
#
# SAMSUNG make no representations or warranties about the suitability of
# the software, either express or implied, including but not limited to
# the implied warranties of merchantability, fitness for a particular
# purpose, or non-infringement. SAMSUNG shall not be liable for any
# damages suffered by license as a result of using, modifying or
# distributing this software or its derivatives.

# it's recommended to keep latest svace tool in user home directory
# build.sh will keep common build configuration
# but it may needs to change project specific path
BUILD_CMD="./build.sh"
SAM_TOOL_ROOT="$HOME/SAM-Tools"
SCRA_ROOT="$SAM_TOOL_ROOT/hub/code-analysis-hub/scap/plugins/scra"
SCRA_VERSION="svace-analyzer-3.2-20220501"
OS_VARIANT="x64-linux"
SCRA_DIST="$SCRA_VERSION-$OS_VARIANT"
SCRA_PATH="$SCRA_ROOT/$SCRA_DIST/bin"

# Check SAM Tools, if it's not installed, download and extract it
# Only works in Samsung Intranet and requires CORP AD ID/PW
if [ ! -d $SAM_TOOL_ROOT ]; then
    echo "Not Found SAM-Tools, Try to download..."
    cd $HOME
    git clone https://github.sec.samsung.net/RS7-Architectural-Refactoring/SAM-Tools
else
    echo "Found SAM-Tools at "$SAM_TOOL_ROOT
fi
if [ ! -d $SCRA_PATH ]; then
    echo "Not Found SCRA TOOL from "$SCRA_PATH", Try to download..."
    cd $SCRA_ROOT
    wget http://analysishub.sec.samsung.net/resources/svace/${SCRA_VERSION}/$SCRA_DIST.tbz2
    tar xjvf $SCRA_DIST.tbz2
else
    echo "Found SCRA at "$SCRA_PATH
fi

# exclude test from SAM analysis
# need to update src_exclude.cfg to exclude test folder
# src_exclude.cfg requires absolute path
echo "# Generated at "$(date) > src_exclude.cfg
echo $PWD/tests >> src_exclude.cfg

# remove previous generated files, just keep output folder
rm -rf .scap/ .svace-dir/ .log/

# initialize scap, scra and svace analysis gen-folder
$SAM_TOOL_ROOT/hub/code-analysis-hub/bin/scap init
mkdir -p .scap/workspace/scra/
$SCRA_PATH/svace init

# configure analysis options
$SCRA_PATH/svace scra-params SCRA_BUILD_IGNORE_MACRO_METRIC on
# Optonal for C/CPP
$SCRA_PATH/svace scra-params SCRA_RUNLINK_DYNAMIC_LIBS on
$BUILD_CMD "-c"
$SCRA_PATH/svace build --scra-only --verbose $BUILD_CMD

# Check build state
SVACE_BUILD_RETURN=$?
if [ $SVACE_BUILD_RETURN -ne 0 ]; then
    echo "svce build return error code :" $SVACE_BUILD_RETURN
    echo "Stop script"
    exit 1
else
    echo "return code :" $SVACE_BUILD_RETURN
fi

# scra-analyze
# [C/CPP] svace scra-analyze -T -o .scap/workspace/scra/
# [JAVA]  svace scra-analyze-java -o .scap/workspace/scra/
#$SAM_TOOL_ROOT/hub/code-analysis-hub/scap/plugins/scra/svace-analyzer-3.0-20200625-x64-linux/bin/svace scra-analyze-java -o .scap/workspace/scra/
#$SAM_TOOL_ROOT/hub/code-analysis-hub/scap/plugins/scra/svace-analyzer-3.1-20210322-x64-linux/bin/svace scra-analyze -T -o .scap/workspace/scra/

# run svace analysis for architecture and code complexity
$SCRA_PATH/svace scra-analyze -T -o .scap/workspace/scra/

# run SAM analysis and calculate metric
$SAM_TOOL_ROOT/sam_cli.sh sam_cli.cfg
