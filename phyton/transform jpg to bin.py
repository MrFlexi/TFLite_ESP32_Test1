import numpy as np
import os.path
from PIL import Image
import matplotlib.pyplot as plt
import glob

INPUT_IMAGE_SIZE = 14

# Path to TensorFlow Lite model file
tflite_model_file = 'model.tflite'

# Path to images folder
path = 'test_images14x14/'
file_format = ".jpg"


files = [files for files in glob.glob(path + "*" + file_format, recursive=False)]
print("Found {0} {1} files".format(len(files), file_format))

for f in files:
    # Read image
    print(f)
    raw_image = Image.open(f).convert('L')
    img = np.array(raw_image, dtype=np.uint8)
    print(img)
    fname = os.path.splitext(f)[0] + '.txt'
    print(fname)
    np.savetxt(fname, img, delimiter=',' ,newline='', fmt='%u')