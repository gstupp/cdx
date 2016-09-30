#!/usr/intel/bin/bash
tmpfile=$(mktemp /tmp/cdx_upate.XXXXXX)

echo -n "home.."
find ${HOME}/ -not -path '*/\.*' -type d | tee -a ${tmpfile} | wc -l

echo -n "proj-*.."
find ${HOME}/kfw/proj-* -not -path '*/\.*' -type d | tee -a ${tmpfile} | wc -l

tmp=0
echo -n "KFW workspaces..."
for d in  ~/kfw ~/kfw/*FP* ~/kfw/*FP*/* ~/kfw/*FP*/results/* ~/kfw/*FP*/configuration/*; do
    if [ -d "$d" ]; then
	echo "$d" 
	(( tmp+=1 ))
    fi
done >> ${tmpfile}
echo $tmp

echo "writing to ${CDX_FILE}..."
sort -u ${tmpfile} > "${CDX_FILE}" # Important for uniqueness
rm "$tmpfile"
