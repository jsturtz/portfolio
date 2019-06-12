from selenium import webdriver
from selenium.webdriver.common.keys import Keys
import utilities
import time
from selenium.webdriver.support.ui import Select
import json_controller

def grade(driver):

    unit_text = {"Unit 1": "",
             "Unit 2": "",
             "Unit 3": "",
             "Unit 4": "",
             "Unit 5": "",
             "Unit 6": "",
             "Unit 7": "",
             "Unit 8": ""}
    driver.refresh()
    # checks to see if on correct page
    try:
        title = driver.find_element_by_id("pageTitleText").get_attribute("textContent")
    except:
        print("ERROR: not the correct page to call grade function. Couldn't find pageTitleText")
        return ''

    if "Grade Assignment" in title:
        start = title.find("Unit")
        end = title.find("Assignment", start) - 1
        unit = title[start: end]

    elif "Grade Discussion Forum" in title:
        print("this is a discussion forum bitch!")

    else:
        print("ERROR: not the correct page to call grade function")
        return ''

    # open grading pane
    pane_btn = driver.find_element_by_id("currentAttempt_gradeDataPanelLink")
    pane_btn.click()
    time.sleep(0.1)

    rubric_list = driver.find_element_by_id("collabRubricList")
    rubric_list.find_element_by_class_name("itemHead").click()

    # grabs elements from page to produce interface for user
    grading_pane = driver.find_elements_by_class_name('rubricGradingRow.columnPalette')
    rubric_elements = []
    for elem in grading_pane:
        title = elem.find_element_by_tag_name("h4").get_attribute("textContent")
        cells = elem.find_elements_by_class_name("rubricGradingCell")
        buttons = []
        for cell in cells:
            radio_label = cell.find_element_by_class_name("radioLabel").get_attribute("textContent")
            radio = cell.find_element_by_class_name("rubricCellRadio")
            buttons.append([radio_label, radio])
        rubric_elements.append({"title": title, "buttons": buttons})

    # checks json file and asks user to update responses if no keys exist for rubric elements
    for elem in rubric_elements:
        title = elem["title"]
        key = json_controller.find_key("rubric_strings.json", title)
        if key is None:
            key = json_controller.update_strings("rubric_strings.json")
        else:
            pass
        elem["content"] = key

    # collects user input for grading and clicks radios
    for elem in rubric_elements:
        i = 0
        print(elem["title"])
        print("Choose a number:")

        for btn in elem["buttons"]:
            print(str(i) + "  " + btn[0])
            i += 1

        while True:
            answer = input("--->  ")
            try:
                answer = int(answer)
            except:
                print("You must enter an integer value only.")
                continue
            if not answer < len(elem["buttons"]):
                print("You must choose an integer between 0 and " + str(len(elem["buttons"]) - 1))
                continue
            break

        # weird behavior with the radios. If the btn is already selected, it will click the wrong radio
        # when you try to click the selected btn. To avoid this, I identify the parent of the radio,
        # and if the parent's class tag has the string "selectedCell" in it, then we will not press the
        # btn.

        btn = elem["buttons"][answer][1]
        parent_btn = btn.find_element_by_xpath("..")
        if parent_btn.get_attribute("class").find("selectedCell") == -1:
            btn.click()

        # updates all of the percent fields with the highest possible for the grade
        # WARNING: is coded to select the last option in the drop-down boxes. This should work
        # unless for some reason the last element is not the largest
        percent_field = parent_btn.find_element_by_class_name("selectedPercentField")
        options = percent_field.find_elements_by_tag_name("option")
        selector = Select(percent_field)
        selector.select_by_index(len(options) - 1)

    # saves rubric changes
    driver.find_element_by_link_text("Save Rubric").click()

    # sets variable "name" to student's name
    name_node = driver.find_element_by_id("anonymous_element_19")
    span_elems = name_node.find_elements_by_tag_name("span")
    for elem in span_elems:
        if elem.text.find("Attempt") != -1:
            name = elem.text
            name = name[:name.find(" ")]

    # posts pre-made content into the feedback window
    driver.switch_to.frame("feedbacktext_ifr")
    driver.find_element_by_id("tinymce").clear()
    driver.find_element_by_id("tinymce").send_keys("Hi " + name + ",", Keys.ENTER)
    driver.find_element_by_id("tinymce").send_keys(unit_text[unit])

    driver.switch_to.default_content()

    # gives user chance to choose to submit or exit
    while True:
        print("Submit or exit?")
        print("0  Submit")
        print("1  Exit")
        answer = input("--->  ")
        if answer == "0":
            try:
                driver.find_element_by_class_name("button-3").click()
                return ''
            except:
                print("ERROR: Could not find element 'button-3'")
        elif answer == "1":
            return ''
        else:
            print('Your reply must be either "0" or "1"')

def grade_center(driver):
    try:
        driver.get(
            "https://post.blackboard.com/webapps/gradebook/do/"
            "instructor/enterGradeCenter?course_id=_77469_1&cvid=fullGC")
    except:
        print("Something went wrong")

    def go_to_assignment():
        pass

def navigate(driver):
    try:
       driver.get("https://post.blackboard.com/webapps/assignment/gradeAssignmentRedirector?outcomeDefinitionId=_1630138_1&currentAttemptIndex=1&numAttempts=16&anonymousMode=false&sequenceId=_77469_1_0&course_id=_77469_1&source=cp_gradebook&viewInfo=fullGradeCenterMsg&attempt_id=_7921725_1&courseMembershipId=_7318979_1&cancelGradeUrl=%2Fwebapps%2Fgradebook%2Fdo%2Finstructor%2FenterGradeCenter%3Fcourse_id%3D_77469_1&submitGradeUrl=%2Fwebapps%2Fgradebook%2Fdo%2Finstructor%2FperformGrading%3Fcourse_id%3D_77469_1%26cmd%3Dnext%26sequenceId%3D_77469_1_0")
    except:
        print("something went wrong")

def dbupdate():
    pass

