"""
This file contains lists of basic knowledge about the Minecraft environment
represented as python code.  The code will either consist of simple hardcoded
atom insertion statements, or for more detailed knowledge, loops or functions
creating tens or hundreds of similar atoms according to some algorithm.  The
code should be broken down into a very modular format so very small chunks of
information can be fed into the atomspace one piece at a time, or read out to a
separate atomspace to compare against the atomspace of the bot to test how well
it learned grounded knowledge that we didn't tell it about.

Also, all code should try to input knowledge into the bot in the order "simple
to complex".  In this context, simple knowledge is what a typical player (for
example a 10 year old playing the game for the first time) would know going
into the game and complex are things they would only learn in game after
playing for a long time (things like the fact that creepers explode, or where
to find diamonds or redstone).  If we arrange the knowledge in this order we
can pass to each function a number representing how far along we want to teach
the bot that type of knowledge.  This will allow us to intiailize the bot to
different levels of grounded knowledge for each domain.  So we could load in
100% of the knowledge about breaking blocks, but nothing about enemies or
crafting recipes.  Then spawn a different instance of the bot that knows 50% of
all blocks, enemies, and crafting recipes; and so forth.
"""

from opencog.atomspace import types, TruthValue
from opencog.type_constructors import *
from opencog.atomspace import Atom
from atomspace_util import add_predicate
from minecraft_data.v1_8 import blocks_list
from minecraft_data.v1_8 import items_list
from minecraft_data.v1_8 import entities_list
from minecraft_data.v1_8 import windows_list
from minecraft_data.v1_8 import biomes_list
from minecraft_data.v1_8 import instruments_list



