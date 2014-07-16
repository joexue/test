#!/bin/sh -x

BASE=`dirname $0`
QUEUE=${BASE}/queue
LOCK=${BASE}/lock
PRJDIR=${BASE}/openwrt
GDIR=${GIT_DIR}
export GIT_DIR=${BASE}/openwrt/.git
cd ${PRJDIR}

#$1 project
#$2 branch
#$3 commit
#$4 url
#$5 patchset
check()
{
	project=$1
	branch=$2
	commit=$3
	url=$4
	patchset=$5
	changeid=${url##*/}

	cd ${PRJDIR}
	git branch  | grep -q $branch
	rc=$?
	if [ $rc -ne 0 ]; then
		git checkout --track origin/$branch
	fi
	git checkout $branch
	git pull
	git checkout -b $commit
	git am --abort
	${BASE}/gerrit-cherry-pick origin $changeid/$patchset
	rc=$?
	if [ $rc -ne 0 ]; then
		ssh -p 29418 loadbuild@localhost gerrit review -p ${project} --verified -1 -m '"Can not be merged"' $commit
		return
	fi
	make posroute
	rc=$?
	if [ $rc -ne 0 ]; then
		ssh -p 29418 loadbuild@localhost gerrit review -p ${project} --verified -1 -m '"Build fail"' $commit
		return
	fi
	ssh -p 29418 loadbuild@localhost gerrit review -p ${project} --verified +1 -m '"Build successfull, go ahead"' $commit
	git checkout $branch
	git branch -D $commit
}

pick_check()
{
	line=`sed -n "1p" ${QUEUE}`
	while [ ! -z "$line" ]
	do
		echo $line
		ARGS=`getopt -o a -l  change:,project:,branch:,commit:,is-draft:,change-url:,uploader:,patchset: -- $line`
		#set -- "${ARGS}"
		eval set -- "${ARGS}"
		while true
		do
			case "$1" in
				--project)
					project="$2"
					shift 2
					;;
				--branch)
					branch="$2"
					shift 2
					;;
				--commit)
					commit="$2"
					shift 2
					;;
				--is-draft)
					shift 2
					;;
				--change-url)
					changeurl="$2"
					shift 2
					;;
				--uploader)
					shift 2
					;;
				--patchset)
					patchset="$2"
					shift 2
					;;
				--change)
					shift 2
					;;
				--)
					shift
					break
					;;
				*)
					echo "error"
					exit 1
					;;
			esac
			#shift
		done
		echo "project=$project branch=$branch commit=$commit" > /tmp/gerrit2.log
		if [ "${project}x" = "openwrtx" ]; then
			check $project $branch $commit $changeurl $patchset 
		fi
		sed -i "1d" ${QUEUE}
		line=`sed -n "1p" ${QUEUE}`
	done
}


echo $* >> ${QUEUE}
if [ ! -f ${LOCK} ]; then
	touch ${LOCK}
	pick_check
	rm -f ${LOCK}
fi

export GIT_DIR=$GDIR
exit 0
