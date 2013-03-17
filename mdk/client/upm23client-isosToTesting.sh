#!/bin/sh

rsync -azPy --bwlimit=45 m23client-*.iso hhabermann,m23@web.sf.net:/home/project-web/m23/htdocs/m23testing
