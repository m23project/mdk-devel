#!/bin/sh

rsync -azPy --bwlimit=55 m23client-i386.iso hhabermann,m23@web.sf.net:/home/project-web/m23/htdocs/m23testing
