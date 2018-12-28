import sys
import os
from urllib.request import urlretrieve

DEFAULT_NUM = 10
PATHS_FILE = "wet.paths"
DOWNLOAD_DIR = os.path.join(os.getcwd(), "pages/")
PREFIX = "https://commoncrawl.s3.amazonaws.com/"

# Create download directory
if not (os.path.exists(DOWNLOAD_DIR) and os.path.isdir(DOWNLOAD_DIR)):
    print("Download directory doesn't exist.")
    try:
        os.mkdir(DOWNLOAD_DIR)
    except OSError:
        print ("Failed to create download directory %s" % DOWNLOAD_DIR)
    else:
        print ("Successfully created download directory %s" % DOWNLOAD_DIR)

# Determine number of files to download
try:
    readNum = int(eval(sys.argv[1]))
except:
    print("Reading default number of lines")
    readNum = DEFAULT_NUM

# Read file and download files
with open(PATHS_FILE, 'r') as f:
    while readNum > 0:
        filepath = f.readline()[:-1]   # remove trailing newline character
        filename = filepath.split('/')[-1]
        destination = DOWNLOAD_DIR + filename
        if os.path.exists(destination):
            print("{0} already exists".format(filename))
        else:
            print("Downloading {0}".format(filename))
            url = PREFIX + filepath
            urlretrieve(url, destination)
        readNum -= 1
