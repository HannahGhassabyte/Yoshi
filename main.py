# Third party modules
import numpy
from PIL import Image

imagename = "pyraa"

def get_image(image_path):
    """Get a numpy array of an image so that one can access values[x][y]."""
    image = Image.open(image_path, "r")
    width, height = image.size
    pixel_values = list(image.getdata())
    if image.mode == "RGBA":
        channels = 4
    elif image.mode == "L":
        channels = 1
    else:
        print("Unknown mode: %s" % image.mode)
        return None
    pixel_values = numpy.array(pixel_values).reshape((height, width, channels))
    return pixel_values
def padded_hex(i, l):
    given_int = i
    given_len = l

    hex_result = hex(given_int)[2:] # remove '0x' from beginning of str
    num_hex_chars = len(hex_result)
    extra_zeros = '0' * (given_len - num_hex_chars) # may not get used..

    return ('0x' + hex_result if num_hex_chars == given_len else
            '?' * given_len if num_hex_chars > given_len else
            '0x' + extra_zeros + hex_result if num_hex_chars < given_len else
            None)

def padded_bin(i, l):
    given_int = i
    given_len = l

    bin_result = bin(given_int)[2:] # remove '0b' from beginning of str
    num_bin_chars = len(bin_result)
    extra_zeros = '0' * (given_len - num_bin_chars) # may not get used..

    return ('0b' + bin_result if num_bin_chars == given_len else
            '?' * given_len if num_bin_chars > given_len else
            '0b' + extra_zeros + bin_result if num_bin_chars < given_len else
            None)

image = get_image(imagename+'.png')
hexvals = []
i = 0
for row in image:
    hexvals.append([])
    for pixel in row:
        #hexvals[i].append(str(pixel))

        val = padded_bin(round(pixel[0]/255.0*31*bool(pixel[3])), 5)[2:] + padded_bin(round(pixel[1]/255.0*63 * bool(pixel[3])), 6)[2:] + padded_bin(round(pixel[2]/255.0*31*bool(pixel[3])), 5)[2:]
        hexvals[i].append(padded_hex(int(val, 2), 4))

    i += 1


new_hex = []
i = 0
inc = int((len(image))/34)
print(inc)
'''
for r in range(inc-1, len(image)-inc, inc):
    new_hex.append([])
    for p in range(inc-1, len(image[0])-inc, inc):
        new_hex[i].append(hexvals[r][p])
    i += 1
'''
new_hex = hexvals
print("Height:", len(new_hex), "\nWidth:", len(new_hex[0]))

file = open(imagename+'.txt', "w+")
content = str(new_hex)
content = content.replace('\'', '').replace('[', '{').replace(']', '}') + ';'
file.write(content)
file.close()