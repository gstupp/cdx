The CD Extender utility helps in the navigation of very large directory trees by figuring out the target directory based on minimal input from the user (ideally just the last part of the pathname). 
In itself `cdext` is just a textual utility that searches for a pattern in text files, very much like `grep`. A wrapping shell function is supplied to do the actual moving around, as in this example

```
CDX_FILE=${HOME}/.cdx_db
function cdx ()
{
     cd "$(cdext -p ${PWD} ${@} ${CDX_FILE})" && echo "${PWD}"
}
```

This repository contains the sources of cdext that can be compiled along with a bash file that should be source-ed (`cdx.d`) and an example shell script (`cdx_update.sh`) that can be used to create a database.

# Introduction
Direcroty trees can be very large. For example, my current home is the root for over a million directories when including soft links (there are probably loops so this is just an estimate); discovering this tree took over 15 hours.

```
>time find -L ${HOME}/  -not -path '*/\.*' -type d | wc -l
1629587

real    915m12.806s
user    3m56.711s
sys     14m7.513s
```

The `cdext` utility along with its associated shell scripts helps the navigation of this tree by focusing only on a small subset of the tree that is significant and suppling convineint tools such as globbing and longest prefix match to traverse this subset. Consider as an example the following directory tree 
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

Assuming that `cdx.d` was sourced and that `find / -print | sort -u > ${CDX_FILE}` was executed the following can be used to traverse the tree

```
> cdx doc     # globbing of *doc*
/projC/doc
> cdx A//bin # globbing of *A*
/projA
> cdx bin  # globbing of *bin* and longest prefix match
/projA/dev/bin
> cdx prod// # globbing + shortest prefix match + longest suffix match


