#!/bin/bash

echo '=============================  Troubleshooter Output ============'
#echo '***************************** BEGIN .bashrc *********************'
#cat ~/.bashrc
#echo '***************************** END .bashrc ***********************'
echo
echo '***************************** Ubuntu Version ********************'
lsb_release -a
echo
echo '***************************** python version ********************'
python -V
echo
echo '****************************** PYTHON PATH **********************'
echo $PYTHONPATH
echo
echo '****************************** LD_LIBRARY_PATH ******************'
echo $LD_LIBRARY_PATH
echo
echo '**** python .so libs from /usr/local/share/opencog/python/opencog ***'
ls /usr/local/share/opencog/python/opencog
echo
echo '********* opencog lib*.so files from /usr/local/lib/opencog ******'
ls /usr/local/lib/opencog
echo
echo '***************************** spockbot eggs *********************'
ls /usr/local/lib/python2.7/dist-packages | grep 'mine\|spock'
echo
echo '***************************** python sys.path *******************'
python -c 'import sys; print sys.path'
echo
echo '**** Import tests: atomspace, spatial, spacetime ****' 
python -c 'from opencog.atomspace import AtomSpace; from opencog.spacetime import SpaceTimeAndAtomSpace; from opencog.spatial import get_near_free_point;'
echo
echo '**** Import tests: spockextras, spockbot ****'
python -c 'from spockbot.plugins.base import pl_announce;  from spockextras.plugins.helpers.MineAndPlace import MineAndPlacePlugin'
echo

