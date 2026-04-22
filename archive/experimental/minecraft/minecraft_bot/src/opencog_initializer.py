#! /usr/bin/env python

# A python script executed in cogserver to initialize all things
import os
import roslib
roslib.load_manifest('minecraft_bot')
import rospy
import time
from opencog.spacetime import SpaceTimeAndAtomSpace
from opencog.atomspace import AtomSpace, types
from opencog.type_constructors import set_type_ctor_atomspace
from opencog.utilities import initialize_opencog, finalize_opencog
from perception_module import PerceptionManager
from attention_module import AttentionController
from action_gen import ActionGenerator
from grounded_knowledge import GroundedKnowledge

rospy.init_node('OpenCog_Perception')
spacetime = SpaceTimeAndAtomSpace()
full_path = os.path.realpath(__file__)
config_file_name = os.path.dirname(full_path) + "/opencog_python_eval.conf"
print(config_file_name)
# import GSN/GPN schema
initialize_opencog(spacetime.get_atomspace(), config_file_name)

set_type_ctor_atomspace(spacetime.get_atomspace())
pm = PerceptionManager(spacetime.get_atomspace(),
                       spacetime.get_space_server(),
                       spacetime.get_time_server())
ag = ActionGenerator(spacetime.get_atomspace(),
                     spacetime.get_space_server(),
                     spacetime.get_time_server())
ac = AttentionController(spacetime.get_atomspace())

gn = GroundedKnowledge(spacetime.get_atomspace(),
                       spacetime.get_space_server(),
                       spacetime.get_time_server())

time_step = 1

print("\nAtomspace contains %s atoms" % len(spacetime.get_atomspace()))
gn.load_block_knowledge(1.0)
print("Atomspace contains %s atoms" % len(spacetime.get_atomspace()))
gn.load_item_knowledge(1.0)
print("Atomspace contains %s atoms" % len(spacetime.get_atomspace()))
gn.load_entity_knowledge(1.0)
print("Atomspace contains %s atoms" % len(spacetime.get_atomspace()))
gn.load_category_knowledge(1.0)
print("Atomspace contains %s atoms" % len(spacetime.get_atomspace()))
gn.load_goal_knowledge(1.0)
print("Atomspace contains %s atoms" % len(spacetime.get_atomspace()))
gn.load_window_knowledge(1.0)
print("Atomspace contains %s atoms" % len(spacetime.get_atomspace()))
gn.load_biome_knowledge(1.0)
print("Atomspace contains %s atoms" % len(spacetime.get_atomspace()))
gn.load_instrument_knowledge(1.0)
print("Atomspace contains %s atoms" % len(spacetime.get_atomspace()))


while not rospy.is_shutdown():
    print("\n\nTime Step: ", time_step)
    time_step += 1

    temp_time = time.time()
    ac.control_av_in_atomspace()
    time_control_av_in_atomspace = time.time() - temp_time

    temp_time = time.time()
    ag.generate_action()
    time_generate_action = time.time() - temp_time

    time_total = time_control_av_in_atomspace + time_generate_action
    print("\nTime spent\nattention value: %s\ngenerate action: %s\n-----------\ntotal: %s" % (time_control_av_in_atomspace, time_generate_action, time_total))

    if(time_total > 1.0):
        print("WARNING: AI took more than 1 second to execute!")

    print("\nAtomspace contains %s atoms" % len(spacetime.get_atomspace()))

    rospy.sleep(1.0 - time_total)