class GroundedKnowledge:

    def __init__(self, atomspace, space_server, time_server):

        self._atomspace = atomspace
        self._space_server = space_server
        self._time_server = time_server

    def load_block_knowledge(self, knowledge_level):
        """ Creates a number of atoms in atomspace which represent grounded
        knowledge about what kinds of blocks drop what resource when mined with
        a given tool.  The info is hardcoded into this python routine as a
        dictionary and then the dictionary info is converted into atomese by
        some for loops which loop over this dictionary.
        """

        print("\nLoading grounded knowledge: blocks and mining")

        # This root atom is the atom that all block types are a subtype of.
        block_type_root_atom = self._atomspace.add_node(
            types.ConceptNode, "BLOCK_TYPE")

        # Loop over the block types in the spock bot library and load each one
        # into the atomspace.
        for block in blocks_list:
            concept_node = self._atomspace.add_node(
                types.ConceptNode, block["displayName"])
            repr_node = add_predicate(self._atomspace,
                                      "Represented in Minecraft by",
                                      concept_node,
                                      NumberNode(str(block["id"])))
            inh_node = add_predicate(
                self._atomspace,
                "be",
                block_type_root_atom,
                concept_node)
            # print inh_node
            # print repr_node

            # If this block type has a recorded hardness
            hard_node = 0
            if "hardness" in block and block["hardness"] >= 0:
                hard_node = add_predicate(self._atomspace,
                                          "Block hardness",
                                          concept_node,
                                          NumberNode(str(block["hardness"])))
                # print hard_node

            # Some block id's use a second 'metadata' value to store a subtype
            # of a particular block type.  If this block type has variations,
            # load them all.
            if "variations" in block:
                for variant in block["variations"]:
                    concept_node = self._atomspace.add_node(
                        types.ConceptNode, variant["displayName"])
                    repr_node = add_predicate(self._atomspace,
                                              "Represented in Minecraft by",
                                              concept_node,
                                              NumberNode(str(block["id"])),
                                              NumberNode(str(variant["metadata"])))
                    inh_node = add_predicate(
                        self._atomspace, "be", block_type_root_atom, concept_node)
                    # print inh_node
                    # print repr_node

                    # If the main block type had a hardness then set the same
                    # hardness for this variation as well.
                    if hard_node != 0:
                        hard_node = add_predicate(
                            self._atomspace, "Block hardness", concept_node,
                            NumberNode(str(block["hardness"])))
                        # print hard_node

    def load_item_knowledge(self, knowledge_level):
        """ Creates nodes in the atomspace for each of the items and their
        various properties
        """

        print("\nLoading grounded knowledge: items")

        # This root atom is the atom that all item types are a subtype of.
        item_type_root_atom = self._atomspace.add_node(
            types.ConceptNode, "ITEM_TYPE")

        # Loop over the items in the spock bot library and load each one into
        # the atomspace.
        for item in items_list:
            atom = self._atomspace.add_node(
                types.ConceptNode, item["displayName"])
            repr_node = add_predicate(self._atomspace,
                                      "Represented in Minecraft by",
                                      atom,
                                      NumberNode(str(item["id"])))
            inh_node = add_predicate(
                self._atomspace, "be", item_type_root_atom, atom)
            # print repr_node
            # print inh_node

            if "variations" in item:
                for variant in item["variations"]:
                    concept_node = self._atomspace.add_node(
                        types.ConceptNode, variant["displayName"])
                    repr_node = add_predicate(self._atomspace,
                                              "Represented in Minecraft by",
                                              concept_node,
                                              NumberNode(str(item["id"])),
                                              NumberNode(str(variant["metadata"])))
                    inh_node = add_predicate(
                        self._atomspace, "be", item_type_root_atom, concept_node)
                    # print repr_node
                    # print inh_node

    def load_entity_knowledge(self, knowledge_level):
        """ Creates atoms representing the different kinds of entities that can
        be encountered in the minecraft world.  Entities are things like the
        small dropped versions of mined blocks which fall on the ground after
        mining, or other creatures like animals or enemies, etc.
        """

        print("\nLoading grounded knowledge: entities")

        # This root atom is the atom that all entity types are a subtype of.
        entity_type_root_atom = self._atomspace.add_node(
            types.ConceptNode, "ENTITY_TYPE")

        # Loop over the entities in the spock bot library and load each one
        # into the atomspace.
        for entity in entities_list:
            atom = self._atomspace.add_node(
                types.ConceptNode, entity["displayName"])
            repr_node = add_predicate(self._atomspace,
                                      "Represented in Minecraft by",
                                      atom,
                                      NumberNode(str(entity["id"])))
            inh_node = add_predicate(
                self._atomspace, "be", entity_type_root_atom, atom)
            # print repr_node
            # print inh_node

            if "type" in entity:
                type_node = add_predicate(
                    self._atomspace, "Minecraft entity type", atom,
                    ConceptNode(str(entity["type"])))
                # print type_node

    def load_category_knowledge(self, knowledge_level):
        """ Creates inheritance links for a bunch of manually defined
        "convenience categories" which are useful for humans to interact with
        the bot, both in the python code and in chat communication.  These are
        also useful in rule learning because rules learned about something
        which is in a category with other things might also apply to those
        other things.  Having some basic categories to nudge the pattern mining
        algorithms in the right direction should make learning initial things
        about the world a bit easier.
        """

        print("\nLoading grounded knowledge: categories")

        categories_dict = {
            "WOOD_BLOCK": (
                "Wood",
                "Oak wood facing up/down",
                "Spruce wood facing up/down",
                "Birch wood facing up/down",
                "Jungle wood facing up/down",
                "Oak wood facing East/West",
                "Spruce wood facing East/West",
                "Birch wood facing East/West",
                "Jungle wood facing East/West",
                "Oak wood facing North/South",
                "Spruce wood facing North/South",
                "Birch wood facing North/South",
                "Jungle wood facing North/South",
                "Oak wood with only bark",
                "Spruce wood with only bark",
                "Birch wood with only bark",
                "Jungle wood with only bark",
                "Wood (Acacia/Dark Oak)",
                "Acacia wood facing up/down",
                "Dark Oak wood facing up/down",
                "Acacia wood facing East/West",
                "Dark Oak wood facing East/West",
                "Acacia wood facing North/South",
                "Dark Oak wood facing North/South",
                "Acacia wood with only bark",
                "Dark Oak wood with only bark"),
            "STONE_BLOCK": (
                "STONE",
                "COBBLESTONE"),
            "ORE_BLOCK": (
                "COAL_ORE",
                "IRON_ORE",
                "GOLD_ORE",
                "DIAMOND_ORE",
                "LAPIS_ORE",
                "REDSTONE_ORE",
                "GLOWSTONE"),
            "PHYSICS_BLOCK": (
                "WATER",
                "LAVA",
                "SAND",
                "GRAVEL"),
            "FLOWING_BLOCK": (
                "WATER",
                "LAVA"),
            "FALLING_BLOCK": (
                "SAND",
                "GRAVEL"),
        }

        # Loop over all the categories.
        for cat_base in categories_dict.keys():
            # Within each category, loop over all of the objects that are in
            # that category.
            for subclass_object in categories_dict[cat_base]:
                base_atom = self._atomspace.add_node(
                    types.ConceptNode, cat_base)
                subclass_atom = self._atomspace.add_node(
                    types.ConceptNode, subclass_object)

                # TODO: Maybe delete this permanantly, for now just store as
                #  a predicate, not as an InheritanceLink
                #inh_atom = self._atomspace.add_link(types.InheritanceLink,
                # [subclass_atom, base_atom])

                pred_atom = add_predicate(
                    self._atomspace, "be", subclass_atom, base_atom)
                # print inh_atom
                # print pred_atom

    def load_goal_knowledge(self, knowledge_level):
        """ Creates atoms for a bunch of different goals to achieve.  The
        satisfaction of these goals leads to the overall happiness of the bot.
        """

        print("\nLoading grounded knowledge: goals")

        goal_root_node = self._atomspace.add_node(types.ConceptNode, "GOAL")

        goal_dict = {
            "Gather resources": {
                "description": "Gather resources like wood, stone, ore, etc. \
                                The base resources which are needed to craft \
                                tools and other items.",
                "init_need": 1,
                "init_desire": 10,
            },

            "Rest": {
                "description": "Stand around doing nothing.",
                "init_need": 0,
                "init_desire": 0.01,
            },

            "Explore": {
                "description": "Discover new blocks.",
                "init_need": 1,
                "init_desire": 2,
            },

            "Discover": {
                "description": "Discover new kinds of blocks.",
                "init_need": 0,
                "init_desire": 100,
            },

            "Look around": {
                "description": "Patrol the already explored area to look at \
                                blocks that have not been seen in a long time.",
                "init_need": 0,
                "init_desire": 0.1,
            },
        }

        for goal in goal_dict:
            concept_node = self._atomspace.add_node(types.ConceptNode, goal)
            inh_node = add_predicate(
                self._atomspace, "be", goal_root_node, concept_node)
            # print inh_node

        current_goal = self._atomspace.add_node(
            types.ConceptNode, "CURRENT_GOAL")
        goal_link = self._atomspace.add_link(
            types.Link, (current_goal, ConceptNode("Look around")))
        # print goal_link

    def load_window_knowledge(self, knowledge_level):
        """ Creates nodes in the atomspace for each of the windows and their
        various properties
        """
        print("\nLoading grounded knowledge: windows")

        # This root atom is the atom that all windows types are a subtype of.
        window_type_root_atom = self._atomspace.add_node(
            types.ConceptNode, "WINDOW_TYPE")

        # Loop over the windows in the spock bot library and load each one into
        # the atomspace.
        for window in windows_list:
            atom = self._atomspace.add_node(types.ConceptNode, window["name"])
            id_concept_node = self._atomspace.add_node(
                types.ConceptNode, window["id"])
            repr_node = add_predicate(
                self._atomspace,
                "Represented in Minecraft by",
                atom,
                id_concept_node)
            inh_node = add_predicate(
                self._atomspace, "be", window_type_root_atom, atom)
            # print "repr_node", repr_node
            # print "inh_node", inh_node

            if "slots" in window:
                for slot in window["slots"]:
                    concept_node = self._atomspace.add_node(
                        types.ConceptNode, slot["name"])
                    index_node = add_predicate(
                        self._atomspace, "Slot index", concept_node, NumberNode(str(slot["index"])))

                    if "size" in slot:
                        size_node = add_predicate(
                            self._atomspace, "Slot index", concept_node, NumberNode(str(slot["size"])))
                        # print "size_node", size_node
                    slot_node = add_predicate(
                        self._atomspace, "Slot", atom, concept_node)
                    # print "index_node", index_node
                    # print "slot_node", slot_node

            if "properties" in window:
                for property in window["properties"]:
                    concept_node = self._atomspace.add_node(
                        types.ConceptNode, property)
                    property_node = add_predicate(
                        self._atomspace, "Property", atom, concept_node)
                    # print "property_node", property_node

            if "openedWith" in window:
                for opened_with in window["openedWith"]:
                    concept_node = self._atomspace.add_node(
                        types.ConceptNode, opened_with["type"])
                    id_node = add_predicate(self._atomspace,
                                            "Opened with ID",
                                            concept_node,
                                            NumberNode(str(opened_with["id"])))
                    opened_with_node = add_predicate(
                        self._atomspace, "Opened with", atom, concept_node)
                    # print "id_node", id_node
                    # print "opened_with_node", opened_with_node

    def load_biome_knowledge(self, knowledge_level):
        """ Creates nodes in the atomspace for each of the biomes and their
        various properties
        """
        print("\nLoading grounded knowledge: biomes")

        # This root atom is the atom that all biome types are a subtype of.
        biome_type_root_atom = self._atomspace.add_node(
            types.ConceptNode, "BIOME_TYPE")

        # Loop over the biome types in the spock bot library and load each one
        # into the atomspace.
        for biome in biomes_list:
            concept_node = self._atomspace.add_node(
                types.ConceptNode, biome["name"])
            repr_node = add_predicate(self._atomspace,
                                      "Represented in Minecraft by",
                                      concept_node,
                                      NumberNode(str(biome["id"])))
            inh_node = add_predicate(
                self._atomspace,
                "be",
                biome_type_root_atom,
                concept_node)
            # print inh_node
            # print repr_node

            color_node = add_predicate(
                self._atomspace, "Color", concept_node, NumberNode(str(biome["color"])))
            # print color_node
            rainfall_node = add_predicate(
                self._atomspace, "Rainfall", concept_node, NumberNode(str(biome["rainfall"])))
            # print color_node
            temperature_node = add_predicate(
                self._atomspace, "Temperature", concept_node, NumberNode(str(biome["temperature"])))
            # print temperature_node

    def load_instrument_knowledge(self, knowledge_level):
        """ Creates nodes in the atomspace for each of the instruments and their
        various properties
        """
        print("\nLoading grounded knowledge: instruments")

        # This root atom is the atom that all instrument types are a subtype
        # of.
        instrument_type_root_atom = self._atomspace.add_node(
            types.ConceptNode, "INSTRUMENT_TYPE")

        # Loop over the instrument types in the spock bot library and load each
        # one into the atomspace.
        for instrument in instruments_list:
            concept_node = self._atomspace.add_node(
                types.ConceptNode, instrument["name"])
            repr_node = add_predicate(self._atomspace,
                                      "Represented in Minecraft by",
                                      concept_node,
                                      NumberNode(str(instrument["id"])))
            inh_node = add_predicate(
                self._atomspace,
                "be",
                instrument_type_root_atom,
                concept_node)
            # print inh_node
            # print repr_node
