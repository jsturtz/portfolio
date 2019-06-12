import json_controller

class Rubric_Elems():
    def __init__(self, title_str, buttons):
        self.title_str = title_str
        self.buttons = buttons
        self.content = [x for x in self.buttons.keys()]

    json_controller.find_key("")


