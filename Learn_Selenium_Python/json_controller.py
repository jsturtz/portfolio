import json

file_r = open("rubric_responses.json", mode="r")
file_w = open("rubric_responses.json", mode="w")

def add_entry(key, titles, grade_levels):
    with file_r:
        data = json.load(file_r)
        data[key] = {"titles": titles,
                     "grade_levels": grade_levels}

    with file_w:
        json.dump(data, file_w, ensure_ascii=False, indent=4, sort_keys=True)

def remove_entry(key):
    with file_r:
        data = json.load(file_r)
        del data[key]

    with file_w:
        json.dump(data, file_w, ensure_ascii=False, indent=4, sort_keys=True)

# functions pertaining to rubric_strings
def find_key(title):
    with file_r:
        data = json.load(file_r)

    for key, value in data.items():
        if title in value["titles"]:
            return key
        else:
            pass
    return None

def add_title(key, title):

    with file_r:
        data = json.load(file_r)

    data[key]["titles"].append(title)

    with file_w:
        json.dump(data, file_w, ensure_ascii=False, indent=4, sort_keys=True)

def make_new_content(grade_levels):
    with file_r:
        data = json.load(file_r)

    new_entry = {}
    for grade in grade_levels:
        key = grade
        value = input("Content for: " + key + " ")
        new_entry[key] = value

    return new_entry

def update_strings(string):
    with file_r:
        data = json.load(file_r)
    keys = str(list(data.keys()))
    print("You can either add this rubric element to an existing key, or else you can make a new entry")
    print("Existing keys: " + keys)
    print()
    print("0 - Add to existing key")
    print("1 - Make new key")

    while True:
        reply = input("--> ")

        try:
            reply = int(reply)
        except:
            print("Your answer must be an integer")
            continue

        if reply < 2:
            break

    if reply == 0:

        while True:
            reply1 = input("enter the key value: ")
            if not reply1 in keys:
                print("You must enter a value that matches one of the keys")
                print("keys: " + keys)
            else:
                break

        add_title("rubric_strings.json", reply1, string)

    if reply == 1:

        while True:
            new_key = input("new key: ")
            if find_key("rubric_responses.json", new_key) is None:
                break
            else:
                print("That key already exists. Choose another.")

        new_value = make_new_content("rubric_responses.json")
        add_entry("rubric_responses.json", new_key, new_value)
        add_string("rubric_strings.json", new_key, string)

