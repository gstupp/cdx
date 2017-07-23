CDX_MANUAL_FILE="${HOME}/.cdx_manual"
export CDX_FILE="${HOME}/.cdx_db"

function cdx ()
{   local ndir
    if [[ -n "${@}" ]]; then
	ndir="$(cdext -ip ${PWD} ${@} ${CDX_FILE} ${CDX_MANUAL_FILE})"
	if [ -n "${ndir}" ]; then
            cd "${ndir}" && echo "${PWD}"
	fi
    fi
}

# list all entries that are in the database that match the wildcard
function cdxl ()
{
    if [[ -n "${@}" ]]; then
	cdext -lp ${PWD} ${@} ${CDX_FILE} | sort -g -k 3 -k 5 -k 7 | cut -d':' -f5
    fi
}

# more verbose list. Includes information such as file origin, distance from current dir etc.
function cdxvll ()
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
