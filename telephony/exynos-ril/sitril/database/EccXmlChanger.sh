#!/bin/bash
#
# Convert DbEcc_***.xml to DbEcc_***.h
#
COPYRIGHT=$(cat <<-END
/*
 * Copyright Samsung Electronics Co., LTD.
 *
 * This software is proprietary of Samsung Electronics.
 * No part of this software, either material or conceptual may be copied or distributed, transmitted,
 * transcribed, stored in a retrieval system or translated into any human or computer language in any form by any means,
 * electronic, mechanical, manual or otherwise, or disclosed
 * to third parties without the express written permission of Samsung Electronics.
*/
END
)

BASEDIR=$(dirname $0)
for entry in ${BASEDIR}/xml/DbEcc*
do
filename=$(basename $entry)
mcc=$(echo $filename | awk -F _ '{print substr($2,0,length($2)-4)}')
str=$(awk -v mcc="$mcc" '/EccEntry/ {print "{\""mcc"\", "substr($2,5,length($2)-4)", "substr($3,8,length($3)-7)", "substr($4,11,1)", 0b"substr($5,12,length($5)-12)"},"}' $entry)
cat > ${BASEDIR}/header/DbEcc_$mcc.h <<EOF
$COPYRIGHT
$str
EOF
done
