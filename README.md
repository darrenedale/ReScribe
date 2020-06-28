# ReScribe
A simple disk image writer.

This project is still in the very early stages of development, it's little more than a proof of concept.
Currently it has a UI and this basic functionality:

- read local disk images or download remote disk images
- read information from local disk images in certain cirumstances (ISO only)
- authorise write to device file
- write disk image to device file

## Todo
In no particular order:
- checksum verification for downloaded images
- scan downloaded images for malware
- on-the-fly image decompression
- pause and resume during download and write
- image write verification
- actually design the UI rather than bolting onto the POC
- investigate options for download and write in parallel
