import uni_post
import post_bus311
import post_mkt235
from selenium import webdriver
from selenium.webdriver.common.keys import Keys
from selenium.webdriver.common.by import By
from selenium.webdriver.support.ui import WebDriverWait
from selenium.webdriver.support import expected_conditions as EC
from selenium.webdriver.common.action_chains import ActionChains
from selenium.webdriver.firefox.firefox_profile import FirefoxProfile
import json

def has_correct_args(command, args):

    num_args = {"help": [0, [None]],
                "login": [0, [None]],
                "refresh": [0, [None]],
                "grade": [1, [["bus311", "mkt235"]]],
                "grade_center": [1, [["bus311", "mkt235"]]],
                "db": [1, [["bus311", "mkt235"]]],
                "navigate": [1, [["bus311", "mkt235"]]]}

    allowed_args = { }

    if num_args[command][0] != len(args):
        print(args)
        print(num_args[command][0])
        print(len(args))
        print("ERROR: " + command + " takes " + str(len(args)) + " arguments")
        return False

    for i in range(len(args)):
        acceptable_args = num_args[command][1][i]
        if not args[i] in acceptable_args:
            print("ERROR: " + args[i] + " is not an acceptable argument of " + command)
            return False

    return True

def main():

    courses = {"bus311": post_bus311,
               "mkt235": post_mkt235}

    driver = webdriver.Firefox()
    driver.implicitly_wait(10)
    uni_post.login(driver)
    post_bus311.navigate(driver)

    with open("rubric_responses.json", "w") as outfile:
        json.dump("Does this work?", outfile)

    rubric_file = open("rubric_responses.json", "w+")
    rubric_file.write("Does this work?")

    while False:
        print('Type "help" to see quick guide to using this application')
        print("Course Tags: bus311, mkt235")
        command, *args = input("--> ").split(" ")

        if command == "help":
            print("login - will login to Post")
            print("refresh - will refresh the webdriver if needed")
            print("grade_center <course> - will take you to the grade center for that course.")
            print("grade <course> - Use this function only when the assignment is currently open on webdriver.")
            print("db <course> - Use this function WHEN? to update discussion posts for that course")

        elif command == "refresh":
            driver.close()
            driver = webdriver.Firefox()

        elif command == "login":
            if has_correct_args(command, args):
                uni_post.login(driver)

        elif command == "grade_center":
            if has_correct_args(command, args):
                courses[args[0]].grade_center(driver)

        elif command == "grade":
            if has_correct_args(command, args):
                courses[args[0]].grade(driver)

        elif command == "navigate":
            if has_correct_args(command, args):
                courses[args[0]].navigate(driver)

        elif command == "db":
            if has_correct_args(command, args):
                courses[args[0]].dbupdate()

        else:
            print("Not an available command.")

main()

