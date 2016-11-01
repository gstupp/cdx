#!/usr/intel/bin/bash
tmpfile=$(mktemp /tmp/cdx_upate.XXXXXX)

echo -n "home.."
find ${HOME}/ -not -path '*/\.*' -type d | tee -a ${tmpfile} | wc -l

echo -n "proj-*.."
find ${HOME}/kfw/proj-* -not -path '*/\.*' -type d | tee -a ${tmpfile} | wc -l

echo "writing to ${CDX_FILE}..."
sort -u ${tmpfile} > "${CDX_FILE}" # Important for uniqueness
rm "$tmpfile"
