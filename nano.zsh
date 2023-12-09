lsusb
v4l2-ctl --list-devices (confirm that we see the /dev/video0
gst-launch-1.0 v4l2src device=/dev/video0 ! 'image/jpeg,width=640,height=480,framerate=30/1' ! jpegdec ! xvimagesink
gst-launch-1.0 v4l2src device=/dev/video0 ! 'image/jpeg,width=640,height=480,framerate=30/1' ! jpegdec ! videoflip method=rotate-180 ! xvimagesink
gst-launch-1.0 v4l2src device=/dev/video0 num-buffers=1 ! 'image/jpeg,width=640,height=480,framerate=30/1' ! filesink location=snapshot.jpg
gst-launch-1.0 v4l2src device=/dev/video0 num-buffers=1 ! 'image/jpeg,width=640,height=480,framerate=30/1' ! jpegdec ! videoflip method=rotate-180 ! jpegenc ! filesink location=flipped_snapshot.jpg
