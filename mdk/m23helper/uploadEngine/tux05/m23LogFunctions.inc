#####
# name saveRet
# description Saves the return code of the last called tool to /tmp/HSCommandExecutionStatus.code.
# parameter none
#####
saveRet() {
	ret=$?

	cf="/tmp/HSCommandExecutionStatus.code"
	aret=`cat $cf`

	expr $ret \+ $aret > $cf
}





#####
# name resetRet
# description Resets the file that contains the accumulated return codes of the commands to execute.
# parameter none
#####
resetRet() {
	echo 0 > /tmp/HSCommandExecutionStatus.code
}




#####################
# stopLog and startLog are based on the KVM guest launcher. The author wishes to include the following notice:
#
# Copyright (c) 2010, Noah Spurrier <noah@noah.org>
# Permission to use, copy, modify, and/or distribute this software for any
# purpose with or without fee is hereby granted, provided that the above
# copyright notice and this permission notice appear in all copies.
#
# THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
# WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
# MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
# ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
# WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
# ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
# OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
#
# Version 1
#####################





#####
# name startLog
# description Starts logging stdout and stderr to /tmp/HSCommandExecutionStatus.message.
# parameter none
#####
startLog()
{
	LOGFILE="/tmp/HSCommandExecutionStatus.message"
	PIPEFILE="/tmp/HSCommandExecutionStatus.pipe"
	rm ${LOGFILE}
	mkfifo ${PIPEFILE}
	tee ${LOGFILE} < ${PIPEFILE} &
	TEEPID=$!
	# Redirect subsequent stdout and stderr output to named pipe.
	exec > ${PIPEFILE} 2>&1
	trap stopLog EXIT
}





#####
# name stopLog
# description Stops logging stdout and stderr to /tmp/HSCommandExecutionStatus.message.
# parameter none
#####
stopLog()
{
	messageHeader="$1"

	# Close stdin and stdout which closes our end of the pipe
	# and tells `tee` we are done.
	exec 1>&- 2>&-
	# Wait for `tee` process to finish. If we exited here then the `tee`
	# process might get killed before it hand finished flushing its buffers
	# to the logfile.
	wait ${TEEPID}
	rm ${PIPEFILE}

	ret=`cat /tmp/HSCommandExecutionStatus.code`

	if [ $ret -ne 0 ]
	then
		cat /tmp/HSCommandExecutionStatus.message | mail -s "$messageHeader: Fehler aufgetreten" hauke@goos-habermann.de
	else
		echo "$messageHeader" |  mail -s "$messageHeader: alles ok" hauke@goos-habermann.de
	fi

	return $ret
}





#####
# name gpgUpload
# description Encrypts and uploads a file
# parameter none
#####
gpgUpload()
{
	if [ $2 ]
	then
		gpgAdd="-r $2"
	else
		gpgAdd=''
	fi

	cat $1 | gpg --trust-model always --batch --no-tty --yes --digest-algo sha1 -e -r 2814BE38 $gpgAdd --no-use-agent > $1.gpg
	resetRet
	webdavbackup.sh backup $1.gpg

#	share-online.biz-uploader $1.gpg
#	freakshare-uploader $1.gpg
#	fileSonic-uploader $1.gpg
	resetRet
	rm $1.gpg
}
