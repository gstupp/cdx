The `cdext` utility and `cdx` shell script (function) help in the navigation of very large directory trees. The intent is to allow navigation with miminal input from the user.

# Examples
Consider the following directory tree 
```
/
|-- projA
|   |-- dev
|   |   |-- bin
|   |   |-- include
|   |   `-- src
|   `-- prod
|       |-- bin
|       |-- include
|       `-- src
|-- projB
`-- projC
    |-- doc
    |-- html
    `-- latex
```

Assuming a db file was created with `find / -print | sort -u > ${CDX_FILE}` the following can be used to traverse the tree
```
> pwd
> /
> cdx doc     # chdir to the folder whose path matches *doc* 
/projC/doc
> cdx A//bin #  chdir to the folder whose path matches *A*bin*. 
             #  As a convinence, "//" can be used instead of "*".
/projA/dev/bin
# When multiple folders match the pattern the one which is logically the nearest
# (has the longest prefix match with the current folder) is chosen
> cdx src   
/projA/dev/src
# As a special case, if the patttern ends with "//" then of all matches with the longest
# prefix match we choose the one with the longest suffix match.
# This can be used to change a directory in the path that is not the last one.
> cdx prod// 
/projA/prod/src
```
# Introduction
Direcroty trees can be very large. My home direcorty is the root for over a million directories when including soft links (there are probably loops so this is just an estimate); discovering this tree took over 15 hours.
```
>time find -L ${HOME}/  -not -path '*/\.*' -type d | wc -l
1629587

real    915m12.806s
user    3m56.711s
sys     14m7.513s
```
The `cdext` utility and `cdx` shell script help in the navigation of directory trees by focusing on a small, predefined, subset of the tree that is useful and by suppling tools such as globbing and longest prefix match to traverse it.  By itself `cdext` is just a textual utility that searches for a pattern in text files, very much like `grep`. The wrapping shell function `cdx` which is very similar to the following does the actual moving around. This is necessary because `cdext` is executed in a sub-process which cannot change the current path of the shell process.

```
CDX_FILE=${HOME}/.cdx_db
function cdx ()
{
     cd "$(cdext -p ${PWD} ${@} ${CDX_FILE})" && echo "${PWD}"
}
```
The `cdext` command accepts as paramters a `pivot` (typically the current directory), a  `pattern` and a list of files (typically `${CDX_FILE}`). If `pattern` contains `//` they are swapped by `*`. Then a new (extended) pattern is defined to be `*patten!(*/*)?(/)` which is compared against every line in the input. For each line that matches the new pattern the LPM (Longest prefix match) to the pivot is computed. If the patten ends with `//` the LSM (Longest Suffix Match) is computed as well. Finally the first entry that matched  `pattern`, had the best LPM to the pivot and, optionally, also the best LSM is returned.

# Notes
1. The reason `pattern` is  changed to the extended glob `*patten!(*/*)?(/)` instead of just `*pattern*` is so that for example `cdx A` will match `/projA` but not `/projA/dev/bin` in the example above.
1. `cdext` matches only directories that appear explicitly (have their own entry). No attmpt is made to deduce partial paths from the input.
1. If the wildcard pattern plus the LPM (and LSM when used) score still return more than one candidate the one closest to the pivot in the directory tree is chosen.

# Installation
This repository contains the sources of `cdext` which should be compiled and placed in the `PATH`, along with a bash file (`cdx.d`) that should be sourced, typically from `~/.bashrc`. Also an example shell script (`cdx_update_example.sh`) is supplied that can be used to create and update a database of directories. 


