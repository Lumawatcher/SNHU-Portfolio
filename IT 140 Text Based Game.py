# Jared Smith
rooms = {
    # this dictionary contains a dictionary for each room, containing valid move directions, a description and
    # available items. It also contains a dictionary of replacement descriptions for when an item is taken
    'showers': {'north': 'lathe', 'south': 'press', 'description':
                'The room you woke up in, mostly bare, with a drain in the center of the floor. \n'
                'There is a door north and south.'},
    'lathe': {'south': 'showers', 'east': 'ammo', 'west': 'woodworking', 'item': 'tube', 'description':
              'A small room with a large metal machine in the corner. The machine is spinning hollow metal tubes.\n'
              'There is a finished tube on a desk to the side.\n'
              'There are doors east, south and west.'},
    'woodworking': {'east': 'lathe', 'item': 'stock', 'description':
                    'a room with somewhat familiar tools, for woodworking. Scattered about the floor are what appear to'
                    ' be failed projects. On the workbench is a stock of wood with a large \'47\' painted on the side'
                    '\nThere is a door east.'},
    'ammo': {'west': 'lathe', 'item': 'tins', 'description':
             'A large room with dozens of boxes, each filled with hundreds of tiny metal darts. '
             'You find a single crate that contains a few thin metal tins, filled with these darts'
             '\nThere is a door west.'},
    'press': {'north': 'showers', 'south': 'receiving', 'east': 'study', 'item': 'boat', 'description':
              'A room filled with large mechanical presses that appear to be shaping metal, with extreme force, into'
              ' tiny rectangular metal boats. You see what appears to be a completed metal boat, '
              'marked with white chalk on one of the machines.'
              '\nThere are doors north, south and east.'},
    'receiving': {'north': 'press', 'item': 'handle', 'description':
                  'A room that appears to be a shipping area. There are many sealed crates you cannot pry open, and '
                  'registers of materials coming and leaving the workshop. Near the door, you find a small wooden '
                  'handle. While you can\'t tell what it might go to, it fits nicely in your hand'
                  '\nThere is a door north.'},
    'study': {'west': 'press', 'east': 'villain', 'item': 'manual', 'description':
              'A small study, crammed with books on topics you cannot understand. '
              'Although the office is a disaster, there is a manual placed neatly on top the desk, all by itself. '
              'The pictures contained within it seem familiar to you...'
              '\nThere are doors east and west.  You hear ominous noises to the east...'},
    'no item': {
        'lathe': 'A small room with a large metal machine in the corner.The machine is spinning hollow metal tubes'
                 '.\nThere are doors east, south and west.',

        'woodworking': 'a room with somewhat familiar tools, for woodworking. Scattered about the '
                       'floor are what appear to be failed projects.\nThere is a door east.',

        'ammo': 'A large room with dozens of boxes, each filled with hundreds of tiny metal darts.'
                '\nThere is a door west.',

        'press': 'A room filled with large mechanical presses that appear to be shaping metal, '
                 'with extreme force, into tiny rectangular metal boats.'
                 '\nThere are doors north, south and east.',

        'receiving': 'A room that appears to be a shipping area. There are many sealed crates you cannot pry open,'
                     ' and registers of materials coming and leaving the workshop.\nThere is a door north.',

        'study': 'A small study, crammed with books on topics you cannot understand.'
                 '\nThere are doors east and west. You hear ominous noises to the east...'
    }
}

player_location = 'showers'
player_inventory = []
instructions = ('To move, type \'go, (direction)\'. Directions are north, south, east or west.\n'
                'To get an item, type \'get (item name)\'. All items names are one word.\n'
                'To see these instructions, type \'help\'')


# functions
def move_player(move_list, location, dict2):
    if len(move_list) < 2:
        # checks that the player input a direction
        move_list.append(input('Please choose a direction\n').lower().strip())
    if move_list[1] in dict2[location]:
        # checks room's dictionary for valid direction and updates player location
        new_location = dict2[location][move_list[1]]
        input('You go {}\n'.format(move_list[1]))
        return new_location
    else:
        input('Cannot go {}\n'.format(move_list[1]))
        return location


def get_item(input_list, location1, dict1):
    if len(input_list) < 2:
        # checks that the player input an item to get
        input_list.append(input('Get what?\n').lower().strip())
    if ('item' in dict1[location1]) and (dict1[location1]['item'] == input_list[1]):
        # checks that the room has an item to get and that the item to get matches the item the player input
        player_inventory.append(rooms[player_location]['item'])
        rooms[player_location].pop('item')
        rooms[player_location].update({'description': rooms['no item'][player_location]})
        input('Added {} to inventory\n'.format(input_list[1]))
    else:
        input('cannot get {}\n'.format(input_list[1]))


def player_help():
    print(instructions)
    input('press enter to continue\n')

def player_status():
    print('\n\nCurrent inventory: {}\n'
          'Location: {}: {}\n'.format(player_inventory, player_location, rooms[player_location]['description']))


# main starts here
print(instructions)

print('\n\nYou are a valiant knight, pursuing an evil warlock that has been terrorizing your fiefdom. You pursued him '
      'for days, and after cornering him in a tavern miles away from your home, he hexed you with magical sleep.\n\n\n'
      'You awaken, surrounded by unholy machinations completely foreign to your experience. Your only hope for survival'
      ' is to assemble a weapon and escape the warlock\'s workshop.\n\n')
input('press \'enter\' to continue\n')


while not (player_location == 'villain'):
    # runs until the player location is updated to villain, will be updated by command 'go east' in study
    player_status()
    current_input = input('Enter command: \n').lower().strip().split()
    if len(current_input) == 0:
        print('no input detected')
        input('press enter to continue')

    elif current_input[0] == 'go':
        player_location = move_player(current_input, player_location, rooms)

    elif current_input[0] == 'get':
        get_item(current_input, player_location, rooms)

    elif current_input[0] == 'help':
        player_help()

    elif current_input[0] == 'status':
        pass

    else:
        print('input not recognized, please try again.\n')
        input('press enter to continue')

print('You step into the next room, and standing in the middle of a large area is the zombie of Mikhail Kalashnikov!\n'
      'He notices you, and begins shuffling his decaying frame toward you with fatal finality!')
if len(player_inventory) < 6:
    input('You search desperately through your inventory for a weapon, but no matter how hard you try you cannot'
          ' assemble a weapon with the materials you gathered. Kalashnikov moves on you quickly, and you meet your'
          'tragic end at the hands of an undead machinist.\n'
          'Thank you for playing! Try again!')
else:
    input('You use the detailed descriptions in the manual you found to assemble a weapon of unparallelled lethality\n'
          'You turn this weapon on the revenant of it\'s creator, and fire a stream of red hot metal through the room'
          '\n\nGreat job! you win!\nThank you for playing!')