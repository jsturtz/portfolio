from selenium import webdriver
from selenium.webdriver.common.keys import Keys

# these represent university-wide functions
def login(driver):
    pass

def grade_center(driver):
    pass

# each new course gets added here as a function. The functions within these courses are course-specific functions
def bus311():

    def grade(driver):
        print("you graded bus311!")

    def other_func(driver):
        pass

    return {"grade": grade,
            "other_func": other_func}

def mkt235():

    def grade(driver):
        pass

    def other_func(driver):
        pass

    return {"grade": grade,
            "other_func": other_func}

# only these functions are accessed through index.py
def get_name():
    return "baker"

def get_methods():
    return {"login": login,
            "grade_center": grade_center}

def get_courses():
    return {"bus311": bus311,
            "mkt235": mkt235}