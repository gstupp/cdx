The `cdext` utility and associated shell script `cdx` help in the navigation of very large directory trees. The intent is to allow navigation with the miminal amount of input from the user.

# Examples
Consider as an example the following directory tree 
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
> cdx doc     # Change to the folder that matches *doc* (more precisly the extended glob *doc!(*/*)?(/))
/projC/doc
> cdx A//bin #  As a shortcut "//" can be used in the pattern instead of "*"
/projA/dev/bin
# When multiple folders match the pattern the one which has the longest prefix match with the current folder is chosen
> cdx src   
/projA/dev/src
# If the patttern ends with "//" then of all matches with the longest prefix match we pick the one with the longest suffix match.
> cdx prod// 
/projA/proc/src
```

# Introduction
Direcroty trees can be very large. My current home direcorty is the root for over a million directories when including soft links (there are probably loops so this is just an estimate); discovering this tree took over 15 hours.

```
>time find -L ${HOME}/  -not -path '*/\.*' -type d | wc -l
1629587

real    915m12.806s
user    3m56.711s
sys     14m7.513s
```

The `cdext` utility helps the navigation of directory trees by focusing only on a small subset of the tree that is significant and by suppling convineint tools such as globbing and longest prefix match to traverse this subset.  By itself `cdext` is just a textual utility that searches for a pattern in text files, very much like `grep`. A wrapping shell function is supplied to do the actual moving around, similar to the following

```
CDX_FILE=${HOME}/.cdx_db
function cdx ()
{
     cd "$(cdext -p ${PWD} ${@} ${CDX_FILE})" && echo "${PWD}"
}
```

This repository contains the sources of `cdext` along with a bash file (`cdx.d`) that should be sourced, typically from `~/.bashrc`, and an example shell script (`cdx_update_example.sh`) that can be used to create and update a database of directories. 

# Notes
1. `cdext` matches only directories that appear explicitly. That is, no attmpt is made to deduce partial paths from the input.
1. The pattern `pattern` is actually changed to the extended glob `*patten!(*/*)?(/)`. For example, `cdx A` will match `/projA` and not `/projA/dev/bin` in the case above.
