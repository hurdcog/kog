import os
import sys
import argparse
import minecraft_bot
import spockextras

# Formation of the argument parser to parse the arguments when executed as
# command line script.
arg_parser = argparse.ArgumentParser(description="Align coding style \
            according to PEP8 guidelines.")

arg_parser.add_argument("-c", "--check", action="store_true", help="shows \
                        all PEP8 errors for file passed as argument.")

arg_parser.add_argument("-p", "--preview", action="store_true", help="shows \
                        refactored file after resolving PEP8 errors, though \
                        keeps the file unchanged.")

arg_parser.add_argument("-r", "--refactor", action="store_true",
                        help="refactors file passed as argument to resolve \
                             PEP8 errors, making changes in place.")

arg_parser.add_argument("-a", "--all", action="store_true", help="applies same \
                        action to all files in src directory, must be used \
                        alongwith one of -c, -p, -r flags, if not, -r will \
                        be default flag.")

arg_parser.add_argument("-i", "--interactive", action="store_true",
                        help="enables interactive mode for purpose.")

arg_parser.add_argument("-f", "--file", help="name of .py file in src \
                        directory, passed as an argument")

# A list containing names of all the python source files.
all_files = ["minecraft_bot/src/" + f + ".py" for f in
             minecraft_bot.src.__all__] + \
            ["spockextras/plugins/cores/" + f + ".py" for f in
             spockextras.plugins.cores.__all__] + \
            ["spockextras/plugins/helpers/" + f + ".py" for f in
             spockextras.plugins.helpers.__all__] + \
            ["spockextras/" + f + ".py" for f in spockextras.__all__]

# A list containing dirpaths of four main directories containing source files.
all_dirs = ["minecraft_bot/src/", "spockextras/plugins/cores/",
            "spockextras/plugins/helpers/", "spockextras/"]


def check_pep8_errors(filename):
    """
    Takes in a single file as argument and shows the errors pertaining to
    PEP8 guidelines in each line of the file.
    Args:
        filename: The path of the file to be worked upon.

    Returns:
        None.
    """
    print("***** PEP8 errors in " + filename + " *****")
    os.system("pep8 " + filename)
    print("DONE !\n\n")


def preview_autopep8(filename):
    """
    Takes in a single file as argument and shows the preview of file modified
    according to PEP8 guidelines. Contents of the file remain unchanged.
    Args:
        filename: The path of the file to be worked upon.

    Returns:
        None.
    """
    print("***** After refactoring " + filename + " *****")
    os.system("autopep8 -a " + filename)
    print("DONE !\n\n")


def refactor_autopep8(filename):
    """
    Takes in a single file as argument and refactors the code within the file
    pertaining to PEP8 guidelines. Contents of the file are over written, so
    write permissions to file are required.
    Args:
        filename: The path of the file to be worked upon.

    Returns:
        None.
    """
    print("***** Refactoring " + filename + " *****")
    os.system("autopep8 -i -a " + filename)
    print("DONE !\n\n")


def check_pep8_errors_all():
    """
    Iteratively performs check_pep8_errors to all the python source files of
    the project.

    Returns:
        None.
    """
    for directory in all_dirs:
        all_filenames = [f for f in os.listdir(directory)
                         if os.path.isfile(os.path.join(directory, f))]

        for filename in all_filenames:
            if filename[-3:] == ".py":
                check_pep8_errors(directory + filename)


def preview_autopep8_all():
    """
    Iteratively performs preview_autopep8 to all the python source files of
    the project.

    Returns:
        None.
    """
    for directory in all_dirs:
        all_filenames = [f for f in os.listdir(directory)
                         if os.path.isfile(os.path.join(directory, f))]

        for filename in all_filenames:
            if filename[-3:] == ".py":
                preview_autopep8(directory + filename)


def refactor_autopep8_all():
    """
    Iteratively performs refactor_autopep8 to all the python source files of
    the project.

    Returns:
        None.
    """
    for directory in all_dirs:
        all_filenames = [f for f in os.listdir(directory)
                         if os.path.isfile(os.path.join(directory, f))]

        for filename in all_filenames:
            if filename[-3:] == ".py":
                refactor_autopep8(directory + filename)


def start_interactive_mode():
    """
    Starts a command line interactive mode to perform different tasks for
    different files any number of time required by user. Each time,
    te filename will be asked, along with the task to be performed. After
    execution, the user will e asked whether to continue to do next task.

    Returns:
        None.
    """
    print("***** INTERACTIVE MODE *****")
    arg_parser.print_help()
    new_task = "y"

    while new_task == "y":
        filename = raw_input("Enter filename: ")
        for a_file in all_files:
            # This step is to get the complete filepath of the filename.
            if a_file.split("/")[-1] == filename:
                filename = a_file
                break

        action = raw_input("Choose action [c/p/r]: ")
        # Perform task according to action given by user.
        if action == "c":
            check_pep8_errors(filename)
        elif action == "p":
            preview_autopep8(filename)
        elif action == "r":
            refactor_autopep8(filename)
        else:
            print("Not a valid option! ")

        new_task = raw_input("Proceed with new task? [y/n]: ")


if __name__ == '__main__':
    # Get all the arguments.
    arguments = vars(arg_parser.parse_args(sys.argv[1:]))

    # Nested if-else block here performs tasks according to arguments.
    if arguments["all"]:
        if arguments["check"]:
            check_pep8_errors_all()

        if arguments["preview"]:
            check_pep8_errors_all()

        if not arguments["check"] and arguments["preview"]:
            refactor_autopep8_all()
    elif arguments["interactive"]:
        start_interactive_mode()
    else:
        try:
            for a_file in all_files:
                if a_file.split("/")[-1] == arguments["file"]:
                    python_filename = a_file
                    break
        except Exception as e:
            print("Encountered " + type(e).__name__)
            # Print help message and exit in case of exceptions.
            arg_parser.print_help()

        if arguments["check"]:
            check_pep8_errors(python_filename)

        if arguments["preview"]:
            preview_autopep8(python_filename)

        if arguments["refactor"]:
            refactor_autopep8(python_filename)
