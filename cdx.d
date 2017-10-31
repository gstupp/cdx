CDX_MANUAL_FILE="${HOME}/.cdx_manual"
export CDX_FILE="${HOME}/.cdx_db"

function _cdx () 
{ local cur
  local res
  local g
  COMPREPLY=()   # Array variable storing the possible completions.
  cur=${COMP_WORDS[COMP_CWORD]}
  # Remove last "/" which Bash may have automatically added
  if [[ $cur == */ ]]; then
      g="${cur::${#cur}-1}"
  else
      g="${cur}"
  fi
  res="$(cdext -itp ${PWD} $g ${CDX_FILE} ${CDX_MANUAL_FILE})"
  if [ "$res" != "$g" ] ; then 
      COMPREPLY=(${res});
  else
      COMPREPLY=( $(compgen -d -- "$cur") )
  fi
  return 0
}

function cdx ()
{   local ndir
    if [[ -n "${@}" ]]; then
	ndir="$(cdext -itp ${PWD} ${@} ${CDX_FILE} ${CDX_MANUAL_FILE})"
	if [ -d "${ndir}" ]; then
            cd "${ndir}" && echo "${PWD}"
	elif [ -d "${@}" ]; then
	    cd "${@}" && echo "${PWD}"
	fi
    fi
}

# list all entries that are in the database that match the wildcard
function cdxl ()
{
    if [[ -n "${@}" ]]; then
	cdext -lp ${PWD} ${@} ${CDX_FILE} | sort -g -k 3 -k 5 | cut -d':' -f4
    fi
}

# more verbose list. Includes information such as file origin, distance from current dir etc.
function cdxll ()
{
    if [[ -n "${@}" ]]; then
	cdext -vp ${PWD} ${@} ${CDX_FILE} 
    fi
}

# add current directory to manual database
function cdxa ()
{
    echo "${PWD}" >> $CDX_MANUAL_FILE
}

complete -F _cdx -o filenames -o nospace -o bashdefault cdx
