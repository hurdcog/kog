# action_schemas.py
#! usr/bin/env python2.7 python2 python
"""
action schemas called by GroundedPredicate/SchemaNode in ActionGenerator

When importing this it will ask for the ROS service to ROS.
So to make it work we have to start the embodiment-testing/actionsnode.py
Note:
1. All function arguments must be atom. Or the GPN/GSN will fail.
   If the return value of function is not atom, the GSN will fail.
   If the return value of function is not TruthValue, the GPN will fail.
2. Because the GSN/GPN will find functions in outermost shell scope,
   we have to import schemas in starting script(e.g. opencog_initializer.py),
   not the action generator module.
   Or, the python evaluator in Opencog will fail to find the function.
3. We assume the atomspace and Space/Time server in main loop
   are the ones in the opencog.spacetime.SpaceTimeAndAtomSpace instance.
   If not, all the schemas will fail because they can't find the argument atoms.
4. For now(20150822) they are imported through the config file passed to
   opencog.utilities.initialize_opencog() in starting script. If not do so,
   the Python evaluator in Opencog cannot find function name through module,
   ex. It can find "is_attractive"
       but unable to find "action_schemas.is_attractive".
   This behavior is strange; for now to make code easier to read we want to
   put the module name(action_schema) in the GPN/GSN. So we have to import it
   in such a weird way.

Method:
is_attractive(atom, sti_std) : Judge an atom is attractive (large sti).
move_toward_block(block_atom) : move to a place near the input block.
set_look(pitch_atom, yaw_atom) : look toward the given direction.
set_relative_look(pitch_atom, yaw_atom): look toward current + given direction.
set_relative_move(yaw_atom, dist_atom, jump_atom): move toward the given yaw.

Exception:
    rospy.ServiceException: raised when errored in asking for ROS services.

TODO:
Seperate the schemas to different modules: some schemas are for decision making
    (ex. is_attractive), and others are for executing action.
    We should put them in different modules or it will be hard to manage them.
    For now it's OK because there's only a few functions.
    And a problem is that, the initialize_opencog() can't read the config file
    correctly when we set multiple path in PYTHON_PRELOAD_FUNCTIONS. It must be
    fixed if we want to seperate schemas.
"""

import math
import roslib
roslib.load_manifest('minecraft_bot')
import rospy
from minecraft_bot.srv import look_srv, rel_move_srv, abs_move_srv, dig_srv
from opencog.spacetime import SpaceTimeAndAtomSpace
from opencog.spatial import get_near_free_point
from opencog.atomspace import types, TruthValue
from opencog.type_constructors import *

rospy.wait_for_service('set_relative_look')
rospy.wait_for_service('set_look')
rospy.wait_for_service('set_relative_move')
rospy.wait_for_service('set_move')
rospy.wait_for_service('set_dig')

atomspace = SpaceTimeAndAtomSpace().get_atomspace()
space_server = SpaceTimeAndAtomSpace().get_space_server()

try:
    _ros_set_relative_look = rospy.ServiceProxy('set_relative_look', look_srv)
    _ros_set_look = rospy.ServiceProxy('set_look', look_srv)
    _ros_set_relative_move = rospy.ServiceProxy(
        'set_relative_move', rel_move_srv)
    _ros_set_move = rospy.ServiceProxy('set_move', abs_move_srv)
    _ros_set_dig = rospy.ServiceProxy('set_dig', dig_srv)
except rospy.ServiceException as e:
    print("service call failed: %s" % e)


def is_attractive(atom, sti_std=1):
    """
    Judge if atom is attractive enough

    Args:
        atom(opencog.atomspace.Atom): The atom to be checked itself.
        sti_std: Standard sti value against which the atom would be judged.

    Returns: TruthValue(1,1) if it's attractive else TruthValue(0,1)
    """

    # print 'is_attractive'
    sti = atom.av['sti']
    if sti > sti_std:
        # print 'attractive!'
        return TruthValue(1, 1)
    else:
        # print 'boring'
        return TruthValue(0, 1)


