'''

NAMING CONVENTIONS AND VARIABLES:
The variables "base_commands", "uni_commands", "course_commands" and "all_commands" are always merely lists holding
the strings which get displayed in the application. In contrast, the variables "uni_methods" and "course_methods" are
dictionaries to be used within the code to call various methods.

The "uni_methods" and "course_methods" variables are
dictionaries holding strings corresponding to the names of the functions to be called. These dictionaries are returned
by functions within the university.py files. For example, uni_post has a function called "get_methods" which
returns a dictionary of the methods associated with that university. This dictionary is assigned to the variable
"uni_methods" once a university is selected by the user of the application. The get_courses function is more complex.
It returns function objects which themselves each return a dictionary containing the methods available to those
courses.

HOW TO UPDATE THE APPLICATION:
- Whenever we add a university to the application, we must do two things:
    (1) import the file into index.py
    (2) update the uni_files variable to include a new dictionary entry

- If a base command is added, its display-name must be included in the "base_commands" variable below. Also, the
exact string used in the display name must also be used in the conditional when building the code
(e.g. if command == "new_command") or else the code will not work. All other updates to the functionality of
the application are done through modifying the methods in each university file.

'''

from selenium import webdriver
import uni_snhu, uni_baker, post_bus311

uni_files = {"snhu": uni_snhu, "baker": uni_baker, "post": post_bus311}

active_uni_name = "none"
active_course_name = "none"

base_commands = ["refresh driver", "choose university"]
uni_commands = []
course_commands = []
all_commands = base_commands + uni_commands + course_commands

uni_methods = {}
course_methods = {}



def is_correct_input(input, length):
    try:
        input = int(input)

        if input <= length:
            return True
        else:
            print("ERROR: value must be between 0 and " + str(length - 1))
            return False
    except:
        print("ERROR: only integer values are acceptable")
        return False

driver = webdriver.Firefox()

while True:

    count = 0
    for cmd in base_commands:
        print(str(count) + " - " + cmd)
        count += 1

    if len(uni_commands) > 0:
        for cmd in uni_commands:
            print(str(count) + " - " + active_uni_name + ": " + cmd)
            count += 1
    if len(course_commands) > 0:
        for cmd in course_commands:
            print(str(count) + " - " + active_course_name + ": " + cmd)
            count += 1

    all_commands = base_commands + uni_commands + course_commands

    reply = input("--> ")
    while not is_correct_input(reply, len(all_commands)):
        reply = input("--> ")

    command = all_commands[int(reply)]

    if command == "choose university":
        uni_names = list(uni_files.keys())
        for i in range(len(uni_files)):
            print(str(i) + " - " + uni_names[i])

        reply = input("--> ")
        while not is_correct_input(reply, len(uni_names)):
            reply = input("--> ")

        active_uni_name = uni_names[int(reply)]
        active_uni_file = uni_files[active_uni_name]
        uni_methods = active_uni_file.get_methods()

        uni_commands.clear()
        uni_commands.append("choose course")

        for key in uni_methods.keys():
            uni_commands.append(key)

    elif command == "refresh driver":
        driver.close()
        driver = webdriver.Firefox()

    elif command == "choose course":
        courses = active_uni_file.get_courses()
        course_names = list(courses.keys())
        for i in range(len(course_names)):
            print(str(i) + " - " + str(course_names[i]))

        reply = input("--> ")
        while not is_correct_input(reply, len(course_names)):
            reply = input("--> ")

        active_course_name = list(courses.keys())[int(reply)]
        course_methods = courses[active_course_name]()

        for key in course_methods.keys():
            course_commands.append(key)

    elif command in uni_methods.keys():
        uni_methods[command](driver)

    elif command in course_methods.keys():
        course_methods[command](driver)

