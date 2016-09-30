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
