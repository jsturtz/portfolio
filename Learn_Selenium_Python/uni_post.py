import utilities

def login(driver):
    # password hidden

    driver.get(home)

    # detects whether cookie window popped up
    try:
        agree_button = driver.find_element_by_id('agree_button')
        agree_button.click()
    except:
        pass

    # detects whether user is logged in
    try:
        login_button = utilities.find_element_by_href(driver,
                                                      "https://post.blackboard.com/webapps/login/?action=relogin")
        login_button.click()

        user_name = driver.find_element_by_id("user_id")
        user_name.send_keys("0000004008")
        password = driver.find_element_by_id("password")
        password.send_keys("BB030468bb")
        login_button = driver.find_element_by_id("entry-login")
        login_button.click()
    except:
        pass
