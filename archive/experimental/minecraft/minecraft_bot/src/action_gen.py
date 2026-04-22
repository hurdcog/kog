# action_gen.py
#! /usr/bin/env python2.7 python2 python
""" module to generate action in Opencog

This is the part to decide and execute action in Minecraft embodiment.
For now all the work is handled by ActionGenerator class.
It will determine the behavior by a behavior tree and
execute this behavior by calling GPN/GSN.

TODO:
In the future we should seperate the decision making
and action execution to make the ActionGenerator class cleaner.
Also we should have a more systematic way to handle decision,
not just determining behavior by if-else statement.

available class:
    ActionGenerator: used in the main loop of Opencog bot to decide action
"""
from opencog.atomspace import types, TruthValue
from opencog.type_constructors import *
from opencog.bindlink import bindlink, evaluate_atom
from opencog.atomspace import Atom

import random


class ActionGenerator:
    """ determining and executing action of Opencog bot in each loop

    The main loop will call the generate_action() to do all the work.

    Args:
        atomspace(opencog.atomspace.AtomSpace): the AtomSpace in main loop
        space_server(opencog.spacetime.SpaceServer): the SpaceServer in main loop
        time_server(opencog.spacetime.TimeServer): the TimeServer in main loop
    Method:
        generate_action(): generate and execute the action by behavior tree
    """

    def __init__(self, atomspace, space_server, time_server):
        self._atomspace = atomspace
        self._space_server = space_server
        self._time_server = time_server
        self.steps_since_goal_change = 1

    def generate_action(self):
        # TODO: This documantation is outdated.
        """ generate and execute the action by behavior tree

        Now (20150822) we generate action by such a behavior tree:

        If target block found in atomspace:
            If target block is attractive(sti is large enough):
                move to the near place of the target block
        else:
            moving 2 units toward yaw = 90 degree

        For testing, we set a target block(gold, which block id is 14)
        And we assume the world is superflat world so we only need to move on
        x-y direction.
        Note that because the behavior of GSN/GPN,
        we have to import all schemas(action_schemas.py)
        in the main script(opencog_initializer.py).
        Or it will fail to find the function name.

        TODO:
            Build random action function: It's for the 'else' part
                in above behavior tree. This will make the bot behaves more
                naturally.
            Add pathfinding technique: so the bot can calculate the procedure to
                get to the destination.
                it will be able to move to any place he wants.
                (ex. place block to jump to a higher place)
                In Opencog there has been an OCPlanner implementation.
                It should be possible to migrate the old code.
            More higher level ways to making decision: For now it's just testing
                so we only use a simple behavior tree to demo. But in general,
                we should regard these behavior tree as lower level schemas.
                And we should use a higher level cognition ways(e.g. OpenPsi)
                to decide what behavior tree we want to execute.
        """

        # The goal_sucess_rate represents how well a given strategy is working
        # toward achieving the success of the overall goal it is trying to
        # achieve.  This sucess rate defaults to 0 here, and during the main
        # goal execution the result is something that is furthering the goal
        # then it can set this to a higher value.  The more successful we are
        # in satisfying a certain goal, the more likely we are to continue
        # doing this thing, i.e. if we are in the process of doing something
        # that takes time and are making progress, don't switch to some other
        # goal right in the middle of that.
        goal_success_rate = 0.0

        # Read the current goal from atomspace
        goal = bindlink(self._atomspace,
                        BindLink(
                            VariableList(
                                TypedVariableLink(
                                    VariableNode("$goal"),
                                    TypeNode("ConceptNode")
                                ),
                            ),

                            Link(
                                ConceptNode("CURRENT_GOAL"),
                                VariableNode("$goal")
                            ),
                            VariableNode("$goal")
                        )
                        )

        goal_name = goal.out[0].name
        print("goal_name: ", goal_name)

        #######################################################################
        #                           Main Action Tree
        #######################################################################
        # This if - elif chain is the main action generation code for the bot.
        # The chain of if statements here branches off of the currently
        # selected goal.  Inside each if block is code which should further
        # advance that particular goal for the bot.  So for example, on the
        # gather resources goal, the code looks through the bot's memory for
        # blocks which are wood/ore/etc and then travels to one of them and
        # mines it out.
        if goal_name == "Gather resources":
            print("action_gen: gather resources.")
            # Find resources:
            # This bindlink looks through all of the blocks currently in the bot's
            # memory about the world and returns a list of all the ones that are
            # just the base wood type (i.e. what trees are made out of, not planks,
            # slabs, buttons, etc).
            result = bindlink(self._atomspace,
                              BindLink(
                                  VariableList(
                                      TypedVariableLink(
                                          VariableNode("$block"),
                                          TypeNode("StructureNode")
                                      ),
                                      TypedVariableLink(
                                          VariableNode("$material"),
                                          TypeNode("ConceptNode")
                                      ),
                                  ),

                                  AndLink(
                                      EvaluationLink(
                                          PredicateNode("material"),
                                          ListLink(
                                              VariableNode("$block"),
                                              VariableNode("$material")
                                          )
                                      ),
                                      EvaluationLink(
                                          PredicateNode("be"),
                                          ListLink(
                                              VariableNode("$material"),
                                              ConceptNode("WOOD_BLOCK")
                                          )
                                      ),
                                      EvaluationLink(
                                          GroundedPredicateNode(
                                              "py: action_schemas.is_attractive"),
                                          ListLink(
                                              VariableNode("$block")
                                          )
                                      ),
                                      EvaluationLink(
                                          GroundedPredicateNode(
                                              "py: action_schemas.dig_block"),
                                          ListLink(
                                              VariableNode("$block")
                                          )
                                      )
                                  ),
                                  VariableNode("$block")
                              )
                              )
            print("action_gen: result", result)

            # If we sucessfully mined out a block of wood we have been very
            # successful in fulfilling this goal and should continue to try to
            # mine more unless something else really urgent comes up.  If we
            # failed, then we should try to find something else to do.
            if result.out != []:
                goal_success_rate = 5.0
            else:
                goal_success_rate = -5.0

        elif goal_name == "Explore":
            print("action_gen: random walk.")

            # Random walk:
            # Choose a random direction and walk a short distance in that direction and
            # either execute a normal walk or a walk + jump in that direction.
            evaluate_atom(self._atomspace,
                          EvaluationLink(
                              GroundedPredicateNode(
                                  "py: action_schemas.set_relative_move"),
                              ListLink(
                                  RandomChoiceLink(
                                      NumberNode("0"),
                                      NumberNode("45"),
                                      NumberNode("90"),
                                      NumberNode("135"),
                                      NumberNode("180"),
                                      NumberNode("225"),
                                      NumberNode("270"),
                                      NumberNode("315"),
                                  ),
                                  RandomChoiceLink(
                                      NumberNode("1"),
                                      NumberNode("2"),
                                      NumberNode("3"),
                                      NumberNode("4"),
                                  ),
                                  ConceptNode("jump")
                              )
                          )
                          )

            goal_success_rate = 1.0

        elif goal_name == "Look around":
            print("action_gen: look around.")

            # Random walk:
            # Choose a random direction and walk 1 block in that direction and
            # either execute a normal walk or a walk + jump in that direction.
            evaluate_atom(self._atomspace,
                          EvaluationLink(
                              GroundedPredicateNode(
                                  "py: action_schemas.set_relative_look"),
                              ListLink(
                                  RandomChoiceLink(
                                      NumberNode("0"),
                                      NumberNode("90"),
                                      NumberNode("180"),
                                      NumberNode("270"),
                                  ),
                                  NumberNode("0")
                              )
                          )
                          )

            goal_success_rate = 0.5

        else:
            # If we got here then there was no handler coded yet for the
            # currently selected goal.  Return a very negative
            # goal_success_rate so that we switch to another goal since
            # standing around doing nothing is not productive.
            goal_success_rate = -20.0

        # Decide whether or not we should change the current goal, or if we
        # should keep doing the same thing in the next time step.
        print("It has been %s time steps since the goal was changed." % self.steps_since_goal_change)

        # Make it more and more likely to change the current goal depending on
        # how long we have been on the current goal.
        if random.normalvariate(0.0, 1.0) >= 1.5 - 0.1 * \
                self.steps_since_goal_change + 0.1 * goal_success_rate:
            print("\n\n\n\t\t\tChanging current goal\n\n\n")
            self.steps_since_goal_change = 1

            # Get the full list of all the goals in the atomspace
            goal_atoms = bindlink(self._atomspace,
                                  BindLink(
                                      TypedVariableLink(
                                          VariableNode("$goal"),
                                          TypeNode("ConceptNode")
                                      ),
                                      EvaluationLink(
                                          PredicateNode("be"),
                                          ListLink(
                                              ConceptNode("GOAL"),
                                              VariableNode("$goal")
                                          ),
                                      ),
                                      VariableNode("$goal")
                                  ))

            # print "All goals: ", goal_atoms_list
            goal_atoms_list = goal_atoms.out

            # TODO: This should be done in atomese.
            random_goal = goal_atoms_list[
                random.randint(0, len(goal_atoms_list) - 1)]
            print("Random goal: ", random_goal)

            # delete the existing CURRENT_GOAL link and then
            # create a new one pointing to the newly chosen goal.
            self._atomspace.remove(
                Link(
                    ConceptNode("CURRENT_GOAL"),
                    ConceptNode(goal_name)))
            self._atomspace.add_link(
                types.Link, (ConceptNode("CURRENT_GOAL"), random_goal))
        else:
            self.steps_since_goal_change += 1

        print("action_gen end")
