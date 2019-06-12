def grade_center(driver):
    try:
        driver.get(
            "https://post.blackboard.com/webapps/gradebook/do/instructor/enterGradeCenter?course_id=_79542_1&cvid=fullGC")
    except:
        print("Something went wrong")