def move_toward_block(block_atom):
    """
    Make bot move near the block. If there's no surrounding block info,
    the bot may find no place to stand on and stop moving.
    Also if there's no empty place near ( distance <= 2) the block,
    the bot will not move.
    Args:
        block_atom(opencog.atomspace.Atom): The atom representing block,
                towards which bot is made to move.

    Returns: TruthValue(1,1) if move success else TruthValue(0,1).

    TODO:
        Make it faster: the calculation of near place is slow.
        Add distance argument: make caller control the judgement of "near"
    """

    print('move toward atom', block_atom)

    # Get the block position info from the atomspace, to do this we query
    # spacemap.
    jump = False
    map_handle = (atomspace.get_atoms_by_name(
        types.SpaceMapNode, "MCmap")[0]).h
    cur_map = space_server.get_map(map_handle)
    cur_er = space_server.get_entity_recorder(map_handle)
    block_pos = cur_map.get_block_location(block_atom.h)

    # If we did not find the block in the spacemap than return false, otherwise
    # continue the function.
    if block_pos is None:
        print('block position not found.', block_atom)
        return TruthValue(0, 1)

    # Try to find an open block we can stand on near the target block.
    dest = get_near_free_point(
        atomspace, cur_map, block_pos, 2, (1, 0, 0), True)

    if dest is None:
        print('get_no_free_point')

    print('block_pos, dest', block_pos, dest)

    # Get the position of the bot.
    self_handle = cur_er.get_self_agent_entity()
    self_pos = cur_er.get_last_appeared_location(self_handle)

    # Check to see if we are already standing where we want, if so then we are
    # done, otherwise actually pass the move call along to ROS.
    if (math.floor(self_pos[0]) == dest[0]
            and math.floor(self_pos[1]) == dest[1]
            and math.floor(self_pos[2]) == dest[2]):
        print('has arrived there')
        return TruthValue(1, 1)
    else:
        # Pass the call to ROS and return its success value back to the caller of this function.
        # TODO: In Minecraft the up/down direction is y coord
        # but we should swap y and z in ros node, not here..
        response = _ros_set_move(block_pos[0], block_pos[1], jump)
        # TODO: The AI is expected to finish a full step in less than 1 second,
        # so we should figure out a better value for this wait timer.
        rospy.sleep(1)
        print('action_schemas: abs_move response', response)
        if response.state:
            print('move success')
            return TruthValue(1, 1)
        else:
            print('move fail')
            return TruthValue(0, 1)


def dig_block(block_atom):
    """
    Make the bot mine the block with the currently selected tool until
    the block is mined out.

    Args:
        block_atom(opencog.atomspace.Atom): The atom representing block,
                which the bot has to mine.

    Returns: TruthValue(1,1) if block is mined and TruthValue(0,1) otherwise.
    """

    print('Dig block:', block_atom)

    map_handle = (atomspace.get_atoms_by_name(
        types.SpaceMapNode, "MCmap")[0]).h
    cur_map = space_server.get_map(map_handle)
    block_pos = cur_map.get_block_location(block_atom.h)
    if block_pos is None:
        print('block position not found.', block_atom)
        return TruthValue(0, 1)
    else:
        # TODO: Flipping y and z positions for ROS to Minecraft convention
        response = _ros_set_dig(block_pos[0], block_pos[2], block_pos[1])
        return TruthValue(1, 1)


def set_look(pitch_atom, yaw_atom):
    """
    Set look toward the given direction.
    The look direction will be changed to the direction passed.

    Args:
        pitch_atom(opencog.atomspace.Atom): NumberNode representing pitch in degree
        yaw_atom(opencog.atomspace.Atom): NumberNode representing yaw in degree

    Returns: TruthValue(1,1) if set look success else TruthValue(0,1)
    """

    pitch = float(pitch_atom.name)
    yaw = float(yaw_atom.name)
    response = _ros_set_look(yaw, pitch)
    if response == True:
        return TruthValue(1, 1)
    else:
        return TruthValue(0, 1)


def set_relative_look(pitch_atom, yaw_atom):
    """
    Set relative look toward the given direction.
    The look will be changed by adding pitch and yaw on current direction.

    Args:
        pitch_atom(opencog.atomspace.Atom): NumberNode representing relative
                pitch in degree
        yaw_atom(opencog.atomspace.Atom): NumberNode representing relative
                yaw in degree

    Returns: TruthValue(1,1) if set look success else TruthValue(0,1)
    """

    pitch = float(pitch_atom.name)
    yaw = float(yaw_atom.name)
    response = _ros_set_relative_look(pitch, yaw)
    if response is True:
        return TruthValue(1, 1)
    else:
        return TruthValue(0, 1)


def set_relative_move(yaw_atom, dist_atom, jump_atom):
    """
    Set relative move toward the given direction.
    The bot will move toward the yaw direction in input distance.
    For now the jump_atom is not used. We assume bot will not jump
    during the move.

    Args:
        yaw_atom(opencog.atomspace.Atom): NumberNode representing yaw in degree.
        dist_atom(opencog.atomspace.Atom): NumberNode representing distance in
                Minecraft unit.
        jump_atom(opencog.atomspace.Atom):
            The decision of jumping is taken by checking the TV of this atom.

    Returns: TruthValue(1,1) if move success else TruthValue(0,1)
    """

    print('set_rel_move')
    yaw = float(yaw_atom.name)
    dist = float(dist_atom.name)

    if jump_atom.tv == TruthValue(1,1):
        jump = True
    else:
        jump = False
    response = _ros_set_relative_move(yaw, dist, jump)
    print('set_rel_move: yaw, dist, jump, res', yaw, dist, jump, response)
    if response is True:
        return TruthValue(1, 1)
    else:
        return TruthValue(0, 1)



