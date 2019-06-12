import time

class Rubric_Element():
    def __init__(self, title_text, button_list):
        self.title_text = title_text
        self.button_list = button_list

def find_element_by_href(elem, href):
    anchors = elem.find_elements_by_tag_name("a")
    for anch in anchors:
        if anch.get_attribute("href") == href:
            return anch

def remove_tags(string):
    new_string = ""
    add_mode = True
    for char in string:
        if add_mode:
            if char != "<":
                new_string += char
            else:
                add_mode = False
        else:
            if char == ">":
                add_mode = True
    return new_string




'''
def wait_for(condition_function):
    start_time = time.time()
    while time.time() < start_time + 3:
        if condition_function():
            return True
        else:
            time.sleep(0.1)
    raise Exception(
        'Timeout waiting for {}'.format(condition_function.__name__)
    )

def click_through_to_new_page(link_text, browser):
    link = browser.find_element_by_link_text('my link')
    link.click()

    def link_has_gone_stale():
        try:
            # poll the link with an arbitrary call
            link.find_elements_by_id('doesnt-matter')
            return False
        except StaleElementReferenceException:
            return True

    wait_for(link_has_gone_stale)

class wait_for_page_load(object):

    def __init__(self, browser):
        self.browser = browser

    def __enter__(self):
        self.old_page = self.browser.find_element_by_tag_name('html')

    def page_has_loaded(self):
        new_page = self.browser.find_element_by_tag_name('html')
        return new_page.id != self.old_page.id

    def __exit__(self, *_):
        wait_for(self.page_has_loaded)

with wait_for_page_load(browser):
    browser.find_element_by_link_text("my link").click()
'''