import cv2
import apriltag 
from apriltag_import import detection_pose
# estimate tagpose 
# 

vid = cv2.VideoCapture(0)

options = apriltag.DetectorOptions(families="tag36h11")
detector = apriltag.Detector(options)

r_tag_id = []
results_data = []
while (True):
    ret, frame = vid.read()
    gray = cv2.cvtColor(frame, cv2.COLOR_BGR2GRAY) # turn to grayscale

    results = detector.detect(gray) # start detecting
    results_data.append(results)
    
    camera_params = []
    tag_size = 0.165 # this is the apriltag size

     
    
    for r in results:
        pose, init_error, final_error = detection_pose(r, camera_params, tag_size)
        (ptA, ptB, ptC, ptD) = r.corners
        ptB = (int(ptB[0]), int(ptB[1]))
        ptC = (int(ptC[0]), int(ptC[1]))
        ptD = (int(ptD[0]), int(ptD[1]))
        ptA = (int(ptA[0]), int(ptA[1]))

        cv2.line(frame, ptA, ptB, (0, 255, 0), 2)
        cv2.line(frame, ptB, ptC, (0, 255, 0), 2)
        cv2.line(frame, ptC, ptD, (0, 255, 0), 2)
        cv2.line(frame, ptD, ptA, (0, 255, 0), 2)
        r_tag_id.append(r.tag_id)
    
    if r_tag_id:
        max_tag = max(r_tag_id)
    # process results here - find highest value tag

    cv2.imshow('zp webcam', frame)
    if cv2.waitKey(1) & 0xFF == ord('q'):
        break

vid.release()
cv2.destroyAllWindows

print(r_tag_id)
print(results_data)