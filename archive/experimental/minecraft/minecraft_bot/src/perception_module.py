# perception_module.py
# ! /usr/bin/env python2.7 python2 python

from opencog.atomspace import AtomSpace, Atom, TruthValue, types, \
    get_refreshed_types
from opencog.type_constructors import *
from opencog.cogserver_type_constructors import *
from opencog.spacetime import SpaceServer, TimeServer

types = get_refreshed_types()  # update spacetime types imported
from opencog.atomspace import Atom
from atomspace_util import add_predicate, add_location
from atomspace_util import get_predicate, get_most_recent_pred_val
from ros_perception import ROSPerceptionInterface
from spockbot.mcdata import blocks

default_map_timestamp = 0
default_map_name = "MCmap"
default_map_resolution = 1
default_map_agent_height = 1
default_map_floor_height = -255


def swap_y_and_z(coord):
    temp_y = coord.y
    coord.y = coord.z
    coord.z = temp_y
    return coord


class PerceptionManager:
    def __init__(self, atomspace, space_server, time_server):
        self._handle_dict = {
            "client_position_data": self.handle_self_pos_message,
            "camera_vis_data": self.handle_vision_message}
        self._receiver = ROSPerceptionInterface(self._handle_dict)
        self._atomspace = atomspace
        self._space_server = space_server
        self._time_server = time_server
        self._space_server.add_map(default_map_timestamp,
                                   default_map_name,
                                   default_map_resolution)

    def _get_map(self, map_name=default_map_name):
        try:
            map_handle = (self._atomspace.get_atoms_by_type(
                types.SpaceMapNode)[0])
        except IndexError:
            return None, None
        return map_handle, self._space_server.get_map(map_handle)

    def _get_er(self, er_name=default_map_name):
        # Note: Currently we use the single SpaceMapNode to
        # index the SpaceMap and EntityRecorder
        # But here we still call it er_handle
        try:
            er_handle = (self._atomspace.get_atoms_by_type(
                types.SpaceMapNode)[0])
        except IndexError:
            return None, None
        return er_handle, self._space_server.get_entity_recorder(er_handle)

    def handle_vision_message(self, data):
        # print "handle_visiion_message"
        # TODO: In Minecraft the up/down direction is y coord
        # but we should swap y and z in ros node, not here..
        for block in data.blocks:
            swap_y_and_z(block)
        material_dict = {}
        map_handle, cur_map = self._get_map()
        for block in data.blocks:
            old_block_handle = cur_map.get_block((block.x, block.y, block.z))
            updated_eval_links = []

            # Count how many of each block type we have seen during this vision frame.
            # Also store the new block material in case it differs from the existing material.
            # TODO: Use this dict for something or it can be removed, currently
            # it is created and filled up but not used by anything else.
            block_material = blocks.get_block(
                block.blockid, block.metadata).display_name

            if block_material in material_dict:
                material_dict[block_material] += 1
            else:
                material_dict[block_material] = 1

            # If this is the first time this block has been seen
            if old_block_handle == None:
                # Create the block in atomspace and set its initial attention
                # value.
                blocknode, updated_eval_links = self._build_block_nodes(
                    block, map_handle)
                # TODO: Make the 200 a constant, this occurs one other place.

                blocknode.av['sti'] = 200
            else:
                # Block already exists, check to see if it is still the same
                # type.
                old_block_type_node = get_predicate(self._atomspace, "material",
                                                    old_block_handle, 1)
                old_block_type = old_block_type_node.name
                if old_block_type == block_material:
                    # Add short term importance to this block since it is in
                    # the field of view.
                    cur_sti = old_block_handle.av['sti']
                    # TODO: Make these numbers constants.
                    # TODO: Make the amount added be dependendant on the
                    # distance to the block.
                    cur_sti = min(cur_sti + 20, 500)
                    old_block_handle.av['sti'] = cur_sti
                    continue
                elif block.blockid == 0:
                    # Block used to be solid and is now an air block, remove it
                    # from the atomspace and mark the old block as being
                    # disappeared for attention allocation routine to look at.

                    blocknode, updated_eval_links = Atom(-1), []
                    disappeared_link = add_predicate(
                        self._atomspace, "disappeared", old_block_handle)
                    updated_eval_links.append(disappeared_link)
                else:
                    # NOTE: There is a bit of a bug here since the attention
                    # value does not increase here, but that is ok because this
                    # is rare anyway so skipping an increase is no big deal.
                    blocknode, updated_eval_links = self._build_block_nodes(
                        block,
                        map_handle)

                disappeared_link = add_predicate(
                    self._atomspace, "disappeared", old_block_handle)
                updated_eval_links.append(disappeared_link)

            # Add the block to the spaceserver and the timeserver.
            self._space_server.add_map_info(blocknode, map_handle, False,
                                            False,
                                            block.ROStimestamp,
                                            block.x, block.y, block.z)
            if old_block_handle == None:
                self._time_server.add_time_info(
                    blocknode, block.ROStimestamp, "ROS")
                self._time_server.add_time_info(
                    blocknode, block.MCtimestamp, "MC")
            for link in updated_eval_links:
                self._time_server.add_time_info(
                    link, block.ROStimestamp, "ROS")
                self._time_server.add_time_info(
                    link, block.MCtimestamp, "MC")
                # print blocknode
                # print updated_eval_links

        # TODO: The code below stores the number of blocks of each type seen in
        # the current field of view into the atomspace.  It is commented out as
        # it should probably not store this until the code to erase old values
        # is also added otherwise this data just piles up as new links from the
        # same root node and it becomes a jumbled mess.

        # print "\nBlock material summary:  saw %s kinds of blocks" %
        # len(material_dict)
        """
        for material in material_dict.keys():
            #print "Saw %s '%s' blocks this frame." % (material_dict[material], material)
            pred_node = add_predicate(self._atomspace, "Currently seen blocks of type", ConceptNode(material), NumberNode(str(material_dict[material])))
            self._atomspace.add_link(types.Link, [ConceptNode("Currently seen block stats"), pred_node])
        """

        # print "handle_vision_message end"

    def handle_self_pos_message(self, data):
        # print 'handle_self_pos_message'
        # TODO: In Minecraft the up/down direction is y coord
        # but we should swap y and z in ros node, not here..
        swap_y_and_z(data)
        map_handle, cur_map = self._get_map()
        _, cur_er = self._get_er()
        old_self_handle = cur_er.get_self_agent_entity()
        self_node, updated_eval_links = self._build_self_pos_node(
            data, map_handle)

        # TODO: pass timestamp in message
        timestamp = 0
        self._space_server.add_map_info(self_node, map_handle,
                                        True, True, timestamp,
                                        data.x, data.y, data.z, "ROS")
        if old_self_handle == None:
            self._time_server.add_time_info(self_node, timestamp, "ROS")
            self._time_server.add_time_info(self_node, timestamp, "MC")
        for link in updated_eval_links:
            self._time_server.add_time_info(link, timestamp, "ROS")
            self._time_server.add_time_info(link, timestamp, "MC")
            # print self_node
            # print self._atomspace.get_incoming(self_node.h)
            # print "handle_self_pos_message_end"

    def _build_self_pos_node(self, client, map_handle):
        # TODO: for now because we only input self client so we define node name as "self"
        # but this should be included in the attribute
        client_node = self._atomspace.add_node(types.EntityNode, "self")
        updated_eval_links = []

        at_location_link = add_location(self._atomspace, client_node,
                                        map_handle,
                                        [client.x, client.y, client.z])
        updated_eval_links.append(at_location_link)

        type_node = self._atomspace.add_node(types.ConceptNode, "client")
        type_link = add_predicate(self._atomspace, "clienttype",
                                  client_node, type_node)
        updated_eval_links.append(type_link)

        yaw_node = self._atomspace.add_node(types.NumberNode, str(client.yaw))
        pitch_node = self._atomspace.add_node(
            types.NumberNode, str(client.pitch))
        look_link = add_predicate(self._atomspace, "look",
                                  client_node, yaw_node, pitch_node)
        updated_eval_links.append(look_link)
        return client_node, updated_eval_links

    def _build_block_nodes(self, block, map_handle):

        # hack to make static object No. variable in class method
        if not hasattr(self._build_block_nodes.__func__, "objNo"):
            self._build_block_nodes.__func__.objNo = 0
        # Note: in 3DSpaceMap using structure node to represent block,
        # entity node to represent entity

        obj_node = self._atomspace.add_node(types.StructureNode,
                                            "obj%s" % (
                                                self._build_block_nodes.__func__.objNo))
        self._build_block_nodes.__func__.objNo += 1
        updated_eval_links = []

        at_location_link = add_location(self._atomspace, obj_node, map_handle,
                                        [block.x, block.y, block.z])
        updated_eval_links.append(at_location_link)

        type_node = self._atomspace.add_node(
            types.ConceptNode, blocks.get_block(
                block.blockid, block.metadata).display_name)
        material_link = add_predicate(self._atomspace, "material",
                                      obj_node, type_node)
        updated_eval_links.append(material_link)

        new_appeared_link = add_predicate(self._atomspace, "new_block",
                                          obj_node)
        updated_eval_links.append(new_appeared_link)

        return obj_node, updated_eval_links

    def _build_entity_node(self, entity, map_handle):
        entity_node = self._atomspace.add_node(
            types.EntityNode, str(entity.eid))
        updated_eval_links = []

        at_location_link = add_location(self._atomspace, entity_node,
                                        map_handle,
                                        [entity.x, entity.y, entity.z])
        updated_eval_links.append(at_location_link)

        type_node = self._atomspace.add_node(
            types.ConceptNode, str(entity.mob_type))
        type_link = add_predicate(self._atomspace, "entitytype",
                                  entity_node, type_node)
        updated_eval_links.append(type_link)

        yaw_node = self._atomspace.add_node(
            types.NumberNode, str(entity.head_yaw))
        pitch_node = self._atomspace.add_node(
            types.NumberNode, str(entity.head_pitch))
        look_link = add_predicate(self._atomspace, "look",
                                  entity_node, yaw_node, pitch_node)
        updated_eval_links.append(look_link)

        length_node = self._atomspace.add_node(
            types.NumberNode, str(entity.length))
        width_node = self._atomspace.add_node(
            types.NumberNode, str(entity.width))
        height_node = self._atomspace.add_node(
            types.NumberNode, str(entity.height))
        sizelink = add_predicate(self._atomspace, "size",
                                 entity_node, length_node, width_node,
                                 height_node)
        updated_eval_links.append(sizelink)

        v_x_node = self._atomspace.add_node(
            types.NumberNode, str(entity.velocity_x))
        v_y_node = self._atomspace.add_node(
            types.NumberNode, str(entity.velocity_y))
        v_z_node = self._atomspace.add_node(
            types.NumberNode, str(entity.velocity_z))
        velocitylink = add_predicate(self._atomspace, "velocity",
                                     entity_node, v_x_node, v_y_node, v_z_node)
        updated_eval_links.append(velocitylink)
        return entity_node, updated_eval_links
