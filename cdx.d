export CDX_FILE="${HOME}/.cdx_db"

function cdx ()
{   local ndir
    if [[ -n "${@}" ]]; then
	ndir="$(cdext -p ${PWD} ${@} ${CDX_FILE})"
	if [ -n "${ndir}" ]; then
            cd "${ndir}" && echo "${PWD}"
	fi
    fi
}

function cdxl ()
{
    if [[ -n "${@}" ]]; then
	cdext -lp ${PWD} ${@} ${CDX_FILE} | sort -g -k 3 -k 5 -k 7 | cut -d':' -f5
    fi
}

function cdxv ()
{
    if [[ -n "${@}" ]]; then
	cdext -vp ${PWD} ${@} ${CDX_FILE} 
    fi
}
